#ifndef TARALIST_H
#define TARALIST_H

#include <time.h>
#include <Arduino.h>
#include <WiFi.h>
#include <Math.h>

class Taralist{
public:
  void init(unsigned long updateInterval, long timeZone, long dstOffset);
  void loop();
  void manualSync();
  void registerTaralistCallback(void(*taralistCallback)(uint8_t*data, int dataLen));

  bool initialized = false;

  long  gmtOffset_sec = -21600;

  uint8_t enterConfigMode[8] = {170, 5, 254, 33, 140, 86, 120, 40};
  uint8_t exitConfigMode[8] = {170, 5, 254, 33, 140, 86, 0, 176};
  uint8_t successConfigResponse[4] = {170, 1, 86, 1};
  uint8_t successRunResponse[4] = {170, 1, 85, 0};

private:
  void convertToBCD(int year, int month, int dMon, int dWeek, int hour, int min, int sec, uint8_t* buff, size_t buffSize);
  void wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer);
  uint8_t convertToBCDByte(int v);
  void (*_taralistCallback)(uint8_t*data, int dataLen);

  unsigned long lastUpdate = 0;
  unsigned long _updateInterval;
  const char* ntpServer = "time.google.com";
};
#endif
