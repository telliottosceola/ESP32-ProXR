#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <Settings.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
class TCPServer{
public:
  bool ready = false;
  bool clientConnected = false;
  void init(Settings &s);
  void loop();
  void registerTCPDataCallback(void(*tcpDataCallback)(uint8_t*data, int dataLen));
  void sendData(uint8_t* data, int dataLen);
private:
  Settings *settings;
  void (*_tcpDataCallback)(uint8_t*data, int dataLen);
  WiFiServer server;
  WiFiClient client;
  uint8_t previousClientStatus = 0;
};
#endif
