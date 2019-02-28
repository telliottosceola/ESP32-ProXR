#include <Settings.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <RGBLED.h>
#include <HardwareSerial.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <Device.h>
#include <Broadcast.h>
#include <WiFiHandler.h>
#include <TCPServer.h>
#include <Bluetooth.h>
#include <HTTPControl.h>
#include <MQTT.h>
#include <SoftAPHandler.h>

Settings settings;
RGBLED rgbLED;
Device device;
Broadcast broadcast;
WiFiHandler wifiHandler;
TCPServer tcpServer;
Bluetooth bluetooth;
MQTT mqtt;
SoftAPHandler softAPHandler;

//Handling for http control interface
HTTPControl httpControl;
bool requestPending;
AsyncWebServerRequest *pendingRequest;
unsigned long requestSendTime;
unsigned long requestTiemout = 1000;

String macAddress;
char macBytes[18];
char moduleIPString[17];
IPAddress moduleIP;

//Communication interfaces callbacks
void deviceDataCallback(uint8_t* data, int dataLen);
void tcpDataCallback(uint8_t* data, int dataLen);
void bluetoothDataCallback(uint8_t* data, int dataLen);
void httpDataCallback(uint8_t* data, int dataLen, AsyncWebServerRequest *request);
void mqttDataCallback(uint8_t* data, int dataLen);

//Main functions
void factoryReset();
void checkButton();

//Setup Mode Stuff
uint8_t button = 32;
unsigned long buttonPressTime;
bool buttonPressed;
bool setupButtonTimeout = 3000;
unsigned long factoryResetTimeout = 5000;
bool factoryResetEnable = false;
bool previousButtonState;
bool setupMode = false;

//RGB handling
unsigned long minimumFlashTime = 100;
unsigned long dataReceivedTime;
bool dataReceivedLED = false;
