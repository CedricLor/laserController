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
    myWebServerBase();

    static void startAsyncServer();
    static void sendWSData(JsonDocument& doc);

    // static Task tSendWEventData;

  private:
    static AsyncWebServer _asyncServer;
    static AsyncWebSocket _ws; // access at ws://[esp ip]/ws
    static AsyncEventSource _events; // event source (Server-Sent events)

    static JsonObject _empty_obj;

    static Task _tSendWSDataIfChangeStationIp;

    static uint32_t _ws_client_id;
    static void _tcbSendWSData();
    // static void _tcbSendWEventData();
    static void _prepareWSData(const short int _iMessageType, JsonObject& _subdoc=_empty_obj);
    static void _decodeWSMessage(uint8_t *data);

    static void _listAllCollectedHeaders(AsyncWebServerRequest *request);
    static void _listAllCollectedParams(AsyncWebServerRequest *request);

    static String _processor(const String& var);

    static void _onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
    static void _onRequest(AsyncWebServerRequest *request);
    static void _onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

};

#endif
