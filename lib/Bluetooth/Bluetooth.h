#ifndef BLUETOOTH_H
#define BLUETOOTH_H
#include <Arduino.h>
#include <Settings.h>
#include <BluetoothSerial.h>

class Bluetooth{
  #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
  #endif
public:
  void init(Settings &s);
  void loop();
  void registerBluetoothDataCallback(void(*BluetoothDataCallback)(uint8_t*data, int dataLen));
  void sendData(uint8_t* data, int dataLen);
  bool deviceConnected = false;
private:
  Settings *settings;
  BluetoothSerial SerialBT;
  void (*_bluetoothDataCallback)(uint8_t*data, int dataLen);
  bool previousDeviceConnected = false;
};
#endif
