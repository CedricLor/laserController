/*
  LaserGroupedUnitsArray.cpp - Library to handle the array of grouped laser units.
  Created by Cedric Lor, January 19, 2019.
*/

#include "Arduino.h"
#include "LaserGroupedUnitsArray.h"

short LaserGroupedUnitsArray::loadedLaserUnits = 0;

LaserGroupedUnitsArray::LaserGroupedUnitsArray()
{
}

////////////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
////////////////////////////////////////////////////////////////////////////////
/* PAIRING ALL LaserPins into LaserGroupedUnits of 2 lasers
   Corresponds to:
   - LaserPinsArray::initLaserPins() (called from main.cpp in setup());
   - LaserPinsArray::pairUnpairAllPins (called from myMeshController, myWebServerController and pirStartupController).
*/
void LaserGroupedUnitsArray::_pairingAll() {
  short int _pinParityWitness = 0;
  short int _groupUnitsCounter = 0;
  for (short int thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Put each pin into a LaserGroupedUnit
    LaserPins[thisPin].laserGroupedUnitId = LaserGroupedUnits[_groupUnitsCounter].index_number;
    // increment the counter of laserUnits effectively loaded
    // note: the loadedLaserUnits variable will then be used below, by irStartupSwitch for instance, to limit the iterations over the LaserGroupedUnits
    loadedLaserUnits = _groupUnitsCounter;
    /*
      if _pinParityWitness is equal to 1, it means that we already have 2 lasers in the LaserGroupUnit.
       We then need to store the next pins in a new LaserGroupUnit.
       For this purpose, we increment the _groupUnitsCounter if _pinParityWitness is equal 1 (i.e. the current pin is the second pin stored in the LaserGroupedUnit).
       At the next iteration of this foor loop, the index_number of another LaserGroupedUnit will be marked in the next instance of LaserPins
       Sidenote: For the moment, I hard code this 2 lasers limit, but this will need to be changed in the future.
    */
    if (_pinParityWitness == 1) {
      _groupUnitsCounter = _groupUnitsCounter + 1;
    }
    // Switch _pinParityWitness between 1 and
    _pinParityWitness = (_pinParityWitness == 0) ? 1 : 0;
  }
  _pinParityWitness = 0;
}

////////////////////////////////////////////////////////////////////////////////
/* UNPAIRING ALL LaserPins from their respective owner LaserGroupedUnit
   Putting each LaserPin into its own LaserGroupedUnit
   Corresponds to:
   - LaserPinsArray::initLaserPins() (called from main.cpp in setup());
   - LaserPinsArray::pairUnpairAllPins (called from myMeshController, myWebServerController and pirStartupController).
*/
void LaserGroupedUnitsArray::_unpairingAll() {
  short int _groupUnitsCounter = 0;
  for (short int thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Put each pin into a LaserGroupedUnit
    LaserPins[thisPin].laserGroupedUnitId = LaserGroupedUnits[_groupUnitsCounter].index_number;
    // increment the counter of laserUnits effectively loaded
    // note: the loadedLaserUnits variable will then be used below, by irStartupSwitch for instance, to limit the iterations over the LaserGroupedUnits
    loadedLaserUnits = _groupUnitsCounter;
    _groupUnitsCounter = _groupUnitsCounter + 1;
  }
}

////////////////////////////////////////////////////////////////////////////////
/* PAIR UNPAIR ALL PINS
  Pairing_type setter and Wrapper function to simplify calls to pairing and unpairing functions
   Choses to pair or unpair depending on the param passed to it.
   Corresponds to:
   - LaserPinsArray::initLaserPins() (called from main.cpp in setup());
   - LaserPinsArray::pairUnpairAllPins (called from myMeshController, myWebServerController and pirStartupController).
*/
void LaserGroupedUnitsArray::pairUnpairAllPins(const short _sPairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/) {
  (_sPairingType == -1) ? _unpairingAll(): _pairingAll();
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].pairing_type = _sPairingType;
  }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE
