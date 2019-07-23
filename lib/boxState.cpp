/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h


reverse dependency graph
  |--boxState.h
  |  |--boxState.cpp
  |  |  |--main.cpp

*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_boxTargetState = 0;
bool boxState::_boxActiveStateHasBeenReset = 0;
const short int boxState::BOX_STATES_COUNT = 14;
boxState boxState::boxStates[BOX_STATES_COUNT];
const short int boxState::_NAME_CHAR_COUNT = 25;
const short int boxState::_HTML_TAG_CHAR_COUNT = 4;





// Constructors
boxState::boxState() {
}






// Initialisers
void boxState::_initBoxState(const char cpName[_NAME_CHAR_COUNT], const char cpHtmlTag[_NAME_CHAR_COUNT], const unsigned long ulDuration, const byte bAssociatedSequence, const byte bIRTrigger, const byte bMeshTrigger){
  // Serial.println("void boxState::_initBoxState(). Starting.");
  strcpy(cName, cpName);
  strcpy(cHtmlTag, cpHtmlTag);
  _ulDuration = ulDuration;
  _bAssociatedSequence = bAssociatedSequence;
  _bIRTrigger = bIRTrigger;
  _bMeshTrigger = bMeshTrigger;
  // Serial.println("void boxState::_initBoxState(). Ending.");
};

