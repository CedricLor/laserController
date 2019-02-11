/*
  LaserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "LaserPin.h"


bool const LaserPin::_default_pin_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const LaserPin::_default_pin_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
const unsigned long LaserPin::_max_interval_on = 600000UL;

/* Default constructor: required by the global.cpp
   Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
   Upon creating this array, the pins do not receive any parameters, except the default parameters
*/
// Instanciated in global.cpp
LaserPin::LaserPin()
{
  _on_off = _default_pin_on_off_state;
  _on_off_target = _default_pin_on_off_target_state;
  _last_time_on = 0;             // set at 0 at startup
  _last_time_off = millis();     // set at current time at startup
  _last_interval_on = 0;         // set at 0 at startup
}

////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZE LASER PINS
/////////////////////////////////////////
// Called from LaserPinsArray::initLaserPins() only
void LaserPin::physicalInitLaserPin(const short _sPhysicalPinNumber)
{
  _physical_pin_number = _sPhysicalPinNumber;
  pinMode(_sPhysicalPinNumber, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(_sPhysicalPinNumber, HIGH);  // setting default value of the pins at HIGH (relay closed)
}

////////////////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE GETTERS
/////////////////////////////////////////
// PUBLIC GETTERS
bool LaserPin::blinking() {
  return LaserGroupedUnits[laserGroupedUnitId].currentOnOffState;
}

unsigned long LaserPin::blinkingInterval() {
  if (!(LaserGroupedUnits[laserGroupedUnitId].isIdenticalToArray)) {
    return LaserGroupedUnits[laserGroupedUnitId].currentBlinkingInterval;
  }
  return LaserGroupedUnitsArray::currentPinBlinkingInterval;
}

short LaserPin::pairedWith() {
  for (short _iteratorsIndex = 0; _iteratorsIndex < PIN_COUNT; _iteratorsIndex++) {
    if (LaserPins[_iteratorsIndex].index_number == index_number) {
      return LaserPins[_iteratorsIndex].index_number;
    }
  }
  return -1;
}

bool LaserPin::pirState() {
  return LaserGroupedUnits[laserGroupedUnitId].currentPirState;
}

/////////////////////////////////////////
// PRIVATE GETTERS
unsigned long LaserPin::_previousTime() {
  return (_last_time_on > _last_time_off) ? _last_time_on : _last_time_off;
}

////////////////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE SETTERS
////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE SETTERS
// Helper function for executePinStateChange
// Private function
void LaserPin::_markTimeChanges() {
  const unsigned long __currentTime = millis();

  // If instruction is to turn laserPin on
  if (_on_off_target == LOW) {
    _last_time_off = __currentTime;
    return;
  }
  // If instruction is to turn laserPin off
  _last_time_on = __currentTime;
  _last_interval_on = _last_time_on - _last_time_off;
}

////////////////////////////////////////////////////////////////////////////////////////////
/* BLINK LASER IN BLINKING CYCLE
   Blinks the laser when the laser is in blinking cycle.
   Called from (i) laserSafetyLoop::loop().
*/
void LaserPin::setOnOffTarget() {
  /*
    Checks:
    1. if the pin is in blinking mode
    2. if so, if the blinking interval of this laser has elapsed
    If both conditions are fullfilled, switches the pin on/off target variable to the contrary of the current pin on/off
    TO ANALYSE: this may overwrite other changes that have been requested at other stages
  */
  if (blinking()) {
    const unsigned long _ulCurrentTime = millis();
    if (_ulCurrentTime - _previousTime() > blinkingInterval()) {
        _on_off_target = !_on_off;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// IO FUNCTIONS
/////////////////////////////////////////
// Execute Updates
/* Called from:
   - laserSafetyLoop::loop()
   for each pin.
*/
void LaserPin::executePinStateChange() {
  /*
    Checks whether the current _on_off_target state is different than the current _on_off state
    If so:
    1. turn on or off the laser as requested in the _on_off_target_state
    2. align the _on_off state with the _on_off_target state
    3. reset the safety blinking timer of this pin
    // TO ANALYSE: I have the feeling that the condition to be tested shall be different
    // in the case a) a laser is in a blinking cycle and in the case b) a laser is not in
    // a blinking cycle and cooling off
  */
  if (_on_off != _on_off_target) {
    _markTimeChanges();
    digitalWrite(_physical_pin_number, _on_off_target);
    _on_off = _on_off_target;
  }
}

/////////////////////////////////////////
// Laser Protection Switch
// Function to protect the lasers from staying on over 60 seconds or being turned on again before a 60 seconds delay after having been turned off
/* Called from:
   - LaserPinsArray::loop()
*/
void LaserPin::laserProtectionSwitch() {
  const unsigned long __currentTime = millis();
  // OLD DRAFTING --
  // if ((digitalRead(_physical_pin_number) == LOW) && ((currentTime - _last_time_on > _max_interval_on) || (currentTime - _last_time_off <  _last_interval_on))) {
  // digitalWrite(_physical_pin_number, HIGH);
  // PROPOSED REDRAFTING --
  // if ((digitalRead(_physical_pin_number) == LOW) && ((currentTime - _last_time_off > _max_interval_on) || (currentTime - _last_time_on <  _last_interval_on))) {
  // digitalWrite(_physical_pin_number, HIGH);
  if (digitalRead(_physical_pin_number) == LOW) {
    if (__currentTime - _last_time_off > _max_interval_on) {
      digitalWrite(_physical_pin_number, HIGH);
      return;
    }
    if (__currentTime - _last_time_on < _last_interval_on) {
      digitalWrite(_physical_pin_number, HIGH);
    }
  }
}
