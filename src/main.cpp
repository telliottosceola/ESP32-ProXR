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

  rgbLED.init(2,21,13,COMMON_ANODE);
  rgbLED.setMode(rgbLED.MODE_BOOT);
  rgbLED.loop();
  // delay(1000);

  gpioHandler.registerButtonPressCallback(buttonPressCallback);
  gpioHandler.init(settings, rgbLED);

  httpHandler.registerHTTPDataCallback(httpDataCallback);
  httpHandler.registerWSDataCallback(wsDataCallback);

  // taralist.registerTaralistCallback(taralistCallback);

  xTaskCreatePinnedToCore(backgroundTasks, "BackGround Tasks", 20000, NULL, 1, &backgroundTask, 1);

  wifiHandler.init(settings, rgbLED);
  wifiHandler.scanNetworks();

  if(gpioHandler.checkCFGButton()){
    gpioHandler.setupMode = true;
    #ifdef DEBUG
    Serial.println("Setup Mode");
    #endif
    rgbLED.setMode(rgbLED.MODE_SETUP);
    wifiHandler.scanNetworks();
    device.registerDeviceDataCallback(deviceDataCallback);
    device.init(settings.baudRate, 500);
    httpHandler.init(settings, true, wifiHandler);
    setupMode = true;

  }else{
    device.registerDeviceDataCallback(deviceDataCallback);
    device.init(settings.baudRate, 500);
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
        if(settings.tcpClientEnabled){
          tcpClient.registerTCPClientDataCallback(tcpClientDataCallback);
          tcpClient.init(settings);
        }
      }
      httpHandler.init(settings, false, wifiHandler);
      if(settings.mqttEnabled){
        mqtt.registerMQTTDataCallback(mqttDataCallback);
        mqtt.init(settings);
      }
    }
  }
}

void loop() {

  if(previousSetupMode != setupMode){
    previousSetupMode = setupMode;
    #ifdef DEBUG
    Serial.printf("SetupMode: %s\n",setupMode?"True":"False");
    #endif
    if(setupMode){
      if(WiFi.status() == WL_CONNECTED){
        #ifdef DEBUG
        Serial.println("Disconnecting WiFi");
        #endif
        WiFi.disconnect();
        while(WiFi.status() == WL_CONNECTED);
        #ifdef DEBUG
        Serial.println("Disconnected");
        #endif
      }
      rgbLED.setMode(rgbLED.MODE_SETUP);
      delay(1000);
      httpHandler.init(settings, true, wifiHandler);
    }
  }

  if(setupMode){
    httpHandler.loop();
  }else{
    //Run MODE
    device.loop();

    if(settings.bluetoothEnabled){
      bluetooth.loop();
    }

    if(settings.wifiEnabled && strcmp("blank",settings.wlanSSID) != 0 && strcmp("",settings.wlanSSID) != 0){
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
        if(settings.tcpClientEnabled){
          tcpClient.loop();
        }

        if(settings.httpControlEnabled && requestPending){
          if(millis() > requestSendTime+requestTiemout){
            requestPending = false;
            pendingRequest->send(201, "text/plain", "failed to execute command");
          }
        }

        if(tcpServer.clientConnected || bluetooth.deviceConnected || tcpClient.connected){
          if(!setupMode)rgbLED.setMode(rgbLED.MODE_CLIENT_CONNECTED);
        }else{
          if(!setupMode)rgbLED.setMode(rgbLED.MODE_ALL_CLEAR);
        }

        // if(settings.taralistEnabled){
        //   if(!taralistInitialized){
        //     taralist.init(10000, settings.taralistTimeZone, (settings.taralistDST)?3600:0);
        //     taralistInitialized = true;
        //   }else{
        //     taralist.loop();
        //   }
        // }

      }else{
        if(!setupMode)rgbLED.setMode(rgbLED.MODE_WIFI_DISCONNECTED);
      }
    }else{
      rgbLED.setMode(rgbLED.MODE_ALL_CLEAR);
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
  if(settings.wifiEnabled && settings.tcpClientEnabled && tcpClient.ready){
    tcpClient.sendData(data, dataLen);
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
    httpHandler.requestPending = false;
    pendingRequest->send(200, "text/plain", responseData);
  }
  if(settings.httpControlEnabled && httpHandler.hasClient){
    httpHandler.sendToClient(data, dataLen);
  }
  if(settings.mqttEnabled){
    mqtt.mqttPublish(data, dataLen);
  }
}

void tcpDataCallback(uint8_t* data, int dataLen){
  rgbLED.setMode(rgbLED.MODE_DATA_RECEIVED);
  device.write(data, dataLen);
}

void tcpClientDataCallback(uint8_t* data, size_t dataLen){
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
  httpHandler.requestPending = true;
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

// void taralistCallback(uint8_t*data, int dataLen){
//   // Serial.print("Taralist updating with command: ");
//   // for(int i = 0; i < dataLen; i++){
//   //   Serial.printf("%i ", data[i]);
//   // }
//   // Serial.println();
//   uint8_t returnBuffer[4];
//   if(device.write(taralist.enterConfigMode, sizeof(taralist.enterConfigMode), returnBuffer, sizeof(returnBuffer), 50)){
//     if(device.write(data, dataLen, returnBuffer, sizeof(returnBuffer), 50)){
//       device.write(taralist.exitConfigMode, sizeof(taralist.exitConfigMode), returnBuffer, sizeof(returnBuffer), 50);
//     }
//   }
// }

void buttonPressCallback(unsigned long duration){
  if(duration > 50){
    #ifdef DEBUG
    Serial.printf("Button pressed for %ims\n",(int)duration);
    #endif
  }
  if(!setupMode){
    setupMode = true;
    rgbLED.setMode(rgbLED.MODE_SETUP);
    // ESP.restart();
    return;
  }else{
    if(duration <= 4000){
      setupMode = false;
      // httpHandler.stop();
      rgbLED.setMode(rgbLED.MODE_BOOT);
    }else{
      rgbLED.setMode(rgbLED.RANDOM);
      unsigned long start = millis();
      while(millis() < start+2000){
        rgbLED.loop();
      }
      settings.factoryReset();
      // httpHandler.stop();
      setupMode = false;
      ESP.restart();
    }
  }

}

void backgroundTasks(void* pvParameters){
  for(;;){
    rgbLED.loop();
    gpioHandler.loop();
    vTaskDelay(10);
  }
  vTaskDelete( NULL );
}
