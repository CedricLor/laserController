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
void LaserPin::physicalInitLaserPin(const short __sPhysicalPinNumber)
{
  _physical_pin_number = __sPhysicalPinNumber;
  pinMode(_physical_pin_number, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(_physical_pin_number, HIGH);  // setting default value of the pins at HIGH (relay closed)
}

////////////////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE GETTERS
/////////////////////////////////////////
// PUBLIC GETTERS
bool LaserPin::_isLGUOn() {
  return !(LaserGroupedUnits[laserGroupedUnitId].targetOnOffState);
}

bool LaserPin::_LGUHasChanged() {
  return (LaserGroupedUnits[laserGroupedUnitId].targetOnOffState == LaserGroupedUnits[laserGroupedUnitId].currentOnOffState
  && LaserGroupedUnits[laserGroupedUnitId].targetBlinkingInterval == LaserGroupedUnits[laserGroupedUnitId].currentBlinkingInterval
  && LaserGroupedUnits[laserGroupedUnitId].targetPirState == LaserGroupedUnits[laserGroupedUnitId].currentPirState);
}

bool LaserPin::_amIlastInLGU() {
  return (index_number > pairedWith());
}

unsigned long LaserPin::blinkingInterval() {
  return LaserGroupedUnits[laserGroupedUnitId].targetBlinkingInterval;
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
/* SET ON / OFF STATE
   Called from (i) laserPinsArray::loop().
*/
void LaserPin::setOnOffTarget() {

  // First (I), check whether the LGU to which this LP pertains is "on".
  // If the LGU is "off", check whether this LP is also "off"
  // If the LP is not "off", turn it off and exit
  if ((!(_isLGUOn())) && (_on_off == LOW)) {                        // (I) if the LGU to which this LP pertains is "off" and this LP is "on"
    _on_off_target = HIGH;                                          // (I) turn off the LP
    IamBlinking = false;                                            // (I) take the LP out of a blinking cycle
    return;                                                         // (I) exit
  }

  // Second (II): if the LGU to which this pins pertains is "on".
  // check whether this LP is in a blinking cycle.
  //
  if (IamBlinking) {                                                // (IIA) is this LaserPin blinking (= is it already aware that the LGU is "on"?)?
    const unsigned long _ulCurrentTime = millis();                  // (IIA) get current time
    if (_ulCurrentTime - _previousTime() > blinkingInterval()) {      // (IIB) if the timer for this LP has elapsed
      _on_off_target = !_on_off;                                      // (IIB) Switch on/off state of LP
      return;                                                         // (IIB) exit
    }
    return;                                                         // (IIB) if the timer has not elapsed, do nothing and exit
  }

  // Third (III): if the LGU is "on" and this LP is not in a blinking cycle.
  // check whether this LP is paired or in twin pairing.
  // if unpaired or in twin pairing, turn this LP "on".
  short __pairedPinId = pairedWith();
  if ((__pairedPinId == -1) || (LaserGroupedUnits[laserGroupedUnitId].pairing_type == 0)) {
    _on_off_target = LOW;                                           // (III) turn this LP "on"
    IamBlinking = true;                                             // (III) include this LP in the blinking cycle
    return;                                                         // (III) exit
  }                                                                 // (III) continue if this LP is paired in cooperative pairing

  // Fourth (IV): if the LGU is "on", this LP is not in a blinking cycle and is in cooperative pairing,
  // light the coolest of the lasers "on" (or the first one in the pair of lasers if they are both at the same level)
  if (_last_time_on < LaserPins[__pairedPinId]._last_time_on) {
    _on_off_target = LOW;                                           // (IV) turn this LP on
    IamBlinking = true;                                             // (IV) include this LP in the blinking cycle
    return;                                                         // (IV) exit
  }
  if ((_last_time_on == LaserPins[__pairedPinId]._last_time_on) && (index_number < __pairedPinId)) {
    _on_off_target = LOW;                                           // (IV) turn this LP on
    IamBlinking = true;                                             // (IV) include this LP in the blinking cycle
    return;                                                         // (IV) exit
  }
  IamBlinking = true;                                               // (IV) include this LP in the blinking cycle
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LGU STATE SETTER
void LaserPin::updateLGUState() {
    if (_amIlastInLGU()) {
      LaserGroupedUnits[laserGroupedUnitId].updateCurrentStates();
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
