/*
  pirStartupController.h - Library to store functions related to the startup of the infrared sensor.
  Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirStartupController_h
#define pirStartupController_h

#include "Arduino.h"

class pirStartupController
{
  public:
    pirStartupController();

    static void cbtPirStartUpDelayBlinkLaser();
    static bool onEnablePirStartUpDelayBlinkLaser();
    static void onDisablePirStartUpDelayBlinkLaser();

    static void cbtPirStartUpDelayPrintDash();

    static Task tLaserOff;
    static Task tLaserOn;

  private:
    static void cbtLaserOff();
    static void cbtLaserOn();

};

#endif
