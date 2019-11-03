/*
  myWSSender.h - Library to handle web socket responder and sender related functions.
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

class myWSSender
{
  public:
    myWSSender(AsyncWebSocket & __asyncWebSocketInstance);

    void prepareAllIFDataMessage(AsyncWebSocketClient * _client=nullptr);
    void sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client=nullptr);

  private:
    AsyncWebSocket & _asyncWebSocketInstance;
    AsyncWebSocketMessageBuffer * _loadBuffer(JsonObject& _joMsg);
    void _sendMsg(AsyncWebSocketMessageBuffer * _buffer, AsyncWebSocketClient * _client);
};




class myWSResponder
{
  private:
    AsyncWebSocket & _asyncWebSocketInstance;

    StaticJsonDocument<256> _jDoc;
    JsonObject _joMsg = _jDoc.to<JsonObject>();

    bool _checkWhetherUnsignaledNewBox(ControlerBox & _controlerBox);
    bool _checkWhetherUnsignaledDefaultStateChange(ControlerBox & _controlerBox);
    bool _checkWhetherUnsignaledBoxStateChange(ControlerBox & _controlerBox);
    bool _checkWhetherUnsignaledDeletedBox(ControlerBox & _controlerBox);

    void _checkBoxStateAndSendMsgATCMB(uint16_t _ui16BoxIndex, controllerBoxesCollection & _ctlBxColl, myWSSender & _myWSSender);
    void _tcbSendWSDataIfChangeStationIp();
    void _tcbSendWSDataIfChangeBoxState();
    void _resetAllControlerBoxBoolsToTrueByIdNumber(const uint16_t _ui8BoxIndex);
    void _resetAllControlerBoxBoolsToTrueByCB(ControlerBox & _controlerBox);

  public:
    myWSResponder(AsyncWebSocket & __asyncWebSocketInstance);

    Task tSendWSDataIfChangeStationIp;
    Task tSendWSDataIfChangeBoxState;

    void signalNewBox(ControlerBox & _controlerBox);
};

#endif
