// /*
//   LaserGroupedUnit.cpp - Library to handle the laser grouped units.
//   Created by Cedric Lor, February 7, 2019.
// */
//
// #include "Arduino.h"
// #include "LaserGroupedUnit.h"
//
//
// bool const LaserGroupedUnit::_default_laser_group_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
// bool const LaserGroupedUnit::_default_laser_group_pir_state_value = LOW;       // by default, the pin is not controlled by the PIR
// const unsigned long LaserGroupedUnit::_max_interval_on = 600000UL;
//
// /* Default constructor: required by the global.cpp
//    Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
//    Upon creating this array, the pins do not receive any parameters, except the default parameters
// */
// LaserGroupedUnit::LaserGroupedUnit()
// {
//   currentOnOffState = _default_laser_group_on_off_state;
//   previousOnOffState = _default_laser_group_on_off_state;
//   targetOnOffState = _default_laser_group_on_off_state;
//
//   currentPirState = _default_laser_group_pir_state_value;
//   previousPirState = _default_laser_group_pir_state_value;
//   targetPirState = _default_laser_group_pir_state_value;
//
//   currentBlinkingInterval = pinBlinkingInterval;
//   previousBlinkingInterval = pinBlinkingInterval;
//   targetBlinkingInterval = pinBlinkingInterval;
// }
//
// ////////////////////////////////////////////////////////////////////////////////
// // STATE MACHINE SETTERS
// // setOnOffTargetState
// /* This function sets the currentOnOffState property of the LaserGroupedUnit.
//    Corresponds to LaserPin::switchOnOffVariables
//    which is called from:
//    (i) LaserPinsArray ((a) LaserPinsArray::switchPirRelays);
//    (ii) LaserPin itself (private calls) ((a) LaserPin::manualSwitchOneRelay;
//    (iii) LaserGroupedUnit (LaserGroupedUnit::switchOnOff)
//    LaserPin::switchOnOffVariables sets the blinking property of its LaserPin. This was not retaken here as a LaserGroupUnit has not blinking property.
//    The blinking property of the LaserPins was invented mark them on and off during the cycles.
// */
// void LaserGroupedUnit::setOnOffTargetState(const bool _bTargetOnOffState) {
//   previousOnOffState = currentOnOffState;
//   targetOnOffState = _bTargetOnOffState;
// }
//
// void LaserGroupedUnit::setTargetPirState(const short int __sTargetPirState){
//   previousPirState = currentPirState;
//   targetPirState = __sTargetPirState;
// }
//
// void LaserGroupedUnit::setTargetBlinkingInterval(const unsigned long _ulTargetBlinkingInterval){
//   previousBlinkingInterval = currentBlinkingInterval;
//   targetBlinkingInterval = _ulTargetBlinkingInterval;
// }
//
// void LaserGroupedUnit::updateCurrentStates() {
//   currentOnOffState = targetOnOffState;
//   currentPirState = targetPirState;
//   currentBlinkingInterval = targetBlinkingInterval;
// }
// ////////////////////////////////////////////////////////////////////////////////
// // SWITCHES
// /* MANUAL SWITCH
//    This function switches this LaserGroupUnit on and off (and sets the targetPirState property of this LaserGroupedUnit to LOW)
//    It is a manual switch in the sense that, by setting the pir_state of the pins to LOW,
//    the pin is no longer reacting to signals sent by the PIR (IR) sensor.
//    Corresponds to LaserPin::manualSwitchOneRelay,
//    wihch is called from (i) myWebServerController and (ii) LaserPinsArray
// */
// void LaserGroupedUnit::_manualSwitch(const bool _bTargetOnOffState) {
//   setOnOffTargetState(_bTargetOnOffState);
//   targetPirState = LOW;
// }
//
// /* PIR SUBJECTION SWITCH
//    When clicking on the "PIR On" or "PIR Off" button of the corresponding LaserGroupedUnit,
//    this function will subject this LaserGroupUnit to or releases it from the control of the PIR.
//    Corresponds to the former LaserPin::inclExclOneRelayInPir,
//    which was called from myWebServerController (myWebServerControler::_webInclExclRelaysInPir) ONLY
// */
// void LaserGroupedUnit::_inclExclInPir(const bool _bTargetPirState) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
//   targetPirState = _bTargetPirState;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
//   setOnOffTargetState(HIGH);
// }
//
// /* BLINKING INTERVAL CONTROL
//    Changes the blinking interval of this LaserGroupedUnit.
//    Corresponds to the former LaserPin::changeIndividualBlinkingInterval,
//    which was called from:
//    (i) LaserPinsArray (LaserPinsArray::changeGlobalBlinkingInterval); and
//    (ii) myWebServerController (myWebServerControler::_webChangeBlinkingInterval).
//    This function is called from:
//    (i) laserPinsArray (LaserGroupedUnitsArray::changeBlinkingIntervalAll);
//    (ii) myWebServerController (myWebServerControler::_webChangeBlinkingInterval);
// */
// void LaserGroupedUnit::_changeBlinkingInterval(const unsigned long _ulTargetBlinkingInterval) {
//   targetBlinkingInterval = _ulTargetBlinkingInterval;
// }
