/*
  LaserGroupedUnit.cpp - Library to handle the laser grouped units.
  Created by Cedric Lor, February 7, 2019.
*/

#include "Arduino.h"
#include "LaserGroupedUnit.h"


bool const LaserGroupedUnit::_default_laser_group_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const LaserGroupedUnit::_default_laser_group_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
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
  pir_state = _default_laser_group_pir_state_value;
  previous_time = millis();
  blinking_interval = pinBlinkingInterval;
  last_time_on = 0;     // set at 0 at startup
  last_time_off = millis();    // set at 0 at startup
  last_interval_on = 0; // set at 0 at startup
}

////////////////////////////////////////////////////////////////////////////////
// SWITCHES
// SwitchOnOff
/* This function sets the on_off_target property of the LaserPins which are included in this LaserGroupUnit
   (sidenote: the called function sets the blinking property of this LaserPin accordingly).
   Corresponds to LaserPin::switchOnOffVariables
   which is called from:
   (i) LaserPinsArray ((a) LaserPinsArray::irPinsSwitch and (b) LaserPinsArray::switchPirRelays);
   (ii) LaserPin itself (private calls) ((a) LaserPin::manualSwitchOneRelay and (b) LaserPin::inclExclOneRelayInPir);
   (iii) LaserGroupedUnit (LaserGroupedUnit::switchOnOff)
*/
// --> should be redrafted; should not interact directly with the LaserPins (none of the other functions do it.
// Should be limited to setting a variable that will be read by a reader/setter of laserPins
void LaserGroupedUnit::switchOnOff(const bool _bTargetOnOffState) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    if (LaserPins[thisPin].laserGroupedUnitId == index_number) {
      LaserPins[thisPin].switchOnOffVariables(_bTargetOnOffState);
    }
  }
}

/* manualSwitch
   This function switches this LaserGroupUnit on and off (and sets the pir_state property of this LaserGroupedUnit to LOW)
   It is a manual switch in the sense that, by setting the pir_state of the pins to LOW,
   the pin is no longer reacting to signals sent by the PIR (IR) sensor.
   Corresponds to LaserPin::manualSwitchOneRelay,
   wihch is called from (i) myWebServerController and (ii) LaserPinsArray
*/
void LaserGroupedUnit::manualSwitch(const bool _bTargetOnOffState) {
  switchOnOff(_bTargetOnOffState);
  pir_state = LOW;
}

/* PIR SUBJECTION SWITCHES
   When clicking on the "PIR On" or "PIR Off" button of the corresponding LaserGroupedUnit,
   this function will subject this LaserGroupUnit to or releases it from the control of the PIR.
   Corresponds to LaserPin::inclExclOneRelayInPir,
   which is called from myWebServerController (myWebServerControler::_webInclExclRelaysInPir) ONLY
*/
void LaserGroupedUnit::inclExclInPir(const bool _bTargetPirState) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = _bTargetPirState;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOff(HIGH);
}

/* PIR SUBJECTION SWITCHES
   Changes the blinking interval of this LaserGroupedUnit.
   Corresponds to LaserPin::changeIndividualBlinkingInterval,
   which is called from (i) LaserPinsArray and (ii) myWebServerController
*/
void LaserGroupedUnit::changeBlinkingInterval(const unsigned long targetBlinkingInterval) {
  blinking_interval = targetBlinkingInterval;
}
