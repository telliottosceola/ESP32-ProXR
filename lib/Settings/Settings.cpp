#include <Settings.h>

#define DEBUG

bool Settings::init(){
  if(!SPIFFS.begin(true)){
    #ifdef DEBUG
    Serial.println("SPIFFS Mount Failed");
    #endif
    return false;
  }else{
    return true;
  }
}

bool Settings::storeSettings(String s){
  DynamicJsonBuffer newSettingsBuffer;
  JsonObject& newSettingsJSON = newSettingsBuffer.parseObject(s);
  if(newSettingsJSON.success()){
    int storedSettingsLength = fileSystem.getFileSize(SPIFFS, "/settings.txt");
    if(storedSettingsLength != 0){
      char storedSettingsFileBuffer[storedSettingsLength+1];
      if(fileSystem.readFile(SPIFFS, "/settings.txt", storedSettingsFileBuffer, storedSettingsLength)){
        JsonObject& oldSettingsJSON = newSettingsBuffer.parseObject(String(storedSettingsFileBuffer));
        if(oldSettingsJSON.success()){
          //Get loaded settings into JSON
          for(auto kvp:newSettingsJSON){
            if(oldSettingsJSON.containsKey(kvp.key)){
              oldSettingsJSON[kvp.key] = kvp.value;
            }else{
              #ifdef DEBUG
              Serial.printf("invalid settings key: %s\n", kvp.key);
              #endif
            }
          }
          //set public variables based on new Settings
          setPublicVariables(oldSettingsJSON);

          File file = SPIFFS.open("/settings.txt", FILE_WRITE);
          if(file){
            oldSettingsJSON.printTo(file);
            file.close();
            return true;
          }else{
            #ifdef DEBUG
            Serial.println("failed to save settings to file in Settings.storeSettings");
            #endif
            return false;
          }

        }else{
          #ifdef DEBUG
          Serial.println("Invalid JSON in old settings");
          Serial.println(storedSettingsFileBuffer);
          #endif
          return false;
        }

      }else{
        #ifdef DEBUG
        Serial.println("readFile failed in Settings.storeSettings");
        #endif
        return false;
      }
    }else{
      #ifdef DEBUG
      Serial.println("settings.txt is empty in Settings.storeSettings");
      #endif
      return false;
    }
  }else{
    #ifdef DEBUG
    Serial.println("New Settings invalid JSON");
    Serial.println(s);
    #endif
    return false;
  }
}

bool Settings::loadSettings(){
  int storedSettingsLength = fileSystem.getFileSize(SPIFFS, "/settings.txt");
  if(storedSettingsLength != 0){
    char storedSettingsFileBuffer[storedSettingsLength+1];
    if(fileSystem.readFile(SPIFFS, "/settings.txt", storedSettingsFileBuffer, storedSettingsLength)){
      DynamicJsonBuffer jsonBuffer;
      JsonObject& storedSettingsJSON = jsonBuffer.parseObject(String(storedSettingsFileBuffer));
      if(storedSettingsJSON.success()){
        setPublicVariables(storedSettingsJSON);
        discoveredNetworks = wifiHandler.scanNetworks(wlanSSID);
        char mac[4] = "...";
        char ip[4] = "...";
        returnSettings(mac,ip,true);
        return true;
      }else{
        #ifdef DEBUG
        Serial.println("Invalid JSON in Settings.loadSettings");
        Serial.println(storedSettingsFileBuffer);
        #endif
        factoryReset();
        return false;
      }
    }else{
      #ifdef DEBUG
      Serial.println("readFile failed in Settings.loadSettings");
      #endif
      factoryReset();
      return false;
    }
  }else{
    #ifdef DEBUG
    Serial.println("settings.txt is empty in Settings.loadSettings");
    #endif
    factoryReset();
    return false;
  }
}

