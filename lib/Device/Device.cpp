#include <Device.h>
// #define DEBUG
HardwareSerial deviceSerial(2);

void Device::init(int baudRate, unsigned long cTimeout){
  deviceSerial.begin(baudRate);
  commandTimeout = cTimeout;
}
void Device::loop(){
  if(deviceSerial.available()){
    delay(10);
    uint8_t buffer[256];
    int index = 0;
    while(deviceSerial.available() && index < 255){
      buffer[index] = deviceSerial.read();
      index++;
    }
    uint8_t returnData[index];
    memcpy(returnData, buffer, sizeof(returnData));
    _deviceDataCallback(returnData, sizeof(returnData));
  }
  //tunnel incomming data on USB serial to Device Serial
  if(Serial.available()){
    while(Serial.available()){
      deviceSerial.write(Serial.read());
    }
  }
}
void Device::write(uint8_t* data, int dataLen){
  if(data[0] == 171){
    delay(5);
    loop();
    if(!validatePacket(data, dataLen)){
      return;
    }
    uint8_t apiCommand[dataLen-1];
    apiCommand[0] = 170;
    apiCommand[1] = data[2];
    int cs = apiCommand[0]+apiCommand[1];
    for(int i = 2; i < sizeof(apiCommand)-1; i++){
      apiCommand[i] = data[i+1];
      cs+=apiCommand[i];
    }
    uint8_t csByte = cs&255;
    apiCommand[sizeof(apiCommand)-1] = csByte;
    #ifdef DEBUG
    Serial.print("Sending to Board: ");
    for(int i = 0; i < sizeof(apiCommand); i++){
      Serial.printf("%02X ", apiCommand[i]);
    }
    Serial.println();
    #endif
    deviceSerial.write(apiCommand, sizeof(apiCommand));

    unsigned long startTime = millis();
    while(deviceSerial.available() == 0 && millis() < startTime+commandTimeout);
    if(deviceSerial.available() == 0){
      return;
    }
    while(deviceSerial.available()){
      uint8_t startByte;
      while(deviceSerial.available()){
        startByte = deviceSerial.read();
        if(startByte == 170){
          break;
        }
      }
      if(startByte != 170){
        return;
      }
      while(deviceSerial.available() < 1 && millis() < startTime+commandTimeout);
      if(deviceSerial.available() < 1){
        return;
      }
      uint8_t responseLen = deviceSerial.read();
      uint8_t receiveBuffer[responseLen+3];
      receiveBuffer[0] = startByte;
      receiveBuffer[1] = responseLen;
      int count = 0;
      while(count != responseLen+1 && millis() < startTime+commandTimeout){
        if(deviceSerial.available() != 0){
          receiveBuffer[count+2] = deviceSerial.read();
          count++;
        }
      }
      if(count<responseLen+1){
        return;
      }
      #ifdef DEBUG
      Serial.print("Received from board: ");
      for(int i = 0; i < sizeof(receiveBuffer); i++){
        Serial.printf("%02X ", receiveBuffer[i]);
      }
      Serial.println();
      #endif
      if(!validatePacket(receiveBuffer, sizeof(receiveBuffer))){
        return;
      }

      uint8_t indexedPacket[sizeof(receiveBuffer)+1];
      indexedPacket[0] = 171;
      indexedPacket[1] = data[1];
      indexedPacket[2] = receiveBuffer[1];
      for(int i = 2; i < sizeof(receiveBuffer)-1; i++){
        indexedPacket[i+1] = receiveBuffer[i];
      }
      int cs = indexedPacket[0]+indexedPacket[1]+indexedPacket[2];
      for(int i = 3; i < sizeof(indexedPacket)-1; i++){
        cs+=apiCommand[i+2];
      }
      uint8_t csByte = cs&255;
      indexedPacket[sizeof(indexedPacket)-1] = csByte;
      #ifdef DEBUG
      Serial.print("Sending back: ");
      for(int i = 0; i < sizeof(indexedPacket); i++){
        Serial.printf("%02X ", indexedPacket[i]);
      }
      Serial.println();
      #endif
      _deviceDataCallback(indexedPacket, sizeof(indexedPacket));
    }

  }else{
    deviceSerial.write(data, dataLen);
  }

}

bool Device::write(uint8_t* data, int dataLen, uint8_t* returnBuffer, size_t returnBufferSize, unsigned long timeout){
  //Clear any data in the buffer
  while(deviceSerial.available() != 0){
    deviceSerial.read();
  }
  deviceSerial.write(data, dataLen);
  unsigned long startTime = millis();
  while(deviceSerial.available() < returnBufferSize && millis() < startTime+timeout);
  if(deviceSerial.available() < returnBufferSize){
    //We did not get the expected bytes back so clear the buffer and return false
    while(deviceSerial.available() != 0){
      deviceSerial.read();
    }
    return false;
  }
  deviceSerial.readBytes(returnBuffer, returnBufferSize);
  return true;
}

void Device::registerDeviceDataCallback(void(*deviceDataCallback)(uint8_t*data, int dataLen)){
  _deviceDataCallback = deviceDataCallback;
}

bool Device::validatePacket(uint8_t* data, size_t len){
  return true;
  int sum;
  for(int i = 0; i < len-1; i++){
    sum+=data[i];
  }
  if((sum&255) == data[len-1]){
    #ifdef DEBUG
    Serial.println("valid packet");
    #endif
    return true;
  }else{
    #ifdef DEBUG
    Serial.println("invalid packet");
    Serial.printf("Checksum was %02X, should have been %02X\n", data[len-1], sum&255);
    #endif
    return false;
  }
}