void boxState::initBoxStates() {
  Serial.println("void boxState::_initBoxStates(). Starting.");
  boxStates[0]._initBoxState("manual / off", "man", 1000000, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].cName: ");
  // Serial.println(boxStates[0].cName);
  // Serial.println("void boxState::_initBoxStates(). boxStates[0]._ulDuration");
  // Serial.println(boxStates[0]._ulDuration);
  boxStates[1]._initBoxState("align lasers", "ali", 1000000, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  boxStates[2]._initBoxState("pir Startup", "irs", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  boxStates[3]._initBoxState("waiting both", "wab", 1000000, 5, 1, 1); // sequence "all of" for indefinite time until trigger from either IR or mesh
  boxStates[4]._initBoxState("waiting ir", "wai", 1000000, 5, 1, 0); // sequence "all of" for indefinite time until trigger from IR
  boxStates[5]._initBoxState("waiting mesh", "wam", 1000000, 5, 0, 1); // sequence "all of" for indefinite time until trigger from mesh
  boxStates[6]._initBoxState("pir High both interrupt", "ihb", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[7]._initBoxState("pir High ir interrupt", "ihr", 120000, 0, 1, 0); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  boxStates[8]._initBoxState("pir High mesh interrupt", "ihm", 120000, 0, 0, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  boxStates[9]._initBoxState("pir High no interrupt", "ihn", 120000, 0, 0, 0); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh
  boxStates[10]._initBoxState("mesh High both interrupt", "mhb", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[11]._initBoxState("mesh High ir interrupt", "mhi", 120000, 0, 1, 0); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  boxStates[12]._initBoxState("mesh High mesh interrupt", "mhm", 120000, 0, 0, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  boxStates[13]._initBoxState("mesh High no interrupt", "mhn", 120000, 0, 0, 0); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

  Serial.println("void boxState::_initBoxStates(). Ending.");
}





// boxState player wrapper
// tPlayBoxStates starts the execution of the various boxStates.
// It is enabled at the end of the setup.
// It then iterates indefinitely every seconds.
// onEnable, it puts the box into IR Startup boxState (which is set to last for 1 minute)
// then
Task boxState::tPlayBoxStates(1000L, -1, &_tcbPlayBoxStates, &userScheduler, false, &_oetcbPlayBoxStates);

/*
  At each pass of tPlayBoxStates, _tcbPlayBoxStates() will check whether the
  following values have changed (the catchers):
  - ControlerBox::valFromPir (when the current boxState is set to react to signals from the PIR);
  - ControlerBoxes[PARENT].boxActiveState (when the current boxState is set to react to signals from the mesh);
  - _boxActiveStateHasBeenReset;
  - _boxTargetState;
  Depending on the changes, it will:
  - either start a new boxState or extend the duration of the current boxState; or
  - reset the catchers' values to their origin value.
  If the catchers have not changed, nothing happens.
*/

void boxState::_tcbPlayBoxStates() {
  // Serial.println("void boxState::_tcbPlayBoxStates(). Starting.");
  // Serial.print("void boxState::_tcbPlayBoxStates(). Iteration:");
  // Serial.println(tPlayBoxStates.getRunCounter());
  uint16_t _ui16masterBoxIndexInCB = ControlerBoxes[gui16MyIndexInCBArray].bMasterBoxName - gui16ControllerBoxPrefix;

  // A. Analyse the signal catchers and set the box target state accordingly
  _setBoxTargetStateFromSignalCatchers(_ui16masterBoxIndexInCB);

  // B. Once read, reset all the signal catchers
  _resetSignalCatchers(_ui16masterBoxIndexInCB);

  // C. If the active state (actually, the targetState) has been reset, start playing
  // the corresponding state
  _restart_tPlayBoxState();
  // Serial.println("void boxState::_tcbPlayBoxStates(). Ending.");
};

// Upon tPlayBoxStates being enabled (at startup), the _boxTargetState is being
// changed to 2 (pir Startup).
bool boxState::_oetcbPlayBoxStates() {
  // Serial.println("void boxState::_oetcbPlayBoxStates(). Starting.");
  _setBoxTargetState(2); // 2 for pir Startup; upon enabling the task tPlayBoxStates, play the pirStartup boxState
  // Serial.println("void boxState::_oetcbPlayBoxStates(). Ending.");
  return true;
}


//////////////////////////////////////////////
// _tcbPlayBoxStates() sub functions
void boxState::_setBoxTargetStateFromSignalCatchers(uint16_t _ui16masterBoxIndexInCB) {
  // Read the signal catchers and change the targetState accordingly

  // 1. Check the web signal catcher. If it has changed, set the new targetState
  // and return
  if (!(ControlerBox::valFromWeb == -1)) {
    _setBoxTargetState(ControlerBox::valFromWeb);
    return;
  }

  // 2. If the current boxState has both IR and mesh triggers, check both values
  if (boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bIRTrigger == 1 && boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bMeshTrigger == 1) {
    // if both IR and mesh have sent a signal, compare the time at which each of
    // them came and give priority to the latest
    if (ControlerBox::valFromPir == HIGH &&
      // testing if IR has been set HIGH
      ControlerBoxes[_ui16masterBoxIndexInCB].boxActiveState != -1 &&
      // testing if the state of the master boxActiveState has been set to something
      ControlerBoxes[_ui16masterBoxIndexInCB].boxActiveStateHasBeenTakenIntoAccount == false
      // testing if the state of the master boxActiveState has been taken into account
      ){
        // compare the times at which each signal catcher has been set
        if (ControlerBox::uiSettingTimeOfValFromPir > ControlerBoxes[_ui16masterBoxIndexInCB].uiBoxActiveStateStartTime) {
          _setBoxTargetState(3);
        } else {
          _setBoxTargetState(6);
        }
      }
    return;
  }

  // 3. If the current boxState has IR trigger and the valueFromIR is HIGH,
  // change state and put it in IR high
  if (boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bIRTrigger == 1 && ControlerBox::valFromPir == HIGH) {
    _setBoxTargetState(3);
    return;
  }

  // 4. If the current boxState has Mesh trigger and
  // its parent box has a state other than -1 and
  // its activeState has not been taken into account
  if (boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bMeshTrigger == 1 &&
    ControlerBoxes[_ui16masterBoxIndexInCB].boxActiveState != -1 &&
    ControlerBoxes[_ui16masterBoxIndexInCB].boxActiveStateHasBeenTakenIntoAccount == false
    ){
      _setBoxTargetState(6);
      return;
    }
}

void boxState::_resetSignalCatchers(uint16_t _ui16masterBoxIndexInCB) {
  // once the new boxState has been set, in accordance with the signal catchers,
  // reset all the signals catchers to their initial values
  ControlerBox::valFromPir = LOW;
  ControlerBox::uiSettingTimeOfValFromPir = 0;
  ControlerBoxes[_ui16masterBoxIndexInCB].boxActiveStateHasBeenTakenIntoAccount = true;
  ControlerBox::valFromWeb = -1;
}

void boxState::_restart_tPlayBoxState() {
  if (_boxActiveStateHasBeenReset == 1) {
    _boxActiveStateHasBeenReset = 0;
    // Serial.print("void boxState::_tcbPlayBoxStates() boxStates[_boxTargetState]._ulDuration: ");
    // Serial.println(boxStates[_boxTargetState]._ulDuration);
    _tPlayBoxState.setInterval(boxStates[_boxTargetState]._ulDuration);
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    if (!(_boxTargetState == ControlerBoxes[gui16MyIndexInCBArray].boxActiveState)) {
      ControlerBox::setBoxActiveState(gui16MyIndexInCBArray, _boxTargetState, laserControllerMesh.getNodeTime());
    }
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState about to be enabled");
    _tPlayBoxState.restartDelayed();
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState enabled");
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    // Serial.println("*********************************************************");
  }
}






/*
  _tPlayBoxState plays a boxState once (it iterates only once).
  It is enabled by tPlayBoxStates.
  Its main iteration is delayed until aInterval has expired. aInterval is set in its onEnable callback.
  It is equal to the duration of the boxState selected by ControlerBoxes[gui16MyIndexInCBArray].boxActiveState.
  Upon being enabled, its onEnable callback:
  1. sets the aInterval of _tPlayBoxState, based on the _ulDuration of the currently active boxState;
  2. looks for the associated sequence using the ControlerBoxes[gui16MyIndexInCBArray].boxActiveState variable to select the relevant boxState in boxStates[];
  3. sets the active sequence by a call to sequence::setActiveSequence();
  4. enables the task sequence::tPlaySequenceInLoop.
  Task sequence::tPlaySequenceInLoop is set to run indefinitely, for so long as it is not disabled.
  - Task sequence::tPlaySequenceInLoop is equivalent to Task tLED(TASK_IMMEDIATE, TASK_FOREVER, NULL, &ts, false, NULL, &LEDOff)
  in the third example provided with the taskScheduler library.
  - Task _tPlayBoxState is equivalent to Task tBlink(5000, TASK_ONCE, NULL, &ts, false, &BlinkOnEnable, &BlinkOnDisable)
  in the third example provided with the taskScheduler library.
  After expiration of aInterval, its onDisable callback disables sequence::tPlaySequenceInLoop.
*/
Task boxState::_tPlayBoxState(0, 1, NULL, &userScheduler, false, &_oetcbPlayBoxState, &_odtcbPlayBoxState);

bool boxState::_oetcbPlayBoxState(){
  Serial.println("bool boxState::_oetcbPlayBoxState(). Starting.");
  Serial.print("bool boxState::_oetcbPlayBoxState(). Box State Number: ");Serial.println(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState);
  // Look for the sequence number to read when in this state
  short int _activeSequence = boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bAssociatedSequence;
  Serial.print("bool boxState::_oetcbPlayBoxState() _activeSequence: ");
  Serial.println(_activeSequence);
  // set the active sequence
  Serial.println("bool boxState::_oetcbPlayBoxState() calling sequence::setActiveSequence(_activeSequence)");
  sequence::setActiveSequence(_activeSequence);

  // Play sequence in loop until end
  Serial.println("bool boxState::_oetcbPlayBoxState() sequence::tPlaySequenceInLoop about to be enabled");
  sequence::tPlaySequenceInLoop.enable();
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();
  Serial.println("bool boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}

void boxState::_odtcbPlayBoxState(){
  // Upon disabling the task which plays a given boxState,
  // (i) disable the associated sequence player; and
  // (ii) if the state which was being played was not the default state, set it to its default state

  Serial.println("void boxState::_odtcbPlayBoxState(). Starting.");
  // Serial.print("void boxState::_odtcbPlayBoxState() _tPlayBoxState.getInterval(): ");
  // Serial.println(_tPlayBoxState.getInterval());
  // disable the associated sequence player
  sequence::tPlaySequenceInLoop.disable();
  // Serial.println("void boxState::_odtcbPlayBoxState(): ControlerBoxes[gui16MyIndexInCBArray].boxActiveState");
  // Serial.println(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState);
  // Serial.println("void boxState::_odtcbPlayBoxState(): _boxTargetState");
  // Serial.println(_boxTargetState);
  if (!(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState == ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState)) {
    _setBoxTargetState(ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState);
  }
  Serial.println("void boxState::_odtcbPlayBoxState(). Ending.");
}





// _setBoxTargetState receive boxState change requests from other classes
void boxState::_setBoxTargetState(const short boxTargetState) {
  Serial.println("void boxState::_setBoxTargetState(). Starting.");
  Serial.print("void boxState::_setBoxTargetState(). targetBoxState: ");Serial.println(boxTargetState);
  _boxActiveStateHasBeenReset = 1;
  _boxTargetState = boxTargetState;
  // Serial.print("void boxState::_setBoxTargetState(). _boxTargetState: ");
  // Serial.println(_boxTargetState);
  Serial.println("void boxState::_setBoxTargetState(). Ending.");
};
