/*
  myWebServerWS.h - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWebServerWS_h
#define myWebServerWS_h

#include "Arduino.h"
#include <myWSSender.h>
#include <myWSReceiver.h>

class myWSServer
{
  private:
    AsyncWebSocket _asyncWebSocketInstance;
    myWSResponder _myWSResponder;

  public:
    myWSServer();

    AsyncWebSocket & getAsyncWebSocketInstance();
    myWSResponder & getMyWSResponder();

    void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};

#endif
