/*
  pirController.h - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirController_h
#define pirController_h

#include "Arduino.h"
#include <controllerBoxesCollection.h>
#include <myMeshViews.h>

class pirController
{
  public:
    pirController(const uint8_t _INPUT_PIN);
    
    void check();

    static void init();

    static Task tSetPirTimeStampAndBrdcstMsg;
    static Task tSpeedBumper;

  private:
    const uint8_t _inputPin;

    static uint16_t _speedBumper;
};

extern pirController myPirController;

#endif
