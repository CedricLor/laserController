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
    AsyncWebSocketMessageBuffer * _loadBuffer(JsonObject& _joMsg);
    void _sendMsg(AsyncWebSocketMessageBuffer * _buffer, AsyncWebSocketClient * _client);
    AsyncWebSocket & _server;
};




class myWSResponder
{
  private:
    AsyncWebSocket & _asyncWebSocketInstance;

    void _checkBoxStateAndSendMsgATMB(uint16_t _ui16BoxIndex, controllerBoxesCollection & _ctlBxColl, myWSSender & _myWSSender);
    void _tcbSendWSDataIfChangeStationIp();
    void _tcbSendWSDataIfChangeBoxState();
    void _resetAllControlerBoxBoolsToTrue(const uint16_t _ui8BoxIndex);

  public:
    myWSResponder(AsyncWebSocket & __asyncWebSocketInstance);

    Task tSendWSDataIfChangeStationIp;
    Task tSendWSDataIfChangeBoxState;

};











#endif
