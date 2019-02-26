#ifndef HTTPCONTROL_H
#define HTTPCONTROL_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class HTTPControl{
public:
  void init();
  void onRequest(AsyncWebServerRequest *request);
  void registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request));
  bool requestPending = false;
private:

  void (*_httpDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request);
};
#endif
