#include <MQTT.h>

// #define DEBUG

WiFiClientSecure wClient;
WiFiClient wClientOpen;
PubSubClient mqttClient(wClientOpen);

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
  if(settings->tls){
    if(!settings->hasRootCert){
      return false;
    }
  }
  if(!WiFi.isConnected()){
    return false;
  }
  if(mqttClientInitialized){
    if(mqttClient.connected()){
      connected = true;
      return true;
    }
  }

  if(strcmp(settings->mqttHost,"blank")==0 && settings->mqttHostIP[0]==0 && settings->mqttHostIP[1]==0 && settings->mqttHostIP[2]==0 && settings->mqttHostIP[3]==0){
    return false;
  }
  String macAddress = WiFi.macAddress();
  memset(macAddressChar, 0, 18);
  macAddress.toCharArray(macAddressChar, 18);

  //Set Client accordingly
  if(!mqttClientInitialized){
    if(settings->tls){
      // mqttClient.setClient(wClient);
      if(!settings->hasRootCert){
        return false;
      }else{
        wClient.setCACert(settings->root_ca);
        Serial.println("Root CA set");
        if(settings->hasClientCert){
          // Serial.printf("client cert set to:\n%s\n", clientCert);
          wClient.setCertificate(settings->clientCert);
          Serial.println("Client Cert Set");
        }
        if(settings->hasPrivateKey){
          // Serial.printf("private key set to:\n%s\n", privateKey);
          wClient.setPrivateKey(settings->privateKey);
          Serial.println("Private Key Set");
        }
        mqttClient.setClient(wClient);
        Serial.println("MQTT client set to secure client");
      }
    }else{
      Serial.println("Non Secure Client");
      mqttClient.setClient(wClientOpen);
    }
    mqttClientInitialized = true;
  }


  //Set Host accordingly
  if(strcmp(settings->mqttHost, "blank") != 0){
    mqttClient.setServer(settings->mqttHost, settings->mqttPort);
    #ifdef DEBUG
    Serial.printf("MQTT Client host set to: %s, Port set to: %i\n",settings->mqttHost,settings->mqttPort);
    #endif
  }else{
    mqttClient.setServer(settings->mqttHostIP, settings->mqttPort);
  }

  #ifdef TEST
  Serial.println("Attempting WiFI Client Secure connection");
  delay(50);
  if(wClient.connect(settings->mqttHostIP, settings->mqttHostIP)){
    Serial.println("WiFi Secure Client connected to IP");
  }else{
    Serial.println("WiFi Secure Client failed to connect to IP");
  }
  Serial.println("After Attempting WiFI Client Secure connection");
  delay(50);
  #endif

  if(strcmp(settings->mqttUserName, "blank") == 0 || strlen(settings->mqttUserName) == 0){
    if(mqttClient.connect(settings->mqttClientID)){
      #ifdef DEBUG
      Serial.println("MQTT Connected(No username/password)");
      #endif
      mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqttCallback(topic, payload, length); });
      mqttClient.subscribe(settings->mqttSubscribeTopic);
      connected = true;
      return true;
    }else{
      #ifdef DEBUG
      Serial.println("MQTT Connection Failed(No username/password)");
      #endif
      connected = false;
      return false;
    }
  }else{
    #ifdef DEBUG
    Serial.printf("Connecting to MQTT server with Username:%s Password:%s\n", settings->mqttUserName, settings->mqttPassword);
    delay(50);
    #endif
    if(strlen(settings->mqttPassword) == 0){
      #ifdef DEBUG
      Serial.println("No password");
      #endif
      if(mqttClient.connect(settings->mqttClientID, settings->mqttUserName, NULL)){
        #ifdef DEBUG
        Serial.println("MQTT Connected(using username/password)");
        #endif
        mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqttCallback(topic, payload, length); });
        mqttClient.subscribe(settings->mqttSubscribeTopic);
        connected = true;
        return true;
      }
    }
    if(mqttClient.connect(settings->mqttClientID, settings->mqttUserName, settings->mqttPassword)){
      #ifdef DEBUG
      Serial.println("MQTT Connected(using username/password)");
      #endif
      mqttClient.setCallback([this] (char* topic, byte* payload, unsigned int length) { this->mqttCallback(topic, payload, length); });
      mqttClient.subscribe(settings->mqttSubscribeTopic);
      connected = true;
      return true;
    }else{
      #ifdef DEBUG
      Serial.println("MQTT Connection Failed(using username/password)");
      #endif
      connected = false;
      return false;
    }
  }

}

void MQTT::registerMQTTDataCallback(void(*MQTTDataCallback)(uint8_t*data, int dataLen)){
  _MQTTDataCallback = MQTTDataCallback;
}
