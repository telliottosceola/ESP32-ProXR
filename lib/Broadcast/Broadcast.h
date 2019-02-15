#ifndef BROADCAST_H
#define BROADCAST_H
#include <WiFiUdp.h>
#include <WiFi.h>
#include <Settings.h>
class Broadcast{
public:
  void init(Settings &s);
  void loop();
  bool ready = false;
private:
  WiFiUDP udp;
  IPAddress broadcastIP;
  uint16_t broadcastPort = 55555;
  uint8_t broadcastPacket[120];
  unsigned long lastBroadcast;
  unsigned long broadcastInterval = 5000;
};
#endif
