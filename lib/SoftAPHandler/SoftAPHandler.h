#ifndef SOFTAPHANDLER_H
#define SOFTAPHANDLER_H

#include <Settings.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <WiFi.h>

class SoftAPHandler{
public:
  void init(Settings &s);
  void loop();
private:
  const byte DNS_PORT = 53;
  DNSServer dnsServer;

  Settings *settings;
  void onRequest(AsyncWebServerRequest *request);
  void configUpdate(AsyncWebServerRequest *request);
};
#endif
