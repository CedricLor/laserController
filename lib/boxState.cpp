/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.
*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_activeBoxState = 0;
short int boxState::_targetActiveBoxState = 0;
short int boxState::_activeBoxStateHasBeenReset = 0;
const short int boxState::BOX_STATES_COUNT = 7;
boxState boxState::boxStates[BOX_STATES_COUNT];
const short int boxState::_NAME_CHAR_COUNT = 15;






// Constructors
boxState::boxState() {
}

boxState::boxState(const char cName[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  Serial.println("void boxState::boxState(). Starting.");
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
  Serial.println("void boxState::boxState(). Ending.");
}





// Initialisers
void boxState::initBoxState(const char cName[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  Serial.println("void boxState::initBoxState(). Starting.");
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
  Serial.println("void boxState::initBoxState(). Ending.");
};

void boxState::initBoxStates() {
  Serial.println("void boxState::initBoxStates(). Starting.");
  boxStates[0].initBoxState("default - manual", 0, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::initBoxStates(). boxStates[0]._cName: ");
  // Serial.println(boxStates[0]._cName);
  // Serial.println("void boxState::initBoxStates(). boxStates[0]._ulDuration");
  // Serial.println(boxStates[0]._ulDuration);
  boxStates[1].initBoxState("align lasers", 0, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  boxStates[2].initBoxState("pir Startup", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  boxStates[3].initBoxState("pir High", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[4].initBoxState("mesh High", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[5].initBoxState("waiting", 0, 5, 1, 1); // sequence "all of" for indefinite time until trigger from either IR or mesh

  Serial.println("void boxState::initBoxStates(). Ending.");
}





// boxState player wrapper
// tPlayBoxStates starts the execution of the various boxStates.
// It is enabled at the end of the setup.
// It then iterates indefinitely every seconds.
Task boxState::tPlayBoxStates(1000L, -1, &tcbPlayBoxStates, &userScheduler, false, &oetcbPlayBoxStates);

/*
  At each pass of tPlayBoxStates, tcbPlayBoxStates() will check whether _activeBoxStateHasBeenReset has changed
  If it has changed, it:
  - resets _activeBoxStateHasBeenReset to 0;
  - sets the interval _tPlayBoxState to the value _activeBoxState to the value of _activeBoxState/_targetActiveBoxState;
  - checks whether the value of _targetActiveBoxState is equal to the value of _activeBoxState;
  - if _targetActiveBoxState is different than _activeBoxState, it:
    - enables _tPlayBoxState with delayed, to restart the sequence player;
    - resets _activeBoxState to _targetActiveBoxState.
  Otherwise, nothing happens.
*/
void boxState::tcbPlayBoxStates() {
  Serial.println("void boxState::tcbPlayBoxStates(). Starting.");
  if (boxStates[_activeBoxState]._iIRTrigger == 1) {
    if (ControlerBox::valPir == HIGH) {
      ControlerBox::valPir = LOW;
      setTargetActiveBoxState(3);
    }
  }
  if (_activeBoxStateHasBeenReset == 1) {
    _activeBoxStateHasBeenReset = 0;
    // Serial.print("void boxState::tcbPlayBoxStates() boxStates[_activeBoxState]._ulDuration: ");
    // Serial.println(boxStates[_targetActiveBoxState]._ulDuration);
    // Serial.print("void boxState::tcbPlayBoxStates() _tPlayBoxState.getInterval(): ");
    // Serial.println(_tPlayBoxState.getInterval());
    _tPlayBoxState.setInterval(boxStates[_targetActiveBoxState]._ulDuration);
    if (!(_targetActiveBoxState == _activeBoxState)) {
      _tPlayBoxState.enableDelayed();
      _activeBoxState = _targetActiveBoxState;
    }
  }
  Serial.println("void boxState::tcbPlayBoxStates(). Ending.");
};

// Upon tPlayBoxStates being enabled (at startup), the _targetActiveBoxState is being
// changed to 2 (pir Startup).
bool boxState::oetcbPlayBoxStates() {
  Serial.println("void boxState::oetcbPlayBoxStates(). Starting.");
  setTargetActiveBoxState(2); // 2 for pir Startup; upon enabling the task tPlayBoxStates, play the pirStartup boxState
  Serial.println("void boxState::oetcbPlayBoxStates(). Ending.");
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
  // Serial.println("void boxState::_oetcbPlayBoxState(). Starting.");
  // Serial.print("void boxState::_oetcbPlayBoxState(). Box State Number: ");
  // Serial.println(_activeBoxState);
  // Look for the sequence number to read when in this state
  short int _activeSequence = boxStates[_activeBoxState]._iAssociatedSequence;
  Serial.print("void boxState::_oetcbPlayBoxState() _activeSequence: ");
  Serial.println(_activeSequence);
  // set the active sequence
  Serial.println("void boxState::_oetcbPlayBoxState() calling sequence::setActiveSequence(_activeSequence)");
  sequence::setActiveSequence(_activeSequence);
  // Play sequence in loop until end
  Serial.print("void boxState::_oetcbPlayBoxState() sequence::tPlaySequenceInLoop.enable()");
  sequence::tPlaySequenceInLoop.enable();
  myMeshViews::statusMsg("on"); // TO UPDATE TO SEND THE BOX STATUS TO THE MESH
  Serial.println("void boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}

void boxState::_odtcbPlayBoxState(){
  Serial.println("void boxState::_odtcbPlayBoxState(). Starting.");
  sequence::tPlaySequenceInLoop.disable();
  myMeshViews::statusMsg("of"); // TO UPDATE TO SEND THE BOX STATUS TO THE MESH
  Serial.println("void boxState::_odtcbPlayBoxState(). Ending.");
}





// setTargetActiveBoxState receive boxState change requests from other classes
void boxState::setTargetActiveBoxState(const short targetActiveBoxState) {
  _activeBoxStateHasBeenReset = 1;
  _targetActiveBoxState = targetActiveBoxState;
};
