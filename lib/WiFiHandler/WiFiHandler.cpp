#include <WiFiHandler.h>

// #define DEBUG

void WiFiHandler::init(Settings &s, RGBLED &r){
  settings = &s;
  rgbLED = &r;
}

bool WiFiHandler::checkWiFi(bool setupMode){
  if(strcmp(settings->wlanSSID, "blank") == 0 || strlen(settings->wlanSSID) == 0){
    return false;
  }
  if(WiFi.isConnected()){
    char currentSSID[WiFi.SSID().length()+1];
    WiFi.SSID().toCharArray(currentSSID, sizeof(currentSSID));
    if(strcmp(currentSSID, settings->wlanSSID) != 0){
      WiFi.disconnect();
    }
  }
  if(!WiFi.isConnected()){
    if(!settings->dhcpEnabled){
      WiFi.config(settings->staticIP, settings->defaultGateway, settings->subnetMask, settings->primaryDNS, settings->secondaryDNS);
    }
    unsigned long wifiConnectTimeout = 10000;
    unsigned long startConnectTime = millis();
    if(strcmp(settings->wlanPASS, "blank") == 0 || strlen(settings->wlanPASS) == 0){
      WiFi.begin(settings->wlanSSID);
    }else{
      if(strcmp(settings->wpaEnterpriseUsername, "")!=0){
        WiFi.disconnect(true);
        esp_wifi_sta_wpa2_ent_set_username((uint8_t *)settings->wpaEnterpriseUsername, strlen(settings->wpaEnterpriseUsername));
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *)settings->wlanPASS, strlen(settings->wlanPASS));
        if(strcmp(settings->wpaEnterpriseIdentity, "")!=0){
          esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)settings->wpaEnterpriseIdentity, strlen(settings->wpaEnterpriseIdentity));
        }
        esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT();
        esp_wifi_sta_wpa2_ent_enable(&config);
        // esp_wifi_sta_wpa2_ent_enable();
        WiFi.begin(settings->wlanSSID);
      }else{
        WiFi.begin(settings->wlanSSID, settings->wlanPASS);
      }
    }

    while(WiFi.status() != WL_CONNECTED && millis() < startConnectTime+wifiConnectTimeout){
      if(digitalRead(button) == 0){
        if(!setupMode){
          if(!buttonPressed){
            buttonPressed = true;
            buttonPressTime = millis();
          }else{
            if(millis() > buttonPressTime + setupButtonTimeout){
              rgbLED->setMode(rgbLED->MODE_SETUP);
              rgbLED->loop();
              ESP.restart();
            }
          }
        }else{
          //In setup mode so holding the button here means they want a factory reset
          if(!buttonPressed){
            buttonPressed = true;
            buttonPressTime = millis();
          }else{
            if((millis() > buttonPressTime + factoryResetTimeout) && factoryResetEnable){
              unsigned long flashDuration = 100;
              unsigned long lastFlash;
              while(digitalRead(button) == 0){
                if(millis()>lastFlash+flashDuration){
                  rgbLED->writeRandom();
                  lastFlash = millis();
                }
              }
              settings->factoryReset();
              ESP.restart();
            }
          }
        }

      }else{
        buttonPressed = false;
        factoryResetEnable = true;
      }
      rgbLED->loop();
    }
    if(WiFi.status() != WL_CONNECTED){
      #ifdef DEBUG
      Serial.println("WiFi reconnect failed");
      #endif
      return false;
    }else{
      macAddress = WiFi.macAddress();
      memset(macBytes, 0, 18);
      macAddress.toCharArray(macBytes, 18);
      moduleIP = WiFi.localIP();
      sprintf(moduleIPString, "%i.%i.%i.%i", moduleIP[0], moduleIP[1], moduleIP[2], moduleIP[3]);
      #ifdef DEBUG
      Serial.printf("WiFi connected to %s\n", settings->wlanSSID);
      Serial.printf("IPAddress: %s\n", moduleIPString);
      delay(50);
      #endif
      return true;
    }

  }else{
    return true;
  }
  return false;
}

void WiFiHandler::scanNetworks(){
  #ifdef DEBUG
  Serial.println();
  Serial.println("Scanning for networks");
  #endif
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  DynamicJsonBuffer jsonBuffer;
  JsonArray& foundNetworks = jsonBuffer.createArray();

  int8_t n = -3;
  unsigned long startTime = millis();
  unsigned long wifiScanTimeout = 10000;
  while(n < 1 && millis() < startTime+wifiScanTimeout){
    n = WiFi.scanComplete();
    rgbLED->loop();
    if(n == -2){
      WiFi.scanNetworks(true);
    }
  }
  if(n < 1){
    #ifdef DEBUG
    Serial.println("No Networks Found");
    #endif
    delay(500);
  }else{
    for(int i = 0; i < n; i++){
      JsonObject& network = foundNetworks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      network["encryption_type"] = WiFi.encryptionType(i);
      uint8_t apMac[6];
      memcpy(apMac, WiFi.BSSID(i), 6);
      char apMacChar[18];
      char* macFormat = "%02X:%02X:%02X:%02X:%02X:%02X";
      sprintf(apMacChar, macFormat, apMac[0], apMac[1], apMac[2], apMac[3], apMac[4], apMac[5]);
      network["bssid"] = apMacChar;
      network["channel"] = WiFi.channel(i);
      if(settings->wlanSSID == network["ssid"]){
        network["selected"] = true;
      }
    }
  }
  foundNetworks.printTo(settings->WiFiNetworks);
  #ifdef DEBUG
  Serial.print("Scan Networks found:");
  foundNetworks.printTo(Serial);
  Serial.println();
  #endif
  delay(100);
  checkWiFi(true);
}
