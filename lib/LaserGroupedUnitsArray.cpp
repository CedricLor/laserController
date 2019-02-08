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

// Pairing
void LaserGroupedUnitsArray::pairing() {
  short int _pinParityWitness = 0;
  short int _groupUnitsCounter = 0;
  for (short int thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Put each pin into a LaserGroupedUnit
    LaserPins[thisPin].laserGroupedUnitId = LaserGroupedUnits[_groupUnitsCounter].index_number;
    // increment the counter of laserUnits effectively loaded
    // note: the loadedLaserUnits variable will then be used below, by irStartupSwitch for instance, to limit the iterations over the LaserGroupedUnits
    loadedLaserUnits = _groupUnitsCounter - 1;
    // if _pinParityWitness is equal to 1, it means that we already have 2 lasers in the LaserGroupUnit.
    // We then need to store the next pins in a new LaserGroupUnit.
    // For this purpose, we increment the _groupUnitsCounter if _pinParityWitness is equal 1 (i.e. the current pin is the second pin stored in the LaserGroupedUnit).
    // At the next iteration of this foor loop, the index_number of another LaserGroupedUnit will be marked in the next instance of LaserPins
    // Sidenote: For the moment, I hard code this 2 laser limit, but this will need to be changed in the future.
    if (_pinParityWitness == 1) {
      _groupUnitsCounter = _groupUnitsCounter + 1;
    }
    // Switch _pinParityWitness between 1 and
    _pinParityWitness = (_pinParityWitness == 0) ? 1 : 0;
  }
  _pinParityWitness = 0;
}

short int LaserGroupedUnitsArray::globalModeWitness;
const char* LaserGroupedUnitsArray::GLOBAL_WITNESS_TEXT_DESCRIPTORS[6] = {"pirStartUp cycle", "IR waiting", "IR cycle on", "slave cycle on", "manual, in on state", "manual, in off state"};


////////////////////////////////////////////////////////////////////////////////
// SWITCHES
// IR STARTUP SWITCH
// Switches relay pins on and off during PIRStartUp
// Corresponds to LaserPinsArray::irPinsSwitch(const bool targetState)
// which was called from pirStartupController exclusively
void LaserGroupedUnitsArray::irStartupSwitch(const bool _bTargetState) {                     // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {        // loop around all the structs representing the pins controlling the relays
    LaserGroupedUnits[thisLaserGroupedUnit].switchOnOff(_bTargetState);
  }
}

////////////////////////////////////////////////////////////////////////////////
// MANUAL SWITCH
// Switches on and off all the lasers
// Manual in the sense that it also switches the pir_state of the LaserGroupedUnit to LOW
// (i.e. the LaserGroupedUnit will no longer be reacting to IR signals)
// Corresponds to LaserPinsArray::manualSwitchAllRelays
// which is called (i) myMesh, (ii) myWebServerController and (iii) this class (LaserPin)
void LaserGroupedUnitsArray::manualSwitchAll(const bool _bTargetState) {
  _bTargetState == HIGH ? globalModeWitness = 4 : globalModeWitness = 5;      // 4 for "manual with cycle off", 5 for "manual with cycle off"
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].manualSwitch(_bTargetState);
  }
}

////////////////////////////////////////////////////////////////////////////////
// PIR CYCLE SWITCHES
// Switches on and off all the LaserGroupUnits under Pir control
// Corresponds to LaserPinsArray::switchPirRelays
// which is called from pirController twice
// loop over each of the loaded LaserGroupUnits to turn them on or off (if they are controlled by the PIR)
void LaserGroupedUnitsArray::pirSwitchAll(const bool _bTargetState) {
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
// This function subjects or frees all the LaserGroupUnits to or of the control of the PIR
// Corresponds to LaserPinsArray::inclExclAllRelaysInPir
// Called from (i) myWebServerController, (ii) pirStartupController and (iii) this class (LaserPin)
void LaserGroupedUnitsArray::inclExclAllInPir(const bool _bTargetPirState) {
  if (_bTargetPirState == HIGH) { globalModeWitness = 1;}                      // 1 for "IR waiting"
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].pir_state = _bTargetPirState;
  }
}

//////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
// Called exclusively from pirStartupController
// Loops around all the pins and pairs or unpairs them
void LaserGroupedUnitsArray::pairUnpairAllPins(const short _sPairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/) {
  pinParityWitness = 0;
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].pairUnpairPin(pinParityWitness, _sPairingType);
    pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
  }
  pinParityWitness = 0;
}


//////////////////////////////////////////////////////////////////////////
// BLINKING DELAY Control
// Changes the blinking delay of each pin and saves such new blinking delay in Preferences
// Called exclusively from myWebServerController
void LaserGroupedUnitsArray::changeGlobalBlinkingInterval(const unsigned long _ulTargetBlinkingInterval) {
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].changeIndividualBlinkingInterval(_ulTargetBlinkingInterval);
    pinBlinkingInterval = _ulTargetBlinkingInterval;
    mySavedPrefs::savePreferences();
  }
}

//////////////////////////////////////////////////////////////////////////
// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
short LaserGroupedUnitsArray::_siSlaveBoxCycleIterations = 60;

bool LaserGroupedUnitsArray::_tcbOeSlaveBoxCycle() {
  globalModeWitness = 3;      // 3 for "slave cycle on"
  myMeshViews::statusMsg("on");
  manualSwitchAll(LOW);
  Serial.print("-------- Auto Switch cycle started............ --------\n");
  return true;
}

void LaserGroupedUnitsArray::_tcbOdSlaveBoxCycle() {
  myMeshViews::statusMsg("off");
  manualSwitchAll(HIGH);
  inclExclAllRelaysInPir(HIGH);     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
}

Task LaserGroupedUnitsArray::_tSlaveBoxCycle( 1000, _siSlaveBoxCycleIterations, NULL, &userScheduler, false, &_tcbOeSlaveBoxCycle, &_tcbOdSlaveBoxCycle );

void LaserGroupedUnitsArray::slaveBoxSwitchAllRelays(const bool _bTargetState) {
  if (_bTargetState == LOW) {
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
