/*
  laserSafetyLoop.cpp - Library to handle the transfer of update to the instances of the LaserPin class into HIGH/LOW states of the pins.
  Created by Cedric Lor, January 4, 2019.
*/

#include "Arduino.h"
#include "laserSafetyLoop.h"

laserSafetyLoop::laserSafetyLoop()
{
  // pinMode(pin, OUTPUT);
  // _pin = pin;
}

// LASER SAFETY TIMER -- EXECUTED AT THE END OF EACH LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
