/*
  myWebServerViews.h - Library to serve the views of the laser controller web server.
  Created by Cedric Lor, January 2, 2019.

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

myWebServerViews contains the webPage being displayed by the webServer.

*/

#ifndef myWebServerViews_h
#define myWebServerViews_h

#include "Arduino.h"
#include <ESPAsyncWebServer.h>
#include <global.h>
#include <ControlerBox.h>

class myWebServerViews
{
  public:
    myWebServerViews(AsyncWebServerRequest *request, const char* url);

  private:
    static String _processor(const String& var);
};

#endif
