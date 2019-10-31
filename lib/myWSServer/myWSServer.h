/*
  myWSServer.h - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWSServer_h
#define myWSServer_h

#include "Arduino.h"
#include <myWSSender.h>
#include <myWSReceiver.h>

class myWSServer
{
  private:
    AsyncWebSocket _asyncWebSocketInstance;
    myWSResponder _myWSResponder;

    void _handleEventTypeConnect(AsyncWebSocket * __server, AsyncWebSocketClient * __client);
    void _handleEventTypeDisconnected(AsyncWebSocket * __server, AsyncWebSocketClient * __client);
    void _handleEventTypeError(AsyncWebSocket * __server, AsyncWebSocketClient * __client, void * __arg, uint8_t *__data);
    void _handleEventTypePong(AsyncWebSocket * __server, AsyncWebSocketClient * __client, uint8_t *__data, size_t __len);

  public:
    myWSServer();

    AsyncWebSocket & getAsyncWebSocketInstance();
    myWSResponder & getMyWSResponder();

    void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};

#endif
