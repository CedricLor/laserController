/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef myOta_h
#define myOta_h

#include "Arduino.h"

class myOta
{
  public:
    myOta();
    void OTAConfig();
    void startOTA();
    void endOTA();
    void progressOTA();
    void errorOTA();
  private:
    int _pin;
};

#endif
