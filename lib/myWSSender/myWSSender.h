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
    myWSSender(AsyncWebSocket & __server);

    void prepareAllIFDataMessage(AsyncWebSocketClient * _client=nullptr);
    void sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client=nullptr);

  private:
    AsyncWebSocket & _server;
};




class myWSSenderTasks
{
  public:
    myWSSenderTasks(AsyncWebSocket & __asyncWebSocketInstance);

    Task tSendWSDataIfChangeStationIp;
    Task tSendWSDataIfChangeBoxState;

  private:
    AsyncWebSocket & _asyncWebSocketInstance;

    void _tcbSendWSDataIfChangeStationIp();
    void _tcbSendWSDataIfChangeBoxState();
    void _resetAllControlerBoxBoolsToTrue(const uint16_t _ui8BoxIndex);
};




class myWSResponder
{
  private:
    AsyncWebSocket & _asyncWebSocketInstance;
    myWSSenderTasks _myWSSenderTasks;

  public:
    myWSResponder(AsyncWebSocket & __asyncWebSocketInstance);

    myWSSenderTasks & getMyWSSenderTasks();

    void prepareWSData(const int8_t _i8messageType, AsyncWebSocketClient * _client=nullptr);
    void sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client=nullptr);    
};











#endif
