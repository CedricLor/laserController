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

    static short int pinParityWitness;

    static const char* PIN_GLOBAL_WITNESS_TEXT_DESCRIPTORS[6];
    static char* pinGlobalMode;
};

#endif
