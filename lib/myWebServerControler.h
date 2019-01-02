/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2007.
*/

#ifndef myWebServerControler_h
#define myWebServerControler_h

#include "Arduino.h"

class myWebServerControler
{
  public:
    myWebServerControler(int pin);
    void dot();
    void dash();
  private:
    int _pin;
};

#endif
