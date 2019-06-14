/*
  LaserPinsArray.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "LaserPinsArray.h"

LaserPinsArray::LaserPinsArray()
{
}

////////////////////////////////////////////////////////////////////////////////
// INITIALIZE LASER PINS ARRAY
// Called from main.cpp exclusively
void LaserPinsArray::initLaserPins() {
  Serial.print("SETUP: initLaserPins(): starting\n");
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Initialize Laser Pin
    LaserPins[__thisPin].index_number = __thisPin;
    LaserPins[__thisPin].physicalInitLaserPin(relayPins[__thisPin] /* physical pin number to which this LaserPin is attached */);
  }
  // LaserGroupedUnitsArray::pairUnpairAllPins(1); // cooperatively pair all the pins
  Serial.print("SETUP: initLaserPins(): done\n");
}

////////////////////////////////////////////////////////////////////////////////
// END LOOP
// Called once, in the loop() function of main.cpp
void LaserPinsArray::endloop() {
  // LaserGroupedUnitsArray::updateLaserGroupUnits();
  /* Loop around each LaserPin representing a pin connected to a relay that
     controls a laser before restarting a global loop and makes any update that may be required. For instance:
     - safety time elapsed of lasers in blinking cycle (blinking every 10 to 30 s., to avoid burning the lasers);
     - update the paired laser or its pair if the lasers are paired;
     and then, execute the updates. */
  // updateLaserPins();
}

void LaserPinsArray::updateLaserPins() {
  /* The order of the following function matters:
  - setOnOffTarget is called first; it is not an IO function; it only updates the states of the LaserPin state machine;
  however, it relies on values updated by updateLaserPinValuesFromLaserGroupedUnitOwner.
  - updateLGUState is called second .
  - executePinStateChange is called third as it executes pinChanges (IO function).
  - laserProtectionSwitch comes last, as it is just an additional check at the end that turns off lasers in case of bug. */
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    LaserPins[__thisPin].setOnOffTarget();
    LaserPins[__thisPin].updateLGUState();
    LaserPins[__thisPin].executePinStateChange();
    LaserPins[__thisPin].laserProtectionSwitch();
  }
}
