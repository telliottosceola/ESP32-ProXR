#include <Bluetooth.h>

// #define DEBUG

void Bluetooth::init(Settings &s){
  settings = &s;
  SerialBT.begin(settings->bluetoothName);
}

void Bluetooth::loop(){
  if(SerialBT.hasClient()){
    deviceConnected = true;
    if(!previousDeviceConnected){
      #ifdef DEBUG
      Serial.println("Bluetooth Client Connected");
      delay(50);
      #endif
    }
  }else{
    deviceConnected = false;
    if(previousDeviceConnected){
      #ifdef DEBUG
      Serial.println("Bluetooth Client Disconnected");
      delay(50);
      #endif
    }
    return;
  }
  if(SerialBT.available()){
    uint8_t buffer[256];
    int index = 0;
    #ifdef DEBUG
    Serial.printf("Bluetooth received %i bytes\n", SerialBT.available());
    delay(50);
    #endif
    while(SerialBT.available() && index < 255){
      buffer[index] = SerialBT.read();
      index++;
    }
    #ifdef DEBUG
    Serial.println("Bluetooth copying buffer data to return buffer");
    delay(50);
    #endif
    uint8_t returnData[index];
    memcpy(returnData, buffer, sizeof(returnData));
    #ifdef DEBUG
    Serial.println("Bluetooth firing data callback");
    delay(50);
    #endif
    Serial.print("Received: ");
    for(int i = 0; i < sizeof(returnData); i++){
      Serial.printf("%02X ",returnData[i]);
    }
    Serial.println();
    _bluetoothDataCallback(returnData, sizeof(returnData));
  }
}

void Bluetooth::sendData(uint8_t* data, int dataLen){
  Serial.print("Bluetooth Returning ");
  for(int i = 0; i < dataLen; i++){
    Serial.printf("%02X ",data[i]);
  }
  Serial.println();
  SerialBT.write(data, dataLen);
}

void Bluetooth::registerBluetoothDataCallback(void(*BluetoothDataCallback)(uint8_t*data, int dataLen)){
  _bluetoothDataCallback = BluetoothDataCallback;
}
