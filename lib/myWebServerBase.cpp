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


AsyncWebServer myWebServerBase::_asyncServer(80);


myWebServerBase::myWebServerBase()
{
}


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
  // starts the AsyncServer and sets a couple of callbacks, which will respond to requests
  // same as myMesh::meshSetup(), but respectively for the mesh server and the web server.

  // attach AsyncWebSocket
  myWebServerWS::ws.onEvent(myWebServerWS::onEvent);
  _asyncServer.addHandler(&myWebServerWS::ws);
  // _asyncServer.addHandler(&_ws);

  // respond to GET requests by sending index.htm to the browser
  _asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      if (MY_DG_WEB) {
        Serial.print("myWebServerBase::startAsyncServer():: In handler of \"/\" request -------\n");
        // List all collected headers
        _listAllCollectedHeaders(request);
        // List all parameters
        _listAllCollectedParams(request);
      }

      // Decode request and change behavior of this controller box
      myWebServerControler __myWebServerControler;
      __myWebServerControler.decodeRequest(request);   // Call to "child" class myWebServerControler

      // Send a response (i.e. display a web page)
       myWebServerViews __myWebServerView(request);

  }); // end _asyncServer.on("/", ...)

  // respond to GET requests requesting index.css by sending index.css to the browser
  _asyncServer.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(SPIFFS, "/index.css", "text/css");
   });

   // respond to GET requests requesting index.js by sending index.js to the browser
   _asyncServer.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.js", "text/javascript");
    });

  _asyncServer.onNotFound(&_onRequest);
  _asyncServer.onRequestBody(&_onBody);

  _asyncServer.begin();
}





// Call back on unknown request
void myWebServerBase::_onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

// Call back on body request
void myWebServerBase::_onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
  if(!index){
    if (MY_DG_WEB) {
      Serial.printf("myWebServerBase::_onBody. BodyStart: %u B\n", total);
    }
  }
  for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
  }
  if(index + len == total){
    if (MY_DG_WEB) {
      Serial.printf("BodyEnd: %u B\n", total);
    }
  }
}
