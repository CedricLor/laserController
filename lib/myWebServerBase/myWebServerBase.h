/*
  myWebServerBase.h - Library web server controller related functions.
  Created by Cedric Lor, January 19, 2019.
*/

#ifndef myWebServerBase_h
#define myWebServerBase_h

#include "Arduino.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <myWebServerWS.h>

class myWebServerBase : public AsyncWebServer
{
  public:
    myWebServerBase(uint16_t port=80);

    uint8_t getStatus();

    myWSServer _myWSServer;

  private:

    void _setWebSocketHandler();
    void _setGetRequestHandlers();
    void _setDefaultHandlers();

    void _onRequest(AsyncWebServerRequest *request);
    void _onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
