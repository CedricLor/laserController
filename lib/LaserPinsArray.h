/*
  LaserPinsArray.h - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 19, 2019.
*/

#ifndef LaserPinsArray_h
#define LaserPinsArray_h

#include "Arduino.h"

class LaserPinsArray
{
  public:
    LaserPinsArray();

    static void initLaserPins();

    static short int highPinsParityDuringStartup;

    static void manualSwitchAllRelays(const bool targetState);

    static void switchPirRelays(const bool state);

    static short int pinParityWitness;

    static void changeGlobalBlinkingInterval(const unsigned long targetBlinkingInterval);

    static short int pinGlobalModeWitness;   // 0 pirStartUp cycle, 1 IR waiting, 2 IR cycle on, 3 slave cycle on, 4 manual with cycle on, 5 manual with cycle off
    static const char* PIN_GLOBAL_WITNESS_TEXT_DESCRIPTORS[6];
    static char* pinGlobalMode;
};

#endif
