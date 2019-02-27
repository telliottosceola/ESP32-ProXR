#include <MQTT.h>

#define DEBUG

WiFiClient mqttWiFiClient;
PubSubClient mqttClient(mqttWiFiClient);

void MQTT::init(Settings &s){
  settings = &s;
  checkMQTT();
}

void MQTT::loop(){
  if(checkMQTT()){
    mqttClient.loop();
  }

}
//Response will be in this format: {data:[85,85,85]}
void MQTT::mqttPublish(uint8_t* payload, unsigned int length){
  if(checkMQTT()){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& dataArray = root.createNestedArray("data");
    for(int i = 0; i < length; i++){
      dataArray.add((int)payload[i]);
    }
    char payloadChar[root.measureLength()+1];
    root.printTo((char*)payloadChar, sizeof(payloadChar));
    #ifdef DEBUG
    Serial.printf("Publishing %s to topic %s\n", payloadChar, settings->mqttPublishTopic);
    #endif

    if(!mqttClient.publish(settings->mqttPublishTopic, payloadChar, sizeof(payloadChar))){
      #ifdef DEBUG
      Serial.println("Publish failed");
      #endif
    }
  }
}

//Expected MQTT payload format {sendCommand:[254,108,1]}
void MQTT::mqttCallback(char* topic, byte* payload, unsigned int length) {
  char payloadChar[length+1];
  memset(payloadChar, 0, length+1);
  memcpy(payloadChar, (char*)payload, length);
  String messageTopic = String(topic);
  String message = String(payloadChar);
  #ifdef DEBUG
  Serial.print("topic:");
  Serial.println(messageTopic);
  Serial.print("payload:");
  Serial.println(message);
  #endif
  if(strcmp(topic, settings->mqttSubscribeTopic) == 0){
    DynamicJsonBuffer jsonBuffer;
    JsonObject& payloadJSON = jsonBuffer.parseObject(payload);
    if(payloadJSON.success()){
      if(payloadJSON.containsKey("sendCommand")){
        JsonArray& dataArray = payloadJSON["sendCommand"].as<JsonArray&>();
        uint8_t data[dataArray.size()];
        int index = 0;
        for(int value : dataArray){
          data[index] = dataArray[index];
          index++;
        }
        _MQTTDataCallback(data, sizeof(data));
      }
    }
  }
}

bool MQTT::checkMQTT(){
  if(mqttClient.connected()){
    return true;
  }
  mqttClient.setServer(settings->mqttHost, settings->mqttPort);

  if((strcmp(settings->mqttClientID, "blank") != 0)){
    char mqttID[strlen(settings->mqttClientID)+1];
    memset(mqttID, 0, sizeof(mqttID));
    memcpy(mqttID, settings->mqttClientID, strlen(settings->mqttClientID));
    if(strcmp(settings->mqttUserName, "blank") == 0 && strcmp(settings->mqttPassword, "blank") == 0){
      #ifdef DEBUG
      Serial.println("Connecting MQTT with only mqtt client id");
      #endif
      mqttClient.connect(settings->mqttClientID);
    }else if(strcmp(settings->mqttPassword, "blank") == 0){
      #ifdef DEBUG
      Serial.println("Connecting MQTT with only mqtt client id and username");
      #endif
      mqttClient.connect(settings->mqttClientID, settings->mqttUserName, "");
    }else{
      #ifdef DEBUG
      Serial.println("Connecting MQTT with client id, username, and password");
      #endif
      mqttClient.connect(settings->mqttClientID, settings->mqttUserName, settings->mqttPassword);
    }

  }else{
    #ifdef DEBUG
    Serial.println("No MQTT Client set");
    #endif
    return false;
  }

  if(mqttClient.connected()){
    mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqttCallback(topic, payload, length); });

    #ifdef DEBUG
    Serial.println("MQTT connected.");
    #endif
    //Subscribe to data in topic
    if(mqttClient.subscribe(settings->mqttSubscribeTopic)){
      #ifdef DEBUG
      Serial.printf("Subscribed to topic %s\n", settings->mqttSubscribeTopic);
      #endif
    }else{
      #ifdef DEBUG
      Serial.printf("Subscription to topic %s failed\n", settings->mqttSubscribeTopic);
      #endif
    }
  }
  return true;
}

void MQTT::registerMQTTDataCallback(void(*MQTTDataCallback)(uint8_t*data, int dataLen)){
  _MQTTDataCallback = MQTTDataCallback;
}
