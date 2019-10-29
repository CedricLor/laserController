/*
  myWebServerBase.cpp - Library to handle web server controller related functions.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "myWebServerBase.h"


myWebServerBase::myWebServerBase(uint16_t port):
  AsyncWebServer(port)
{
  if (globalBaseVariables.isInterface) {
    // starts the AsyncServer and sets a couple of callbacks, which will respond to requests
    // same as myMesh::meshSetup(), but respectively for the mesh server and the web server.
    // attach AsyncWebSocket
    myWebServerWS::ws.onEvent(myWebServerWS::onEvent);
    addHandler(&myWebServerWS::ws);
    // __asyncServer.addHandler(&_ws);

    // respond to GET requests by sending index.htm to the browser
    on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      // Send a response (i.e. display a web page)
      request->send(SPIFFS, "/index.htm", String(), false);
    });

    // respond to GET requests requesting index.css by sending index.css to the browser
    on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.css", "text/css");
    });

    // respond to GET requests requesting index.js by sending index.js to the browser
    on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.js", "text/javascript");
      });

    onNotFound([](AsyncWebServerRequest *request) {
      request->send(404);
    });

    onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      this->_onBody(request, data, len, index, total);
    });

    begin();
  }
}




uint8_t myWebServerBase::getStatus() {
  return _server.status();
}  




// Call back on body request
void myWebServerBase::_onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
  if(!index){
    if (globalBaseVariables.MY_DG_WEB) {
      Serial.printf("myWebServerBase::_onBody. BodyStart: %u B\n", total);
    }
  }
  for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
  }
  if(index + len == total){
    if (globalBaseVariables.MY_DG_WEB) {
      Serial.printf("BodyEnd: %u B\n", total);
    }
  }
}
