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

#ifndef myWebServerBase_h
#define myWebServerBase_h

#include "Arduino.h"
#include "AsyncTCP.h"
#include "myWebServerViews.cpp"
#include "myWebServerControler.cpp"

class myWebServerBase
{
  public:
    myWebServerBase(/*int pin*/);

    static void startAsyncServer();

  private:
    static AsyncWebServer _asyncServer;

    static void _listAllCollectedHeaders(AsyncWebServerRequest *request);
    static void _listAllCollectedParams(AsyncWebServerRequest *request);

    static String _processor(const String& var);

    static void _onRequest(AsyncWebServerRequest *request);
    static void _onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif
