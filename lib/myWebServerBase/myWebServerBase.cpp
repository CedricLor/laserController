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



myWebServerBase::myWebServerBase()
{
  AsyncWebServer _asyncServer(80);
  startAsyncServer(_asyncServer);
}


void myWebServerBase::startAsyncServer(AsyncWebServer& _asyncServer) {
  // starts the AsyncServer and sets a couple of callbacks, which will respond to requests
  // same as myMesh::meshSetup(), but respectively for the mesh server and the web server.

  // attach AsyncWebSocket
  myWebServerWS::ws.onEvent(myWebServerWS::onEvent);
  _asyncServer.addHandler(&myWebServerWS::ws);
  // _asyncServer.addHandler(&_ws);

  // respond to GET requests by sending index.htm to the browser
  _asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Send a response (i.e. display a web page)
    myWebServerViews __myWebServerView(request, "/index.htm");
  }); 

  // respond to GET requests requesting index.css by sending index.css to the browser
  _asyncServer.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(SPIFFS, "/index.css", "text/css");
   });

   // respond to GET requests requesting index.js by sending index.js to the browser
   _asyncServer.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.js", "text/javascript");
    });

  _asyncServer.onNotFound([this](AsyncWebServerRequest *request) {
    request->send(404);
  });
  _asyncServer.onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    this->_onBody(request, data, len, index, total);
  });

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
