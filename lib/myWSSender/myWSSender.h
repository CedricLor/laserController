/*
  myWSSender.h - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWSSender_h
#define myWSSender_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <globalBasementVars.h>
#include <secret.h>
#include <ControlerBox.h>

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
