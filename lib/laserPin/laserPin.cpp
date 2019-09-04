/*
  laserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, July 19, 2019.
*/

#include "Arduino.h"
#include "laserPin.h"



laserPin laserPin::_laserPins[PIN_COUNT];




laserPin::laserPin() { }





laserPin::laserPin(const short __s_index_number, const short __s_physical_pin_number):
  _sIndexNumber(__s_index_number),
  _sPhysicalPinNumber(__s_physical_pin_number)
{ 
  pinMode(_sPhysicalPinNumber, OUTPUT);  // initialization of the pin connected to each of the relay as output
  _switchPin(HIGH);                       // setting default value of the pins at HIGH (relay closed)
}





void laserPin::_initLaserPins() {
  Serial.print("SETUP: _initLaserPins(): starting\n");
  for (short __i = 0; __i < PIN_COUNT; __i++) {
    _laserPins[__i] = {__i, relayPins[__i]};
  }
  Serial.print("SETUP: _initLaserPins(): done.\n");
}





void laserPin::_switchPin(const bool __b_target_state) {
  digitalWrite(_sPhysicalPinNumber, __b_target_state);
}