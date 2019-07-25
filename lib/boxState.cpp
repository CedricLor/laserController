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

/*
- _tPlayBoxStates detects boxState change request coming from the outside, depending
on the settings (IR Trigger, Mesh Trigger) of the currently active boxState.
- _setBoxTargetState stores the change requests from _tPlayBoxStates.
- _tPlayBoxState starts and stops the boxState and underlying sequence, depending
on the settings (duration, associated sequence number) of the currently active
boxState, and upon request from its parent Task _tPlayBoxStates.

What needs to be changed:
1. _odtcbPlayBoxState(), step 2: reset to default upon expiration of a boxState
-> maybe shall be able to choose to play something else.
2. _tcbPlayBoxStates() -> _setBoxTargetStateFromSignalCatchers: currently goes to
hard coded boxStates. Shall go to parameterized boxStates.
3. add (i) onExpire, (ii) onIR and (iii) onMesh properties to boxStates
4. create setters for:
(i) onExpire,
(ii) onIR,
(iii) onMesh,
(iv) ulDuration
(v) ui16AssociatedSequence
5. create an interface class to set the boxState at each step of a session
6. create an interface class to set the sessions
*/


#include "Arduino.h"
#include "boxState.h"

short int boxState::_boxTargetState = 0;
bool boxState::_boxActiveStateHasBeenReset = 0;
const short int boxState::BOX_STATES_COUNT = 14;
boxState boxState::boxStates[BOX_STATES_COUNT];




// Constructors
boxState::boxState() {
}

// Initialize a Box with custom dimensions
// boxState::boxState(const unsigned long _ulDuration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire)
//     : ulDuration(_ulDuration), ui16AssociatedSequence(_ui16AssociatedSequence), i16onIRTrigger(_i16onIRTrigger), i16onMeshTrigger(_i16onMeshTrigger), i16onExpire(_i16onExpire)
// {
// }




// Initialisers
void boxState::_initBoxState(const unsigned long _ulDuration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire){
  // Serial.println("void boxState::_initBoxState(). Starting.");
  ulDuration = _ulDuration;
  ui16AssociatedSequence = _ui16AssociatedSequence;
  i16onIRTrigger = _i16onIRTrigger;
  i16onMeshTrigger = _i16onMeshTrigger;
  i16onExpire = _i16onExpire;
  // Serial.println("void boxState::_initBoxState(). Ending.");
};


