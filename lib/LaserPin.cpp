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

/* PIR SUBJECTION SWITCHES
   When clicking on the "On" or "Off" button on the webpage in the PIR column,
   this function subjects one relay to or releases it from the control of the PIR
   Called from (i) myWebServerController and (ii) this class (LaserPin) */
void LaserPin::inclExclOneRelayInPir(const bool state) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pir_state = state;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOffVariables(HIGH);
}

// Pairs or unpairs two pins together
// Called from LaserPinsArray
void LaserPin::pairUnpairPin(const short thisPin, const bool targetPairingState, const short _pinParityWitness) {
  if (targetPairingState == false) {
    paired = -1;
  } else {
    pairWithNextPin(thisPin, _pinParityWitness);
  }
}

// Pairs two adjacent pins together (adjacent in the LaserPinsArray)
// Called from (i) LaserPinsArray class and (ii) pairUnpairPin
void LaserPin::pairWithNextPin(const short thisPin /* index number of this pin in LaserPinsArray */, const short _pinParityWitness)
{
  const short thePairedPinIndexNumber = (_pinParityWitness == 0) ? thisPin + 1 : thisPin - 1;
  paired = thePairedPinIndexNumber;
}

// Pairs two adjacent pins together (adjacent in the LaserPinsArray)
// Test function; not in use for the moment
void LaserPin::pairWithNextPinPlusOne(const short thisPin /* index number of this pin in LaserPinsArray */, const short _pinQuaternaryWitness)
{
  const short thePairedPinIndexNumber = ((_pinQuaternaryWitness == 0 || _pinQuaternaryWitness == 1)) ? thisPin + 2 : thisPin - 2;
  paired = thePairedPinIndexNumber;
}

/*
  Flexible pairing of pin: this function permits pairing this pin:
  (i) with the next (or the previous) one of index +1 (or -1) or
  (ii) with the next (or the previous) one of a higher index
  The pairing index is defined in the pairingIndex variable
*/
// void LaserPin::flexiblePairPin(const short thisPin /* index number of this pin in LaserPinsArray */, const short pairingIndex)
// {
//   const short thePairedPinIndexNumber = (LaserPinsArray::pinParityWitness == 0) ? thisPin + 1 : thisPin - 1;
//   paired = thePairedPinIndexNumber;
// }

// Changes the blinking delay of a single pin and saves such new blinking delay in Preferences
// Called from (i) LaserPinsArray and (ii) myWebServerController
void LaserPin::changeIndividualBlinkingDelay(const unsigned long blinkingDelay) {
  blinking_interval = blinkingDelay;
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
    digitalWrite(number, on_off_target);
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
  if ((digitalRead(number) == LOW) && ((currentTime - last_time_on > _max_interval_on) || (currentTime - last_time_off <  last_interval_on))) {
    digitalWrite(number, HIGH);
  }
};
