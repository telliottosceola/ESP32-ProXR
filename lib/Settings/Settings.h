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
  bool udpRemoteBroadcastEnabled;
  bool tcpListenerEnabled;
  bool tcpClientEnabled;

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
  uint8_t mqttHostIP[4];

  int relayCount;

  char defaultHTML[50];

  bool taralistEnabled = true;
  long taralistTimeZone = -6;
  bool taralistDST = false;

  char remoteHostURL[200];
  IPAddress remoteHostIP;
  int remoteHostPort;

  char wpaEnterpriseUsername[50];
  char wpaEnterpriseIdentity[50];

  //MQTT TLS
  bool tls = false;
  char clientCert[1500];
  char privateKey[2000];
  char root_ca[2000];
  bool hasRootCert = false;
  bool hasClientCert = false;
  bool hasPrivateKey = false;

private:
  FileSystem fileSystem;
  String discoveredNetworks;
  void setPublicVariables(JsonObject& settingsJSON);
  String loadedSettings;
};
#endif
