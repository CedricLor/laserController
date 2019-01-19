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


/* Default constructor: required by the global.cpp
   Upon initialization of the board, we create an array of LaserPins without which will be later initialized.
   Upon creating this array, the pins do not receive any parameters, except the default parameters
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
// Called from LaserPinsArray
void LaserPin::initLaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */)
{
  number = pinNumber;
  short pairedPinNumber = (thisPin % 2 == 0) ? (thisPin + 1) : (thisPin - 1);
  paired = pairedPinNumber;
}

void LaserPin::physicalInitLaserPin()
{
  pinMode(number, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(number, HIGH);  // setting default value of the pins at HIGH (relay closed)
}

////////////////////////////////////////////////////////////////////////////////
// SWITCHES
/* Switches relay pins on and off
   Called from (i) pirController, (ii) myMesh, (iii) LaserPinsArray and (iii) this class (LaserPin) */
void LaserPin::switchOnOffVariables(const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: switchOnOffVariables(const short thisPin, const bool targetState): switching on/off variables for LaserPins[%u] with targetState = %s \n", thisPin, (targetState == 0 ? "on (LOW)" : "off (HIGH)"));
  switchPointerBlinkCycleState(targetState);                        // turn the blinking state of the struct representing the pin on or off
  on_off_target = targetState;                                               // turn the on_off_target state of the struct on or off
                                                                             // the actual pin will be turned on or off in the LASER SAFETY TIMER
}

/* Switches the blinking state of the pin
   Called from (i) myMesh and (ii) this class (LaserPin)*/
void LaserPin::switchPointerBlinkCycleState(const bool state) {
  /* If the state passed on to the function is LOW (i.e.
     probably the targetState in the calling function),
     marks that the pin is in a blinking cycle.
     If not, marks that the blinking cycle for this pin is off. */
  (state == LOW) ? blinking = true : blinking = false;
}

/* Switches on and off a single laser
   Manual in the sense that it also switches the pir_state of the pins to LOW (i.e. the pin is no longer reacting to IR signals)
   Called from (i) myWebServerController and (ii) this class (LaserPin) */
void LaserPin::manualSwitchOneRelay(const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: manualSwitchOneRelay(const short thisPin, const bool targetState): switching LaserPins[%u] to targetState %s\n", thisPin, (targetState == 0 ? ": on" : ": off"));      // MIGHT CAUSE A BUG!!!
  switchOnOffVariables(targetState);
  pir_state = LOW;
}

//////////////////////////////////////////////////////////////////////////
// PIR SUBJECTION SWITCHES
// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects or frees all the relays to or of the control of the PIR
// Called from (i) myWebServerController, (ii) pirStartupController and (iii) this class (LaserPin)
void LaserPin::inclExclAllRelaysInPir(LaserPin *LaserPins, const bool state) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].pir_state = state;
  }
}

// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects one relay to or releases it from the control of the PIR
// Called from (i) myWebServerController and (ii) this class (LaserPin)
void LaserPin::inclExclOneRelayInPir(const bool state) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = state;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOffVariables(HIGH);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
// Called exclusively from pirStartupController
// Loops around all the pins and pairs or unpairs them
void LaserPin::pairAllPins(LaserPin *LaserPins, const bool targetPairingState /*This variable is equal to TRUE or FALSE; TRUE is pair all the pins; FALSE is unpair all the pins.*/) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].pairPin(LaserPins, thisPin, targetPairingState);
    pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
  }
  pinParityWitness = 0;
}


// Pairs or unpairs two pins together
// Private function: called exclusively by LaserPin::pairAllPins
void LaserPin::pairPin(LaserPin *LaserPins, const short thisPin, const bool targetPairingState) {
  const short thePairedPin = (pinParityWitness == 0) ? thisPin + 1 : thisPin - 1;
  if (targetPairingState == false) {
    _rePairPin(LaserPins, 8, 8);
  } else {
    _rePairPin(LaserPins, thisPin, thePairedPin);
  }
}

// Helper function for LaserPin::_pairPin
// Private function: called exclusively by LaserPin::_pairPin
void LaserPin::_rePairPin(LaserPin *LaserPins, const short thisPin, const short thePairedPin) {
  paired = thePairedPin;
  LaserPins[thePairedPin].paired = thisPin;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// BLINKING DELAY Control
// Changes the blinking delay of each pin and saves such new blinking delay in Preferences
// Called exclusively from myWebServerController
void LaserPin::changeGlobalBlinkingDelay(LaserPin *LaserPins, const unsigned long blinkingDelay) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].changeTheBlinkingInterval(blinkingDelay);
    pinBlinkingInterval = blinkingDelay;
    mySavedPrefs::savePreferences();
  }
}

// Changes the blinking delay of a single pin and saves such new blinking delay in Preferences
// Called exclusively from myWebServerController
void LaserPin::changeIndividualBlinkingDelay(const unsigned long blinkingDelay) {
  changeTheBlinkingInterval(blinkingDelay);
}

// Changes the blinking delay of a single pin and saves such new blinking delay in Preferences
// Private function: called from (i) changeGlobalBlinkingDelay and (ii) changeIndividualBlinkingDelay
void LaserPin::changeTheBlinkingInterval(const unsigned long blinkingDelay) {
  blinking_interval = blinkingDelay;
}
