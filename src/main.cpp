#include <Arduino.h>
#include <main.h>

// #define DEBUG
#define WALLED_GARDEN
#define TEST
int transmissionCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(button,INPUT_PULLUP);

  if(!SPIFFS.begin(true)){
    #ifdef DEBUG
    Serial.println("SPIFFS Mount Failed");
    #endif
  }
  settings.loadSettings();

  rgbLED.init(2,15,13,COMMON_ANODE, false);
  rgbLED.setMode(rgbLED.MODE_BOOT);
  rgbLED.loop();
  wifiHandler.init(settings, rgbLED);
  device.registerDeviceDataCallback(deviceDataCallback);
  device.init(settings.baudRate);

  if(digitalRead(button) == 0 || strcmp("blank",settings.wlanSSID) == 0 || strcmp("",settings.wlanSSID) == 0){
    #ifdef DEBUG
    Serial.println("Setup Mode");

    #endif
    rgbLED.setMode(rgbLED.MODE_SETUP);
    WiFi.mode(WIFI_AP);
    delay(200);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    Serial.printf("AP SSID:%s\n",settings.apSSID);
    Serial.printf("AP Pass:%s\n",settings.apPass);
    WiFi.softAP(settings.apSSID, settings.apPass, 1, 0, 1);
    #ifdef WALLED_GARDEN
    dnsServer.setTTL(0);
    dnsServer.start(DNS_PORT, "*", apIP);
    #endif
    setupMode = true;

  }else{
    // device.loop();
    if(settings.wifiEnabled){
      if(wifiHandler.checkWiFi(setupMode)){
        broadcast.init(settings);
        tcpServer.registerTCPDataCallback(tcpDataCallback);
        tcpServer.init(settings);
      }
    }
  }
}

void loop() {
  checkButton();
  rgbLED.loop();

  if(setupMode){
    if(boot){
      server.onNotFound(onRequest);
      server.begin();
      boot = false;
    }
    #ifdef WALLED_GARDEN
    dnsServer.processNextRequest();
    #endif
  }else{
    device.loop();
    //Run MODE
    if(settings.wifiEnabled){
      if(wifiHandler.checkWiFi(setupMode)){
        if(!broadcast.ready){
          broadcast.init(settings);
        }
        broadcast.loop();
        if(!tcpServer.ready){
          tcpServer.init(settings);
        }
        tcpServer.loop();
        rgbLED.setMode(rgbLED.MODE_ALL_CLEAR);
      }else{
        rgbLED.setMode(rgbLED.MODE_WIFI_DISCONNECTED);
      }
    }
  }
}

void deviceDataCallback(uint8_t* data, int dataLen){
  #ifdef DEBUG
  Serial.print("Received from Device: ");
  for(int i = 0; i < dataLen; i++){
    Serial.printf("%i ", data[i]);
  }
  Serial.println();
  #endif
  if(Serial.availableForWrite()){
    Serial.write(data, dataLen);
  }
  if(tcpServer.ready && tcpServer.clientConnected){
    tcpServer.sendData(data, dataLen);
  }
}

void tcpDataCallback(uint8_t* data, int dataLen){
  device.write(data, dataLen);
}

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  #ifdef DEBUG
  Serial.printf("params: %i\n",request->params());
  Serial.println(request->url());
  Serial.println("onRequest fired");
  #endif
  if(request->url() == "/generate_204" || request->url() == "/gen_204"){
    //request->send(204,"");
    // return;
  }
  if(request->url() == "/favicon.ico"){
    request->send(400,"");
    return;
  }
  if(request->url() == "/loadSettings"){
    request->send(200, "text/plain", settings.returnSettings("...","...",false));
    return;
  }
  if(request->url() == "/update"){
    #ifdef DEBUG
    Serial.println("handling /update request");
    #endif
    return;
  }
  if(request->url() == "/factoryReset"){
    request->send(200, "text/plain", "Resetting all settings, gateway will power cycle and will now be in setup mode");
    factoryReset();
  }
  if(setupMode){
    #ifdef DEBUG
    Serial.println("Sending config");
    #endif
    request->send(200, "text/plain", "Setup Mode");
    return;
  }else{
    request->send(200, "text/plain", "Run Mode");
  }

}

bool checkWiFi(){
  if(strcmp(settings.wlanSSID, "blank") == 0 || strcmp(settings.wlanPASS, "blank") == 0){
    return false;
  }
  if(WiFi.isConnected()){
    char currentSSID[WiFi.SSID().length()+1];
    WiFi.SSID().toCharArray(currentSSID, sizeof(currentSSID));
    if(strcmp(currentSSID, settings.wlanSSID) != 0){
      WiFi.disconnect();
    }
  }
  if(!WiFi.isConnected()){
    if(!settings.dhcpEnabled){
      WiFi.config(settings.staticIP, settings.defaultGateway, settings.subnetMask, settings.primaryDNS, settings.secondaryDNS);
    }

    unsigned long wifiConnectTimeout = 10000;
    unsigned long startConnectTime = millis();
    WiFi.begin(settings.wlanSSID, settings.wlanPASS);
    while(WiFi.status() != WL_CONNECTED && millis() < startConnectTime+wifiConnectTimeout){
      checkButton();
      rgbLED.loop();
    }
    if(WiFi.status() != WL_CONNECTED){
      rgbLED.setMode(rgbLED.MODE_WIFI_DISCONNECTED);
      #ifdef DEBUG
      Serial.println("WiFi reconnect failed");
      #endif
      return false;
    }else{
      rgbLED.setMode(rgbLED.MODE_WIFI_CONNECTED);
      macAddress = WiFi.macAddress();
      memset(macBytes, 0, 18);
      macAddress.toCharArray(macBytes, 18);
      moduleIP = WiFi.localIP();
      sprintf(moduleIPString, "%i.%i.%i.%i", moduleIP[0], moduleIP[1], moduleIP[2], moduleIP[3]);
      #ifdef DEBUG
      Serial.printf("WiFi connected to %s\n", settings.wlanSSID);
      Serial.printf("IPAddress: %s\n", moduleIPString);
      #endif
      return true;
    }

  }else{
    return true;
  }
  return false;
}

void factoryReset(){
  if(settings.factoryReset()){
    ESP.restart();
  }
}

void checkButton(){
  if(digitalRead(button) == 0){
    if(!setupMode){
      if(!buttonPressed){
        buttonPressed = true;
        buttonPressTime = millis();
      }else{
        if(millis() > buttonPressTime + setupButtonTimeout){
          rgbLED.setMode(rgbLED.MODE_SETUP);
          rgbLED.loop();
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
          while(digitalRead(button) == 0){
            rgbLED.writeRandom();
            delay(100);
          }
          factoryReset();
        }
      }
    }

  }else{
    buttonPressed = false;
    factoryResetEnable = true;
  }
}
