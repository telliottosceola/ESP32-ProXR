#ifndef HTML_HANDLER_H
#define HTML_HANDLER_H
#include <Settings.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <WiFiHandler.h>
#include <Math.h>
class HTMLHandler{
public:
  void init(Settings &s, bool sMode, WiFiHandler &wHandler);
  void loop();

  void registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request));
  void registerWSDataCallback(void(*WSDataCallback)(uint8_t*data, int dataLen));

  bool requestPending = false;
  bool hasClient = false;
  void sendToClient(uint8_t* data, int dataLen);

private:
  bool setupMode;
  unsigned long commandReceiveTime = millis();
  const byte DNS_PORT = 53;
  DNSServer dnsServer;
  bool timeRequestPending = false;

  Settings *settings;
  WiFiHandler *wifiHandler;
  void onRequest(AsyncWebServerRequest *request);
  void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
  void configUpdate(AsyncWebServerRequest *request);

  AsyncWebSocketClient * connectedClient;
  AsyncWebSocket * wsServer;

  void (*_httpDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request);
  void (*_WSDataCallback)(uint8_t*data, int dataLen);
  void wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer);
};
#endif
