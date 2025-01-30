#include <Arduino.h>
#include "RGBLED.h"

// Constructor
void FASTLEDHANDLER::init() {
	FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
	FastLED.setBrightness(25);
}

void FASTLEDHANDLER::writeRGB(int red, int green, int blue) {
  if(red == 255 && green == 255){
	leds[0] = CRGB::Yellow;
	FastLED.show();
	return;
  }
  if(red == 255){
	leds[0] = CRGB::Red;
	FastLED.show();
	return;
  }
  if(green == 255){
	leds[0] = CRGB::Green;
	FastLED.show();
	return;
  }
  if(blue == 255){
	leds[0] = CRGB::Blue;
	FastLED.show();
	return;
  }
}

void FASTLEDHANDLER::turnOff() {
	FastLED.clear(true);
}

void FASTLEDHANDLER::writeRandom() {
	leds[0] = CHSV(random(),255,255);
	FastLED.show();
}

void FASTLEDHANDLER::setMode(uint8_t MODE){
	if(MODE == MODE_DATA_RECEIVED){
		dataReceivedLED = true;
		dataReceivedTime = millis();
		writeRGB(255,165,0);
	}

	if(_MODE == MODE){
		return;
	}
	// digitalWrite(buzzer,HIGH);
	_MODE=MODE;
	previousTime = 0;
	flashIndex = 0;
}

void FASTLEDHANDLER::setSignalStrength(uint8_t signalStrength){
	if(_signalStrength/2 != signalStrength){
		_signalStrength = signalStrength*2;
	}

}

void FASTLEDHANDLER::loop(){

	if(dataReceivedLED && millis() < dataReceivedTime+minimumFlashTime){
		return;
	}else{
		dataReceivedLED = false;
	}

	switch(_MODE){
		case 0:{
			if(previousTime == 0){
				if(MODE_ERROR_MQTT_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_ERROR_MQTT_COLOR[0], MODE_ERROR_MQTT_COLOR[1], MODE_ERROR_MQTT_COLOR[2]);
				}
				previousTime = millis();

			}else{
				if(millis() > previousTime+MODE_ERROR_MQTT_SEQUENCE[flashIndex]){
					(flashIndex >= (MODE_ERROR_MQTT_SIZE-1))? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_ERROR_MQTT_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_ERROR_MQTT_COLOR[0], MODE_ERROR_MQTT_COLOR[1], MODE_ERROR_MQTT_COLOR[2]);
					}
				}
			}
			break;
		}
		case 1:{
			if(previousTime == 0){
				if(MODE_ERROR_I2C_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_ERROR_I2C_COLOR[0], MODE_ERROR_I2C_COLOR[1], MODE_ERROR_I2C_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_ERROR_I2C_SEQUENCE[flashIndex]){
					(flashIndex >= MODE_ERROR_I2C_SIZE-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_ERROR_I2C_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_ERROR_I2C_COLOR[0], MODE_ERROR_I2C_COLOR[1], MODE_ERROR_I2C_COLOR[2]);
					}
				}
			}
			break;
		}
		case 2:{
			if(previousTime == 0){
				if(MODE_ERROR_COMMS_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_ERROR_COMMS_COLOR[0], MODE_ERROR_COMMS_COLOR[1], MODE_ERROR_COMMS_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_ERROR_COMMS_SEQUENCE[flashIndex]){
					(flashIndex >= MODE_ERROR_COMMS_SIZE-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_ERROR_COMMS_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_ERROR_COMMS_COLOR[0], MODE_ERROR_COMMS_COLOR[1], MODE_ERROR_COMMS_COLOR[2]);
					}
				}
			}
			break;
		}
		case 3:{
			if(previousTime == 0){
				if(MODE_SIGNAL_STRENGTH_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_SIGNAL_STRENGTH_COLOR[0], MODE_SIGNAL_STRENGTH_COLOR[1], MODE_SIGNAL_STRENGTH_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_SIGNAL_STRENGTH_SEQUENCE[flashIndex]){
					(flashIndex >= _signalStrength-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_SIGNAL_STRENGTH_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_SIGNAL_STRENGTH_COLOR[0], MODE_SIGNAL_STRENGTH_COLOR[1], MODE_SIGNAL_STRENGTH_COLOR[2]);
					}
				}
			}
			break;
		}
		case 4:{
			if(previousTime == 0){
				if(MODE_BOOT_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_BOOT_COLOR[0], MODE_BOOT_COLOR[1], MODE_BOOT_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_BOOT_SEQUENCE[flashIndex]){
					(flashIndex >= MODE_BOOT_SIZE-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_BOOT_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_BOOT_COLOR[0], MODE_BOOT_COLOR[1], MODE_BOOT_COLOR[2]);
					}
				}
			}
			break;
		}
		case 6:{
			if(previousTime == 0){
				if(MODE_SETUP_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_SETUP_COLOR[0], MODE_SETUP_COLOR[1], MODE_SETUP_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_SETUP_SEQUENCE[flashIndex]){
					(flashIndex >= MODE_SETUP_SIZE-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_SETUP_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_SETUP_COLOR[0], MODE_SETUP_COLOR[1], MODE_SETUP_COLOR[2]);
					}
				}
			}
			break;
		}
		case 7:{
			if(previousTime == 0){
				if(MODE_WIFI_DISCONNECT_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_WIFI_DISCONNECT_COLOR[0], MODE_WIFI_DISCONNECT_COLOR[1], MODE_WIFI_DISCONNECT_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_WIFI_DISCONNECT_SEQUENCE[flashIndex]){
					(flashIndex >= MODE_WIFI_DISCONNECT_SIZE-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_WIFI_DISCONNECT_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_WIFI_DISCONNECT_COLOR[0], MODE_WIFI_DISCONNECT_COLOR[1], MODE_WIFI_DISCONNECT_COLOR[2]);
					}
				}
			}
			break;
		}
		case 8:{
			if(previousTime == 0){
				if(MODE_ALL_CLEAR_INDEX_STATE[flashIndex] == 0){
					turnOff();
				}else{
					writeRGB(MODE_ALL_CLEAR_COLOR[0], MODE_ALL_CLEAR_COLOR[1], MODE_ALL_CLEAR_COLOR[2]);
				}
				previousTime = millis();
			}else{
				if(millis() > previousTime+MODE_ALL_CLEAR_SEQUENCE[flashIndex]){
					(flashIndex >= MODE_ALL_CLEAR_SIZE-1)? flashIndex = 0 : flashIndex++;
					previousTime = millis();
					if(MODE_ALL_CLEAR_INDEX_STATE[flashIndex] == 0){
						turnOff();
					}else{
						writeRGB(MODE_ALL_CLEAR_COLOR[0], MODE_ALL_CLEAR_COLOR[1], MODE_ALL_CLEAR_COLOR[2]);
					}
				}
			}
			break;
		}
		case 9:{
			writeRGB(0,255,0);
			break;
		}
		case 10:{
			writeRGB(255,165,0);
			break;
		}
		case 11:{
			if(previousTime == 0){
				writeRandom();
				previousTime = millis();
			}else{
				if(millis() > previousTime+200){
					previousTime = millis();
					writeRandom();
				}
			}
		}
		case 12:{
			writeRGB(0, 0, 255);
			break;
		}
	}
}
