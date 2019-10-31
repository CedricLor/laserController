/*
  myWSSender.h - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWSSender_h
#define myWSSender_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <ESPAsyncWebServer.h>
#include <globalBasementVars.h>
#include <secret.h>
#include <controllerBoxThis.h>
#include <myWSControllerBox.h>

class myWSSender
{
  public:
    myWSSender(AsyncWebSocket & __server, Task & __tSendWSDataIfChangeStationIp);

    void prepareWSData(const int8_t _i8messageType, AsyncWebSocketClient * _client=nullptr);
    void sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client=nullptr);

  private:
    AsyncWebSocket & _server;
    Task & _tSendWSDataIfChangeStationIp;
};




class myWSSenderTasks
{
  public:
    myWSSenderTasks(AsyncWebSocket & __server);

    Task tSendWSDataIfChangeStationIp;
    Task tSendWSDataIfChangeBoxState;

  private:
    AsyncWebSocket & _server;

    void _tcbSendWSDataIfChangeStationIp();
    void _tcbSendWSDataIfChangeBoxState(AsyncWebSocket & __server);
    void _resetAllControlerBoxBoolsToTrue(const uint16_t _ui8BoxIndex);
};




class myWSResponder
{
  private:
    AsyncWebSocket & _server;

  public:
    myWSResponder(AsyncWebSocket & __server);

    void prepareWSData(const int8_t _i8messageType, AsyncWebSocketClient * _client=nullptr);
    void sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client=nullptr);
    
    myWSSenderTasks _thisWSSenderTasks;
};











#endif
