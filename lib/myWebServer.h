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
    myWebServer(LaserPin *LaserPins, unsigned long pinBlinkingInterval, const short PIN_COUNT, short iSlaveOnOffReaction);
  private:
    unsigned long _pinBlinkingInterval;
    LaserPin *_LaserPins;
    short _PIN_COUNT;
    
    short _iSlaveOnOffReaction;
    const char* _slaveReaction[4] = {"synchronous: on - on & off - off", "opposed: on - off & off - on", "always on: off - on & on - on", "always off: on - off & off - off"};
    const char* _slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};

    String printSlaveReactionSelect();
    String printLabel(const String labelText, const String labelFor);
    String printOption(const String optionValue, const String optionText, const String selected);
    String printIndivLaserCntrls();
    String printCurrentStatus(const short thisPin);
    String printOnOffControl(const short thisPin);
    String printPirStatusCntrl(const short thisPin);
    String printBlinkingDelayWebCntrl(const short thisPin);
    String printPairingCntrl(const short thisPin);
    String printDelaySelect(const short thisPin);
    String printHiddenLaserNumb(const short thisPin);
};

#endif