String Settings::returnSettings(char* macAddress,char* ipAddress, bool load){
  if(load){
    //Load Top Level Settings
    int storedSettingsLength = fileSystem.getFileSize(SPIFFS, "/settings.txt");
    if(storedSettingsLength != 0){
      char storedSettingsFileBuffer[storedSettingsLength+1];
      if(fileSystem.readFile(SPIFFS, "/settings.txt", storedSettingsFileBuffer, storedSettingsLength)){
        loadedSettings = String(storedSettingsFileBuffer);
      }
    }
  }

  DynamicJsonBuffer jsonBuffer;
  JsonObject& storedSettingsJSON = jsonBuffer.parseObject(loadedSettings);
  if(storedSettingsJSON.success()){
    if(storedSettingsJSON.containsKey("ssid_password")){
      if(strcmp(storedSettingsJSON["ssid_password"], "") == 0){
        storedSettingsJSON.remove("ssid_password");
      }else{
        storedSettingsJSON["ssid_password"] = "__SET__";
      }
    }
    if(storedSettingsJSON.containsKey("ssid_password")){
      storedSettingsJSON["apPass"] = "__SET__";
    }
    storedSettingsJSON.set("mac",macAddress);
    storedSettingsJSON.set("local_ip",ipAddress);
    String Settings;
    storedSettingsJSON.printTo(Settings);
    return Settings;
  }else{
    #ifdef DEBUG
    Serial.println("Parse JSON in stored settings failed in Settings.returnSettings");
    #endif
    return "";
  }

}

bool Settings::factoryReset(){
  #ifdef DEBUG
  Serial.println("factoryReset Settings");
  #endif
  int defaultSettingsLength = fileSystem.getFileSize(SPIFFS, "/settings_D.txt");
  if(defaultSettingsLength != 0){
    char defaultSettingsFileBuffer[defaultSettingsLength+1];
    if(fileSystem.readFile(SPIFFS, "/settings_D.txt", defaultSettingsFileBuffer, defaultSettingsLength)){
      DynamicJsonBuffer jsonBuffer;
      JsonObject& defaultSettingsJSON = jsonBuffer.parseObject(String(defaultSettingsFileBuffer));
      if(defaultSettingsJSON.success()){
        File file = SPIFFS.open("/settings.txt", FILE_WRITE);
        if(file){
          defaultSettingsJSON.printTo(file);
          file.close();
          return true;
        }
      }
    }
  }
  #ifdef DEBUG
  Serial.println("Default settings file is empty");
  #endif
  return false;
}

void Settings::setPublicVariables(JsonObject& settingsJSON){
  //set public variables based on new Settings
  memset(wlanSSID, 0, 50);
  strcpy(wlanSSID, settingsJSON["ssid"]);
  memset(wlanPASS, 0, 50);
  strcpy(wlanPASS, settingsJSON["ssid_password"]);
  memset(apSSID, 0, 50);
  strcpy(apSSID, settingsJSON["apSSID"]);
  memset(apPass, 0, 50);
  strcpy(apPass, settingsJSON["apPass"]);
  memset(bluetoothPairing, 0, 50);
  strcpy(bluetoothPairing, settingsJSON["bluetoothPairing"]);
  memset(bluetoothName, 0, 50);
  strcpy(bluetoothName, settingsJSON["bluetoothName"]);
  baudRate = settingsJSON["baudRate"].as<int>();
  tcpListenPort = settingsJSON["tcpListenPort"].as<int>();
  dhcpEnabled = settingsJSON["dhcp_enabled"].as<bool>();

  JsonArray& staticIPArray = settingsJSON["static_ip"];
  for(int i = 0; i < 4; i++){
    staticIP[i] = staticIPArray[i].as<int>();
  }

  JsonArray& gatewayIPArray = settingsJSON["default_gateway"];
  for(int i = 0; i < 4; i++){
    defaultGateway[i] = gatewayIPArray[i].as<int>();
  }

  JsonArray& subnetMaskArray = settingsJSON["subnet_mask"];
  for(int i = 0; i < 4; i++){
    subnetMask[i] = subnetMaskArray[i].as<int>();
  }

  JsonArray& dnsPrimaryArray = settingsJSON["dns_primary"];
  for(int i = 0; i < 4; i++){
    primaryDNS[i] = dnsPrimaryArray[i].as<int>();
  }

  JsonArray& dnsSecondaryArray = settingsJSON["dns_secondary"];
  for(int i = 0; i < 4; i++){
    secondaryDNS[i] = dnsSecondaryArray[i].as<int>();
  }
}
