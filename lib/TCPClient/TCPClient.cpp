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
  if(client.connected()){
    if(millis() > connectedTime+connectionTimeout){
      client.stop();
    }
    if(client.available()){
      connectedTime = millis();
      delay(5);
      uint8_t buffer[client.available()];
      client.read(buffer, sizeof(buffer));
      sendData(buffer, sizeof(buffer));
    }
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
