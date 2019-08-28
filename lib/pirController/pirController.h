/*
  pirController.h - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirController_h
#define pirController_h

#include "Arduino.h"
#include <ControlerBox.h>

class pirController
{
  public:
    pirController(const uint8_t _INPUT_PIN);
    
    void check();

  private:
    const uint8_t _inputPin;
};

extern pirController myPirController;

#endif
