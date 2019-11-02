/*
  laserSignal.cpp - Library to handle the impact of signals read from the mesh 
  on the boxStates of this box
  Created by Cedric Lor, August 28, 2019.
*/

#include "Arduino.h"
#include "laserSignal.h"





/** class constructor
 * */
laserSignal::laserSignal(ControlerBox & __thisCtrlerBox, myMeshViews & __thisMeshViews):
  _thisCtrlerBox(__thisCtrlerBox),
  ctlBxColl{},
  thisBxStateColl(_thisCtrlerBox, __thisMeshViews)
{
  tSetBoxState.set(0, 1, NULL, NULL, NULL);
}





/** void laserSignal::startup()
 * 
 *  Set this box boxState to 2: "pir Startup"
 *  
 *  Called from main::setup()::enableTasks() (??? yes...), if 
 *  thisControllerBox.globBaseVars.hasLasers == true.
 *  => not called if this box is an interface without lasers.
 *  
 *  TODO for next implementation: block box in "pir Startup" for 
 *  60 seconds, time for (i) the PIR to warmup and (ii) connect to the mesh
 *  and synchronize time.
 * */
void laserSignal::startup() {
  thisBxStateColl._setBoxTargetState(2); 
}




/*******************************************/
/** TASK STARTERS **************************/
/*******************************************/

/** laserSignal::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb)
 * 
 * Setter for i16boxStateRequestedFromWeb
 * 
 *  Called from: 
 *  - myMeshController, upon receiving a changeBox request from the web.
 * */
void laserSignal::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
  /** Set the Task that will check whether this change shall have an impact
   *  on this box's boxState, add it to the Scheduler and restart it. */
  tSetBoxState.setCallback([&, _i16boxStateRequestedFromWeb](){
    _tcbSetBoxStateFromWeb(_i16boxStateRequestedFromWeb);
  });
  tSetBoxState.restart();
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







/*******************************************/
/** TASK CALLBACKS *************************/
/*******************************************/

/*************************************/
/** FOR WEB SIGNAL *******************/
/*************************************/

/** laserSignal::_tcbSetBoxStateFromWeb(const int16_t _i16boxStateRequestedFromWeb):
 * 
 *  set the target boxState from a changeBox request.
 * */
void laserSignal::_tcbSetBoxStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
  thisBxStateColl._setBoxTargetState(_i16boxStateRequestedFromWeb);
}





/*************************************/
/** FOR CHANGES IN BOX STATE *********/
/*************************************/

/** void laserSignal::_tcbIfMeshTriggered(const ControlerBox & _callingBox)
 * 
 *  Reads the boxState of the calling box in the controller boxes array.
 *  Tests whether this signal shall trigger a change in boxState of this box.
 *  Orders the change in boxState, as the case may be.
*/
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





/*************************************/
/** FOR IR HIGH SIGNALS **************/
/*************************************/

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
 * 
 *  1. the callingBox is a masterBox;
 *  2. the masterBox has sent an IR high signal more recent than the boxState time stamp.
 * */
bool laserSignal::_testIfIRisHigh(const ControlerBox & _callingBox, const boxState & _currentBoxState) {
  // _testIfIRHighIsAmongMasters(_callingBox, _currentBoxState.ui16monitoredMasterBoxesNodeNames);
}



bool laserSignal::_testIfIRHighIsAmongMasters(const ControlerBox & _callingBox, const std::array<uint16_t, 4U> & __ui16monitoredMasterBoxesNodeNames) {
  if (_isCallerInMonitoredArray(_callingBox, __ui16monitoredMasterBoxesNodeNames)) {
    return (_isSignalFresherThanBoxStateStamp(_callingBox.ui32lastRecPirHighTime));
  }
  return false;
}





/*************************************/
/** COMMON SUBS **********************/
/*************************************/

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