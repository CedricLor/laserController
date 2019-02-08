/*
  LaserGroupedUnit.cpp - Library to handle the laser grouped units.
  Created by Cedric Lor, February 7, 2019.
*/

#include "Arduino.h"
#include "LaserGroupedUnit.h"


bool const LaserGroupedUnit::_default_laser_group_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const LaserGroupedUnit::_default_laser_group_pir_state_value = LOW;       // by default, the pin is not controlled by the PIR
const unsigned long LaserGroupedUnit::_max_interval_on = 600000UL;

/* Default constructor: required by the global.cpp
   Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
   Upon creating this array, the pins do not receive any parameters, except the default parameters
*/
LaserGroupedUnit::LaserGroupedUnit()
{
  on_off = _default_laser_group_on_off_state;
  pir_state = _default_laser_group_pir_state_value;
  blinking_interval = pinBlinkingInterval;
}

////////////////////////////////////////////////////////////////////////////////
// SWITCHES
// SWITCH ON OFF
/* This function sets the on_off property of the LaserGroupedUnit.
   Corresponds to LaserPin::switchOnOffVariables
   which is called from:
   (i) LaserPinsArray ((a) LaserPinsArray::irPinsSwitch and (b) LaserPinsArray::switchPirRelays);
   (ii) LaserPin itself (private calls) ((a) LaserPin::manualSwitchOneRelay and (b) LaserPin::inclExclOneRelayInPir);
   (iii) LaserGroupedUnit (LaserGroupedUnit::switchOnOff)
   LaserPin::switchOnOffVariables sets the blinking property of its LaserPin. This was not retaken here as a LaserGroupUnit has not blinking property.
   The blinking property of the LaserPins was invented mark them on and off during the cycles.
*/
void LaserGroupedUnit::switchOnOff(const bool _bTargetOnOffState) {
  /*******************************/
  /* Once this function will have fully replaced LaserPin::switchOnOffVariables, replace it by setting the value of on_off directly */
  /*******************************/
  on_off = _bTargetOnOffState;
  /**************************************************************************/
  /* Former drafting: the LaserGroupedUnit was setting values in LaserPins. */
  /* Kept for reference purpose to be used in a future LaserGroupedUnit reader that should iterate around all pins to read the parent value. */
  /**************************************************************************/
  // for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
  //   if (LaserPins[thisPin].laserGroupedUnitId == index_number) {
  //     LaserPins[thisPin].switchOnOffVariables(_bTargetOnOffState);
  //   }
  // }
}

/* MANUAL SWITCH
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

/* PIR SUBJECTION SWITCH
   When clicking on the "PIR On" or "PIR Off" button of the corresponding LaserGroupedUnit,
   this function will subject this LaserGroupUnit to or releases it from the control of the PIR.
   Corresponds to LaserPin::inclExclOneRelayInPir,
   which is called from myWebServerController (myWebServerControler::_webInclExclRelaysInPir) ONLY
*/
void LaserGroupedUnit::inclExclInPir(const bool _bTargetPirState) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = _bTargetPirState;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOff(HIGH);
}

/* BLINKING INTERVAL CONTROL
   Changes the blinking interval of this LaserGroupedUnit.
   Corresponds to LaserPin::changeIndividualBlinkingInterval,
   which is called from (i) LaserPinsArray and (ii) myWebServerController
*/
void LaserGroupedUnit::changeBlinkingInterval(const unsigned long _ulTargetBlinkingInterval) {
  blinking_interval = _ulTargetBlinkingInterval;
}
