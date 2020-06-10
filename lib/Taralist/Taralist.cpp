#include <Taralist.h>

void Taralist::init(){
  if(WiFi.isConnected()){
    using namespace std::placeholders;
    NTP.onNTPSyncEvent(std::bind(&Taralist::onNTPSynced, this, _1));
    NTP.begin("time.google.com");
    // NTP.setInterval(10);
  }
}

void Taralist::loop(){

}

void Taralist::onNTPSynced(NTPSyncEvent_t ntpEvent){
  if (ntpEvent) {
    if (ntpEvent == noResponse) {
    }
    else if (ntpEvent == invalidAddress) {
    }

    NTP.setInterval(5); //try again soon
  }else{
    ntpSyncedFlag = true;
    // Serial.printf("Got NTP time: ");
    // Serial.printf("%s\n", NTP.getTimeDateString(NTP.getLastNTPSync()).c_str());
    NTP.setInterval(10);
    // Serial.printf("Current timestamp: %d\n", now());
    if(now() > 10){
      Serial.println("NTP Time Sync'd");
    }
  }
}
