/*
  laserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, July 19, 2019.
*/

#include "Arduino.h"
#include "laserPin.h"



laserPin laserPin::laserPins[4];
// laserPin laserPin::laserPins[PIN_COUNT];




laserPin::laserPin()
{
}




void laserPin::_physicalInitLaserPin(const short __sPhysicalPinNumber)
{
  _sPhysicalPinNumber = __sPhysicalPinNumber;
  pinMode(__sPhysicalPinNumber, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(__sPhysicalPinNumber, HIGH);  // setting default value of the pins at HIGH (relay closed)
}




void laserPin::initLaserPins() {
  Serial.print("SETUP: initLaserPins(): starting\n");
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    laserPins[__thisPin].sIndexNumber = __thisPin;
    laserPins[__thisPin]._physicalInitLaserPin(relayPins[__thisPin] /* physical pin number to which this LaserPin is attached */);
  }
  Serial.print("SETUP: initLaserPins(): done.\n");
}
