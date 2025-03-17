#include <TCPClient.h>

WiFiClient client;

void TCPClient::init(Settings &s){
  settings = &s;
  if(settings->remoteHostPort == 0){
    ready = false;
    return;
  }

  if(strlen(settings->remoteHostURL) != 0){
    if(client.connect(settings->remoteHostURL, settings->remoteHostPort)){
      ready = true;
    }
  }else{
    if(client.connect(settings->remoteHostIP, settings->remoteHostPort)){
      ready = true;
    }
  }

}

void TCPClient::loop(){
  if(!client.connected()){
    // Serial.println("Client not connected");
    if(strlen(settings->remoteHostURL) != 0){
      // Serial.println("Using URL");
      if(client.connect(settings->remoteHostURL, settings->remoteHostPort)){
        connectedTime = millis();
        connected = true;
        ready = true;
      }
    }else{
      // Serial.println("Using IP");
      if(client.connect(settings->remoteHostIP, settings->remoteHostPort)){
        connectedTime = millis();
        connected = true;
        ready = true;
      }
    }
  }
  if(client.connected()){
    if(millis() > connectedTime+connectionTimeout && connectionTimeout != 0){
      client.stop();
      connected = false;
    }
    if(client.available()){
      connectedTime = millis();
      delay(5);
      uint8_t buffer[client.available()];
      client.read(buffer, sizeof(buffer));
      _tcpClientDataCallback(buffer, sizeof(buffer));
    }
    connected = true;
  }else{
    ready = false;
    connected = false;
  }
}

void TCPClient::sendData(uint8_t* data, size_t dataLen){
  if(client.connected()){
    client.write(data, dataLen);
  }
}

void TCPClient::registerTCPClientDataCallback(void(*tcpClientDataCallback)(uint8_t* data, size_t dataLen)){
  _tcpClientDataCallback = tcpClientDataCallback;
}
