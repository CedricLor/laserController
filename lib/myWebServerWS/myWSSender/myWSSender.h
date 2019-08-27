/*
  myWSSender.h - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.

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
  |  |  |
  |  |  |--myWebServerWS
  |  |  |  |--myWebServerWS.cpp
  |  |  |  |  |--myWebServerWS.h
  |  |  |  |--myWSReceiver
  |  |  |  |  |--myWSReceiver.cpp
  |  |  |  |  |  |--myWSReceiver.H
  |  |  |  |--myWSSender
  |  |  |  |  |--myWSSender.cpp
  |  |  |  |  |  |--myWSSender.H

*/

#ifndef myWSSender_h
#define myWSSender_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <mns.h>
#include <secret.h>
#include <ControlerBox.h>
#include "../myWebServerWS/myWebServerWS.h"

class myWSSender
{
  public:
    myWSSender();

    void prepareWSData(const int8_t _i8messageType, AsyncWebSocketClient * _client=nullptr);
    void sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client=nullptr);

    static Task tSendWSDataIfChangeStationIp;
    static Task tSendWSDataIfChangeBoxState;

    static AsyncWebSocket * server;
    
  private:

    static void _tcbSendWSDataIfChangeStationIp();

    static void _tcbSendWSDataIfChangeBoxState();

    static void _resetAllControlerBoxBoolsToTrue(const uint16_t _ui8BoxIndex);
};

#endif
