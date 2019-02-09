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

void laserSafetyLoop::loop() {
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
    LaserPins[thisPin].updateLaserPinValuesFromLaserGroupedUnitOwner();
    LaserPins[thisPin].blinkLaserInBlinkingCycle();
    LaserPins[thisPin].executePinStateChange();
    LaserPins[thisPin].laserProtectionSwitch();
  }
  LaserPinsArray::pinParityWitness = 0;
}
