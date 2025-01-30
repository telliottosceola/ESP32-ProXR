#ifndef RGBLED_h
#define RGBLED_h

#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 48
#define NUM_LEDS 1




class FASTLEDHANDLER
{
  CRGB leds[NUM_LEDS];
public:

	//Constructor
	void init();

	// Single method to set all pins
	// red,green,blue values range from 0(off)-255(on)


	// Sets just the red pin
	// Values range from 0(off)-255(on)
	void writeRed(int red);

	// Sets just the greem pin
	// Values range from 0(off)-255(on)
	void writeGreen(int green);

	// Sets just the blue pin
	// Values range from 0(off)-255(on)
	void writeBlue(int blue);

	// Turns all pins off
	void turnOff(void);

	// Show a random color
	void writeRandom(void);

	// Writes the LED based on HSV values
	void writeHSV(int h, double s, double v);

	// Show the HSV Color Wheel
	// Take in the delay (dly) in milliseconds between each color
	void writeColorWheel(int dly);

	// Map a value based on the common_type
	int mapValue(int value);

  void setMode(uint8_t mode);
  void setSignalStrength(uint8_t signalStrength);

  void loop();

  uint8_t MODE_ERROR_MQTT = 0;
  uint8_t MODE_ERROR_I2C = 1;
  uint8_t MODE_ERROR_COMMS = 2;
  uint8_t MODE_SIGNAL_STRENGTH = 3;
  uint8_t MODE_BOOT = 4;
  uint8_t MODE_WIFI_CONNECTED = 5;
  uint8_t MODE_SETUP = 6;
  uint8_t MODE_WIFI_DISCONNECTED = 7;
  uint8_t MODE_ALL_CLEAR = 8;
  uint8_t MODE_CLIENT_CONNECTED = 9;
  uint8_t MODE_DATA_RECEIVED = 10;
  uint8_t RANDOM = 11;
  uint8_t MODE_BOOT_SETUP = 12;

private:

  unsigned long minimumFlashTime = 100;
  unsigned long dataReceivedTime;
  bool dataReceivedLED = false;

  void writeRGB(int red, int green, int blue);
  unsigned long pulseDuration = 200;
  unsigned long offDuration = 200;
  unsigned long cycleDelay = 1000;
  uint8_t _MODE;
  unsigned long previousTime;
  unsigned long flashIndex;
  uint8_t _signalStrength;

  unsigned long MODE_ERROR_MQTT_SEQUENCE[2] = {cycleDelay, pulseDuration}; //One Flash
  int MODE_ERROR_MQTT_COLOR[3] = {255,0,0};
  int MODE_ERROR_MQTT_INDEX_STATE[2] = {0, 1};
  int MODE_ERROR_MQTT_SIZE = 2;

  unsigned long MODE_ERROR_I2C_SEQUENCE[4] = {cycleDelay, pulseDuration, offDuration, pulseDuration}; //One Flash
  int MODE_ERROR_I2C_COLOR[3] = {255,0,0};
  int MODE_ERROR_I2C_INDEX_STATE[4] = {0, 1, 0, 1};
  int MODE_ERROR_I2C_SIZE = 4;

  unsigned long MODE_ERROR_COMMS_SEQUENCE[6] = {cycleDelay, pulseDuration, offDuration, pulseDuration, offDuration, pulseDuration}; //One Flash
  int MODE_ERROR_COMMS_COLOR[3] = {255,0,0};
  int MODE_ERROR_COMMS_INDEX_STATE[6] = {0, 1, 0, 1, 0, 1};
  int MODE_ERROR_COMMS_SIZE = 6;

  unsigned long MODE_SIGNAL_STRENGTH_SEQUENCE[10] = {cycleDelay, pulseDuration, offDuration, pulseDuration, offDuration, pulseDuration, offDuration, pulseDuration, offDuration, pulseDuration}; //One Flash
  int MODE_SIGNAL_STRENGTH_COLOR[3] = {0,255,0};
  int MODE_SIGNAL_STRENGTH_INDEX_STATE[10] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  int MODE_SIGNAL_STRENGTH_SIZE = 10;

  unsigned long MODE_BOOT_SEQUENCE[2] = {cycleDelay, pulseDuration}; //One Flash
  int MODE_BOOT_COLOR[3] = {255,255,0};
  int MODE_BOOT_INDEX_STATE[2] = {1, 0};
  int MODE_BOOT_SIZE = 2;

  unsigned long MODE_WIFI_CONNECTED_SEQUENCE[2] = {cycleDelay, pulseDuration}; //One Flash
  int MODE_WIFI_CONNECTED_COLOR[3] = {0,255,0};
  int MODE_WIFI_CONNECTED_INDEX_STATE[2] = {0, 1};
  int MODE_WIFI_CONNECTED_SIZE = 2;

  unsigned long MODE_SETUP_SEQUENCE[2] = {cycleDelay, pulseDuration}; //One Flash
  int MODE_SETUP_COLOR[3] = {0,0,255};
  int MODE_SETUP_INDEX_STATE[2] = {0, 1};
  int MODE_SETUP_SIZE = 2;

  unsigned long MODE_WIFI_DISCONNECT_SEQUENCE[2] = {cycleDelay, pulseDuration}; //One Flash
  int MODE_WIFI_DISCONNECT_COLOR[3] = {255,0,0};
  int MODE_WIFI_DISCONNECT_INDEX_STATE[8] = {0, 1, 0, 1, 0, 1, 0, 1};
  int MODE_WIFI_DISCONNECT_SIZE = 8;

  unsigned long MODE_ALL_CLEAR_SEQUENCE[2] = {cycleDelay, pulseDuration}; //One Flash
  int MODE_ALL_CLEAR_COLOR[3] = {0,255,0};
  int MODE_ALL_CLEAR_INDEX_STATE[2] = {0, 1};
  int MODE_ALL_CLEAR_SIZE = 2;
};

#endif
