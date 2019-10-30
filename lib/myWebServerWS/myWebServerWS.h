/*
  myWebServerWS.h - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWebServerWS_h
#define myWebServerWS_h

#include "Arduino.h"
#include <myWSSender.h>
#include <myWSReceiver.h>

class myWebServerWS
{
  public:
    myWebServerWS();

    static AsyncWebSocket ws; // access at ws://[esp ip]/ws

    static void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};


class myWSServer
{
  public:
    myWSServer();

    void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
    AsyncWebSocket & getWSServer();

  private:
    AsyncWebSocket _webSocketServer;
};

#endif
