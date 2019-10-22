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

class myWebServerBase
{
  public:
    myWebServerBase();

    static void myWebServerViews(AsyncWebServerRequest *request, const char* url);

  private:
    static AsyncWebServer _asyncServer;

    void _onRequest(AsyncWebServerRequest *request);
    void _onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static String _processor(const String& var);
};

#endif
