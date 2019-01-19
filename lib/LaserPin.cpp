/*
  LaserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "LaserPin.h"

/* Default constructor: required by the global.cpp
   Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
   Upon creating this array, the pins do not receive any parameters
*/
LaserPin::LaserPin()
{
  on_off = _default_pin_on_off_state;
  on_off_target = _default_pin_on_off_target_state;
  blinking = _default_pin_blinking_state;
  previous_time = millis();
  blinking_interval = pinBlinkingInterval;
  pir_state = _default_pin_pir_state_value;
}


short LaserPin::pinParityWitness = 0;  // LaserPin::pinParityWitness is a variable that can be used when looping around the pins structs array.
                             // it avoids using the modulo.
                             // by switching it to 0 and 1 at each iteration of the loop
                             // in principle, the switch takes the following footprint: LaserPin::pinParityWitness = (LaserPin::pinParityWitness == 0) ? 1 : 0;
                             // this footprint shall be inserted as the last instruction within the loop (so that it is set to the correct state for the following iteration).
                             // once the loop is over, it should be reset to 0: LaserPin::pinParityWitness = 0;

////////////////////////////////////////////////////////////////////////////////
// INITIALIZE LASER PINS
void LaserPin::_initLaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */)
{
  number = pinNumber;
  short pairedPinNumber = (thisPin % 2 == 0) ? (thisPin + 1) : (thisPin - 1);
  paired = pairedPinNumber;
}

void LaserPin::_physicalInitLaserPin()
{
  pinMode(number, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(number, HIGH);  // setting default value of the pins at HIGH (relay closed)
}

void LaserPin::initLaserPins(LaserPin *LaserPins) {
  Serial.print("SETUP: initLaserPins(): starting\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Initialize Laser Pin
    LaserPins[thisPin]._initLaserPin(relayPins[thisPin], thisPin);
    LaserPins[thisPin]._physicalInitLaserPin();
  }
  Serial.print("SETUP: initLaserPins(): done\n");
}

//////////////////////////////////////////////////////////////////////////
// SWITCHES
// Switches relay pins on and off during PIRStartUp
void LaserPin::directPinsSwitch(LaserPin *LaserPins, const bool targetState) {              // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisPin = highPinsParityDuringStartup; thisPin < PIN_COUNT; thisPin = thisPin + 2) {        // loop around all the structs representing the pins controlling the relays
    LaserPins[thisPin].switchOnOffVariables(targetState);
  }
}

void LaserPin::switchOnOffVariables(const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: switchOnOffVariables(const short thisPin, const bool targetState): switching on/off variables for LaserPins[%u] with targetState = %s \n", thisPin, (targetState == 0 ? "on (LOW)" : "off (HIGH)"));
  switchPointerBlinkCycleState(targetState);                        // turn the blinking state of the struct representing the pin on or off
  on_off_target = targetState;                                               // turn the on_off_target state of the struct on or off
                                                                             // the actual pin will be turned on or off in the LASER SAFETY TIMER
}

void LaserPin::switchPointerBlinkCycleState(const bool state) {
  // If the state passed on to the function is LOW (i.e.
  // probably the targetState in the calling function),
  // marks that the pin is in a blinking cycle.
  // If not, marks that the blinking cycle for this pin is off.
  (state == LOW) ? blinking = true : blinking = false;
}

//////////////////////////////////////////////////////////////////////////
// MANUAL SWITCHES
// Manually switches all the lasers
void LaserPin::switchAllRelays(LaserPin *LaserPins, const bool state) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].manualSwitchOneRelay(state);
  }
}

// Manually switches a single laser
void LaserPin::manualSwitchOneRelay(const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: manualSwitchOneRelay(const short thisPin, const bool targetState): switching LaserPins[%u] to targetState %s\n", thisPin, (targetState == 0 ? ": on" : ": off"));      // MIGHT CAUSE A BUG!!!
  switchOnOffVariables(targetState);
  pir_state = LOW;
}

//////////////////////////////////////////////////////////////////////////
// PIR SUBJECTION SWITCHES
// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects or frees all the relays to or of the control of the PIR
void LaserPin::inclExclAllRelaysInPir(LaserPin *LaserPins, const bool state) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].pir_state = state;
  }
}

// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects one relay to or releases it from the control of the PIR
void LaserPin::inclExclOneRelayInPir(const bool state) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = state;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOffVariables(HIGH);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
void LaserPin::pairAllPins(LaserPin *LaserPins, const bool targetPairingState /*This variable is equal to TRUE or FALSE; TRUE is pair all the pins; FALSE is unpair all the pins.*/) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].pairPin(LaserPins, thisPin, targetPairingState);
    pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
  }
  pinParityWitness = 0;
}

void LaserPin::pairPin(LaserPin *LaserPins, const short thisPin, const bool targetPairingState) {
  const short thePairedPin = (pinParityWitness == 0) ? thisPin + 1 : thisPin - 1;
  if (targetPairingState == false) {
    rePairPin(LaserPins, 8, 8);
  } else {
    rePairPin(LaserPins, thisPin, thePairedPin);
  }
}

void LaserPin::rePairPin(LaserPin *LaserPins, const short thisPin, const short thePairedPin) {
  paired = thePairedPin;
  LaserPins[thePairedPin].paired = thisPin;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BLINKING DELAY Control
void LaserPin::changeGlobalBlinkingDelay(LaserPin *LaserPins, const unsigned long blinkingDelay) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].changeTheBlinkingInterval(blinkingDelay);
    pinBlinkingInterval = blinkingDelay;
    mySavedPrefs::savePreferences();
  }
}

void LaserPin::changeIndividualBlinkingDelay(const unsigned long blinkingDelay) {
  changeTheBlinkingInterval(blinkingDelay);
}

void LaserPin::changeTheBlinkingInterval(const unsigned long blinkingDelay) {
  blinking_interval = blinkingDelay;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
short LaserPin::_siAutoSwitchInterval = 60;

Task LaserPin::tAutoSwitchAllRelays( 1000, LaserPin::_siAutoSwitchInterval, NULL, &userScheduler, false, &LaserPin::_tcbOaAutoSwitchAllRelays, &LaserPin::_tcbOdAutoSwitchAllRelays );

bool LaserPin::_tcbOaAutoSwitchAllRelays() {
  LaserPin::switchAllRelays(LaserPins, LOW);
  Serial.print("-------- Auto Switch cycle started............ --------\n");
  return true;
}

void LaserPin::_tcbOdAutoSwitchAllRelays() {
  LaserPin::switchAllRelays(LaserPins, HIGH);
  LaserPin::inclExclAllRelaysInPir(LaserPins, HIGH);     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
}
