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
   *     receiving the registration of/connection with a new ControlerBox.
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

  /** The three following lines each associate a function defined here to 
   *  a pointer to function declared in ControlerBox.
   *  
   *  These functions correspond to the three sources of signal:
   *  - from web;
   *  - from mesh;
   *  - from IR. 
   * 
   *  Upon receiving one of these signals, the ControlerBox class:
   *  1- declare the relevant function as callback of a Task tSetBoxState;
   *  2- enable Task tSetBoxState.
   * 
   *  Each of these functions will then check whether the signal is a trigger to change
   *  a boxState. If so, they reset the boxState. */
  ControlerBox::_tcbSetBoxStateFromWeb = *_tcbSetBoxStateFromWeb;
  ControlerBox::_tcbIfMeshTriggered = *_tcbIfMeshTriggered;
  ControlerBox::_tcbIfIRTriggered = *_tcbIfIRTriggered;

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




void signal::startup() {
  boxState::_setBoxTargetState(2); 
  /** 2 for pir Startup; at startUp, put the box in pirStartup state 
   *  TODO for next implementation: 
   *  - define a isIr bool in global;
   *  - define a isLaser bool in global;
   *  - if isIr and isLaser, block thisBox -> setPir for 60 seconds at startup */
}





/** signal::_tcbSetBoxStateFromWeb() set the target boxState from
 *  a changeBox request. */
void signal::_tcbSetBoxStateFromWeb() {
  boxState::_setBoxTargetState(ControlerBox::i16boxStateRequestedFromWeb);
}




/***/
void signal::_tcbIfMeshTriggered(const ControlerBox & _callingBox) {
  const boxState & _currentBoxState = boxState::boxStates[thisBox.i16BoxActiveState];
  // 1. check whether the current boxState is mesh sensitive
  if (_currentBoxState.i16onMeshTrigger == -1) {
    return;
  }
  // 2. if so, check whether mesh has been triggered and act
  if (_testIfMeshisHigh(_currentBoxState, _callingBox)) {
    boxState::_setBoxTargetState(_currentBoxState.i16onMeshTrigger);
  }
}




/** signal::_testIfMeshisHigh: tests, in the following order, whether:
 *  1. the caller is monitored;
 *  2. the masterBox new active state corresponds to state monitored by thisBox in its currentState;
 *  3. if the masterBox new active state has been set more recently than the currentState of thisBox.
 *  
 *  TODO in next implementation:
 *  1. adapt for multiple masterBoxes;
 *  2. reset signal catchers; */
bool signal::_testIfMeshisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox) {
  // is calling box being monitored by thisBox in its current state?
  if (!(_isCallerMonitored(_callingBox, thisBox.ui16MasterBoxName)) ) {
    return false;
  }
  // check whether the boxState of the masterBox matches with any of the monitored states
  return (
    std::find(
     std::begin(_currentBoxState.i16monitoredMasterStates), 
      std::end(_currentBoxState.i16monitoredMasterStates),
      _callingBox.i16BoxActiveState) 
    != std::end(_currentBoxState.i16monitoredMasterStates)
  );
}


/** signal::_tcbIfIRTriggered() tests whether:
 *  1. the current boxState is IR sensitive;
 *  2. IR shall be considered as high;
 *  and if both conditions are fullfilled, it resets/changes
 *  thisBox targetState in boxState. 
 * 
 *  TODO:
 *  1. split this boxes IR signal and the masterBox(es) IR signals; */
void signal::_tcbIfIRTriggered(const ControlerBox & _callingBox) {
  Serial.println("+++++++++++++++++++++++++ _tcbIfIRTriggered +++++++++++++++++++++++++");
  const boxState & _currentBoxState = boxState::boxStates[thisBox.i16BoxActiveState];
  // 1. check whether the current boxState is IR sensitive
  if (_currentBoxState.i16onIRTrigger == -1) {
    return;
  }
  // 2. if so, check whether IR has been triggered
  if (_testIfIRisHigh(_currentBoxState, _callingBox)) {
    boxState::_setBoxTargetState(_currentBoxState.i16onIRTrigger);
  }
}




/** signal::_testIfIRisHigh() tests, in the following order, whether:
 *  1. the caller is thisBox;
 *  2. thisBox IR sensor has received a new signal;
 *  3. the callingBox is the masterBox;
 *  3. the masterBox has sent an IR high signal.
 *  
 *  TODO in next implementation:
 *  1. split this boxes IR signal and the masterBox IR signal;
 *  2. adapt for multiple masterBoxes;
 *     timestamp of thisBox's current state. */
bool signal::_testIfIRisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox) {
  if ( std::addressof(_callingBox) == std::addressof((thisBox)) ) {
    return (_isSignalFresherThanBoxStateStamp(thisBox.ui32lastRecPirHighTime));
  }

  if (_isCallerMonitored(_callingBox, thisBox.ui16MasterBoxName)) {
    return (_isSignalFresherThanBoxStateStamp(_callingBox.ui32lastRecPirHighTime));
  }
  return false;
}




bool signal::_isCallerMonitored(const ControlerBox & _callingBox, const uint16_t _ui16MonitoredNodeName) {
  return (_callingBox.ui16NodeName == _ui16MonitoredNodeName);
  }




bool signal::_isSignalFresherThanBoxStateStamp(const uint32_t _ui32SignalTime) {
  return (_ui32SignalTime > thisBox.ui32BoxActiveStateStartTime);
}