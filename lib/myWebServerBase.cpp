/*
  myWebServerBase.cpp - Library to handle web server controller related functions.
  Created by Cedric Lor, January 19, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myWebServerControler.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerControler.h
  |  |  |  |--MasterSlaveBox.cpp (called to set some values re. master/slave box reactions in global)
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |--//LaserGroupedUnits.cpp
  |  |  |  |--//MasterSlaveBox.cpp
  |  |  |
  |  |  |--myWebServerViews.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerViews.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
  |  |  |  |--//LaserPin.cpp

*/

#include "Arduino.h"
#include "myWebServerBase.h"
#include "myWebServerViews.cpp"
#include "myWebServerControler.cpp"

myWebServerBase::myWebServerBase()
{
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

    // Decode request and change behavior of this controller box
    myWebServerControler::decodeRequest(request);   // Call to "child" class myWebServerControler

    //Send a response (i.e. display a web page)
    ControlerBox[0].updateThisBoxProperties();  // dependency; update this box properties before myWebServerViews reads this box properties
    myWebServerViews __myWebServerView;  // Call to "child" class myWebServerViews
    Serial.print("myWebServerBase::startAsyncServer(): Just after instantiating __myWebServerView\n");
    AsyncResponseStream *__response = request->beginResponseStream("text/html");  // define a response stream
    __response->addHeader("Server","ESP Async Web Server");                       // append stuff to header
    __response->printf(__myWebServerView.returnTheResponse().c_str());            // converts the arduino String in C string (array of chars)
    request->send(__response);                                                    // send the response
  });

  _asyncServer.onNotFound(&_onRequest);  // error: no matching function for call to 'AsyncWebServer::onNotFound(void (&)())'
  _asyncServer.onRequestBody(&_onBody);  // error: no matching function for call to 'AsyncWebServer::onRequestBody(void (&)())'

  _asyncServer.begin();
}

void myWebServerBase::_onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void myWebServerBase::_onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}
