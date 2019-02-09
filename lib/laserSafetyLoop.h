/*
  laserSafetyLoop.h - Library to handle the transfer of update to the instances of the LaserPin class into HIGH/LOW states of the pins.
  Created by Cedric Lor, January 4, 2019.
*/

#ifndef laserSafetyLoop_h
#define laserSafetyLoop_h

#include "Arduino.h"

class laserSafetyLoop
{
  public:
    laserSafetyLoop();

    static void endloop();

};

#endif
