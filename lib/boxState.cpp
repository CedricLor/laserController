/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.
*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_activeBoxState = 0;
short int boxState::_targetActiveBoxState = 0;
short int boxState::_activeBoxStateHasBeenReset = 0;
short int boxState::_defaultBoxState = 5;
const short int boxState::BOX_STATES_COUNT = 7;
boxState boxState::boxStates[BOX_STATES_COUNT];
const short int boxState::_NAME_CHAR_COUNT = 15;





// Constructors
boxState::boxState() {
}

boxState::boxState(const char cName[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  // Serial.println("void boxState::boxState(). Starting.");
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
  // Serial.println("void boxState::boxState(). Ending.");
}





// Initialisers
void boxState::initBoxState(const char cName[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  // Serial.println("void boxState::initBoxState(). Starting.");
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
  // Serial.println("void boxState::initBoxState(). Ending.");
};

void boxState::initBoxStates() {
  Serial.println("void boxState::initBoxStates(). Starting.");
  boxStates[0].initBoxState("manual", 1000000, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::initBoxStates(). boxStates[0]._cName: ");
  // Serial.println(boxStates[0]._cName);
  // Serial.println("void boxState::initBoxStates(). boxStates[0]._ulDuration");
  // Serial.println(boxStates[0]._ulDuration);
  boxStates[1].initBoxState("align lasers", 1000000, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  boxStates[2].initBoxState("pir Startup", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  boxStates[3].initBoxState("pir High", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[4].initBoxState("mesh High", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[5].initBoxState("waiting", 1000000, 5, 1, 1); // sequence "all of" for indefinite time until trigger from either IR or mesh

  Serial.println("void boxState::initBoxStates(). Ending.");
}





// boxState player wrapper
// tPlayBoxStates starts the execution of the various boxStates.
// It is enabled at the end of the setup.
// It then iterates indefinitely every seconds.
Task boxState::tPlayBoxStates(1000L, -1, &tcbPlayBoxStates, &userScheduler, false, &oetcbPlayBoxStates);

/*
  At each pass of tPlayBoxStates, tcbPlayBoxStates() will check whether the
  following values have changed:
  - ControlerBox::valPir (when the current boxState is set to react to signals from the PIR);
  - ControlerBox::valMesh (when the current boxState is set to react to signals from the mesh);
  - _activeBoxStateHasBeenReset;
  - _targetActiveBoxState;
  Depending on the changes, it will:
  - either start a new boxState or extend the duration of the current boxState; or
  - reset the values to their origin value.
  Otherwise, nothing happens.
*/
void boxState::tcbPlayBoxStates() {
  // Serial.println("void boxState::tcbPlayBoxStates(). Starting.");
  // Serial.print("void boxState::tcbPlayBoxStates(). Iteration:");
  // Serial.println(tPlayBoxStates.getRunCounter());
  if (boxStates[_activeBoxState]._iIRTrigger == 1 && ControlerBox::valPir == HIGH) {
    setTargetActiveBoxState(3);
  }
  if (boxStates[_activeBoxState]._iMeshTrigger == 1 && !(ControlerBox::valMesh == -1)) {
    if (ControlerBox::valMesh == 3) { // the value 3 here is just inserted as an example
      setTargetActiveBoxState(4);
    }
  }
  ControlerBox::valPir = LOW;
  ControlerBox::valMesh = -1;
  if (_activeBoxStateHasBeenReset == 1) {
    _activeBoxStateHasBeenReset = 0;
    // Serial.print("void boxState::tcbPlayBoxStates() boxStates[_targetActiveBoxState]._ulDuration: ");
    // Serial.println(boxStates[_targetActiveBoxState]._ulDuration);
    _tPlayBoxState.setInterval(boxStates[_targetActiveBoxState]._ulDuration);
    // Serial.print("void boxState::tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    if (!(_targetActiveBoxState == _activeBoxState)) {
      _activeBoxState = _targetActiveBoxState;
    }
    // Serial.println("void boxState::tcbPlayBoxStates() _tPlayBoxState about to be enabled");
    _tPlayBoxState.restartDelayed();
    // Serial.println("void boxState::tcbPlayBoxStates() _tPlayBoxState enabled");
    // Serial.print("void boxState::tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    // Serial.println("*********************************************************");
  }
  // Serial.println("void boxState::tcbPlayBoxStates(). Ending.");
};

// Upon tPlayBoxStates being enabled (at startup), the _targetActiveBoxState is being
// changed to 2 (pir Startup).
bool boxState::oetcbPlayBoxStates() {
  // Serial.println("void boxState::oetcbPlayBoxStates(). Starting.");
  setTargetActiveBoxState(2); // 2 for pir Startup; upon enabling the task tPlayBoxStates, play the pirStartup boxState
  // Serial.println("void boxState::oetcbPlayBoxStates(). Ending.");
  return true;
}





/*
  _tPlayBoxState plays a boxState once (it iterates only once).
  It is enabled by tPlayBoxStates.
  Its main iteration is delayed until aInterval has expired. aInterval is set in its onEnable callback.
  It is equal to the duration of the boxState selected by _activeBoxState.
  Upon being enabled, its onEnable callback:
  1. sets the aInterval of _tPlayBoxState, based on the _ulDuration of the currently active boxState;
  2. looks for the associated sequence using the _activeBoxState static variable to select the relevant boxState in boxStates[];
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
  Serial.println(_activeBoxState);
  // Look for the sequence number to read when in this state
  short int _activeSequence = boxStates[_activeBoxState]._iAssociatedSequence;
  // Serial.print("void boxState::_oetcbPlayBoxState() _activeSequence: ");
  // Serial.println(_activeSequence);
  // set the active sequence
  // Serial.println("void boxState::_oetcbPlayBoxState() calling sequence::setActiveSequence(_activeSequence)");
  sequence::setActiveSequence(_activeSequence);
  // Play sequence in loop until end
  // Serial.println("void boxState::_oetcbPlayBoxState() sequence::tPlaySequenceInLoop about to be enabled");
  sequence::tPlaySequenceInLoop.enable();
  myMeshViews::statusMsg(_activeBoxState); // TO UPDATE TO SEND THE BOX STATUS TO THE MESH
  // Serial.println("void boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}

void boxState::_odtcbPlayBoxState(){
  // Serial.println("void boxState::_odtcbPlayBoxState(). Starting.------");
  // Serial.print("void boxState::_odtcbPlayBoxState() _tPlayBoxState.getInterval(): ");
  // Serial.println(_tPlayBoxState.getInterval());
  sequence::tPlaySequenceInLoop.disable();
  // Serial.println("void boxState::_odtcbPlayBoxState(): _activeBoxState");
  // Serial.println(_activeBoxState);
  // Serial.println("void boxState::_odtcbPlayBoxState(): _targetActiveBoxState");
  // Serial.println(_targetActiveBoxState);
  if (!(_activeBoxState == _defaultBoxState)) {
    setTargetActiveBoxState(_defaultBoxState);
  }
  // Serial.println("void boxState::_odtcbPlayBoxState(). Ending.");
}





// setTargetActiveBoxState receive boxState change requests from other classes
void boxState::setTargetActiveBoxState(const short targetActiveBoxState) {
  // Serial.println("void boxState::setTargetActiveBoxState(). Starting.");
  // Serial.print("void boxState::setTargetActiveBoxState(). targetActiveBoxState: ");
  // Serial.println(targetActiveBoxState);
  _activeBoxStateHasBeenReset = 1;
  _targetActiveBoxState = targetActiveBoxState;
  // Serial.print("void boxState::setTargetActiveBoxState(). _targetActiveBoxState: ");
  // Serial.println(_targetActiveBoxState);
  // Serial.println("void boxState::setTargetActiveBoxState(). Ending.");
};
