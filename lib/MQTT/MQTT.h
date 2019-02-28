#ifndef MQTT_H
#define MQTT_H

#include<PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Settings.h>

class MQTT{
public:
  void init(Settings &s);
  void loop();
  void registerMQTTDataCallback(void(*MQTTDataCallback)(uint8_t*data, int dataLen));
  void mqttPublish(uint8_t* payload, unsigned int length);
  bool connected = false;

private:
  Settings *settings;
  void mqttCallback(char* topic, byte* payload, unsigned int length);
  bool checkMQTT();
  void (*_MQTTDataCallback)(uint8_t*data, int dataLen);
};
#endif
