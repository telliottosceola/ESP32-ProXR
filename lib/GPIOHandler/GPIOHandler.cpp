#include <GPIOHandler.h>

void GPIOHandler::init(Settings &s, FASTLEDHANDLER &rgb){
  settings = &s;
  rgbLED = &rgb;
  pinMode(button,INPUT_PULLUP);
}

void GPIOHandler::loop(){
  if(digitalRead(button) == 0){
    if(!setupMode){
      if(!buttonPressed){
        buttonPressed = true;
        buttonPressTime = millis();
      }else{
        if(millis() > buttonPressTime + setupButtonTimeout){
          ESP.restart();
        }
      }
    }else{
      //In setup mode so holding the button here means they want a factory reset
      if(!buttonPressed){
        buttonPressed = true;
        buttonPressTime = millis();
      }else{
        if((millis() > buttonPressTime + factoryResetTimeout) && factoryResetEnable){
          while(digitalRead(button) == 0){
            rgbLED->writeRandom();
            delay(100);
          }
          settings->factoryReset();
        }
      }
    }

  }else{
    buttonPressed = false;
    factoryResetEnable = true;
  }
}

bool GPIOHandler::checkCFGButton(){
  if(digitalRead(button) == 0){
    return true;
  }
  return false;
}
