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

  //Network settings
  char wlanSSID[50];
  char wlanPASS[50];
  //Soft AP settings
  char apSSID[50];
  char apPass[50];
  int tcpListenPort;
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

private:
  FileSystem fileSystem;
  String discoveredNetworks;
  void setPublicVariables(JsonObject& settingsJSON);
  String loadedSettings;
};
#endif
