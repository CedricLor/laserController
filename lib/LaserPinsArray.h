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

    static void endloop();

    static short int highPinsParityDuringStartup;

    static short int pinParityWitness;
};

#endif
