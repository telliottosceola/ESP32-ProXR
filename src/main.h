#include <Settings.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <RGBLED.h>
#include <HardwareSerial.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

Settings settings;
RGBLED rgbLED;

bool checkWiFi();

String macAddress;
char macBytes[18];
char moduleIPString[17];
IPAddress moduleIP;

//Soft AP Stuff
AsyncWebServer server(80);
const byte DNS_PORT = 53;
DNSServer dnsServer;
IPAddress apIP(172, 217, 28, 1);
void onRequest(AsyncWebServerRequest *request);
void checkButton();
void factoryReset();

//Setup Mode Stuff
uint8_t button = 32;
unsigned long buttonPressTime;
bool buttonPressed;
bool setupButtonTimeout = 3000;
unsigned long factoryResetTimeout = 5000;
bool factoryResetEnable = false;
bool previousButtonState;
bool setupMode = false;
bool boot = true;
