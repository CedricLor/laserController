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
    myOta(int pin);
    void dot();
    void dash();
  private:
    int _pin;
};

#endif
