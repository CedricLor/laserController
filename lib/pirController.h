/*
pirController.h - Library to store infrared sensor related functions.
Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirController_h
#define pirController_h

#include "Arduino.h"
#include "myMesh.h"

class pirController
{
  public:
    pirController();

    static short const INPUT_PIN;
    static bool valPir;
    static void setPirValue();

    static void switchPirRelays(LaserPin *LaserPins, const bool state);
    static void broadcastPirStatus(const char* state);
    static void stopPirCycle();
    static void startOrRestartPirCycleIfPirValueIsHigh();

    static const int I_PIR_INTERVAL;      // interval in the PIR cycle task (runs every second)
    static const short SI_PIR_ITERATIONS;   // iteration of the PIR cycle

    static Task tPirCntrl;
    static void tcbPirCntrl();

    static Task tPirCycle;
    static bool tcbOnEnablePirCycle();
    static void tcbOnDisablePirCycle();
  private:
};

#endif
