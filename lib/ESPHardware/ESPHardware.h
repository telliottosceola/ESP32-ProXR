#ifndef ESPHARDWARE_H
#define ESPHARDWARE_H
#include <Arduino.h>
class ESPHardware{
public:
  void init();
  void loop();
  void attachSetupModeCallback(void (*setupModeCallback)(bool enterSetupMode));
  void attachFactoryResetCallback(void (*factoryResetCallback)(bool factoryReset));
  bool setupMode = false;
private:
  void (*_setupModeCallBack)(bool enterSetupMode);
  void (*_factoryResetCallback)(bool factoryReset);
  uint8_t button = 32;
  unsigned long buttonPressTime;
  bool buttonPressed;
  bool setupButtonTimeout = 3000;
  unsigned long factoryResetTimeout = 5000;
  bool previousButtonState;
  bool factoryResetEnable = false;
};
#endif
