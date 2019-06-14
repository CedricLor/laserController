/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.
*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_activeBoxState = 0;
short int boxState::_targetActiveBoxState = 0;
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
// It then iterates indefinitely at each pass of the main loop.
Task boxState::tPlayBoxStates(0, -1, &tcbPlayBoxStates, &userScheduler, false, &oetcbPlayBoxStates);

/*
  At each pass of tPlayBoxStates, tcbPlayBoxStates() will check whether _targetActiveBoxState has changed
  If it has changed, it:
  - set the _activeBoxState to the value of _targetActiveBoxState;
  - calls playBoxState, to restart the sequence player.
  Otherwise, nothing happens.
*/
void boxState::tcbPlayBoxStates() {
  Serial.println("void boxState::tcbPlayBoxStates(). Starting.");
  if (!(_targetActiveBoxState == _activeBoxState)) {
    _activeBoxState = _targetActiveBoxState;
    playBoxState();
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





// boxState single player
/*
  playBoxState() controls Task _tPlayBoxState.
  1. It sets the aInterval of _tPlayBoxState, based on the _ulDuration of the currently active boxState.
  2. It then enables _tPlayBoxState, without delay.
*/
void boxState::playBoxState(){
  Serial.println("void boxState::playBoxState(). Starting");
  Serial.print("void boxState::playBoxState(). Box State Number: ");
  Serial.println(_activeBoxState);
  _tPlayBoxState.setInterval(boxStates[_activeBoxState]._ulDuration);
  // Serial.print("void boxState::playBoxState(). _ulDuration: ");
  // Serial.println(boxStates[_activeBoxState]._ulDuration);
  _tPlayBoxState.enable();
  Serial.println("void boxState::playBoxState(). Task _tPlayBoxState enabled");
  Serial.println("void boxState::playBoxState(). Ending");
};

/*
  _tPlayBoxState plays a boxState once (it iterates only once).
  Its main iteration is delayed until aInterval has expired. aInterval is set
  in the playBoxState function. It is equal to the duration of the boxState.
  Upon being enabled, its onEnable callback:
  1. looks for the associated sequence using the _activeBoxState static variable to select the relevant boxState in boxStates[];
  2. sets the active sequence by a call to sequence::setActiveSequence();
  3. enables the task sequence::tPlaySequenceInLoop.
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
  // Look for the sequence number to read when in this state
  short int _activeSequence = boxStates[_activeBoxState]._iAssociatedSequence;
  // set the active sequence
  sequence::setActiveSequence(_activeSequence);
  // Play sequence in loop until end
  sequence::tPlaySequenceInLoop.enable();
  Serial.println("void boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}

void boxState::_odtcbPlayBoxState(){
  sequence::tPlaySequenceInLoop.disable();
}





// setTargetActiveBoxState receive boxState change requests from other classes
void boxState::setTargetActiveBoxState(const short targetActiveBoxState) {
  _targetActiveBoxState = targetActiveBoxState;
};
