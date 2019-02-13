#include <Device.h>
HardwareSerial serial1(2);

void Device::init(int baudRate){
  serial1.begin(baudRate);
}
void Device::loop(){
  if(serial1.available()){
    uint8_t buffer[256];
    int index = 0;
    while(serial1.available() && index < 255){
      buffer[index] = serial1.read();
      index++;
    }
    uint8_t returnData[index+1];
    memcpy(returnData, buffer, sizeof(returnData));
    _deviceDataCallback(returnData, sizeof(returnData));
  }
}
void Device::write(uint8_t* data, int dataLen){

}
void Device::registerDeviceDataCallback(void(*deviceDataCallback)(uint8_t*data, int dataLen)){
  _deviceDataCallback = deviceDataCallback;
}