short int LaserGroupedUnitsArray::_currentState = 0;
short int LaserGroupedUnitsArray::_previousState = 0;
short int LaserGroupedUnitsArray::targetState = 0;
bool LaserGroupedUnitsArray::bTargetStateOfLaserGroupUnits = LOW;
const char* LaserGroupedUnitsArray::GLOBAL_WITNESS_TEXT_DESCRIPTORS[5] = {"pirStartUp cycle", "IR waiting", "IR cycle on", "slave cycle on", "manual"};
void (*LaserGroupedUnitsArray::_stateChangeActions[5])(bool __targetState) = {
  &irStartupSwitch,
  &pirSwitchAll,
  &pirSwitchAll,
  &slaveBoxSwitchAll,
  &manualSwitchAll
};

////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE SETTER
void LaserGroupedUnitsArray::setTargetState(const short int __sTargetState) {
  _previousState = _currentState;
  _currentState = __sTargetState;
}

////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE READER
long int LaserGroupedUnitsArray::_tActUponStateChangesInterval = 2000;
Task LaserGroupedUnitsArray::_tActUponStateChanges(_tActUponStateChangesInterval, TASK_FOREVER, &_tcbActUponStateChanges, &userScheduler, true);

void LaserGroupedUnitsArray::_tcbActUponStateChanges() {
  if (!(_currentState == targetState)) {
    _currentState = targetState;
    // _stateChangeActions[_currentState](bTargetStateOfLaserGroupUnits/* bTargetStateOfLaserGroupUnits added only for compilation test purpose; no params shall be passed; the param shall be read in bTargetStateOfLaserGroupUnits*/);
  }
}

////////////////////////////////////////////////////////////////////////////////
// STATE 0 (pirStartUp cycle) ACTIONS: IR STARTUP SWITCH
// Switches relay pins on and off during PIRStartUp
// Corresponds to LaserPinsArray::irPinsSwitch(const bool targetState)
// which was called from pirStartupController exclusively
void LaserGroupedUnitsArray::irStartupSwitch(bool __targetState) {                     // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {        // loop around all the structs representing the pins controlling the relays
    LaserGroupedUnits[thisLaserGroupedUnit].switchOnOff(bTargetStateOfLaserGroupUnits);
  }
}

////////////////////////////////////////////////////////////////////////////////
// STATES 4 (manual) ACTIONS: MANUAL SWITCH
// Switches on and off all the lasers
// Manual in the sense that it also switches the pir_state of the LaserGroupedUnit to LOW
// (i.e. the LaserGroupedUnit will no longer be reacting to IR signals)
// Corresponds to LaserPinsArray::manualSwitchAllRelays
// which is called (i) myMesh, (ii) myWebServerController and (iii) this class (LaserPin)
void LaserGroupedUnitsArray::manualSwitchAll(bool __targetState) {
  // 4 for "manual with cycle off", 5 for "manual with cycle off"
  _currentState = ((bTargetStateOfLaserGroupUnits == HIGH) ? 4 : 5);
  // Since states 4 and 5 have been condensed into a single manualState mode, might need to be redrafted as follows:
  // _currentState = targetState;
  // or as follows:
  // _currentState = 4;
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].manualSwitch(bTargetStateOfLaserGroupUnits);
  }
}

////////////////////////////////////////////////////////////////////////////////
// STATES 1 (IR waiting) AND 2 (IR cycle on) ACTIONS: PIR CYCLE SWITCHES
// Switches on and off all the LaserGroupUnits under Pir control
// Corresponds to LaserPinsArray::switchPirRelays
// which is called from pirController twice
// loop over each of the loaded LaserGroupUnits to turn them on or off (if they are controlled by the PIR)
void LaserGroupedUnitsArray::pirSwitchAll(bool __targetState) {
  Serial.print("PIR: pirSwitchAll(const bool state): starting -------\n");
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    if (LaserGroupedUnits[thisLaserGroupedUnit].pir_state == HIGH) {
      LaserGroupedUnits[thisLaserGroupedUnit].switchOnOff(bTargetStateOfLaserGroupUnits);
    }
  }
  Serial.print("PIR: pirSwitchAll(const bool state): leaving -------\n");
}

