#include <ESPHardware.h>

void ESPHardware::init(){
  pinMode(button,INPUT_PULLUP);
}

void ESPHardware::loop(){
  if(digitalRead(button) == 0){
    if(!setupMode){
      if(!buttonPressed){
        buttonPressed = true;
        buttonPressTime = millis();
      }else{
        if(millis() > buttonPressTime + setupButtonTimeout){
          _setupModeCallBack(true);
        }
      }
    }else{
      //In setup mode so holding the button here means they want a factory reset
      if(!buttonPressed){
        buttonPressed = true;
        buttonPressTime = millis();
      }else{
        if((millis() > buttonPressTime + factoryResetTimeout) && factoryResetEnable){
          _factoryResetCallback(true);
        }
      }
    }

  }else{
    buttonPressed = false;
    factoryResetEnable = true;
  }
}

void ESPHardware::attachSetupModeCallback(void (*setupModeCallback)(bool enterSetupMode)){
  _setupModeCallBack = setupModeCallback;
}
void ESPHardware::attachFactoryResetCallback(void (*factoryResetCallback)(bool factoryReset)){
  _factoryResetCallback = factoryResetCallback;
}
