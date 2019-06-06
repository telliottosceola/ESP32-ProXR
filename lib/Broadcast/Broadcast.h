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
  Settings* settings;
  WiFiUDP udp;
  IPAddress broadcastIP;
  uint16_t broadcastPort = 55555;
  uint16_t ethernetBroadcastPort = 13000;
  uint8_t broadcastPacket[120];
  char ethernetBroadcastPacket[60];
  char ethernetBroadcastFormat[58] = "%i.%i.%i.%i,%02X%02X%02X%02X%02X%02X,%i,MegaModem,V1.0.0";
  unsigned long lastBroadcast;
  unsigned long broadcastInterval = 5000;
};
#endif
