/*
  LaserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "LaserPin.h"


bool const LaserPin::_default_pin_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const LaserPin::_default_pin_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
bool const LaserPin::_default_pin_blinking_state = false;       // by default, pin starts as in a blinking-cycle TO ANALYSE
bool const LaserPin::_default_pin_pir_state_value = LOW;       // by default, the pin is not controlled by the PIR
const unsigned long LaserPin::_max_interval_on = 600000UL;

/* Default constructor: required by the global.cpp
   Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
   Upon creating this array, the pins do not receive any parameters, except the default parameters
*/
// Instanciated in global.cpp
LaserPin::LaserPin()
{
  on_off = _default_pin_on_off_state;
  on_off_target = _default_pin_on_off_target_state;
  blinking = _default_pin_blinking_state;
  previous_time = millis();
  blinking_interval = pinBlinkingInterval;
  pir_state = _default_pin_pir_state_value;
  last_time_on = 0;     // set at 0 at startup
  last_time_off = millis();    // set at 0 at startup
  last_interval_on = 0; // set at 0 at startup
}

////////////////////////////////////////////////////////////////////////////////
// INITIALIZE LASER PINS
// Called from LaserPinsArray::initLaserPins() only
void LaserPin::physicalInitLaserPin(const short _sPhysicalPinNumber)
{
  _physical_pin_number = _sPhysicalPinNumber;
  pinMode(_sPhysicalPinNumber, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(_sPhysicalPinNumber, HIGH);  // setting default value of the pins at HIGH (relay closed)
}

////////////////////////////////////////////////////////////////////////////////
// SWITCHES
/* Switches the blinking state of the pin
   Called from this class (LaserPin::switchOnOffVariables) only*/
void LaserPin::_switchPointerBlinkCycleState(const bool _bTargetBlinkingState) {
  /* If the _bTargetBlinkingState passed on to the function is LOW,
     this function sets the blinking property of this LaserPin as true,
     meaning it is in a blinking cycle.
     If the targetBlinkingState passed on to the function is LOW,
     this function sets the blinking property of this LaserPin as false,
     meaning it is no longer a blinking cycle. */
  (_bTargetBlinkingState == LOW) ? blinking = true : blinking = false;
}

////////////////////////////////////////////////////////////////////////////////////////////
/* UPDATE VALUES OF THIS LASERPIN WITH VALUES FROM ITS LASER GROUPED UNIT OWNER
   Blinks the laser when the laser is in blinking cycle. Called from (i) laserSafetyLoop::loop()
*/
void LaserPin::updateLaserPinValuesFromLaserGroupedUnitOwner() {
  // Read the value of the blinking_interval of the LaserGroupedUnit to which this pin pertains
  // and update its own blinking interval
  blinking_interval = LaserGroupedUnits[laserGroupedUnitId].currentBlinkingInterval;
}

////////////////////////////////////////////////////////////////////////////////////////////
/* BLINK LASER IN BLINKING CYCLE
   Blinks the laser when the laser is in blinking cycle.
   Called from (i) laserSafetyLoop::loop().
*/
void LaserPin::blinkLaserInBlinkingCycle() {
  /*
    Checks:
    1. if the pin is in blinking mode
    2. if so, if the blinking interval of this laser has elapsed
    If both conditions are fullfilled, switches the pin on/off target variable to the contrary of the current pin on/off
    TO ANALYSE: this may overwrite other changes that have been requested at other stages
  */
  if (blinking == true) {
    const unsigned long _ulCurrentTime = millis();
    if (_ulCurrentTime - previous_time > blinking_interval) {
        on_off_target = !on_off;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////
// IO FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////
// Execute Updates
/* Called from:
   - laserSafetyLoop::loop()
   for each pin.
*/
void LaserPin::executePinStateChange() {
  /*
    Checks whether the current on_off_target state is different than the current on_off state
    If so:
    1. turn on or off the laser as requested in the on_off_target_state
    2. align the on_off state with the on_off_target state
    3. reset the safety blinking timer of this pin
    // TO ANALYSE: I have the feeling that the condition to be tested shall be different
    // in the case a) a laser is in a blinking cycle and in the case b) a laser is not in
    // a blinking cycle and cooling off
  */
  if (on_off != on_off_target) {
    _markTimeChanges();
    digitalWrite(_physical_pin_number, on_off_target);
    on_off = on_off_target;
  }
}

// Helper function for execute updates
// Private function
void LaserPin::_markTimeChanges() {
  const unsigned long currentTime = millis();
  previous_time = currentTime;

  // If instruction is to turn laserPin on
  if (on_off_target == LOW) {
    last_time_off = currentTime;
    return;
  }
  // If instruction is to turn laserPin off
  last_time_on = currentTime;
  last_interval_on = last_time_on - last_time_off;
}

// Laser Protection Switch
// Function to protect the lasers from staying on over 60 seconds or being turned on again before a 60 seconds delay after having been turned off
/* Called from:
   - laserSafetyLoop::loop()
*/
void LaserPin::laserProtectionSwitch() {
  const unsigned long currentTime = millis();
  // OLD DRAFTING --
  // if ((digitalRead(_physical_pin_number) == LOW) && ((currentTime - last_time_on > _max_interval_on) || (currentTime - last_time_off <  last_interval_on))) {
  // digitalWrite(_physical_pin_number, HIGH);
  // PROPOSED REDRAFTING --
  // if ((digitalRead(_physical_pin_number) == LOW) && ((currentTime - last_time_off > _max_interval_on) || (currentTime - last_time_on <  last_interval_on))) {
  // digitalWrite(_physical_pin_number, HIGH);
  if (digitalRead(_physical_pin_number) == LOW) {
    if (currentTime - last_time_off > _max_interval_on) {
      digitalWrite(_physical_pin_number, HIGH);
      return;
    }
    if (currentTime - last_time_on < last_interval_on) {
      digitalWrite(_physical_pin_number, HIGH);
    }
  }
}
