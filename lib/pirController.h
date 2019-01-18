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
  private:
};

#endif
