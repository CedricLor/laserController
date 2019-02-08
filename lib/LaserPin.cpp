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
void LaserPin::physicalInitLaserPin(const short physicalPinNumber)
{
  physical_pin_number = physicalPinNumber;
  pinMode(physicalPinNumber, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(physicalPinNumber, HIGH);  // setting default value of the pins at HIGH (relay closed)
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

/* This function sets the on_off_target property of this LaserPin (and sets the blinking property of this LaserPin accordingly)
   Called from:
   (i) LaserPinsArray ((a) LaserPinsArray::irPinsSwitch and (b) LaserPinsArray::switchPirRelays);
   (ii) this class ((a) LaserPin::manualSwitchOneRelay and (b) LaserPin::inclExclOneRelayInPir);
   (iii) LaserGroupedUnit (LaserGroupedUnit::switchOnOff) */
void LaserPin::switchOnOffVariables(const bool _bTargetOnOffState) {
  _switchPointerBlinkCycleState(_bTargetOnOffState);                                                                        // turn the on_off_target state of the struct on or off; the actual pin will be turned on or off in the LASER SAFETY TIMER
  ((index_number > paired_with) && (_pairing_type == 1)) ? (on_off_target = !_bTargetOnOffState): (on_off_target = _bTargetOnOffState);
}


// TO BE DELETED ONCE EVERYTHING RELATING TO THE LASER PINS HAS BEEN REDRAFTED
/* This function switches this LaserPin on and off (and sets the pir_state property of this LaserPin to LOW)
   It is a manual switch in the sense that, by setting the pir_state of the pins to LOW,
   the pin is no longer reacting to signals sent by the PIR (IR) sensor.
   Called from:
   (i) myWebServerController (myWebServerControler::_webSwitchRelays); and
   (ii) LaserPinsArray (LaserPinsArray::manualSwitchAllRelays(const bool targetState)) */
void LaserPin::manualSwitchOneRelay(const bool _bTargetOnOffState) {
  // Serial.printf("MANUAL SWITCHES: manualSwitchOneRelay(const short thisPin, const bool targetState): switching LaserPins[%u] to targetState %s\n", thisPin, (targetState == 0 ? ": on" : ": off"));      // MIGHT CAUSE A BUG!!!
  switchOnOffVariables(_bTargetOnOffState);
  pir_state = LOW; // To be deleted because will be handled at the level of LaserGroupedUnit;
}

/* PIR SUBJECTION SWITCHES
   When clicking on the "On" or "Off" button on the webpage in the PIR column,
   this function subjects one relay to or releases it from the control of the PIR
   Called from myWebServerController ONLY */
void LaserPin::inclExclOneRelayInPir(const bool targetPirState) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = targetPirState;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOffVariables(HIGH);
}

// Pairs or unpairs two pins together
// Called from LaserPinsArray
void LaserPin::pairUnpairPin(const short _pinParityWitness, const short _sPairingType) {
  if (_sPairingType == -1) {
    _unpairPin();
  } else {
    _pairPin(_pinParityWitness, _sPairingType);
  }
}

// Pairs this pin to another pin and allows to pair in any type of pairing (twin or cooperative)
// Called from pairUnpairPin
void LaserPin::_pairPin(const short _pinParityWitness, const short _sPairingType) {
  _pairing_type = _sPairingType;
  if (_pairing_type == 1) {
    _cooperativePairing(_pinParityWitness);
    return;
  }
  if (_pairing_type == 0) {
    _twinPairing(_pinParityWitness);
    return;
  }
}

// Unpair a pin from other pins
// Called from pairUnpairPin
void LaserPin::_unpairPin() {
  paired_with = -1;
  _pairing_type = -1;
}

// Pairs two adjacent pins together (adjacent in the LaserPinsArray)
// Called from pairUnpairPin
void LaserPin::_cooperativePairing(const short _pinParityWitness)
{
  const short thePairedPinIndexNumber = (_pinParityWitness == 0) ? index_number + 1 : index_number - 1;
  paired_with = thePairedPinIndexNumber;
}

void LaserPin::_twinPairing(const short _pinParityWitness)
{
  const short thePairedPinIndexNumber = (index_number < (PIN_COUNT / 2)) ? index_number + (PIN_COUNT / 2) : index_number - (PIN_COUNT / 2);
  paired_with = thePairedPinIndexNumber;
}

// Changes the blinking delay of a single pin
// Called from (i) LaserPinsArray and (ii) myWebServerController
void LaserPin::changeIndividualBlinkingInterval(const unsigned long targetBlinkingInterval) {
  blinking_interval = targetBlinkingInterval;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Blinking in the class
void LaserPin::blinkLaserInBlinkingCycle() {
  /*
    Checks:
    1. if the pin is in blinking
    2. if so, if the blinking interval of this laser has elapsed
    If both conditions are fullfilled, switches the pin on/off target variable to the contrary of the current pin on/off
    TO ANALYSE: this may overwrite other changes that have been requested at other stages
  */
  if (blinking == true) {
    const unsigned long currentTime = millis();
    if (currentTime - previous_time > blinking_interval) {
        on_off_target = !on_off;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////
// IO Functions
////////////////////////////////////////////////////////////////////////////////////////////
// Execute Updates
void LaserPin::executePinStateChange() {
  /*
      Called from within the laser safety loop for each pin
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
    digitalWrite(physical_pin_number, on_off_target);
    on_off = on_off_target;
  }
}

// Helper function for execute updates
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
void LaserPin::laserProtectionSwitch() {
  const unsigned long currentTime = millis();
  // OLD DRAFTING --
  // if ((digitalRead(physical_pin_number) == LOW) && ((currentTime - last_time_on > _max_interval_on) || (currentTime - last_time_off <  last_interval_on))) {
  // digitalWrite(physical_pin_number, HIGH);
  // PROPOSED REDRAFTING --
  // if ((digitalRead(physical_pin_number) == LOW) && ((currentTime - last_time_off > _max_interval_on) || (currentTime - last_time_on <  last_interval_on))) {
  // digitalWrite(physical_pin_number, HIGH);
  if (digitalRead(physical_pin_number) == LOW) {
    if (currentTime - last_time_off > _max_interval_on) {
      digitalWrite(physical_pin_number, HIGH);
      return;
    }
    if (currentTime - last_time_on < last_interval_on) {
      digitalWrite(physical_pin_number, HIGH);
    }
  }
}
