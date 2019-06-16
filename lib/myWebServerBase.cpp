/*
  myWebServerBase.cpp - Library to handle web server controller related functions.
  Created by Cedric Lor, January 19, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
  |  |  |--myWebServerViews.cpp
  |  |  |  |--myWebServerViews.h
  |  |  |--myWebServerControler.cpp
  |  |  |  |--myWebServerControler.h
  |  |  |  |--MasterSlaveBox.cpp
  |  |  |  |  |--MasterSlaveBox.h
*/

#include "Arduino.h"
#include "myWebServerBase.h"
#include "myWebServerViews.cpp"
#include "myWebServerControler.cpp"

myWebServerBase::myWebServerBase(/*int pin*/)
{
  // pinMode(pin, OUTPUT);
  // _pin = pin;
}

AsyncWebServer myWebServerBase::_asyncServer(80);
char myWebServerBase::_linebuf[80];

void myWebServerBase::_listAllCollectedHeaders(AsyncWebServerRequest *request) {
  int __headers = request->headers();
  for(int __i=0;__i<__headers;__i++){
    AsyncWebHeader* __h = request->getHeader(__i);
    Serial.printf("HEADER[%s]: %s\n", __h->name().c_str(), __h->value().c_str());
  }
}

void myWebServerBase::_listAllCollectedParams(AsyncWebServerRequest *request) {
  int __params = request->params();
  for(int __i=0;__i<__params;__i++){
    AsyncWebParameter* __param = request->getParam(__i);
    if(__param->isPost()){
      Serial.printf("POST[%s]: %s\n", __param->name().c_str(), __param->value().c_str());
    } else {
      Serial.printf("GET[%s]: %s\n", __param->name().c_str(), __param->value().c_str());
      // storeGetParamsInLineBuf(params, i, param);
    }
  }
}

void myWebServerBase::startAsyncServer() {
  _asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.print("myWebServerBase::startAsyncServer():: In handler of \"/\" request -------\n");

    // List all collected headers
    _listAllCollectedHeaders(request);

    // List all parameters
    _listAllCollectedParams(request);

    // Decode request
    myWebServerControler::decodeRequest(request);

    //Send a response
    myWebServerViews __myWebServerView;
    Serial.print("myWebServerBase::startAsyncServer(): Just after instantiating __myWebServerView\n");
    AsyncResponseStream *__response = request->beginResponseStream("text/html");  // define a response stream
    __response->addHeader("Server","ESP Async Web Server");                       // append stuff to header
    __response->printf(__myWebServerView.returnTheResponse().c_str());            // converts the arduino String in C string (array of chars)
    request->send(__response);                                                    // send the response
  });

  _asyncServer.onNotFound(&onRequest);  // error: no matching function for call to 'AsyncWebServer::onNotFound(void (&)())'
  _asyncServer.onRequestBody(&onBody);  // error: no matching function for call to 'AsyncWebServer::onRequestBody(void (&)())'

  _asyncServer.begin();
}

void myWebServerBase::onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void myWebServerBase::onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}
