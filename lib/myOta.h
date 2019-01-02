/*
  myOta.h - Library for storing OTA related code in laserController.
  Created by Cedric Lor, January 1, 2019.
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
