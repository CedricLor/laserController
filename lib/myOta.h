/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Myota_h
#define Myota_h

#include "Arduino.h"

class Myota
{
  public:
    Myota();
    static void OTAConfig();
    static void startOTA();
    static void endOTA();
    static void progressOTA(unsigned int progress, unsigned int total);
    static void errorOTA(ota_error_t error);
  private:
    int _pin;
};

#endif
