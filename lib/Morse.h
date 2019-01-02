/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Morse_h
#define Morse_h

#include "Arduino.h"

class Morse
{
  public:
    Morse();
    void dot();
    void dash();
    void startota();
    void endota();
    void progressota(unsigned int progress, unsigned int total);
    void errorota(ota_error_t error);
  private:
    int _pin;
};

#endif
