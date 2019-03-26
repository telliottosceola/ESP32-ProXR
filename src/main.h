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
#include <MQTT.h>
#include <GPIOHandler.h>
#include <HTML_Handler.h>

Settings settings;
RGBLED rgbLED;
Device device;
Broadcast broadcast;
WiFiHandler wifiHandler;
TCPServer tcpServer;
Bluetooth bluetooth;
MQTT mqtt;
GPIOHandler gpioHandler;

//Handling for http control interface
HTMLHandler httpHandler;
bool requestPending;
AsyncWebServerRequest *pendingRequest;
unsigned long requestSendTime;
unsigned long requestTiemout = 1000;

//Communication interfaces callbacks
void deviceDataCallback(uint8_t* data, int dataLen);
void tcpDataCallback(uint8_t* data, int dataLen);
void bluetoothDataCallback(uint8_t* data, int dataLen);
void httpDataCallback(uint8_t* data, int dataLen, AsyncWebServerRequest *request);
void wsDataCallback(uint8_t* data, int dataLen);
void mqttDataCallback(uint8_t* data, int dataLen);

bool setupMode = false;
