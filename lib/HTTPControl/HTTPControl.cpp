#include <Arduino.h>
#include <HTTPControl.h>

AsyncWebServer controlServer(80);

void HTTPControl::init(){
  controlServer.on("/sendCommand", std::bind(&HTTPControl::onRequest, this, std::placeholders::_1));
  controlServer.begin();
}

void HTTPControl::onRequest(AsyncWebServerRequest *request){
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
        _httpDataCallback(commandBytes, sizeof(commandBytes), request);
      }

    }
  }
}

void HTTPControl::registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request)){
  _httpDataCallback = HTTPDataCallback;
}
