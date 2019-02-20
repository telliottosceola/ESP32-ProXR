#include <TCPServer.h>

// #define DEBUG

void TCPServer::init(Settings &s){
  settings = &s;
  server.begin(settings->tcpListenPort);
  server.setTimeout(5);
  ready = true;
}

void TCPServer::loop(){

  if(client.connected() == 0){
    if(previousClientStatus != 0){
      clientConnected = false;
      #ifdef DEBUG
      Serial.println("Client Disconnected");
      #endif
    }
    client = server.available();
    if(client){

      // client.setTimeout(5);
    }else{
      previousClientStatus = client.connected();
      return;
    }
  }
  if(previousClientStatus == 0 && client.connected() != 0){
    clientConnected = true;
    #ifdef DEBUG
    Serial.println("New Client Connection");
    #endif
  }
  if(client.connected() != previousClientStatus){
    #ifdef DEBUG
    Serial.printf("New Client Status: %i\n", client.connected());
    #endif
  }

  if (client.available()) {
    uint8_t buffer[256];
    int index = 0;
    while(client.available()){
      buffer[index] = client.read();
      index++;
    }
    uint8_t receivedData[index];
    memcpy(receivedData, buffer, sizeof(receivedData));
    #ifdef DEBUG
    Serial.println("TCP Received: ");
    for(int i = 0; i < sizeof(receivedData); i++){
      Serial.printf("%02X ", receivedData[i]);
    }
    Serial.println();
    #endif
    _tcpDataCallback(receivedData, sizeof(receivedData));
  }
  previousClientStatus = client.connected();
}

void TCPServer::sendData(uint8_t* data, int dataLen){
  if(client.connected() != 0){
    client.write(data, dataLen);
  }
}

void TCPServer::registerTCPDataCallback(void(*tcpDataCallback)(uint8_t*data, int dataLen)){
  _tcpDataCallback = tcpDataCallback;
}
