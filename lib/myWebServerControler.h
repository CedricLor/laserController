/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2007.
*/

#ifndef myWebServerControler_h
#define myWebServerControler_h

#include "Arduino.h"
#include "LaserPin.h"

class myWebServerControler
{
  public:
    myWebServerControler();

    void webSwitchRelays(LaserPin *LaserPins, AsyncWebParameter* _p2, bool targetState);
    void webInclExclRelaysInPir(LaserPin *LaserPins, AsyncWebParameter* _p2, bool targetState);
    void decodeRequest(LaserPin *LaserPins, AsyncWebServerRequest *request);

  private:
};

#endif
