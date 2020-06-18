#include <Taralist.h>

// #define DEBUG

void Taralist::init(unsigned long updateInterval, long timeZone, long dstOffset){
  _updateInterval = updateInterval;
  if(WiFi.isConnected()){
    #ifdef DEBUG
    Serial.println("Taralist Init ran");
    #endif
    gmtOffset_sec = timeZone *3600;
    configTime(gmtOffset_sec, dstOffset, ntpServer);
  }
}

void Taralist::loop(){
  if(WiFi.isConnected()){
    if(millis() > lastUpdate+_updateInterval || _updateInterval == 0){
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
        #ifdef DEBUG
        Serial.println("Failed to obtain time");
        #endif
        return;
      }
      lastUpdate = millis();
      #ifdef DEBUG
      Serial.printf("Time: %i:%i:%i\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      Serial.printf("Date: %i/%i/%i\n", timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_year+1900);
      #endif
      uint8_t bcdBuffer[10];
      convertToBCD(timeinfo.tm_year-100, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_wday+1, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, bcdBuffer, sizeof(bcdBuffer));
      uint8_t apiBuffer[sizeof(bcdBuffer)+3];
      wrapAPI(bcdBuffer, sizeof(bcdBuffer), apiBuffer);
      _taralistCallback(apiBuffer, sizeof(apiBuffer));
    }
  }
}

void Taralist::manualSync(){
  if(WiFi.isConnected()){

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      #ifdef DEBUG
      Serial.println("Failed to obtain time");
      #endif
      return;
    }
    lastUpdate = millis();
    #ifdef DEBUG
    Serial.printf("Time: %i:%i:%i\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    Serial.printf("Date: %i/%i/%i\n", timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_year+1900);
    #endif
    uint8_t bcdBuffer[10];
    convertToBCD(timeinfo.tm_year-100, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_wday+1, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, bcdBuffer, sizeof(bcdBuffer));
    uint8_t apiBuffer[sizeof(bcdBuffer)+3];
    wrapAPI(bcdBuffer, sizeof(bcdBuffer), apiBuffer);
    _taralistCallback(apiBuffer, sizeof(apiBuffer));
  }
}

void Taralist::convertToBCD(int year, int month, int dMon, int dWeek, int hour, int min, int sec, uint8_t* buff, size_t buffSize){
  if(buffSize < 10){
    #ifdef DEBUG
    Serial.printf("convertToBCD Buffer too small, it is: %i\n", buffSize);
    #endif
    return;
  }
  int checkSum = 0;
  buff[0] = 65;
  buff[1] = 100;
  buff[2] = convertToBCDByte(sec);
  checkSum+= buff[2];
  buff[3] = convertToBCDByte(min);
  checkSum+= buff[3];
  buff[4] = convertToBCDByte(hour);
  checkSum+= buff[4];
  buff[5] = convertToBCDByte(dMon);
  checkSum+= buff[5];
  buff[6] = convertToBCDByte(month);
  checkSum+= buff[6];
  buff[7] = convertToBCDByte(dWeek);
  checkSum+= buff[7];
  buff[8] = convertToBCDByte(year);
  checkSum+= buff[8];
  buff[9] = checkSum%256;
}

uint8_t Taralist::convertToBCDByte(int v){

  uint8_t a = (uint8_t)(floor(v/10)); //1
  uint8_t b = (uint8_t)(v%10); //1
  #ifdef DEBUG
  Serial.printf("convertToBCDByte, given value: %i, a: %i, b: %i, result: %i\n", v, a, b, a*16+b);
  #endif
  return a*16+b;
}

void Taralist::wrapAPI(uint8_t* data, size_t dataLen, uint8_t* buffer){
  uint8_t packet[dataLen+3];
  packet[0] = 170;
  packet[1] = dataLen;
  int cs = packet[0]+packet[1];
  for(size_t i = 0; i < dataLen; i++){
    packet[i+2] = data[i];
    cs+=data[i];
  }
  uint8_t csByte = cs&255;
  packet[sizeof(packet)-1] = csByte;
  memcpy(buffer, packet, sizeof(packet));
  return;
}

void Taralist::registerTaralistCallback(void(*taralistCallback)(uint8_t*data, int dataLen)){
  _taralistCallback = taralistCallback;
}
