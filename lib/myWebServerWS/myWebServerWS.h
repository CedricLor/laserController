/*
  myWebServerWS.h - Library web socket related functions.
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

*/

#ifndef myWebServerWS_h
#define myWebServerWS_h

#include "Arduino.h"

class myWebServerWS
{
  public:
    myWebServerWS();

    static AsyncWebSocket ws; // access at ws://[esp ip]/ws

    static void sendWSData(JsonDocument& doc);

    static Task _tSendWSDataIfChangeStationIp;
    static Task _tSendWSDataIfChangeBoxState;

  private:

    static JsonObject _empty_obj;

    static uint32_t _ws_client_id;

    static void _tcbSendWSDataIfChangeStationIp();

    static void _tcbSendWSDataIfChangeBoxState();

    static void _prepareWSData(const short int _iMessageType, JsonObject& _subdoc=_empty_obj);
    static void _decodeWSMessage(uint8_t *data);

    static void _onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};

#endif
