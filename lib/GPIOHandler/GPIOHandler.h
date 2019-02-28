#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H
#include <Arduino.h>
#include <Settings.h>
#include <RGBLED.h>
class GPIOHandler{
public:
  void init(Settings &s, RGBLED &rgb);
  void loop();
  bool checkCFGButton();
  bool setupMode;

private:
  Settings* settings;
  RGBLED* rgbLED;
  uint8_t button = 32;
  unsigned long buttonPressTime;
  bool buttonPressed;
  bool setupButtonTimeout = 3000;
  unsigned long factoryResetTimeout = 5000;
  bool factoryResetEnable = false;
  bool previousButtonState;
};
#endif
