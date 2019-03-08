#include <SoftAPHandler.h>

#define WALLED_GARDEN

// #define DEBUG

AsyncWebServer softAPServer(80);
IPAddress apIP(172, 217, 28, 1);

void SoftAPHandler::init(Settings &s){
  settings = &s;

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

  softAPServer.onNotFound(std::bind(&SoftAPHandler::onRequest, this, std::placeholders::_1));
  softAPServer.on("/update", std::bind(&SoftAPHandler::configUpdate, this, std::placeholders::_1));
  softAPServer.begin();
}

void SoftAPHandler::loop(){
  dnsServer.processNextRequest();
}

void SoftAPHandler::onRequest(AsyncWebServerRequest *request){
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
  }
  #ifdef DEBUG
  Serial.println("Sending config");
  #endif
  request->send(SPIFFS, "/Config.html");
  return;
}

void SoftAPHandler::configUpdate(AsyncWebServerRequest *request){
  #ifdef DEBUG
  Serial.println("Running configUpdate");
  Serial.printf("Args: %i\n",request->args());
  Serial.printf("Params: %i\n",request->params());
  #endif

  if(request->args() > 0 && request->hasArg("body")){
    Serial.print("arg(0)");
    uint8_t zero = 0;
    String requestString = request->arg(zero);
    Serial.println(requestString);
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
