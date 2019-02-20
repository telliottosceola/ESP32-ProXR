#include <Device.h>
HardwareSerial deviceSerial(2);

void Device::init(int baudRate){
  deviceSerial.begin(baudRate);
}
void Device::loop(){
  if(deviceSerial.available()){
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
  deviceSerial.write(data, dataLen);
}
void Device::registerDeviceDataCallback(void(*deviceDataCallback)(uint8_t*data, int dataLen)){
  _deviceDataCallback = deviceDataCallback;
}
