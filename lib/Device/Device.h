#ifndef DEVICE_H
#define DEVICE_H
#include <HardwareSerial.h>

class Device{
public:
  void init(int baudRate);
  void loop();
  void write(uint8_t* data, int dataLen);
  void registerDeviceDataCallback(void(*deviceDataCallback)(uint8_t*data, int dataLen));
private:
  void (*_deviceDataCallback)(uint8_t*data, int dataLen);
};
#endif
