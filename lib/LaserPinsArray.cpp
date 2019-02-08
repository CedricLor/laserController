/*
  LaserPinsArray.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "LaserPinsArray.h"


LaserPinsArray::LaserPinsArray()
{
}

short int LaserPinsArray::pinGlobalModeWitness;
const char* LaserPinsArray::PIN_GLOBAL_WITNESS_TEXT_DESCRIPTORS[6] = {"pirStartUp cycle", "IR waiting", "IR cycle on", "slave cycle on", "manual, in on state", "manual, in off state"};

short LaserPinsArray::pinParityWitness = 0;  // LaserPin::pinParityWitness is a variable that can be used when looping around the pins structs array.
                             // it avoids using the modulo.
                             // by switching it to 0 and 1 at each iteration of the loop
                             // in principle, the switch takes the following footprint: LaserPin::pinParityWitness = (LaserPin::pinParityWitness == 0) ? 1 : 0;
                             // this footprint shall be inserted as the last instruction within the loop (so that it is set to the correct state for the following iteration).
                             // once the loop is over, it should be reset to 0: LaserPin::pinParityWitness = 0;

////////////////////////////////////////////////////////////////////////////////
// INITIALIZE LASER PINS ARRAY
// Called from main.cpp exclusively
void LaserPinsArray::initLaserPins() {
  Serial.print("SETUP: initLaserPins(): starting\n");
  pinParityWitness = 0;
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Initialize Laser Pin
    LaserPins[thisPin].pairUnpairPin(pinParityWitness, 1 /* cooperative pairing */);               // by default, each pin is paired with next (or previous) and in cooperative pairing type
    LaserPins[thisPin].index_number = thisPin;
    pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
    LaserPins[thisPin].physicalInitLaserPin(relayPins[thisPin] /* physical pin number to which this LaserPin is attached */);
  }
  pinParityWitness = 0;
  Serial.print("SETUP: initLaserPins(): done\n");
}

////////////////////////////////////////////////////////////////////////////////
// SWITCHES
// Switches relay pins on and off during PIRStartUp
// Called from pirStartupController exclusively
void LaserPinsArray::irPinsSwitch(const bool targetState) {                     // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisPin = highPinsParityDuringStartup; thisPin < PIN_COUNT; thisPin = thisPin + 2) {        // loop around all the structs representing the pins controlling the relays
    LaserPins[thisPin].switchOnOffVariables(targetState);
  }
}

////////////////////////////////////////////////////////////////////////////////
// MANUAL SWITCHES
// Switches on and off all the lasers
// Manual in the sense that it also switches the pir_state of the pins to LOW (i.e. the pin is no longer reacting to IR signals)
// Called from (i) myMesh, (ii) myWebServerController and (iii) this class (LaserPin)
void LaserPinsArray::manualSwitchAllRelays(const bool targetState) {
  targetState == HIGH ? pinGlobalModeWitness = 4 : pinGlobalModeWitness = 5;      // 4 for "manual with cycle off", 5 for "manual with cycle off"
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].manualSwitchOneRelay(targetState);
  }
}

////////////////////////////////////////////////////////////////////////////////
// PIR SWITCHES
// Switches on and off all the lasers under Pir control
// loop over each of the structs representing pins to turn them on or off (if they are controlled by the PIR)
void LaserPinsArray::switchPirRelays(const bool targetState) {
  Serial.print("PIR: switchPirRelays(const bool state): starting -------\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    if (LaserPins[thisPin].pir_state == HIGH) {
      LaserPins[thisPin].switchOnOffVariables(targetState);
    }
  }
  Serial.print("PIR: switchPirRelays(const bool state): leaving -------\n");
}


////////////////////////////////////////////////////////////////////////////////
// PIR SUBJECTION SWITCHES
// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects or frees all the relays to or of the control of the PIR
// Called from (i) myWebServerController, (ii) pirStartupController and (iii) this class (LaserPin)
void LaserPinsArray::inclExclAllRelaysInPir(const bool targetPirState) {
  if (targetPirState == HIGH) { pinGlobalModeWitness = 1;}                      // 1 for "IR waiting"
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].pir_state = targetPirState;
  }
}

//////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
// Called from:
// (i) pirStartupController
// (ii) myMeshController
// (iii) myWebServerController
// Loops around all the pins and pairs or unpairs them
void LaserPinsArray::pairUnpairAllPins(const short pairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/) {
  pinParityWitness = 0;
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].pairUnpairPin(pinParityWitness, pairingType);
    pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
  }
  pinParityWitness = 0;
}


//////////////////////////////////////////////////////////////////////////
// BLINKING DELAY Control
// Changes the blinking delay of each pin and saves such new blinking delay in Preferences
// Called from:
// (i) myWebServerController;
// (ii) myMeshController.
void LaserPinsArray::changeGlobalBlinkingInterval(const unsigned long targetBlinkingInterval) {
  pinBlinkingInterval = targetBlinkingInterval;
  mySavedPrefs::savePreferences();
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].changeIndividualBlinkingInterval(targetBlinkingInterval);
  }
}

//////////////////////////////////////////////////////////////////////////
// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
short LaserPinsArray::_siSlaveBoxCycleIterations = 60;

bool LaserPinsArray::_tcbOeSlaveBoxCycle() {
  myMeshViews::statusMsg("on");
  manualSwitchAllRelays(LOW);
  pinGlobalModeWitness = 3;      // 3 for "slave cycle on"
  Serial.print("-------- Auto Switch cycle started............ --------\n");
  return true;
}

void LaserPinsArray::_tcbOdSlaveBoxCycle() {
  myMeshViews::statusMsg("off");
  manualSwitchAllRelays(HIGH);
  inclExclAllRelaysInPir(HIGH);     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
}

Task LaserPinsArray::_tSlaveBoxCycle( 1000, _siSlaveBoxCycleIterations, NULL, &userScheduler, false, &_tcbOeSlaveBoxCycle, &_tcbOdSlaveBoxCycle );

void LaserPinsArray::slaveBoxSwitchAllRelays(const bool targetState) {
  if (targetState == LOW) {
    _tSlaveBoxCycle.enable();
    return;
  }
  _tSlaveBoxCycle.disable();
}

// void LaserPinsArray::autoSwitchOneRelay(const short thisPin, const bool targetState) {
//   /*  TO DRAFT PROPERLY WHEN NECESSARY
//       LaserPins[thisPin].switchOnOffVariables(targetState);
//       LaserPins[thisPin].switchPointerBlinkCycleState(targetState);
//       isPirCycleOn = false;
//       Serial.println("auto Switch request executed");
//    */
// }
