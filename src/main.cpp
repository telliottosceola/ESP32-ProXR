#include <Arduino.h>
#include <main.h>

// #define DEBUG

void setup() {
  Serial.begin(115200);

  if(!SPIFFS.begin(true)){
    #ifdef DEBUG
    Serial.println("SPIFFS Mount Failed");
    #endif
  }
  settings.loadSettings();



  rgbLED.init(2,21,13,COMMON_ANODE, false);
  rgbLED.setMode(rgbLED.MODE_BOOT);
  rgbLED.loop();
  delay(1000);
  wifiHandler.init(settings, rgbLED);
  gpioHandler.init(settings, rgbLED);

  if(gpioHandler.checkCFGButton() || strcmp("blank",settings.wlanSSID) == 0 || strcmp("",settings.wlanSSID) == 0){
    gpioHandler.setupMode = true;
    #ifdef DEBUG
    Serial.println("Setup Mode");
    #endif
    rgbLED.setMode(rgbLED.MODE_SETUP);
    wifiHandler.scanNetworks();
    device.registerDeviceDataCallback(deviceDataCallback);
    device.init(settings.baudRate);
    softAPHandler.init(settings);
    setupMode = true;

  }else{
    device.registerDeviceDataCallback(deviceDataCallback);
    device.init(settings.baudRate);
    gpioHandler.setupMode = false;
    if(settings.bluetoothEnabled){
      bluetooth.registerBluetoothDataCallback(bluetoothDataCallback);
      bluetooth.init(settings);
    }
    if(settings.wifiEnabled){
      if(wifiHandler.checkWiFi(setupMode)){
        if(settings.udpBroadcastEnabled){
          broadcast.init(settings);
        }
        if(settings.tcpListenerEnabled){
          tcpServer.registerTCPDataCallback(tcpDataCallback);
          tcpServer.init(settings);
        }
      }
      if(settings.httpControlEnabled){
        httpControl.registerHTTPDataCallback(httpDataCallback);
        httpControl.registerWSDataCallback(wsDataCallback);
        httpControl.init(settings);
      }
      if(settings.mqttEnabled){
        mqtt.registerMQTTDataCallback(mqttDataCallback);
        mqtt.init(settings);
      }
    }
  }
}

void loop() {
  gpioHandler.loop();
  rgbLED.loop();

  if(setupMode){
    softAPHandler.loop();
  }else{
    //Run MODE
    device.loop();

    if(settings.bluetoothEnabled){
      bluetooth.loop();
    }

    if(settings.wifiEnabled){
      if(wifiHandler.checkWiFi(setupMode)){
        //WiFi is connected
        if(settings.udpBroadcastEnabled && !broadcast.ready){
          broadcast.init(settings);
        }
        broadcast.loop();
        if(settings.tcpListenerEnabled && !tcpServer.ready){
          tcpServer.init(settings);
        }
        if(settings.tcpListenerEnabled){
          tcpServer.loop();
        }
        if(settings.mqttEnabled){
          mqtt.loop();
        }

        if(settings.httpControlEnabled && requestPending){
          if(millis() > requestSendTime+requestTiemout){
            requestPending = false;
            pendingRequest->send(201, "text/plain", "failed to execute command");
          }
        }

        if(tcpServer.clientConnected || bluetooth.deviceConnected){
          rgbLED.setMode(rgbLED.MODE_CLIENT_CONNECTED);
        }else{
          rgbLED.setMode(rgbLED.MODE_ALL_CLEAR);
        }

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
  if(settings.wifiEnabled && settings.tcpListenerEnabled && tcpServer.ready && tcpServer.clientConnected){
    tcpServer.sendData(data, dataLen);
  }
  if(settings.bluetoothEnabled && bluetooth.deviceConnected){
    bluetooth.sendData(data, dataLen);
  }
  if(settings.httpControlEnabled && requestPending){
    char responseData[dataLen*4];
    memset(responseData, 0, sizeof(responseData));
    sprintf(responseData, "%i ", data[0]);
    for(int i = 1; i < dataLen; i++){
      sprintf(responseData, "%s%i ", responseData, data[i]);
    }
    requestPending = false;
    httpControl.requestPending = false;
    pendingRequest->send(200, "text/plain", responseData);
  }
  if(settings.httpControlEnabled && httpControl.hasClient){
    httpControl.sendToClient(data, dataLen);
  }
  if(settings.mqttEnabled){
    mqtt.mqttPublish(data, dataLen);
  }
}

void tcpDataCallback(uint8_t* data, int dataLen){
  rgbLED.setMode(rgbLED.MODE_DATA_RECEIVED);
  device.write(data, dataLen);
}

void bluetoothDataCallback(uint8_t* data, int dataLen){
  rgbLED.setMode(rgbLED.MODE_DATA_RECEIVED);
  device.write(data, dataLen);
}

void httpDataCallback(uint8_t* data, int dataLen, AsyncWebServerRequest *request){
  rgbLED.setMode(rgbLED.MODE_DATA_RECEIVED);
  requestPending = true;
  httpControl.requestPending = true;
  pendingRequest = request;
  device.write(data, dataLen);
  requestSendTime = millis();
}
void wsDataCallback(uint8_t* data, int dataLen){
  rgbLED.setMode(rgbLED.MODE_DATA_RECEIVED);
  device.write(data, dataLen);
}

void mqttDataCallback(uint8_t* data, int dataLen){
  rgbLED.setMode(rgbLED.MODE_DATA_RECEIVED);
  device.write(data, dataLen);
}
