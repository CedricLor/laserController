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
  |  |  |  |--note.cpp (called to play some member functions)
  |  |  |  |  |--note.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h

*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_boxTargetState = 0;
bool boxState::_boxActiveStateHasBeenReset = 0;
short int boxState::_boxDefaultState = 5;
const short int boxState::BOX_STATES_COUNT = 6;
boxState boxState::boxStates[BOX_STATES_COUNT];
const short int boxState::_NAME_CHAR_COUNT = 15;
const short int boxState::_HTML_TAG_CHAR_COUNT = 4;





// Constructors
boxState::boxState() {
}






// Initialisers
void boxState::_initBoxState(const char cpName[_NAME_CHAR_COUNT], const char cpHtmlTag[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  // Serial.println("void boxState::_initBoxState(). Starting.");
  strcpy(cName, cpName);
  strcpy(cHtmlTag, cpHtmlTag);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
  // Serial.println("void boxState::_initBoxState(). Ending.");
};

void boxState::initBoxStates() {
  Serial.println("void boxState::_initBoxStates(). Starting.");
  boxStates[0]._initBoxState("manual", "man", 1000000, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].cName: ");
  // Serial.println(boxStates[0].cName);
  // Serial.println("void boxState::_initBoxStates(). boxStates[0]._ulDuration");
  // Serial.println(boxStates[0]._ulDuration);
  boxStates[1]._initBoxState("align lasers", "ali", 1000000, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  boxStates[2]._initBoxState("pir Startup", "irs", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  boxStates[3]._initBoxState("pir High", "irh", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[4]._initBoxState("mesh High", "meh", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[5]._initBoxState("waiting", "wai", 1000000, 5, 1, 1); // sequence "all of" for indefinite time until trigger from either IR or mesh

  Serial.println("void boxState::_initBoxStates(). Ending.");
}





// boxState player wrapper
// tPlayBoxStates starts the execution of the various boxStates.
// It is enabled at the end of the setup.
// It then iterates indefinitely every seconds.
Task boxState::tPlayBoxStates(1000L, -1, &_tcbPlayBoxStates, &userScheduler, false, &_oetcbPlayBoxStates);

/*
  At each pass of tPlayBoxStates, _tcbPlayBoxStates() will check whether the
  following values have changed:
  - ControlerBox::valPir (when the current boxState is set to react to signals from the PIR);
  - ControlerBox::valMesh (when the current boxState is set to react to signals from the mesh);
  - _boxActiveStateHasBeenReset;
  - _boxTargetState;
  Depending on the changes, it will:
  - either start a new boxState or extend the duration of the current boxState; or
  - reset the values to their origin value.
  Otherwise, nothing happens.
*/
void boxState::_tcbPlayBoxStates() {
  // Serial.println("void boxState::_tcbPlayBoxStates(). Starting.");
  // Serial.print("void boxState::_tcbPlayBoxStates(). Iteration:");
  // Serial.println(tPlayBoxStates.getRunCounter());
  if (boxStates[ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState]._iIRTrigger == 1 && ControlerBox::valPir == HIGH) {
    _setBoxTargetState(3);
  }
  if (boxStates[ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState]._iMeshTrigger == 1 && !(ControlerBox::valMesh == -1)) {
    if (ControlerBox::valMesh == 3) { // the value 3 here is just inserted as an example
      _setBoxTargetState(4);
    }
  }
  ControlerBox::valPir = LOW;
  ControlerBox::valMesh = -1;
  if (_boxActiveStateHasBeenReset == 1) {
    _boxActiveStateHasBeenReset = 0;
    // Serial.print("void boxState::_tcbPlayBoxStates() boxStates[_boxTargetState]._ulDuration: ");
    // Serial.println(boxStates[_boxTargetState]._ulDuration);
    _tPlayBoxState.setInterval(boxStates[_boxTargetState]._ulDuration);
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    if (!(_boxTargetState == ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState)) {
      ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState = _boxTargetState;
    }
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState about to be enabled");
    _tPlayBoxState.restartDelayed();
    // Serial.println("void boxState::_tcbPlayBoxStates() _tPlayBoxState enabled");
    // Serial.print("void boxState::_tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    // Serial.println("*********************************************************");
  }
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





/*
  _tPlayBoxState plays a boxState once (it iterates only once).
  It is enabled by tPlayBoxStates.
  Its main iteration is delayed until aInterval has expired. aInterval is set in its onEnable callback.
  It is equal to the duration of the boxState selected by ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState.
  Upon being enabled, its onEnable callback:
  1. sets the aInterval of _tPlayBoxState, based on the _ulDuration of the currently active boxState;
  2. looks for the associated sequence using the ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState variable to select the relevant boxState in boxStates[];
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
  Serial.println("void boxState::_oetcbPlayBoxState(). Starting.");
  Serial.print("void boxState::_oetcbPlayBoxState(). Box State Number: ");
  Serial.println(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState);
  // Look for the sequence number to read when in this state
  short int _activeSequence = boxStates[ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState]._iAssociatedSequence;
  // Serial.print("void boxState::_oetcbPlayBoxState() _activeSequence: ");
  // Serial.println(_activeSequence);
  // set the active sequence
  // Serial.println("void boxState::_oetcbPlayBoxState() calling sequence::setActiveSequence(_activeSequence)");
  sequence::setActiveSequence(_activeSequence);
  // Play sequence in loop until end
  // Serial.println("void boxState::_oetcbPlayBoxState() sequence::tPlaySequenceInLoop about to be enabled");
  sequence::tPlaySequenceInLoop.enable();
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState); // TO UPDATE TO SEND THE BOX STATUS TO THE MESH
  // Serial.println("void boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}

void boxState::_odtcbPlayBoxState(){
  // Upon disabling the task which plays a given boxState,
  // (i) disable the associated sequence player; and
  // (ii) if the state which was being played was not the default state, set it to its default state

  // Serial.println("void boxState::_odtcbPlayBoxState(). Starting.------");
  // Serial.print("void boxState::_odtcbPlayBoxState() _tPlayBoxState.getInterval(): ");
  // Serial.println(_tPlayBoxState.getInterval());
  sequence::tPlaySequenceInLoop.disable();
  // Serial.println("void boxState::_odtcbPlayBoxState(): ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState");
  // Serial.println(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState);
  // Serial.println("void boxState::_odtcbPlayBoxState(): _boxTargetState");
  // Serial.println(_boxTargetState);
  if (!(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState == _boxDefaultState)) {
    _setBoxTargetState(_boxDefaultState);
  }
  // Serial.println("void boxState::_odtcbPlayBoxState(). Ending.");
}





// _setBoxTargetState receive boxState change requests from other classes
void boxState::_setBoxTargetState(const short targetBoxState) {
  // Serial.println("void boxState::_setBoxTargetState(). Starting.");
  // Serial.print("void boxState::_setBoxTargetState(). targetBoxState: ");
  // Serial.println(targetBoxState);
  _boxActiveStateHasBeenReset = 1;
  _boxTargetState = targetBoxState;
  // Serial.print("void boxState::_setBoxTargetState(). _boxTargetState: ");
  // Serial.println(_boxTargetState);
  // Serial.println("void boxState::_setBoxTargetState(). Ending.");
};
