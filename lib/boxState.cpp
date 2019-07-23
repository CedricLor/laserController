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
const short int boxState::_NAME_CHAR_COUNT = 25;





// Constructors
boxState::boxState() {
}






// Initialisers
void boxState::_initBoxState(const char _cpName[_NAME_CHAR_COUNT], const unsigned long _ulDuration, const uint16_t _ui16AssociatedSequence, const byte __bIRTrigger, const byte __bMeshTrigger){
  // Serial.println("void boxState::_initBoxState(). Starting.");
  strcpy(cName, _cpName);
  ulDuration = _ulDuration;
  ui16AssociatedSequence = _ui16AssociatedSequence;
  _bIRTrigger = __bIRTrigger;
  _bMeshTrigger = __bMeshTrigger;
  // Serial.println("void boxState::_initBoxState(). Ending.");
};

void boxState::initBoxStates() {
  Serial.println("void boxState::_initBoxStates(). Starting.");
  boxStates[0]._initBoxState("manual / off", 1000000, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].cName: ");
  // Serial.println(boxStates[0].cName);
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].ulDuration");
  // Serial.println(boxStates[0].ulDuration);
  boxStates[1]._initBoxState("align lasers", 1000000, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  boxStates[2]._initBoxState("pir Startup", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  boxStates[3]._initBoxState("waiting both", 1000000, 5, 1, 1); // sequence "all of" for indefinite time until trigger from either IR or mesh
  boxStates[4]._initBoxState("waiting ir", 1000000, 5, 1, 0); // sequence "all of" for indefinite time until trigger from IR
  boxStates[5]._initBoxState("waiting mesh",  1000000, 5, 0, 1); // sequence "all of" for indefinite time until trigger from mesh
  boxStates[6]._initBoxState("pir High both interrupt", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[7]._initBoxState("pir High ir interrupt", 120000, 0, 1, 0); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  boxStates[8]._initBoxState("pir High mesh interrupt", 120000, 0, 0, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  boxStates[9]._initBoxState("pir High no interrupt", 120000, 0, 0, 0); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh
  boxStates[10]._initBoxState("mesh High both interrupt", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[11]._initBoxState("mesh High ir interrupt", 120000, 0, 1, 0); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  boxStates[12]._initBoxState("mesh High mesh interrupt", 120000, 0, 0, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  boxStates[13]._initBoxState("mesh High no interrupt", 120000, 0, 0, 0); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

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
  if (boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bIRTrigger == 1
    && boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bMeshTrigger == 1) {
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
  // once the new _boxTargetState has been set, in accordance with the signal
  // catchers, reset all the signals catchers to their initial values
  ControlerBox::valFromPir = LOW;
  ControlerBox::uiSettingTimeOfValFromPir = 0;
  ControlerBoxes[_ui16masterBoxIndexInCB].boxActiveStateHasBeenTakenIntoAccount = true;
  ControlerBox::valFromWeb = -1;
}


// _restart_tPlayBoxState() starts a new boxState, if the _boxActiveStateHasBeenReset
// has somehow been reset.
//
// If the _boxActiveStateHasBeenReset:
// 1. reset the witness _boxActiveStateHasBeenReset to 0;
// 2. reset the boxActiveState property of this box;
// 3. set the duration to stay in the new boxState (by setting the
// aInterval of the "children" Task _tPlayBoxState; _tPlayBoxState.setInterval), to
// the ulDuration of the target boxState (boxStates[_boxTargetState].ulDuration);
// 4. restart/enable the "children" Task _tPlayBoxState.
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
    // Removed the test -> this is a restart of the same boxState -> it shall
    // update the activeState in ControlerBox (via setBoxActiveState), because
    // the setBoxActiveState() method updates also the timestamp of the boxState
    // if (ControlerBoxes[gui16MyIndexInCBArray].boxActiveState != _boxTargetState ) {
    ControlerBox::setBoxActiveState(gui16MyIndexInCBArray, _boxTargetState, laserControllerMesh.getNodeTime());
    // }
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState about to be enabled");

    // 4. Restart/enable _tPlayBoxState
    _tPlayBoxState.restartDelayed();
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState enabled");
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");Serial.println(_tPlayBoxState.getInterval());
    // Serial.println("*********************************************************");
  }
}






/*
  _tPlayBoxState starts and stops new boxStates.
  It iterates only once. It is enabled by tPlayBoxStates.
  Its main iteration is delayed until aInterval has expired. aInterval is set in
  the main callback of _tPlayBoxStates (the "parent" Task). Such interval is
  equal to the duration of the new boxState.

  Upon being enabled, its onEnable callback:
  1. looks for the new boxState number, in ControlerBoxes[gui16MyIndexInCBArray].boxActiveState;
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
  // Serial.print("bool boxState::_oetcbPlayBoxState(). Box State Number: ");Serial.println(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState);

  // 1. Look for the sequence number to read when in this state
  short int _activeSequence = boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState].ui16AssociatedSequence;
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

  // 1. Disable the associated sequence player
  sequence::tPlaySequenceInLoop.disable();
  // Serial.println("void boxState::_odtcbPlayBoxState(): ControlerBoxes[gui16MyIndexInCBArray].boxActiveState");
  // Serial.println(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState);
  // Serial.println("void boxState::_odtcbPlayBoxState(): _boxTargetState");
  // Serial.println(_boxTargetState);

  // 2. Reset the boxState to default
  if (ControlerBoxes[gui16MyIndexInCBArray].boxActiveState != ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState) {
    _setBoxTargetState(ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState);
  }
  Serial.println("void boxState::_odtcbPlayBoxState(). Ending.");
}





// _setBoxTargetState is the central entry point to request a boxState change.
void boxState::_setBoxTargetState(const short __boxTargetState) {
  Serial.println("void boxState::_setBoxTargetState(). Starting.");
  // Serial.print("void boxState::_setBoxTargetState(). targetBoxState: ");Serial.println(__boxTargetState);
  _boxActiveStateHasBeenReset = 1;
  _boxTargetState = __boxTargetState;
  // Serial.print("void boxState::_setBoxTargetState(). _boxTargetState: "); Serial.println(_boxTargetState);
  Serial.println("void boxState::_setBoxTargetState(). Ending.");
};
