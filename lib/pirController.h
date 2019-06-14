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

    static void initPir();

    static Task tPirCntrl;

  private:
    // PIR controller
    static short const INPUT_PIN;
    static bool valPir;

    static void tcbPirCntrl();
    static bool oetcbPirCntrl();

    static void setPirValue();
    static void startOrRestartPirCycleIfPirValueIsHigh();

    // PIR cycle
    static const int I_PIR_INTERVAL;      // interval in the PIR cycle task (runs every second)
    static const short SI_PIR_ITERATIONS;   // iteration of the PIR cycle
    static Task tPirCycle;
    static bool tcbOnEnablePirCycle();
    static void tcbOnDisablePirCycle();

    static void stopPirCycle();

    static void broadcastPirStatus(const char* state);
};

#endif
