#include <Arduino.h>
#include <Main.h>

#define DEBUG

void setup(){
  if(!settings.init()){
    #ifdef DEBUG
    delay(50);
    #endif
  }
  if(!settings.loadSettings()){
    #ifdef DEBUG
    delay(50);
    #endif
  }
  espHardware.init();
  espHardware.attachSetupModeCallback(setupModeCallback);
  espHardware.attachFactoryResetCallback(factoryResetModeCallback);
}

void loop(){

}

void setupModeCallback(bool enterSetupMode){

}
void factoryResetModeCallback(bool enterFactoryReset){

}
