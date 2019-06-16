/*
  myWebServerBase.h - Library web server controller related functions.
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

#ifndef myWebServerBase_h
#define myWebServerBase_h

#include "Arduino.h"
#include "AsyncTCP.h"

class myWebServerBase
{
  public:
    myWebServerBase(/*int pin*/);

    static void startAsyncServer();

  private:
    // int _pin;
    static AsyncWebServer _asyncServer;
    static char _linebuf[];

    static void _listAllCollectedHeaders(AsyncWebServerRequest *request);
    static void _listAllCollectedParams(AsyncWebServerRequest *request);

    static void onRequest(AsyncWebServerRequest *request);
    static void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
