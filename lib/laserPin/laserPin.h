/*
  laserPin.h - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, July 19, 2019.
*/

#ifndef laserPin_h
#define laserPin_h

#include "Arduino.h"
#include <global.h>

constexpr short    PIN_COUNT                           = 4;

class laserPin
{
  public:
    laserPin();

    static void initLaserPins();

    static laserPin laserPins[];

    short sIndexNumber;          // index number in the array of laserPins

  private:
    short _sPhysicalPinNumber;   // physical number of the pin controlling the relay
    void _physicalInitLaserPin(const short __sPhysicalPinNumber);
};

#endif
