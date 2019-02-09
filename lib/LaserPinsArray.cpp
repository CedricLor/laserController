/*
  LaserPinsArray.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "LaserPinsArray.h"

LaserPinsArray::LaserPinsArray()
{
}

const char* LaserPinsArray::PIN_GLOBAL_WITNESS_TEXT_DESCRIPTORS[6] = {"pirStartUp cycle", "IR waiting", "IR cycle on", "slave cycle on", "manual, in on state", "manual, in off state"};

short LaserPinsArray::pinParityWitness = 0;  // LaserPin::pinParityWitness is a variable that can be used when looping around the pins structs array.
                             // it avoids using the modulo.
                             // by switching it to 0 and 1 at each iteration of the loop
                             // in principle, the switch takes the following footprint: LaserPin::pinParityWitness = (LaserPin::pinParityWitness == 0) ? 1 : 0;
                             // this footprint shall be inserted as the last instruction within the loop (so that it is set to the correct state for the following iteration).
                             // once the loop is over, it should be reset to 0: LaserPin::pinParityWitness = 0;

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