//////////////////////////////////////////////////////////////////////////
// STATE 3 (slave cycle on) ACTION: AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
long LaserGroupedUnitsArray::_ulSlaveBoxCycleInterval = 1000UL;
short LaserGroupedUnitsArray::_siSlaveBoxCycleIterations = 60;

bool LaserGroupedUnitsArray::_tcbOeSlaveBoxCycle() {
  myMeshViews::statusMsg("on");
  manualSwitchAll(LOW);
  _currentState = 3;      // 3 for "slave cycle on"
  Serial.print("-------- Auto Switch cycle started............ --------\n");
  return true;
}

void LaserGroupedUnitsArray::_tcbOdSlaveBoxCycle() {
  myMeshViews::statusMsg("off");
  manualSwitchAll(HIGH);
  inclExclAllInPir(HIGH);     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
}

Task LaserGroupedUnitsArray::_tSlaveBoxCycle( _ulSlaveBoxCycleInterval, _siSlaveBoxCycleIterations, NULL, &userScheduler, false, &_tcbOeSlaveBoxCycle, &_tcbOdSlaveBoxCycle );

void LaserGroupedUnitsArray::slaveBoxSwitchAll(bool __targetState) {
  if (bTargetStateOfLaserGroupUnits == LOW) {
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


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Task(unsigned long aInterval, long aIterations, TaskCallback aCallback, Scheduler* aScheduler, bool aEnable, TaskOnEnable aOnEnable, TaskOnDisable aOnDisable)
Task LaserGroupedUnitsArray::tLaserOff( 0, 1, &_cbtLaserOff, &userScheduler );

Task LaserGroupedUnitsArray::tLaserOn( 0, 1, &_cbtLaserOn, &userScheduler );

void LaserGroupedUnitsArray::_cbtLaserOff() {
  irStartupSwitch(HIGH);
}

void LaserGroupedUnitsArray::_cbtLaserOn() {
  irStartupSwitch(LOW);
}


////////////////////////////////////////////////////////////////////////////////
// SOME SWITCHES
////////////////////////////////////////////////////////////////////////////////
// PIR SUBJECTION SWITCHES
// This function subjects or frees all the LaserGroupUnits to or of the control of the PIR
// Corresponds to LaserPinsArray::inclExclAllRelaysInPir
// which is called from (i) myWebServerController, (ii) pirStartupController and (iii) this class (LaserPin)
void LaserGroupedUnitsArray::inclExclAllInPir(const bool _bTargetPirState) {
  if (_bTargetPirState == HIGH) { _currentState = 1;}                      // 1 for "IR waiting"
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].pir_state = _bTargetPirState;
  }
}


//////////////////////////////////////////////////////////////////////////
// BLINKING INTERVAL Control
// Changes the blinking delay of each LaserGroupedUnit and saves such new blinking delay in Preferences
// Corresponds to LaserPinsArray::changeGlobalBlinkingInterval.
// which is called from:
// (i) myWebServerController;
// (ii) myMeshController.
void LaserGroupedUnitsArray::changeBlinkingIntervalAll(const unsigned long _ulTargetBlinkingInterval) {
  pinBlinkingInterval = _ulTargetBlinkingInterval;
  mySavedPrefs::savePreferences();
  for (short thisLaserGroupedUnit = 0; thisLaserGroupedUnit < loadedLaserUnits; thisLaserGroupedUnit = thisLaserGroupedUnit + 1) {
    LaserGroupedUnits[thisLaserGroupedUnit].changeBlinkingInterval(_ulTargetBlinkingInterval);
    mySavedPrefs::savePreferences();
  }
}
