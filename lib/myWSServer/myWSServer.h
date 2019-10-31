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
    void _handleSingleFrameMessage(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsFrameInfo * __info, uint8_t *__data, size_t __len);
    void _handleMultipleFrameMessage(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsFrameInfo * __info, uint8_t *__data, size_t __len);
    void _handleEventTypeData(AsyncWebSocket * __server, AsyncWebSocketClient * __client, void * __arg, uint8_t *__data, size_t __len);

  public:
    myWSServer();

    AsyncWebSocket & getAsyncWebSocketInstance();
    myWSResponder & getMyWSResponder();

    void onEvent(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsEventType __type, void * __arg, uint8_t *__data, size_t __len);
};

#endif
