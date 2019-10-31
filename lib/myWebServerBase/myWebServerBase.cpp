/*
  myWebServerBase.cpp - Library to handle web server controller related functions.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "myWebServerBase.h"


myWebServerBase::myWebServerBase(uint16_t port):
  AsyncWebServer(port),
  _myWSServer()
{
  /** if this box has a web interface */
  if (globalBaseVariables.hasInterface) {

    _setWebSocketHandler();
    _setGetRequestHandlers();
    _setDefaultHandlers();
  }
}




AsyncWebSocket & myWebServerBase::getAsyncWebSocketInstance() {
  return _myWSServer.getAsyncWebSocketInstance();
}




Task & myWebServerBase::getTSendWSDataIfChangeStationIp() {
  return _myWSServer._myWSResponder._thisWSSenderTasks.tSendWSDataIfChangeStationIp;
}




Task & myWebServerBase::getTSendWSDataIfChangeBoxState() {
  return _myWSServer._myWSResponder._thisWSSenderTasks.tSendWSDataIfChangeBoxState;
}




uint8_t myWebServerBase::getStatus() {
  return _server.status();
}  




/** void myWebServerBase::_setWebSocketHandler()
 * 
 *  add the AsyncWebSocket instance _webSocketServer as handler 
 *  to the webserver.
 * */
void myWebServerBase::_setWebSocketHandler() {
  // addHandler(&myWebServerWS::ws);
  addHandler(&_myWSServer.getAsyncWebSocketInstance());
}




void myWebServerBase::_setGetRequestHandlers() {
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
}




void myWebServerBase::_setDefaultHandlers() {
  onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });

  onRequestBody([this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    this->_onBody(request, data, len, index, total);
  });
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
