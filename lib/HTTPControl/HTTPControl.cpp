#include <Arduino.h>
#include <HTTPControl.h>

#define DEBUG

AsyncWebServer controlServer(80);

void HTTPControl::init(Settings &s){
  settings = &s;
  controlServer.onNotFound(std::bind(&HTTPControl::onRequest, this, std::placeholders::_1));
  controlServer.begin();
}

void HTTPControl::onRequest(AsyncWebServerRequest *request){
  #ifdef DEBUG
  Serial.println("New request to HTTPControl");
  Serial.printf("params: %i\n",request->params());
  Serial.printf("args: %i\n",request->args());
  Serial.println(request->url());
  #endif
  if(requestPending){
    return;
  }
  if(request->url() == "/sendCommand"){
    if(request->hasArg("data")){
      String arg = request->arg("data");
      DynamicJsonBuffer jsonBuffer;
      JsonArray& commandDataArray = jsonBuffer.parseArray(arg);
      if(commandDataArray.success()){
        uint8_t commandBytes[commandDataArray.size()];
        int index = 0;
        for(int value : commandDataArray){
          commandBytes[index] = commandDataArray[index];
          index++;
        }
        if(commandBytes[0] == 254){
          uint8_t apiPacket[sizeof(commandBytes)+3];
          wrapAPI(commandBytes, sizeof(commandBytes), apiPacket);
          _httpDataCallback(apiPacket, sizeof(apiPacket), request);
        }else{
          _httpDataCallback(commandBytes, sizeof(commandBytes), request);
        }
      }
    }
    return;
  }
  if (request->url() == "/relayCount"){
    request->send(200, "text/plain", String(settings->relayCount));
    return;
  }
  if (request->url() == "/relayON"){
    if(request->hasArg("relay")){
      String relayString = request->arg("relay");
      int relayNumber = relayString.toInt();
      relayNumber +=1;
      Serial.printf("Turn on relay %i\n", relayNumber);
      uint8_t command[3];
      command[0] = 254;
      uint8_t bank = 1;
      uint8_t relayCommand = 107;
      if(relayNumber < 9){
        relayCommand = relayCommand+relayNumber;
      }else{
        if(relayNumber > 8 && relayNumber < 17){
          bank = 2;
          relayCommand = relayCommand+(relayNumber - 8);
        }else{
          if(relayNumber > 16 && relayNumber < 25){
            bank = 3;
            relayCommand = relayCommand+(relayNumber - 16);
          }else{
            if(relayNumber > 24 && relayNumber < 33){
              bank = 4;
              relayCommand = relayCommand+(relayNumber - 24);
            }
          }
        }
      }
      command[1] = relayCommand;
      command[2] = bank;
      uint8_t apiPacket[6];
      wrapAPI(command, 3, apiPacket);
      _httpDataCallback(apiPacket, sizeof(apiPacket), request);
    }

    return;
  }
  if (request->url() == "/relayOFF"){
    if(request->hasArg("relay")){
      String relayString = request->arg("relay");
      int relayNumber = relayString.toInt();
      relayNumber +=1;
      Serial.printf("Turn off relay %i\n", relayNumber);
      uint8_t command[3];
      command[0] = 254;
      uint8_t bank = 1;
      uint8_t relayCommand = 99;
      if(relayNumber < 9){
        relayCommand = relayCommand+relayNumber;
      }else{
        if(relayNumber > 8 && relayNumber < 17){
          bank = 2;
          relayCommand = relayCommand+(relayNumber - 8);
        }else{
          if(relayNumber > 16 && relayNumber < 25){
            bank = 3;
            relayCommand = relayCommand+(relayNumber - 16);
          }else{
            if(relayNumber > 24 && relayNumber < 33){
              bank = 4;
              relayCommand = relayCommand+(relayNumber - 24);
            }
          }
        }
      }
      command[1] = relayCommand;
      command[2] = bank;
      uint8_t apiPacket[6];
      wrapAPI(command, 3, apiPacket);
      _httpDataCallback(apiPacket, sizeof(apiPacket), request);
    }

    return;
  }
  request->send(SPIFFS, "/Control.html");
}

void HTTPControl::registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request)){
  _httpDataCallback = HTTPDataCallback;
}

void HTTPControl::wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer){
  uint8_t packet[dataLen+3];
  packet[0] = 170;
  packet[1] = dataLen;
  int cs = packet[0]+packet[1];
  for(size_t i = 0; i < dataLen; i++){
    packet[i+2] = data[i];
    cs+=data[i];
  }
  uint8_t csByte = cs&255;
  packet[sizeof(packet)-1] = csByte;
  memcpy(buffer, packet, sizeof(packet));
  return;
}
