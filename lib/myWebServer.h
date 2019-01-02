/*
  myWebServer.h - Library to handle laser controller web server.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myWebServer_h
#define myWebServer_h

#include "Arduino.h"
#include "LaserPin.h"
#include "LaserPin.cpp"

class myWebServer
{
  public:
    myWebServer(LaserPin pin, unsigned long pinBlinkingInterval);
  private:
    unsigned long _pinBlinkingInterval;
    LaserPin _pin;

    String printBlinkingDelayWebCntrl(const short thisPin);
    String printDelaySelect(const short thisPin);
    String printHiddenLaserNumb(const short thisPin);
};

#endif
