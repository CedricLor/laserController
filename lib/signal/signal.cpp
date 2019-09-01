/*
  signal.cpp - Library to handle the signals received from whichever source
  Created by Cedric Lor, August 28, 2019.
*/

#include "Arduino.h"
#include "signal.h"


signal::signal()
{
  /** 
   *  --> NOT SURE 1. will be needed
   *  1. Define the callback of a Task located in ControlerBox, to be enabled upon
   *     receiving the registration of a new ControlerBox.
   *     This callback shall test:
   *  (a). whether the ControlerBoxes pertains to one of the monitored ControlerBoxes 
   *       (i.e. is one of the masterBoxes);
   *  (b). if so, it shall add a ref to the corresponding 
   *       ControlerBox to an array of references to monitored ControlerBoxes.
   * 
   *     Needed in ControlerBox:
   *     => Task:
   *     Task tIsNewBoxMonitored;
   *     Task ControlerBox::tIsNewBoxMonitored(0, 1, NULL, NULL, false, NULL, NULL);
   * 
   *     Needed here:
   *     => a function: 
   *     static void _tcbIsNewBoxMonitored();
   *     => a storage structure: i.e. a ControlerBox refs array? */


   /** 
   *  2. Define the callback of a Task located in ControlerBox, to be enabled upon
   *     receiving a change in the boxState of one of the ControlerBoxes.
   *     This callback shall test:
   *  (a). whether the ControlerBoxes pertains to one of the monitored ControlerBoxes 
   *       (i.e. is one of the masterBoxes);
   *  (b). whether the new boxState pertains to one of the monitored boxStates for this
   *       ControlerBox;
   *  (c). if both conditions are fullfiled, it shall turn a Mesh HIGH switch (as 
   *       instance property) in boxState. */
  ControlerBox::_tcbNsIsMeshHigh = *_tcbIsMeshHigh;

   /** 
   *  3. Define the callback of a Task located in ControlerBox, to be enabled upon
   *     receiving a change in IR of one of the ControlerBoxes (including thisBox).
   *     This callback shall test:
   *  (a). whether the ControlerBoxes pertains to one of the monitored ControlerBoxes 
   *       (i.e. is one of the masterBoxes);
   *  (b). if so, it shall set the relevant target boxState in boxState. */
  ControlerBox::_tcbNsIsIRHigh = *_tcbIsIRHigh;

  /**  4. Define the callback of a Task located in boxState, to be enabled upon starting
   *     a new boxState (or before, if it can be anticipated), to pass the list of 
   *     ControlerBoxes and the corresponding boxStates to be monitored
   *     to this "signal" class.
   * 
   *     Needed in boxState:
   *     => Task:
   *     Task boxState::tPassControlerBoxListToSignal
   * 
   *     Needed here:
   *     => a function: 
   *     static void _tcbPassControlerBoxListToSignal();
   *     => a storage structure: instance of signal?
   *    */
}




/***/
void signal::_tcbIsMeshHigh(const ControlerBox & _callingBox) {
  const boxState & _currentBoxState = boxState::boxStates[thisBox.i16BoxActiveState];
  // 1. check whether the current boxState is mesh sensitive
  if (_currentBoxState.i16onMeshTrigger == -1) {
    return;
  }
  // 2. if so, check whether mesh has been triggered and act
  if (_testIfMeshisHigh(_currentBoxState)) {
    boxState::_setBoxTargetState(_currentBoxState.i16onMeshTrigger);
  }
}




/** signal::_testIfMeshisHigh: tests, in the following order, whether:
 *  1. a masterBox has been defined and has connected to thisBox;
 *  2. the masterBox active state has been taken into account;
 *  3. the masterBox active state is different than -1; (??? why ???);
 *  4. thisBox in the currentBoxState is monitoring any states of its master;
 *  5. the masterBox active state corresponds to any monitored state of any of its masters.
 *  
 *  TODO in next implementation:
 *  1. rethink the order of the tests (if this box is not monitoring 
 *     any master state, why test if there is a masterBox or its state 
 *     has been taken into account, for instance)
 *  2. adapt for multiple masterBoxes;
 *  3. reset signal catchers;
 *  4. test whether the timestamp of the active state of the masterBox 
 *     predates the starting timestamp of thisBox's current state. */
bool signal::_testIfMeshisHigh(const boxState & _currentBoxState) {
  // has a masterBox been defined and has it connected to thisBox?
  if (_currentBoxState._masterBox == nullptr) {
    return false;
  }
  // has the masterBox active state been taken into account?
  if (_currentBoxState._masterBox->boxActiveStateHasBeenTakenIntoAccount) {
    return false;
  }
  // is the masterBox active state equal to -1 (how could it be)?
  if (_currentBoxState._masterBox->i16BoxActiveState == -1) {
    return false;
  }
  // is thisBox monitoring any state of its masters?
  if (_currentBoxState.i16monitoredMasterStates[0] == -1) {
    return false;
  }
  // check whether the boxState of the masterBox matches with any of the monitored states
  for (uint16_t _i = 0; _i < _currentBoxState.ui16monitoredMasterStatesSize; _i++) {
    if (_currentBoxState._masterBox->i16BoxActiveState == 
      _currentBoxState.i16monitoredMasterStates[_i]) {
      return true;
    }
  }
  return false;
}





void signal::_tcbIsIRHigh(const ControlerBox & _callingBox) {
  Serial.println("+++++++++++++++++++++++++ _tcbIsIRHigh +++++++++++++++++++++++++");
  const boxState & _currentBoxState = boxState::boxStates[thisBox.i16BoxActiveState];
  // 1. check whether the current boxState is IR sensitive
  if (_currentBoxState.i16onIRTrigger == -1) {
    return;
  }
  // 2. if so, check whether IR has been triggered
  if (_testIfIRisHigh(_currentBoxState)) {
    boxState::_setBoxTargetState(_currentBoxState.i16onIRTrigger);
  }
}




/** signal::_testIfIRisHigh: tests, in the following order, whether:
 *  1. thisBox IR sensor has received a new signal;
 *  2. the masterBox has sent an IR signal high.
 *  
 *  TODO in next implementation:
 *  1. split this boxes IR signal and the masterBox IR signal;
 *  2. adapt for multiple masterBoxes;
 *  3. reset signal catchers;
 *  4. compare whether the timestamp of the IR signals predate the starting
 *     timestamp of thisBox's current state. */
bool signal::_testIfIRisHigh(const boxState & _currentBoxState) {
  if (thisBox.ui16hasLastRecPirHighTimeChanged != 0) {
    return true;
  }
  if (_currentBoxState._masterBox == nullptr) {
    return false;
  }
  if (_currentBoxState._masterBox->ui16hasLastRecPirHighTimeChanged != 0) {
    return true;
  }
  return false;
}