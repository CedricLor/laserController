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

    static const short SI_PIR_START_UP_DELAY_ITERATIONS;
    static const long L_PIR_START_UP_DELAY;
    static Task tPirStartUpDelayBlinkLaser;

    static void cbtPirStartUpDelayBlinkLaser();
    static bool onEnablePirStartUpDelayBlinkLaser();
    static void onDisablePirStartUpDelayBlinkLaser();

    static Task tPirStartUpDelayPrintDash;
    static void cbtPirStartUpDelayPrintDash();

    static Task tLaserOff;
    static Task tLaserOn;

  private:
    static void cbtLaserOff();
    static void cbtLaserOn();

};

#endif
