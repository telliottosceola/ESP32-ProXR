#include <HTML_Handler.h>

#define WALLED_GARDEN
// #define DEBUG

AsyncWebServer controlServer(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
IPAddress apIP(172, 217, 28, 1);

void HTMLHandler::init(Settings &s, bool sMode, WiFiHandler &wHandler){
  settings = &s;
  setupMode = sMode;
  wifiHandler = &wHandler;

  if(setupMode){
    WiFi.mode(WIFI_AP);
    delay(200);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    #ifdef DEBUG
    Serial.printf("AP SSID:%s\n",settings->apSSID);
    Serial.printf("AP Pass:%s\n",settings->apPass);
    #endif
    WiFi.softAP(settings->apSSID, settings->apPass, 1, 0, 1);
    #ifdef WALLED_GARDEN
    dnsServer.setTTL(0);
    dnsServer.start(DNS_PORT, "*", apIP);
    #endif
  }

  ws.onEvent(std::bind(&HTMLHandler::onEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
  controlServer.addHandler(&ws);
  controlServer.addHandler(&events);
  controlServer.on("/update", std::bind(&HTMLHandler::configUpdate, this, std::placeholders::_1));
  controlServer.onNotFound(std::bind(&HTMLHandler::onRequest, this, std::placeholders::_1));
  controlServer.begin();
  #ifdef DEBUG
  Serial.println("HTMLHandler initialized");
  #endif
}

void HTMLHandler::loop(){
  if(setupMode){
    dnsServer.processNextRequest();
  }
}

void HTMLHandler::onRequest(AsyncWebServerRequest *request){
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

  if(request->url() == "/loadSettings"){
    if(!setupMode){
      // wifiHandler->scanNetworks();
    }
    #ifdef DEBUG
    Serial.println("/loadSettings request");
    #endif
    request->send(200, "text/plain", settings->returnSettings("...","...",false));
    return;
  }

  if(request->url() == "/factoryReset"){
    request->send(200, "text/plain", "Resetting all settings, gateway will power cycle and will now be in setup mode");
    if(settings->factoryReset()){
      ESP.restart();
    }
    return;
  }

  if(request->url() == "/update"){
    if(request->args() > 0 && request->hasArg("body")){
      uint8_t zero = 0;
      String requestString = request->arg(zero);
      DynamicJsonBuffer requestjsonBuffer;
      JsonObject& requestRoot = requestjsonBuffer.parseObject(requestString);
      requestRoot["wifi_enabled"] = requestRoot.containsKey("wifi_enabled");
      requestRoot["dhcp_enabled"] = requestRoot.containsKey("dhcp_enabled");
      requestRoot["udp_broadcast_enabled"] = requestRoot.containsKey("udp_broadcast_enabled");
      requestRoot["bluetooth_enabled"] = requestRoot.containsKey("bluetooth_enabled");
      requestRoot["tcp_listener_enabled"] = requestRoot.containsKey("tcp_listener_enabled");
      requestRoot["http_enabled"] = requestRoot.containsKey("http_enabled");
      requestRoot["mqtt_enabled"] = requestRoot.containsKey("mqtt_enabled");
      requestRoot["udp_remote_enabled"] = requestRoot.containsKey("udp_remote_enabled");
      String finalFinal;
      requestRoot.printTo(finalFinal);
      if(!settings->storeSettings(finalFinal)){
        #ifdef DEBUG
        Serial.println("Failed to store settings");
        #endif
        request->send(201, "text/plain","Settings update failed");
        return;
      }else{
        request->send(200, "text/plain","Settings updated");
        delay(500);
        ESP.restart();
      }
    }
    return;;
  }

  if(request->url() == "/Config"){
    request->send(SPIFFS, "/Config.html");
    return;
  }

  request->send(SPIFFS, "/Config.html");
}

void HTMLHandler::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    hasClient = true;
    connectedClient = client;
    wsServer = server;
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    hasClient = true;
  } else if(type == WS_EVT_DATA){
    commandReceiveTime = millis();
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      if(info->opcode == WS_TEXT){
        data[len] = 0;
        DynamicJsonBuffer jsonBuffer;
        JsonArray& commandDataArray = jsonBuffer.parseArray((char*)data);
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
            _WSDataCallback(apiPacket, sizeof(apiPacket));
          }else{
            _WSDataCallback(commandBytes, sizeof(commandBytes));
          }
        }
      } else {
      }
    }
  }
}

void HTMLHandler::sendToClient(uint8_t* data, int dataLen){
  if(!hasClient){
    return;
  }
  if(connectedClient->status() != WS_CONNECTED){
    return;
  }

  DynamicJsonBuffer jsonBuffer;
  JsonArray& dataArray = jsonBuffer.createArray();
  for(int i = 0; i < dataLen; i++){
    dataArray.add(data[i]);
  }
  char clientString[dataArray.measureLength()+1];
  dataArray.printTo(clientString, sizeof(clientString));
  connectedClient->text(clientString, sizeof(clientString));
}

void HTMLHandler::registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request)){
  _httpDataCallback = HTTPDataCallback;
}

void HTMLHandler::registerWSDataCallback(void(*WSDataCallback)(uint8_t*data, int dataLen)){
  _WSDataCallback = WSDataCallback;
}

void HTMLHandler::wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer){
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

void HTMLHandler::configUpdate(AsyncWebServerRequest *request){
  #ifdef DEBUG
  Serial.println("Running configUpdate");
  Serial.printf("Args: %i\n",request->args());
  Serial.printf("Params: %i\n",request->params());
  #endif

  if(request->args() > 0 && request->hasArg("body")){
    #ifdef DEBUG
    Serial.print("arg(0)");
    #endif
    uint8_t zero = 0;
    String requestString = request->arg(zero);
    #ifdef DEBUG
    Serial.println(requestString);
    #endif
    delay(500);
    DynamicJsonBuffer requestjsonBuffer;
    JsonObject& requestRoot = requestjsonBuffer.parseObject(requestString);
    requestRoot["wifi_enabled"] = requestRoot.containsKey("wifi_enabled");
    requestRoot["dhcp_enabled"] = requestRoot.containsKey("dhcp_enabled");
    requestRoot["udp_broadcast_enabled"] = requestRoot.containsKey("udp_broadcast_enabled");
    requestRoot["bluetooth_enabled"] = requestRoot.containsKey("bluetooth_enabled");
    requestRoot["tcp_listener_enabled"] = requestRoot.containsKey("tcp_listener_enabled");
    requestRoot["http_enabled"] = requestRoot.containsKey("http_enabled");
    requestRoot["mqtt_enabled"] = requestRoot.containsKey("mqtt_enabled");

    String finalFinal;
    requestRoot.printTo(finalFinal);

    request->send(200, "text/plain", "Settings updated");
    #ifdef DEBUG
    Serial.println("Updating settings");
    #endif
    delay(500);
    if(settings->storeSettings(finalFinal)){
      ESP.restart();
    }
  }
  else{
    request->send(200, "text/plain", "Well that's no good.");
  }
}
