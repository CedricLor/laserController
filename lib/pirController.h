/*
  pirController.h - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirController_h
#define pirController_h

#include "Arduino.h"

class pirController
{
  public:
    pirController();

    static void initPir();

    static void pirCntrl();

  private:
    // PIR controller
    static short const _INPUT_PIN;

};

#endif
