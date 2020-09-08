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
#include <Taralist.h>
#include <Time.h>
#include <TCPClient.h>

RGBLED rgbLED;
Settings settings;
Device device;
Broadcast broadcast;
WiFiHandler wifiHandler;
TCPServer tcpServer;
TCPClient tcpClient;
Bluetooth bluetooth;
MQTT mqtt;
GPIOHandler gpioHandler;
Taralist taralist;
TaskHandle_t backgroundTask;

//Handling for http control interface
HTMLHandler httpHandler;
bool requestPending;
AsyncWebServerRequest *pendingRequest;
unsigned long requestSendTime;
unsigned long requestTiemout = 1000;

//Communication interfaces callbacks
void deviceDataCallback(uint8_t* data, int dataLen);
void tcpDataCallback(uint8_t* data, int dataLen);
void tcpClientDataCallback(uint8_t* data, size_t dataLen);
void bluetoothDataCallback(uint8_t* data, int dataLen);
void httpDataCallback(uint8_t* data, int dataLen, AsyncWebServerRequest *request);
void wsDataCallback(uint8_t* data, int dataLen);
void mqttDataCallback(uint8_t* data, int dataLen);
void taralistCallback(uint8_t*data, int dataLen);
void buttonPressCallback(unsigned long duration);
void backgroundTasks(void* pvParameters);

bool setupMode = false;
bool taralistInitialized = false;
bool previousSetupMode = false;
