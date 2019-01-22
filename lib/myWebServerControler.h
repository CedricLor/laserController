/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myWebServerControler_h
#define myWebServerControler_h

#include "Arduino.h"
#include "LaserPin.h"

class myWebServerControler
{
  public:
    myWebServerControler();
    static void decodeRequest(LaserPin *LaserPins, AsyncWebServerRequest *request);

  private:
    static void _webSwitchRelays(LaserPin *LaserPins, AsyncWebParameter* _p2, bool targetState);
    static void _webInclExclRelaysInPir(LaserPin *LaserPins, AsyncWebParameter* _p2, bool targetState);
    static void _webChangeBlinkingInterval(LaserPin *LaserPins, AsyncWebParameter* _p1, AsyncWebParameter* _p2);
};

#endif