void boxState::initBoxStates() {
  Serial.println("void boxState::_initBoxStates(). Starting.");

  // manual / off
  boxStates[0]._initBoxState(1000000, 5, -1, -1, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].cName: ");
  // Serial.println(boxStates[0].cName);
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].ulDuration");
  // Serial.println(boxStates[0].ulDuration);

  // const unsigned long _ulDuration, const uint16_t _ui16AssociatedSequence
  // const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire

  // align lasers
  boxStates[1]._initBoxState(1000000, 1, -1, -1, 1); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // pir startup waiting mesh
  boxStates[2]._initBoxState(60000, 1, -1, 8/*0-1 8-9 with restriction*/, 3); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh

  // waiting both
  boxStates[3]._initBoxState(1000000, 5, 6/*6-9*/, 10/*10-13*/, 3/*0 3-13*/); // sequence "all of" for indefinite time until trigger from either IR or mesh
  // waiting ir
  boxStates[4]._initBoxState(1000000, 5, 6/*6-9*/, -1, 4/*0 3-13*/); // sequence "all of" for indefinite time until trigger from IR
  // waiting mesh
  boxStates[5]._initBoxState(1000000, 5, -1, 10/*10-13*/, 5/*0 3-13*/); // sequence "all of" for indefinite time until trigger from mesh

  // pir High both interrupt
  boxStates[6]._initBoxState(120000, 0, 6/*6-9*/, 10/*10-13*/, 3/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  // pir High ir interrupt
  boxStates[7]._initBoxState(120000, 0, 6/*6-9*/, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  // pir High mesh interrupt
  boxStates[8]._initBoxState(120000, 0, -1, 10/*10-13*/, 5/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  // pir High no interrupt
  boxStates[9]._initBoxState(120000, 0, -1, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

  // mesh High both interrupt
  boxStates[10]._initBoxState(120000, 0, 6/*6-9*/, 10/*10-13*/, 3/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  // mesh High ir interrupt
  boxStates[11]._initBoxState(120000, 0, 6/*6-9*/, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  // mesh High mesh interrupt
  boxStates[12]._initBoxState(120000, 0, -1, 10/*10-13*/, 5/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  // mesh High no interrupt
  boxStates[13]._initBoxState(120000, 0, -1, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

  Serial.println("void boxState::_initBoxStates(). Ending.");
}




//////////////////////////////////////////////
// Task _tPlayBoxStates and its callbacks
//////////////////////////////////////////////

/* Task _tPlayBoxStates:
    tPlayBoxStates starts at the end of the setup and
    iterates indefinitely.
    Every second:
    1. it checks whether some events may trigger a boxState change
    and if so, requests such boxState changes;
    2. it then resets the event catchers to their default values;
    3. finally, if a boxState change has been requested (by itself or
    by _tPlayBoxState), it restarts _tPlayBoxState.

    onEnable, it puts the box into IR Startup boxState (which is
    set to last for 1 minute).
*/
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

  // A. Analyse the signal catchers and set the box target state accordingly
  _setBoxTargetStateFromSignalCatchers();

  // B. Once read, reset all the signal catchers
  _resetSignalCatchers();

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
//////////////////////////////////////////////

/*
    _setBoxTargetStateFromSignalCatchers:
    1. reads whether the web, the IR or the mesh have been modified in a way
    that could require a change of boxState;
    2. if so, requests a boxState change by calling _setBoxTargetState()
*/
void boxState::_setBoxTargetStateFromSignalCatchers() {
  // Read the signal catchers and change the targetState accordingly

  // 1. Check the web signal catcher. If it has changed, set the new targetState
  // and return
  if (ControlerBox::valFromWeb != -1) {
    Serial.println("--------------------- valFromWeb ----------");
    _setBoxTargetState(ControlerBox::valFromWeb);
    return;
  }

  ControlerBox& _thisBox = ControlerBoxes[gui16MyIndexInCBArray];
  boxState& _currentBoxState = boxStates[_thisBox.boxActiveState];
  // 2. Check whether the current state has both IR and mesh triggers
  if (_currentBoxState._hasBothTriggers()) {
    // check whether both have been triggered
    if (ControlerBox::valFromPir == HIGH && _currentBoxState._meshHasBeenTriggered(_thisBox)) {
      // if so, resolve the conflict and return
      _currentBoxState._resolveTriggersConflict(_thisBox);
      return;
    }
    /*
        if it has both triggers but none or only has been triggered,
        keep going (maybe, the IR has been trigger or the mesh has been
        triggered)
    */
  }

  // 3. If the current boxState has IR trigger
  if (_currentBoxState.i16onIRTrigger != -1){
    _currentBoxState._checkIRTriggerAndAct();
    return;
  }

  // 4. If the current boxState has Mesh trigger and
  // its parent box has a state other than -1 and
  // its activeState has not been taken into account
  if (_currentBoxState.i16onMeshTrigger != -1){
    _currentBoxState._checkMeshTriggerAndAct(_thisBox);
  }
}



/*
  _resetSignalCatchers resets all the signals catchers
  to their initial values, once the new _boxTargetState
  has been set by _setBoxTargetStateFromSignalCatchers
*/
void boxState::_resetSignalCatchers() {
  ControlerBox::valFromWeb = -1;
  ControlerBox::valFromPir = LOW;
  ControlerBox::uiSettingTimeOfValFromPir = 0;
  ControlerBox& _thisBox = ControlerBoxes[gui16MyIndexInCBArray];
  if (_thisBox.bMasterBoxName != 254) {
    ControlerBox& _masterBox = ControlerBoxes[_thisBox.bMasterBoxName - gui16ControllerBoxPrefix];
    _masterBox.boxActiveStateHasBeenTakenIntoAccount = true;
  }
}



/*
    _restart_tPlayBoxState() starts a new boxState, if
    the _boxActiveStateHasBeenReset has somehow been reset, and
    accomplishes a couple of other technical tasks.

    If the _boxActiveStateHasBeenReset, it will:
      1. reset the witness _boxActiveStateHasBeenReset to 0;
      2. set the duration to stay in the new boxState (by setting the
      aInterval of the "children" Task _tPlayBoxState; _tPlayBoxState.setInterval), to
      the ulDuration of the target boxState (boxStates[_boxTargetState].ulDuration);
      3. set the boxActiveState property (and related properties) of this box;
      4. restart/enable the "children" Task _tPlayBoxState.
*/
void boxState::_restart_tPlayBoxState() {
  // if the _boxActiveStateHasBeenReset,
  if (_boxActiveStateHasBeenReset == 1) {
    // 1. Resets the witness to 0 (false)
    _boxActiveStateHasBeenReset = 0;

    // 2. Set the duration of Task _tPlayBoxState
    // Serial.print("void boxState::_tcbPlayBoxStates() boxStates[_boxTargetState].ulDuration: "); Serial.println(boxStates[_boxTargetState].ulDuration);
    _tPlayBoxState.setInterval(boxStates[_boxTargetState].ulDuration);
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): "); Serial.println(_tPlayBoxState.getInterval());

    // 3. Set the boxActiveState to the _boxTargetState
    ControlerBox::setBoxActiveState(gui16MyIndexInCBArray, _boxTargetState, laserControllerMesh.getNodeTime());
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState about to be enabled");

    // 4. Restart/enable _tPlayBoxState
    _tPlayBoxState.restartDelayed();
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState enabled");
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");Serial.println(_tPlayBoxState.getInterval());
    // Serial.println("*********************************************************");
  }
}



/*
    The following subs are helpers for _setBoxTargetStateFromSignalCatchers
*/
bool boxState::_hasBothTriggers() {
  return (i16onIRTrigger != -1
    && i16onMeshTrigger != -1);
}



void boxState::_checkIRTriggerAndAct() {
  // check whether IR has been triggered;
  // if so, set the boxTarget state accordingly
  if (ControlerBox::valFromPir == HIGH) {
    Serial.println("--------------------- IR triggered ----------");
    _setBoxTargetState(i16onIRTrigger);
  }
}



void boxState::_checkMeshTriggerAndAct(ControlerBox& _thisBox) {
  if (_meshHasBeenTriggered(_thisBox)) {
    Serial.println("--------------------- Mesh triggered ----------");
    _setBoxTargetState(i16onIRTrigger);
  }
}



bool boxState::_meshHasBeenTriggered(ControlerBox& _thisBox) {
  // check whether masterBox has been set (the masterBox
  // could have been disconnected, or forgotten to be set)
  if (_thisBox.bMasterBoxName == 254) {
    return false;
  }
  ControlerBox& _masterBox = ControlerBoxes[_thisBox.bMasterBoxName - gui16ControllerBoxPrefix];
  // if masterBox has been set, check whether it has a set state
  if (_masterBox.boxActiveState == -1) {
    return false;
  }
  // if masterBox activeState has been set, check whether this state
  // has been taken into account
  if (_masterBox.boxActiveStateHasBeenTakenIntoAccount){
    return false;
  }
  // else, we have a mesh trigger!
  return true;
}



void boxState::_resolveTriggersConflict(ControlerBox& _thisBox) {
  // check whether both IR and Mesh have been triggered
  Serial.println("--------------------- double trigger ----------");
  // if so, compare the times at which each signal catcher has been set
  // and give priority to the most recent one
  ControlerBox& _masterBox = ControlerBoxes[_thisBox.bMasterBoxName - gui16ControllerBoxPrefix];
  if (ControlerBox::uiSettingTimeOfValFromPir > _masterBox.uiBoxActiveStateStartTime) {
    _setBoxTargetState(i16onIRTrigger);
  } else {
    _setBoxTargetState(i16onMeshTrigger);
  }
}







/*
  _tPlayBoxState starts and stops new boxStates.
  It iterates only once. It is enabled by tPlayBoxStates.
  Its main iteration is delayed until aInterval has expired. aInterval is set in
  the main callback of _tPlayBoxStates (the "parent" Task). Such interval is
  equal to the duration of the new boxState.

  Upon being enabled, its onEnable callback:
  1. looks for the new boxState number, in myActiveState;
  Using the activeState number, it reads the associated sequence number in the
  properties of the corresponding boxState in the boxStates array;
  2. sets the new sequence to be played (by calling sequence::setActiveSequence());
  3. starts playing the sequence (by enabling the task sequence::tPlaySequenceInLoop.

  It iterates only once and does not have a main callback.

  Upon expiration of the Task, its onDisable callback disables
  Task tPlaySequenceInLoop.

  The interplay between Task _tPlayBoxStates, Task _tPlayBoxStates and
  Task sequence::tPlaySequenceInLoop is inspired by the third example provided
  with the taskScheduler library.
  - Task sequence::tPlaySequenceInLoop is equivalent to
  Task tLED(TASK_IMMEDIATE, TASK_FOREVER, NULL, &ts, false, NULL, &LEDOff)
  - Task _tPlayBoxState is equivalent to
  Task tBlink(5000, TASK_ONCE, NULL, &ts, false, &BlinkOnEnable, &BlinkOnDisable).

*/
Task boxState::_tPlayBoxState(0, 1, NULL, &userScheduler, false, &_oetcbPlayBoxState, &_odtcbPlayBoxState);


bool boxState::_oetcbPlayBoxState(){
  Serial.println("bool boxState::_oetcbPlayBoxState(). Starting.");
  // Serial.print("bool boxState::_oetcbPlayBoxState(). Box State Number: ");Serial.println(_thisBox.boxActiveState);

  boxState& _currentBoxState = boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState];

  // 1. Look for the sequence number to read when in this state
  short int _activeSequence = _currentBoxState.ui16AssociatedSequence;
  // Serial.print("bool boxState::_oetcbPlayBoxState() _activeSequence: ");
  // Serial.println(_activeSequence);

  // 2. Set the active sequence
  // Serial.println("bool boxState::_oetcbPlayBoxState() calling sequence::setActiveSequence(_activeSequence)");
  sequence::setActiveSequence(_activeSequence);

  // 3. Enable the sequence player, to play the sequence in loop until _tPlayBoxState
  // expires, for the duration mentionned in the activeState
  // Serial.println("bool boxState::_oetcbPlayBoxState() sequence::tPlaySequenceInLoop about to be enabled");
  sequence::tPlaySequenceInLoop.enable();

  // Signal the change of state to the mesh
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();

  Serial.println("bool boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}


/* Upon disabling the task which plays a given boxState,
  (i) disable the associated sequence player; and
  (ii) if the state which was being played was not the default state,
  set it to its default state.
*/
void boxState::_odtcbPlayBoxState(){
  Serial.println("void boxState::_odtcbPlayBoxState(). Starting.");
  // Serial.print("void boxState::_odtcbPlayBoxState() _tPlayBoxState.getInterval(): ");
  // Serial.println(_tPlayBoxState.getInterval());

  ControlerBox& _thisBox = ControlerBoxes[gui16MyIndexInCBArray];
  boxState& _currentBoxState = boxStates[_thisBox.boxActiveState];

  // 1. Disable the associated sequence player
  sequence::tPlaySequenceInLoop.disable();
  // Serial.println("void boxState::_odtcbPlayBoxState(): thisBox boxActiveState number");
  // Serial.println(_thisBox.boxActiveState);
  // Serial.println("void boxState::_odtcbPlayBoxState(): _boxTargetState");
  // Serial.println(_boxTargetState);

  // 2. Start the following state (timer interrupt)
  // or reset the boxState to default if no following state
  if (_currentBoxState.i16onExpire != -1) {
    _setBoxTargetState(_currentBoxState.i16onExpire);
  } else {
    _setBoxTargetState(_thisBox.sBoxDefaultState);

  }
  Serial.println("void boxState::_odtcbPlayBoxState(). Ending.");
}





// _setBoxTargetState is the central entry point to request a boxState change.
void boxState::_setBoxTargetState(const short __boxTargetState) {
  Serial.println("void boxState::_setBoxTargetState(). Starting.");
  Serial.print("void boxState::_setBoxTargetState(). targetBoxState: ");Serial.println(__boxTargetState);
  _boxActiveStateHasBeenReset = 1;
  _boxTargetState = __boxTargetState;
  // Serial.print("void boxState::_setBoxTargetState(). _boxTargetState: "); Serial.println(_boxTargetState);
  Serial.println("void boxState::_setBoxTargetState(). Ending.");
};
