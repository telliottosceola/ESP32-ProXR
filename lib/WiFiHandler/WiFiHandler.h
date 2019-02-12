#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H
#include <WiFi.h>
#include <ArduinoJson.h>
class WifiHandler{
public:
  String scanNetworks(char* wlanSSID);
private:

};
#endif
