/*
  myWebServer.h - Library to handle laser controller web server.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myWebServer_h
#define myWebServer_h

#include "Arduino.h"
#include "LaserPin.h"
// #include "LaserPin.cpp"

class myWebServer
{
  public:
    myWebServer(LaserPin *LaserPins, unsigned long pinBlinkingInterval);
  private:
    unsigned long _pinBlinkingInterval;
    LaserPin *_LaserPins;

    String printCurrentStatus(const short thisPin);
    String printOnOffControl(const short thisPin);
    String printPirStatusCntrl(const short thisPin);
    String printBlinkingDelayWebCntrl(const short thisPin);
    String printPairingCntrl(const short thisPin);
    String printDelaySelect(const short thisPin);
    String printHiddenLaserNumb(const short thisPin);
};

#endif
