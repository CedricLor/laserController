/*
  LaserGroupedUnitsArray.cpp - Library to handle the array of grouped laser units.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "LaserGroupedUnitsArray.h"

LaserGroupedUnit LaserGroupedUnitsArray::LaserGroupedUnits[PIN_COUNT];
// PIN_COUNT? We define the array of LaserGroupUnits as large as the number of pins connected to the lasers.
// Potentially, the LaserGroupedUnit can composed of a single laser. And in such a case, the LaserGroupedUnitsArray shall be able to contain them all.

short LaserGroupedUnitsArray::loadedLaserUnits = 0;

LaserGroupedUnitsArray::LaserGroupedUnitsArray()
{
}

// Cooperative pairing
void LaserGroupedUnitsArray::cooperativePairing() {
  short int _pinParityWitness = 0;
  short int _counter = 0;
  for (short int thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Put each pin into a LaserGroupedUnit
    LaserGroupedUnits[_counter].laserPinId = LaserPins[thisPin].index_number;
    // The following if() is part of the implementation of cooperative pairing (by default, the pin are grouped in LaserGroupedUnit in cooperative pairing);
    if (_pinParityWitness == 1) {
      _counter = _counter + 1;
    }
    // end of the cooperative pairing algorythm
  }
  // The following line is another part of the implementation of cooperative pairing (by default, the pin are grouped in LaserGroupedUnit in cooperative pairing);
  loadedLaserUnits = _counter - 1;
  _pinParityWitness = 0;
}

short int LaserGroupedUnitsArray::pinGlobalModeWitness;
const char* LaserGroupedUnitsArray::PIN_GLOBAL_WITNESS_TEXT_DESCRIPTORS[6] = {"pirStartUp cycle", "IR waiting", "IR cycle on", "slave cycle on", "manual, in on state", "manual, in off state"};


////////////////////////////////////////////////////////////////////////////////
// SWITCHES
// IR STARTUP SWITCH
// Switches relay pins on and off during PIRStartUp
// Called from pirStartupController exclusively
void LaserGroupedUnitsArray::irPinsSwitch(const bool _bTargetState) {                     // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {        // loop around all the structs representing the pins controlling the relays
    LaserGroupedUnits[thisLaserGroupedUnit].switchOnOff(_bTargetState);
  }
}

////////////////////////////////////////////////////////////////////////////////
// MANUAL SWITCH
// Switches on and off all the lasers
// Manual in the sense that it also switches the pir_state of the pins to LOW (i.e. the pin is no longer reacting to IR signals)
// Called from (i) myMesh, (ii) myWebServerController and (iii) this class (LaserPin)
void LaserGroupedUnitsArray::manualSwitchAllRelays(const bool _bTargetState) {
  _bTargetState == HIGH ? pinGlobalModeWitness = 4 : pinGlobalModeWitness = 5;      // 4 for "manual with cycle off", 5 for "manual with cycle off"
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].manualSwitchOneRelay(_bTargetState);
  }
}

////////////////////////////////////////////////////////////////////////////////
// PIR CYCLE SWITCHES
// Switches on and off all the lasers under Pir control
// loop over each of the structs representing pins to turn them on or off (if they are controlled by the PIR)
void LaserGroupedUnitsArray::switchPirRelays(const bool _bTargetState) {
  Serial.print("PIR: switchPirRelays(const bool state): starting -------\n");
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    if (LaserGroupedUnits[thisLaserGroupedUnit].pir_state == HIGH) {
      LaserGroupedUnits[thisLaserGroupedUnit].switchOnOff(_bTargetState);
    }
  }
  Serial.print("PIR: switchPirRelays(const bool state): leaving -------\n");
}


////////////////////////////////////////////////////////////////////////////////
// PIR SUBJECTION SWITCHES
// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects or frees all the relays to or of the control of the PIR
// Called from (i) myWebServerController, (ii) pirStartupController and (iii) this class (LaserPin)
void LaserGroupedUnitsArray::inclExclAllRelaysInPir(const bool _bTargetPirState) {
  if (_bTargetPirState == HIGH) { pinGlobalModeWitness = 1;}                      // 1 for "IR waiting"
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].pir_state = _bTargetPirState;
  }
}

//////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
// Called exclusively from pirStartupController
// Loops around all the pins and pairs or unpairs them
void LaserGroupedUnitsArray::pairUnpairAllPins(const short pairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/) {
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
// Called exclusively from myWebServerController
void LaserGroupedUnitsArray::changeGlobalBlinkingInterval(const unsigned long targetBlinkingInterval) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].changeIndividualBlinkingInterval(targetBlinkingInterval);
    pinBlinkingInterval = targetBlinkingInterval;
    mySavedPrefs::savePreferences();
  }
}

//////////////////////////////////////////////////////////////////////////
// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
short LaserGroupedUnitsArray::_siSlaveBoxCycleIterations = 60;

bool LaserGroupedUnitsArray::_tcbOeSlaveBoxCycle() {
  pinGlobalModeWitness = 3;      // 3 for "slave cycle on"
  myMeshViews::statusMsg("on");
  manualSwitchAllRelays(LOW);
  Serial.print("-------- Auto Switch cycle started............ --------\n");
  return true;
}

void LaserGroupedUnitsArray::_tcbOdSlaveBoxCycle() {
  myMeshViews::statusMsg("off");
  manualSwitchAllRelays(HIGH);
  inclExclAllRelaysInPir(HIGH);     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
}

Task LaserGroupedUnitsArray::_tSlaveBoxCycle( 1000, _siSlaveBoxCycleIterations, NULL, &userScheduler, false, &_tcbOeSlaveBoxCycle, &_tcbOdSlaveBoxCycle );

void LaserGroupedUnitsArray::slaveBoxSwitchAllRelays(const bool targetState) {
  if (targetState == LOW) {
    _tSlaveBoxCycle.enable();
    return;
  }
  _tSlaveBoxCycle.disable();
}

// void LaserGroupedUnitsArray::autoSwitchOneRelay(const short thisPin, const bool targetState) {
//   /*  TO DRAFT PROPERLY WHEN NECESSARY
//       LaserPins[thisPin].switchOnOffVariables(targetState);
//       LaserPins[thisPin].switchPointerBlinkCycleState(targetState);
//       isPirCycleOn = false;
//       Serial.println("auto Switch request executed");
//    */
// }
