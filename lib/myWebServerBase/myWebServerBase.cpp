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
  // starts the AsyncServer and sets a couple of callbacks, which will respond to requests
  // same as myMesh::meshSetup(), but respectively for the mesh server and the web server.

  // attach AsyncWebSocket
  myWebServerWS::ws.onEvent(myWebServerWS::onEvent);
  _asyncServer.addHandler(&myWebServerWS::ws);
  // _asyncServer.addHandler(&_ws);

  // respond to GET requests by sending index.htm to the browser
  _asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Send a response (i.e. display a web page)
    myWebServerViews(request, "/index.htm");
  }); 

  // respond to GET requests requesting index.css by sending index.css to the browser
  _asyncServer.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(SPIFFS, "/index.css", "text/css");
   });

   // respond to GET requests requesting index.js by sending index.js to the browser
   _asyncServer.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.js", "text/javascript");
    });

  _asyncServer.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });

  _asyncServer.onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    this->_onBody(request, data, len, index, total);
  });

  _asyncServer.begin();
}



void myWebServerBase::myWebServerViews(AsyncWebServerRequest *request, const char* url)
{
  // strcpy(cBoxArray, "");
  // _loadBoxArray();

  //  * Response Stream (You can print/write/printf to it, up to the contentLen bytes)
  //  * */
  // AsyncResponseStream::AsyncResponseStream(const String& contentType, size_t bufferSize){
  // AsyncResponseStream * AsyncWebServerRequest::beginResponseStream(const String& contentType, size_t bufferSize){
  //   return new AsyncResponseStream(contentType, bufferSize);
  // }

   // AsyncResponseStream *__response = request->beginResponseStream("text/html");  // define a response stream
   // __response->addHeader("Server","ESP Async Web Server");                       // append stuff to header
   // __response->printf(__myWebServerView.returnTheResponse().c_str());            // converts the arduino String in C string (array of chars)
   // request->send(__response);

   // AsyncWebServerResponse * AsyncWebServerRequest::beginResponse(FS &fs, const String& path, const String& contentType, bool download, AwsTemplateProcessor callback){
   //   if(fs.exists(path) || (!download && fs.exists(path+".gz")))
   //     return new AsyncFileResponse(fs, path, contentType, download, callback);
   //   return NULL;
   // }
   // AsyncWebServerResponse *_response = request->beginResponse(SPIFFS, "/index.htm", "text/html", false, _processor);
   // request->send(_response);

   // void AsyncWebServerRequest::send(FS &fs, const String& path, const String& contentType, bool download, AwsTemplateProcessor callback){
   //   if(fs.exists(path) || (!download && fs.exists(path+".gz"))){
   //     send(beginResponse(fs, path, contentType, download, callback));
   //   } else send(404);
   // }

  request->send(SPIFFS, url, String(), false, _processor);

}


// Template processor
// Looks for placeholders in template
// If it meets a placeholder, replace it with a given value
String myWebServerBase::_processor(const String& var) {

  if (MY_DG_WEB) {
    Serial.print("myWebServerBase::_processor(): no condition met. Returning String()\n");
  }
  return String();
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
