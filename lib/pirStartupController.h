/*
  pirStartupController.h - Library to store functions related to the startup of the infrared sensor.
  Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirStartupController_h
#define pirStartupController_h

#include "Arduino.h"
#include "pirController.h"

class pirStartupController
{
  public:
    pirStartupController();

    static Task tPirStartUpDelayBlinkLaser;

  private:
    static const short _SI_PIR_START_UP_DELAY_ITERATIONS;
    static const long _L_PIR_START_UP_DELAY;

    static void _cbtPirStartUpDelayBlinkLaser();
    static bool _onEnablePirStartUpDelayBlinkLaser();
    static void _onDisablePirStartUpDelayBlinkLaser();

    static Task _tPirStartUpDelayPrintDash;
    static void _cbtPirStartUpDelayPrintDash();
};

#endif
