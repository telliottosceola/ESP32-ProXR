#include <WifiHandler.h>

String WifiHandler::scanNetworks(char* wlanSSID){
  String WiFiNetworks;
  //scan networks
  WiFi.mode(WIFI_STA);
  delay(100);
  DynamicJsonBuffer jsonBuffer;
  JsonArray& foundNetworks = jsonBuffer.createArray();
  int n = WiFi.scanNetworks();
  if(n == 0){
    Serial.println("No Networks Found");
    delay(500);
  }else{
    for(int i = 0; i < n; i++){
      JsonObject& network = foundNetworks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      if(strcmp(wlanSSID, network["ssid"]) == 0){
        network["selected"] = true;
      }
    }
  }
  foundNetworks.printTo(WiFiNetworks);
  return WiFiNetworks;
}
