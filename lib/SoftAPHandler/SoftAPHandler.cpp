#include <SoftAPHandler.h>

#define WALLED_GARDEN

AsyncWebServer softAPServer(80);
IPAddress apIP(172, 217, 28, 1);

void SoftAPHandler::init(Settings &s){
  settings = &s;

  WiFi.mode(WIFI_AP);
  delay(200);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.printf("AP SSID:%s\n",settings->apSSID);
  Serial.printf("AP Pass:%s\n",settings->apPass);
  WiFi.softAP(settings->apSSID, settings->apPass, 1, 0, 1);
  #ifdef WALLED_GARDEN
  dnsServer.setTTL(0);
  dnsServer.start(DNS_PORT, "*", apIP);
  #endif

  softAPServer.onNotFound(std::bind(&SoftAPHandler::onRequest, this, std::placeholders::_1));
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
    request->send(200, "text/plain", settings->returnSettings("...","...",false));
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
    if(settings->factoryReset()){
      ESP.restart();
    }
  }
  #ifdef DEBUG
  Serial.println("Sending config");
  #endif
  request->send(200, "text/plain", "Setup Mode");
  return;
}
