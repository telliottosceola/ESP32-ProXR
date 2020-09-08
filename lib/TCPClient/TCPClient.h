#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <Settings.h>
#include <WiFi.h>

class TCPClient{
public:
  bool ready = false;
  bool connected = false;
  void init(Settings &s);
  void loop();
  void sendData(uint8_t* data, size_t dataLen);
  void registerTCPClientDataCallback(void(*tcpClientDataCallback)(uint8_t* data, size_t dataLen));
private:
  Settings *settings;
  void (*_tcpClientDataCallback)(uint8_t* data, size_t dataLen);
  uint8_t previousStatus = 0;
};


#endif
