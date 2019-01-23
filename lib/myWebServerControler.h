/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myWebServerControler_h
#define myWebServerControler_h

#include "Arduino.h"

class myWebServerControler
{
  public:
    myWebServerControler();
    static void decodeRequest(AsyncWebServerRequest *request);

  private:
    static void _webSwitchRelays(AsyncWebParameter* _p2, bool targetState);
    static void _webInclExclRelaysInPir(AsyncWebParameter* _p2, bool targetState);
    static void _webChangeBlinkingInterval(AsyncWebParameter* _p1, AsyncWebParameter* _p2);
};

#endif
