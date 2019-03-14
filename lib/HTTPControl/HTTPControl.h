#ifndef HTTPCONTROL_H
#define HTTPCONTROL_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Settings.h>

class HTTPControl{
public:
  void init(Settings &s);
  void onRequest(AsyncWebServerRequest *request);
  void registerHTTPDataCallback(void(*HTTPDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request));
  bool requestPending = false;
private:

  void (*_httpDataCallback)(uint8_t*data, int dataLen, AsyncWebServerRequest *request);
  Settings *settings;
  void wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer);
};
#endif
