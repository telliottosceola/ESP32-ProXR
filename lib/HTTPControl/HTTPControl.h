#ifndef HTTPCONTROL_H
#define HTTPCONTROL_H

// #include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
// #include <AsyncWebSocket.h>
// #include <AsyncEventSource.h>
#include <ArduinoJson.h>
#include <Settings.h>

class HTTPControl{
public:
  void init(Settings &s);
  void onRequest(AsyncWebServerRequest *request);
  void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
  void registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request));
  void registerWSDataCallback(void(*WSDataCallback)(uint8_t*data, int dataLen));
  bool requestPending = false;
  bool hasClient = false;
  void sendToClient(uint8_t* data, int dataLen);
private:
  AsyncWebSocketClient * connectedClient;
  AsyncWebSocket * wsServer;

  void (*_httpDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request);
  void (*_WSDataCallback)(uint8_t*data, int dataLen);
  Settings *settings;
  void wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer);
};
#endif
