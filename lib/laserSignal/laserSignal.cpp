/*
  laserSignal.cpp - Library to handle the impact of signals read from the mesh 
  on the boxStates of this box
  Created by Cedric Lor, August 28, 2019.
*/

#include "Arduino.h"
#include "laserSignal.h"


laserSignal::laserSignal(ControlerBox & __thisCtrlerBox, myMeshViews & __thisMeshViews):
  _thisCtrlerBox(__thisCtrlerBox),
  ctlBxColl{},
  thisBxStateColl(_thisCtrlerBox, __thisMeshViews)
{
  tSetBoxState.set(0, 1, NULL, NULL, NULL);
}




void laserSignal::startup() {
  thisBxStateColl._setBoxTargetState(2); 
  /** 2 for pir Startup; at startUp, put the box in pirStartup state 
   *  TODO for next implementation: 
   *  - define a isIr bool in global;
   *  - define a isLaser bool in global;
   *  - if isIr and isLaser, block this box -> setPir for 60 seconds at startup */
}





/** laserSignal::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb)
 * 
 * Setter for i16boxStateRequestedFromWeb
 * 
 *  Called from: 
 *  - myMeshController, upon receiving a changeBox request from the web. */
void laserSignal::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
  i16boxStateRequestedFromWeb = _i16boxStateRequestedFromWeb;
  /** Set the Task that will check whether this change shall have an impact
   *  on this box's boxState, add it to the Scheduler and restart it. */
  tSetBoxState.setCallback([&](){_tcbSetBoxStateFromWeb();});
  tSetBoxState.restart();
}


/** laserSignal::_tcbSetBoxStateFromWeb() set the target boxState from
 *  a changeBox request. */
void laserSignal::_tcbSetBoxStateFromWeb() {
  thisBxStateColl._setBoxTargetState(i16boxStateRequestedFromWeb);
}



/** const bool laserSignal::checkImpactOfChangeInActiveStateOfOtherBox(const uint16_t __ui16BoxIndex)
 * 
 *  Checks the impact of change in active boxState of another controller box
 *  in the mesh on this boxState.
 * 
 *  Called only from this class (for the other boxes) and by boxState 
 *  (when an effective update has been made). */
const bool laserSignal::checkImpactOfChangeInActiveStateOfOtherBox(const uint16_t __ui16BoxIndex) {
  // Serial.println("laserSignal::checkImpactOfChangeInActiveStateOfOtherBox(): starting");

  /** Set the Task that will check whether this change shall have an impact
   *  on this box's boxState, add it to the Scheduler and restart it testing
   *  whether the callback _tcbIfMeshTriggered has been set and that 
   *  the ControlerBox is not this box (this box does not read its own mesh high
   *  signals; it sends mesh high signals). */
  tSetBoxState.setCallback([&, __ui16BoxIndex](){
    _tcbIfMeshTriggered(ctlBxColl.controllerBoxesArray.at(__ui16BoxIndex));
  });
  tSetBoxState.restart();
  return true;
}



/** const bool laserSignal::checkImpactOfUpstreamInformationOfOtherBox(const uint16_t __ui16BoxIndex)
 * 
 *  This methods does not directly checks the impact of an upstream information 
 *  on this box. It sets and restarts the Task tSetBoxState that will carry out
 *  this test.
 * 
 *  For the moment, tSetBoxState is limited to checking IR upstream information.
 * 
 *  Called from from myMeshController only, upon receiving "usi" messages from
 *  other boxes.
 * 
 *  TODO: 1. for the moment, it is designed to check the impact of "usi" messages 
 *           of type "IR High" only (it is calling _tcbIfIRTriggered()).
 *        2. for the moment, even if it detects that another box has sent an 
 *           "IR High" message, nothing will happen (because the relevant 
 *           "i16onMasterIRTrigger" or similar have not been implemented
 *           in the step and boxState classes).
 *        
 *        Regarding point 2., in the future, I might add an i16onMasterIRTrigger property to boxState
 *        to handle the masterBox(es) IR signals.
 *        
 *        Regarding point 1., in the future, I might add an i16onMasterSequenceTrigger property to boxState
 *        to handle the masterBox(es) sequence signals.
 *  */
const bool laserSignal::checkImpactOfUpstreamInformationOfOtherBox(const uint16_t __ui16BoxIndex) {
/** Set the Task that will check whether this change shall have an impact
   *  on this box's boxState, add it to the Scheduler and restart it. 
   * */
  tSetBoxState.setCallback([&, __ui16BoxIndex](){
    _tcbIfIRTriggered(ctlBxColl.controllerBoxesArray.at(__ui16BoxIndex));
  });
  tSetBoxState.restart();
  return true;
}




/** const bool laserSignal::checkImpactOfThisBoxsIRHigh()
 * 
 *  Checks the impact on the boxState of this box of a IR High signal
 *  coming from the IR of this box.
 * 
 *  Called from PIR Controller only.
*/
const bool laserSignal::checkImpactOfThisBoxsIRHigh() {
  tSetBoxState.setCallback([&](){
    _tcbIfIRTriggered(_thisCtrlerBox);
  });
  tSetBoxState.restart();
  return true;
}




