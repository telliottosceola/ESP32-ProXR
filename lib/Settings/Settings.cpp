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
        char mac[4] = "...";
        char ip[4] = "...";
        returnSettings(mac,ip,true);
        //scan networks
        WiFi.mode(WIFI_STA);
        delay(100);
        DynamicJsonBuffer jsonBuffer;
        JsonArray& foundNetworks = jsonBuffer.createArray();
        int n = WiFi.scanNetworks();
        if(n == 0){
          #ifdef DEBUG
          Serial.println("No Networks Found");
          #endif
          delay(500);
        }else{
          for(int i = 0; i < n; i++){
            JsonObject& network = foundNetworks.createNestedObject();
            network["ssid"] = WiFi.SSID(i);
            network["rssi"] = WiFi.RSSI(i);
            if(wlanSSID == network["ssid"]){
              network["selected"] = true;
            }
          }
          WiFi.mode(WIFI_AP);
        }
        foundNetworks.printTo(discoveredNetworks);
        #ifdef DEBUG
        Serial.println("Returing true from load settings");
        delay(50);
        #endif
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
    storedSettingsJSON["ssid"] = jsonBuffer.parseArray(WiFiNetworks);
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
  int defaultSettingsLength = fileSystem.getFileSize(SPIFFS, "/settings_d.txt");
  if(defaultSettingsLength != 0){
    char defaultSettingsFileBuffer[defaultSettingsLength+1];
    if(fileSystem.readFile(SPIFFS, "/settings_d.txt", defaultSettingsFileBuffer, defaultSettingsLength)){
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
  strcpy(apSSID, settingsJSON["ap_ssid"]);
  memset(apPass, 0, 50);
  strcpy(apPass, settingsJSON["ap_password"]);
  memset(bluetoothPairing, 0, 50);
  strcpy(bluetoothPairing, settingsJSON["bluetooth_pairing"]);
  memset(bluetoothName, 0, 50);
  strcpy(bluetoothName, settingsJSON["bluetooth_name"]);
  memset(deviceID, 0, 33);
  strcpy(deviceID, settingsJSON["wifi_device_id"]);
  baudRate = settingsJSON["baud_rate"].as<int>();
  usbBaudRate = settingsJSON["usb_baud_rate"].as<int>();
  tcpListenPort = settingsJSON["tcp_listen_port"].as<int>();
  dhcpEnabled = settingsJSON["dhcp_enabled"].as<bool>();
  wifiEnabled = settingsJSON["wifi_enabled"].as<bool>();
  bluetoothEnabled = settingsJSON["bluetooth_enabled"].as<bool>();
  httpControlEnabled = settingsJSON["http_control_enabled"].as<bool>();
  udpBroadcastEnabled = settingsJSON["udp_broadcast_enabled"].as<bool>();
  udpRemoteBroadcastEnabled = settingsJSON["udp_remote_enabled"].as<bool>();
  tcpListenerEnabled = settingsJSON["tcp_listener_enabled"].as<bool>();
  memset(mqttHost, 0, 50);
  strcpy(mqttHost, settingsJSON["mqtt_host"]);
  memset(mqttClientID, 0, 50);
  strcpy(mqttClientID, settingsJSON["mqtt_client_id"]);
  memset(mqttUserName, 0, 50);
  strcpy(mqttUserName, settingsJSON["mqtt_username"]);
  memset(mqttPassword, 0, 50);
  strcpy(mqttPassword, settingsJSON["mqtt_password"]);
  memset(mqttSubscribeTopic, 0, 50);
  strcpy(mqttSubscribeTopic, settingsJSON["mqtt_subscribe_topic"]);
  memset(mqttPublishTopic, 0, 50);
  strcpy(mqttPublishTopic, settingsJSON["mqtt_publish_topic"]);
  mqttPort = settingsJSON["mqtt_port"].as<int>();
  mqttEnabled = settingsJSON["mqtt_enabled"].as<bool>();

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
