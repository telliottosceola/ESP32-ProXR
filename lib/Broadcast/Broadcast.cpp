#include <Broadcast.h>

// #define DEBUG;

void Broadcast::init(Settings &s){
  Settings *settings = &s;
  IPAddress moduleIP = WiFi.localIP();
  broadcastIP = WiFi.gatewayIP();
  #ifdef DEBUG
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  #endif
  broadcastIP[3] = 255;
  uint8_t routerMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t macBytes[6];
  WiFi.macAddress(macBytes);
  // uint8_t macStart[3] = {0x00, 0x06, 0x66};
  // memcpy(macBytes, macStart, 3);
  memcpy(broadcastPacket, routerMac, 6);
  broadcastPacket[6] = (uint8_t)WiFi.channel();
  broadcastPacket[7] = WiFi.RSSI();
  broadcastPacket[8] = (uint8_t)((settings->tcpListenPort >>8)&0xFF);
  broadcastPacket[9] = (uint8_t)(settings->tcpListenPort&0xFF);
  uint8_t restOfDataOne[47];
  memset(restOfDataOne, 0, 47);
  memcpy(broadcastPacket+9, restOfDataOne, 47);
  uint8_t deviceIDBytes[32];
  memcpy(deviceIDBytes, settings->deviceID, 32);
  memcpy(broadcastPacket+60, deviceIDBytes, 32);
  uint8_t restOfDataTwo[18];
  memset(restOfDataTwo, 0, 18);
  memcpy(broadcastPacket+91, restOfDataTwo, 18);
  memcpy(broadcastPacket+110, macBytes, 6);
  uint8_t ipBuffer[4] = {moduleIP[0],moduleIP[1],moduleIP[2],moduleIP[3]};
  memcpy(broadcastPacket+116, ipBuffer, 4);
  ready = true;
}

void Broadcast::loop(){
  if(WiFi.isConnected()){
    if(millis() > lastBroadcast+broadcastInterval){
      lastBroadcast = millis();
      udp.beginPacket(broadcastIP, broadcastPort);
      udp.write(broadcastPacket, 120);
      udp.endPacket();
      #ifdef DEBUG
      Serial.print("UDP broadcast sent to IP:");
      Serial.print(broadcastIP);
      Serial.printf(" on port:%i\n",broadcastPort);
      #endif
    }
  }
}
