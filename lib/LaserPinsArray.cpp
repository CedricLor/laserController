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
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Initialize Laser Pin
    LaserPins[thisPin].index_number = thisPin;
    LaserPins[thisPin].physicalInitLaserPin(relayPins[thisPin] /* physical pin number to which this LaserPin is attached */);
  }
  LaserGroupedUnitsArray::pairUnpairAllPins(1); // cooperatively pair all the pins
  Serial.print("SETUP: initLaserPins(): done\n");
}

////////////////////////////////////////////////////////////////////////////////
// END LOOP
// One call located in the loop() function of main.cpp
void LaserPinsArray::endloop() {
  // Loop around each struct representing a pin connected to a laser before restarting a global loop and
  // make any update that may be required. For instance:
  // - safety time elapsed of lasers in blinking cycle (blinking every 10 to 30 s., to avoid burning the lasers);
  // - update the paired laser or its pair if the lasers are paired;
  // and then, execute the updates.
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    /* The order of the following function matters:
    - updateLaserPinValuesFromLaserGroupedUnitOwner is called first as it updates the LaserPin states based on data read into its parent LaserGroupedUnit
    - blinkLaserInBlinkingCycle is called second as it is not an IO function; it only updates the state of the LaserPin state machine;
    however, it relies on values updated by updateLaserPinValuesFromLaserGroupedUnitOwner.
    - executePinStateChange is called second as it executes pinChanges (IO function).
    - laserProtectionSwitch comes last, as it is just an additional check at the end that turns off lasers in case of bug.
    */
    // LaserPins[thisPin].updateLaserPinValuesFromLaserGroupedUnitOwner();
    LaserPins[thisPin].blinkLaserInBlinkingCycle();
    LaserPins[thisPin].executePinStateChange();
    LaserPins[thisPin].laserProtectionSwitch();
  }
}
