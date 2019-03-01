#ifndef SETTINGS_H
#define SETTINGS_H
#include <FileSystem.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WiFi.h>
class Settings{
public:
  bool init();
  bool storeSettings(String s);
  bool loadSettings();
  String returnSettings(char* macAddress,char* ipAddress, bool load);
  bool factoryReset();

  String WiFiNetworks;

  //Network settings
  bool wifiEnabled;
  bool bluetoothEnabled;
  bool httpControlEnabled;
  bool mqttEnabled;
  bool udpBroadcastEnabled;
  bool tcpListenerEnabled;

  char wlanSSID[50];
  char wlanPASS[50];
  //Soft AP settings
  char apSSID[50];
  char apPass[50];
  uint16_t tcpListenPort;
  char deviceID[33] = "Travis ESP32 WiFi Module";
  //Static IP Settings
  bool dhcpEnabled = true;
  IPAddress staticIP;
  IPAddress defaultGateway;
  IPAddress subnetMask;
  IPAddress primaryDNS;
  IPAddress secondaryDNS;
  //Serial Settings
  int baudRate;
  //Bluetooth Settings
  char bluetoothPairing[50];
  char bluetoothName[50];
  //MQTT settings
  char mqttHost[50];
  int mqttPort = 8883;
  char mqttClientID[50];
  char mqttUserName[50];
  char mqttPassword[50];
  char mqttSubscribeTopic[50];
  char mqttPublishTopic[50];

private:
  FileSystem fileSystem;
  String discoveredNetworks;
  void setPublicVariables(JsonObject& settingsJSON);
  String loadedSettings;
};
#endif
