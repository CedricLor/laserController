/*
  LaserGroupedUnit.cpp - Library to handle the laser grouped units.
  Created by Cedric Lor, February 7, 2019.
*/

#include "Arduino.h"
#include "LaserGroupedUnit.h"


bool const LaserGroupedUnit::_default_laser_group_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const LaserGroupedUnit::_default_laser_group_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
bool const LaserGroupedUnit::_default_laser_group_blinking_state = false;       // by default, pin starts as in a blinking-cycle TO ANALYSE
bool const LaserGroupedUnit::_default_laser_group_pir_state_value = LOW;       // by default, the pin is not controlled by the PIR
const unsigned long LaserGroupedUnit::_max_interval_on = 600000UL;

/* Default constructor: required by the global.cpp
   Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
   Upon creating this array, the pins do not receive any parameters, except the default parameters
*/
LaserGroupedUnit::LaserGroupedUnit()
{
  on_off = _default_laser_group_on_off_state;
  on_off_target = _default_laser_group_on_off_target_state;
  blinking = _default_laser_group_blinking_state;
  pir_state = _default_laser_group_pir_state_value;
  previous_time = millis();
  blinking_interval = pinBlinkingInterval;
  last_time_on = 0;     // set at 0 at startup
  last_time_off = millis();    // set at 0 at startup
  last_interval_on = 0; // set at 0 at startup
}

////////////////////////////////////////////////////////////////////////////////
// SWITCHES
/* This function sets the on_off_target property of this LaserPin (and sets the blinking property of this LaserPin accordingly)
   Called from (i) pirController, (ii) myMesh, (iii) LaserPinsArray and (iii) this class (LaserPin) */
void LaserGroupedUnit::switchOnOff(const bool _bTargetOnOffState) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    if (laserPinIds[thisPin]) {
      LaserPins[laserPinIds[thisPin]].switchOnOffVariables(_bTargetOnOffState);
    }
  }
}

/* This function switches this LaserPin on and off (and sets the pir_state property of this LaserGroupedUnit to LOW)
   It is a manual switch in the sense that, by setting the pir_state of the pins to LOW,
   the pin is no longer reacting to signals sent by the PIR (IR) sensor.
   Called from (i) myWebServerController and (ii) LaserPinsArray */
void LaserGroupedUnit::manualSwitch(const bool _bTargetOnOffState) {
  switchOnOff(_bTargetOnOffState);
  pir_state = LOW;
}

/* PIR SUBJECTION SWITCHES
   When clicking on the "On" or "Off" button on the webpage in the PIR column,
   this function subjects one relay to or releases it from the control of the PIR
   Called from myWebServerController ONLY */
void LaserGroupedUnit::inclExclOneRelayInPir(const bool _bTargetPirState) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = _bTargetPirState;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOff(HIGH);
}

// Pairs or unpairs two pins together
// Called from LaserPinsArray
void LaserGroupedUnit::pairUnpairPin(const short _sPinParityWitness, const short _sPairingType) {
  if (_sPairingType == -1) {
    _unpairPin();
  } else {
    _pairPin(_sPinParityWitness, _sPairingType);
  }
}

// Pairs this pin to another pin and allows to pair in any type of pairing (twin or cooperative)
// Called from pairUnpairPin
void LaserGroupedUnit::_pairPin(const short _pinParityWitness, const short pairingType) {
  _pairing_type = pairingType;
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
void LaserGroupedUnit::_unpairPin() {
  paired_with = -1;
  _pairing_type = -1;
}

// Pairs two adjacent pins together (adjacent in the LaserPinsArray)
// Called from pairUnpairPin
void LaserGroupedUnit::_cooperativePairing(const short _pinParityWitness)
{
  const short thePairedPinIndexNumber = (_pinParityWitness == 0) ? index_number + 1 : index_number - 1;
  paired_with = thePairedPinIndexNumber;
}

void LaserGroupedUnit::_twinPairing(const short _pinParityWitness)
{
  const short thePairedPinIndexNumber = (index_number < (PIN_COUNT / 2)) ? index_number + (PIN_COUNT / 2) : index_number - (PIN_COUNT / 2);
  paired_with = thePairedPinIndexNumber;
}

// Changes the blinking delay of a single pin
// Called from (i) LaserPinsArray and (ii) myWebServerController
void LaserGroupedUnit::changeIndividualBlinkingInterval(const unsigned long targetBlinkingInterval) {
  blinking_interval = targetBlinkingInterval;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Blinking in the class
void LaserGroupedUnit::blinkLaserInBlinkingCycle() {
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
