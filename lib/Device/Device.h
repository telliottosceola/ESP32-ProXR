#ifndef DEVICE_H
#define DEVICE_H
#include <HardwareSerial.h>

class Device{
public:
  void init(int baudRate, unsigned long cTimeout);
  void loop();
  void write(uint8_t* data, int dataLen);
  void registerDeviceDataCallback(void(*deviceDataCallback)(uint8_t*data, int dataLen));
private:
  void (*_deviceDataCallback)(uint8_t*data, int dataLen);
  unsigned long commandTimeout = 500;
  bool validatePacket(uint8_t* data, size_t len);
};
#endif
