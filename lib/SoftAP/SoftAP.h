#ifndef SOFTAP_H
#def SOFTAP_H
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
class SoftAP{
public:
  void init();
  void loop();

private:
  //Soft AP Stuff
  AsyncWebServer server(80);
  AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
  AsyncEventSource events("/events"); // event source (Server-Sent events)
  const byte DNS_PORT = 53;
  DNSServer dnsServer;
};
#endif