/***/
void laserSignal::_tcbIfMeshTriggered(const ControlerBox & _callingBox) {
  const boxState & _currentBoxState = thisBxStateColl.boxStatesArray.at(_thisCtrlerBox.i16BoxActiveState);
  // 1. check whether the current boxState is mesh sensitive
  if (_currentBoxState.i16onMeshTrigger == -1) {
    return;
  }
  // 2. if so, check whether mesh has been triggered and act
  if (_testIfMeshisHigh(_currentBoxState, _callingBox)) {
    thisBxStateColl._setBoxTargetState(_currentBoxState.i16onMeshTrigger);
  }
}




/** laserSignal::_testIfMeshisHigh: tests, in the following order, whether:
 *  1. the caller is among the monitored masters;
 *  2. the masterBox new active state corresponds to one of the states
 *     monitored by this box in its currentState;
 *  3. if the masterBox new active state has been set more recently than the currentState of this box.*/
bool laserSignal::_testIfMeshisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox) {
  // is calling box being monitored by this box in its current state?
  if ( !(_isCallerInMonitoredArray(_callingBox, _currentBoxState.ui16monitoredMasterBoxesNodeNames)) ) {
    return false;
  }
  // check whether the boxState of the masterBox (_callingBox) matches with any of the monitored states
  return _isCallerStateInMonitoredStates(_callingBox, _currentBoxState);
}


/** laserSignal::_tcbIfIRTriggered() tests whether:
 *  1. the current boxState is IR sensitive;
 *  2. IR shall be considered as high;
 *  and if both conditions are fullfilled, it resets/changes
 *  thisBox targetState in boxState. 
 * 
 *  TODO:
 *  1. add an i16onMasterIRTrigger property to boxState to handle the masterBox(es) IR signals; */
void laserSignal::_tcbIfIRTriggered(const ControlerBox & _callingBox) {
  Serial.println("+++++++++++++++++++++++++ _tcbIfIRTriggered +++++++++++++++++++++++++");
  const boxState & _currentBoxState = thisBxStateColl.boxStatesArray.at(_thisCtrlerBox.i16BoxActiveState);
  // 1. check whether the current boxState is IR sensitive
  if (_currentBoxState.i16onIRTrigger == -1) {
    return;
  }
  // 2. if so, check whether IR has been triggered
  if (_testIfIRisHigh(_callingBox, _currentBoxState)) {
    thisBxStateColl._setBoxTargetState(_currentBoxState.i16onIRTrigger);
  }
}




/** laserSignal::_testIfIRisHigh() tests, in the following order, whether:
 *  1. the calling box is this box;
 *  2. the signal is more recent than the last registered box state time stamp;
 * 
 * No longer testing:
 *  3. the callingBox is a masterBox;
 *  4. the masterBox has sent an IR high signal more recent than the boxState time stamp.
 *  
 *  TODO in future implementation:
 *  1. activate 3 and 4 re. _testIfIRHighIsAmongMasters;
 *     timestamp of this box's current state. */
bool laserSignal::_testIfIRisHigh(const ControlerBox & _callingBox, const boxState & _currentBoxState) {
  return _testIfIRisHighIsMine(_callingBox);
  // _testIfIRHighIsAmongMasters(_callingBox, _currentBoxState.ui16monitoredMasterBoxesNodeNames);
}



bool laserSignal::_testIfIRisHighIsMine(const ControlerBox & _callingBox) {
  if ( _isCallerThisBox(_callingBox) ) {
    return (_isSignalFresherThanBoxStateStamp(_thisCtrlerBox.ui32lastRecPirHighTime));
  }
  return false;
}




bool laserSignal::_isCallerThisBox(const ControlerBox & _callingBox) {
  return (std::addressof(_callingBox) == std::addressof((_thisCtrlerBox)));
}



bool laserSignal::_testIfIRHighIsAmongMasters(const ControlerBox & _callingBox, const std::array<uint16_t, 4U> & __ui16monitoredMasterBoxesNodeNames) {
  if (_isCallerInMonitoredArray(_callingBox, __ui16monitoredMasterBoxesNodeNames)) {
    return (_isSignalFresherThanBoxStateStamp(_callingBox.ui32lastRecPirHighTime));
  }
  return false;
}




bool laserSignal::_isCallerMonitored(const ControlerBox & _callingBox, const uint16_t _ui16MonitoredNodeName) {
  return (_callingBox.ui16NodeName == _ui16MonitoredNodeName);
  }




bool laserSignal::_isCallerInMonitoredArray(const ControlerBox & _callingBox, const std::array<uint16_t, 4U> & __ui16monitoredMasterBoxesNodeNames) {
  return (std::find(
    std::begin(__ui16monitoredMasterBoxesNodeNames), 
    std::end(__ui16monitoredMasterBoxesNodeNames),
    _callingBox.ui16NodeName) 
    != std::end(__ui16monitoredMasterBoxesNodeNames)
  );
}




bool laserSignal::_isCallerStateInMonitoredStates(const ControlerBox & _callingBox, const boxState & _currentBoxState) {
  return (
    std::find(
     std::begin(_currentBoxState.i16monitoredMasterStates), 
      std::end(_currentBoxState.i16monitoredMasterStates),
      _callingBox.i16BoxActiveState) 
    != std::end(_currentBoxState.i16monitoredMasterStates)
  );
}




bool laserSignal::_isSignalFresherThanBoxStateStamp(const uint32_t _ui32SignalTime) {
  return (_ui32SignalTime > _thisCtrlerBox.ui32BoxActiveStateStartTime);
}