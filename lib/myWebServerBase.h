/*
  myWebServerBase.h - Library web server controller related functions.
  Created by Cedric Lor, January 19, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
  |  |  |
  |  |  |--myWebServerControler.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerControler.h
  |  |  |  |--MasterSlaveBox.cpp (called to set some values re. master/slave box reactions in global)
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp (used to be called to start and stop cycle)
  |  |  |  |--//LaserGroupedUnits.cpp (used to be called to pair and unpair)
  |  |  |
  |  |  |--myWebServerViews.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerViews.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
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

    static void _onRequest(AsyncWebServerRequest *request);
    static void _onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
