/*
  ControlerBox.cpp - Library to replace box_type struct - handles the ControlerBox attributes
  Created by Cedric Lor, January 2, 2019.

  |--main.cpp
  |  |
  |  |--ControlerBox.cpp
  |  |  |--ControlerBox.h
  |  |  |--global.cpp (laserControllerMesh)
  |  |  |  |--global.h

*/

#include "Arduino.h"
#include "ControlerBox.h"


// STATIC VARIABLES - SIGNAL CATCHERS
int16_t ControlerBox::i16boxStateRequestedFromWeb = -1;
short int ControlerBox::connectedBoxesCount = 1;
short int ControlerBox::previousConnectedBoxesCount = 1;
void (*ControlerBox::_tcbNsIsMeshHigh)(const ControlerBox & _callingBox) = nullptr;
void (*ControlerBox::_tcbNsIsIRHigh)(const ControlerBox & _callingBox) = nullptr;
void (*ControlerBox::_tcbSetBoxStateFromWeb)() = nullptr;


// PUBLIC
// Instance Methods

// Constructor
ControlerBox::ControlerBox()
{
  nodeId = 0;
  APIP = {0,0,0,0};
  stationIP = {0,0,0,0};
  ui16NodeName = 254;

  i16BoxActiveState = -1;
  ui32BoxActiveStateStartTime = 0;
  boxActiveStateHasBeenSignaled = true; // to the browser by the interface

  ui32lastRecPirHighTime = 0;

  isNewBoxHasBeenSignaled = true;
  boxDeletionHasBeenSignaled = true;

  // ui16MasterBoxName
  // setters:
  // - here; -> from the global.gui8DefaultMasterNodeName
  // - in updateMasterBoxName() below. updateMasterBoxName() is called from:
  //    - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface)
  //    - myMeshController constructor (on the interface, because on receiving an action "mc" (master changed confirmation) message)
  // - in mySavedPrefs::loadPreferences()
  // tested or used in:
  // - boxState class (on the laser boxes)
  // - here (printProperties)
  // - in myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) to send various messages
  // - in mySavedPrefs::savePreferences()
  ui16MasterBoxName = gui8DefaultMasterNodeName;

  /** bMasterBoxNameChangeHasBeenSignaled
   *  
   *  Setters:
   *  - here; 
   *  - in updateMasterBoxName() below (called from from step::applyStep(),
   *    from myMeshController::_updateMyMasterBoxName() and 
   *    myMeshController::_changedBoxConfirmation());
   *  - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) 
   *    (setting it to true, once a message has been sent to the browser).
   *  Testers:
   *  - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) 
   *    (tests whether a change has been made and whether it needs to inform 
   *    the browser). */
  bMasterBoxNameChangeHasBeenSignaled = true;
  /** bMasterBoxNameChangeHasBeenTakenIntoAccount
   * 
   *  Setters:
   *  - here; 
   *  - in updateMasterBoxName() below (called from from step::applyStep(),
   *    from myMeshController::_updateMyMasterBoxName() and 
   *    myMeshController::_changedBoxConfirmation());
   *  Testers:
   *  - boxState._setMasterBox() to determine whether it shall reset its masterBox pointer. */
  bMasterBoxNameChangeHasBeenTakenIntoAccount = true;

  sBoxDefaultState = gi16BoxDefaultState;
  sBoxDefaultStateChangeHasBeenSignaled = true;
}






void ControlerBox::updateThisBoxProperties() {
  nodeId = laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  APIP = laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  stationIP = laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  ui16NodeName = gui16NodeName;
  // For this box, i16BoxActiveState, boxActiveStateHasBeenSignaled and ui32BoxActiveStateStartTime are updated
  // by a call to setBoxActiveState from boxState
  if (MY_DEBUG == true) {
    Serial.println("ControlerBox::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    thisBox.printProperties(findIndexByNodeName(thisBox.ui16NodeName));
  };
}



void ControlerBox::printProperties(const uint16_t __ui16BoxIndex) {
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].nodeId: %u\n", __ui16BoxIndex, nodeId);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].APIP:", __ui16BoxIndex);Serial.println(APIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].stationIP:", __ui16BoxIndex);Serial.println(stationIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].ui16NodeName: %u\n", __ui16BoxIndex, ui16NodeName);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, i16BoxActiveState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].ui32BoxActiveStateStartTime: %u\n", __ui16BoxIndex, ui32BoxActiveStateStartTime);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, boxActiveStateHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].ui32lastRecPirHighTime: %u\n", __ui16BoxIndex, ui32lastRecPirHighTime);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].isNewBoxHasBeenSignaled: %i\n", __ui16BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxDeletionHasBeenSignaled: %i\n", __ui16BoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].ui16MasterBoxName: %u\n", __ui16BoxIndex, ui16MasterBoxName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxNameChangeHasBeenSignaled: %i\n", __ui16BoxIndex, bMasterBoxNameChangeHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxNameChangeHasBeenTakenIntoAccount: %i\n", __ui16BoxIndex, bMasterBoxNameChangeHasBeenTakenIntoAccount);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultState: %u\n", __ui16BoxIndex, sBoxDefaultState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultStateChangeHasBeenSignaled: %i\n", __ui16BoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}



void ControlerBox::updateMasterBoxName(const uint16_t _ui16MasterBoxName) {
  ui16MasterBoxName = _ui16MasterBoxName;  // see in constructor for information on where this variable is set and read
  bMasterBoxNameChangeHasBeenSignaled = false; // see in constructor for information on where this variable is set and read
  bMasterBoxNameChangeHasBeenTakenIntoAccount = false;
}



void ControlerBox::updateMasterBoxNameFromWeb(const uint16_t _ui16MasterBoxNameFromWeb) {
  updateMasterBoxName(_ui16MasterBoxNameFromWeb + gui16ControllerBoxPrefix);
}



uint16_t ControlerBox::getMasterBoxNameForWeb() {
  return (ui16MasterBoxName - gui16ControllerBoxPrefix);
}




// Setter for the activeState and associated variables
// Called only from this class (for the other boxes) and by
// boxState (when an effective update has been made).
void ControlerBox::setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
  // Serial.println("ControlerBox::setBoxActiveState(): Starting");

  if ( (i16BoxActiveState != _sBoxActiveState) || (ui32BoxActiveStateStartTime != _ui32BoxActiveStateStartTime) ) {
    i16BoxActiveState = _sBoxActiveState;
    // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].i16BoxActiveState);

    boxActiveStateHasBeenSignaled = false;
    // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenSignaled);
    // setters:
    // - by default to true upon init (controlerBox constructor);
    // - to false here (usefull for the IF, for the received states of other boxes);
    // - to true in myMeshViews (for this box only, upon sending a statusMsg);
    // - to true and false in myWebServerBase (by the IF, for the other boxes) --> tracing if it has sent an update to the browser
    // used by the interface mostly

    ui32BoxActiveStateStartTime = _ui32BoxActiveStateStartTime;
    // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].ui32BoxActiveStateStartTime: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].ui32BoxActiveStateStartTime);

    /** Set the Task that will check whether this change shall have an impact
     *  on thisBox boxState, add it to the Scheduler and restart it. */
    if ( (_tcbNsIsMeshHigh != nullptr) && ( std::addressof((ControlerBox&)(*this)) == std::addressof(thisBox) ) ) {
      tNsIsMeshHigh.setInterval(0);
      tNsIsMeshHigh.setIterations(1);
      tNsIsMeshHigh.setCallback([this](){
        _tcbNsIsMeshHigh(*this);
      });
      mns::myScheduler.addTask(tNsIsMeshHigh);
      tNsIsMeshHigh.restart();
    }
  }

  // Serial.println("ControlerBox::setBoxActiveState(): Ending");
}





// Setter for the defaultState and associated variables
// Called only from this class (for the other boxes).
void ControlerBox::setBoxDefaultState(const short _sBoxDefaultState) {
  // Serial.println("ControlerBox::setBoxDefaultState(): Starting");

  sBoxDefaultState = _sBoxDefaultState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].i16BoxActiveState);

  sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenSignaled);

  // Serial.println("ControlerBox::setBoxDefaultState(): Ending");
}




/** Setter for ui32lastRecPirHighTime
 * 
 *  Called from: 
 *  - this class, upon receiving an IR high message from the other boxes;
 *  - the pirController, upon IR high. */
void ControlerBox::setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime) {
  if (_ui32lastRecPirHighTime != ui32lastRecPirHighTime) {
    ui32lastRecPirHighTime = _ui32lastRecPirHighTime;
    /** Set the Task that will check whether this change shall have an impact
     *  on thisBox boxState, add it to the Scheduler and restart it. */
    if (_tcbNsIsIRHigh != nullptr) {
      tNsIsIRHigh.setInterval(0);
      tNsIsIRHigh.setIterations(1);
      tNsIsIRHigh.setCallback([this](){
        _tcbNsIsIRHigh(*this);
      });
      mns::myScheduler.addTask(tNsIsIRHigh);
      tNsIsIRHigh.restart();
    }
  }
}







// Static Methods


/** ControlerBox::updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj)
 * 
 *  Upon receiving a statusMsg from another laser box, this static method will 
 *  try to find the corresponding box in the ControlerBoxes array, using the 
 *  _ui32nodeId.
 * 
 *  If it finds it, it will overwrite the data corresponding to this box 
 *  in the relevant entry of the CB array.
 *  Else, it will save such data in the next empty slot. */
uint16_t ControlerBox::updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj) {
  Serial.printf("ControlerBox::updateOrCreate(): starting with _ui32nodeId = %u\n", _ui32nodeId);
  /** 1. look for the relevant box by nodeId.
   * 
   *  Why looking by NodeID rather than by NodeName?
   *  If I decide to change the nodeName at runtime, the nodeID will stay the same and I
   *  will find my box and would just need to update the data relating to it.
   *  If my nodeId has changed for the same nodeName, it means that I have unplugged an ESP
   *  and physically changed the device. In such a case, the nodeName will probably have been
   *  deleted anyway. */
  uint16_t __ui16BoxIndex = 254;

  for (uint16_t _i = 0; _i < gui16BoxesCount; _i++) {
    if ((ControlerBoxes[_i].nodeId == 0) && (__ui16BoxIndex == 254)) { 
      __ui16BoxIndex = _i;
      continue;
    }
    if (ControlerBoxes[_i].nodeId == _ui32nodeId) {
      printSearchResults(_i, _ui32nodeId, "_ui32nodeId");
      __ui16BoxIndex = _i;
      break;
    }
  }

  /** If we found an existing box or if we have a slot where to save the data from the JSON object 
   *  => save the data */
  if (__ui16BoxIndex != 254) {
    ControlerBoxes[__ui16BoxIndex].updateOtherBoxProperties(_ui32nodeId, _obj, __ui16BoxIndex);
  }

  /** In any case, return the index number to the caller. 
   *  Returning 254 means that: 
   *  (i) the box was not found; and 
   *  (ii) there was no empty slot left in the CB array to save the new data. */
  return __ui16BoxIndex;
}



uint16_t ControlerBox::findIndexByNodeId(uint32_t _ui32nodeId) {
  Serial.printf("ControlerBox::findIndexByNodeId(): looking for ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  uint16_t __ui16BoxIndex = 254;
  for (uint16_t _i = 0; _i < gui16BoxesCount; _i++) {
    if (ControlerBoxes[_i].nodeId == _ui32nodeId) {
      printSearchResults(_i, _ui32nodeId, "_ui32nodeId");
      __ui16BoxIndex = _i;
      break;
    }
  }
  Serial.printf("ControlerBox::findIndexByNodeId(): did not find ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  return __ui16BoxIndex;
}



void ControlerBox::printSearchResults(uint16_t _index, uint32_t _ui32saughtTerm, const char * _saughtIdentifier) {
  Serial.printf("ControlerBox::printSearchResults(): found ControlerBox with %s = %u\n", _saughtIdentifier, _ui32saughtTerm);
  Serial.printf("ControlerBox::printSearchResults(): ControlerBox with %s %u has index: %u\n", _saughtIdentifier, _ui32saughtTerm, _index);
}



uint16_t ControlerBox::findIndexByNodeName(uint16_t _ui16NodeName) {
  const char * _subName = "ControlerBox::findIndexByNodeName():";
  Serial.printf("%s looking for ControlerBox with uint16_t ui16NodeName = %u\n", _subName, _ui16NodeName);
  for (uint16_t _i = 0; _i < gui16BoxesCount; _i++) {
    if (ControlerBoxes[_i].ui16NodeName == _ui16NodeName) {
      printSearchResults(_i, (uint32_t)_ui16NodeName, "_ui16NodeName");
      return _i;
    }
  }
  Serial.printf("%s did not find ControlerBox with _ui16NodeName = %u\n", _subName, _ui16NodeName);
  return 254;
}




Task ControlerBox::tSetBoxStateFromWeb(0, 1, NULL, NULL, false, NULL, NULL);

/** Setter for i16boxStateRequestedFromWeb
 * 
 *  Called from: 
 *  - myMeshController, upon receiving a changeBox request from the web. */
void ControlerBox::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
  i16boxStateRequestedFromWeb = _i16boxStateRequestedFromWeb;
  /** Set the Task that will check whether this change shall have an impact
   *  on thisBox boxState, add it to the Scheduler and restart it. */
  if (_tcbSetBoxStateFromWeb != nullptr) {
    tSetBoxStateFromWeb.setInterval(0);
    tSetBoxStateFromWeb.setIterations(1);
    tSetBoxStateFromWeb.setCallback(_tcbSetBoxStateFromWeb);
    mns::myScheduler.addTask(tSetBoxStateFromWeb);
    tSetBoxStateFromWeb.restart();
  }
}






// updater of the properties of the other boxes in the mesh
// called from myMeshController
void ControlerBox::updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex) {
  const char * _subName = "ControlerBox::updateOtherBoxProperties():";
  Serial.printf("%s Starting\n", _subName);

  // set the nodeId
  if (nodeId == 0) {
    updateConnectedBoxCount(connectedBoxesCount + 1);
    isNewBoxHasBeenSignaled = false;
    Serial.printf("%s ControlerBoxes[%u].isNewBoxHasBeenSignaled = %i\n", _subName, __ui16BoxIndex, isNewBoxHasBeenSignaled);
  }
  nodeId = _ui32SenderNodeId;
  // Serial.printf("%s ControlerBoxes[%u].nodeId = %u\n", __ui16BoxIndex, _ui32SenderNodeId);

  // set the IPs
  if ( _obj.containsKey("APIP")){ 
    APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);
    // Serial.printf("%s ControlerBoxes[%u].APIP = ", __ui16BoxIndex);Serial.println(ControlerBoxes[__ui16BoxIndex].APIP);
  }
  if ( _obj.containsKey("StIP")) {
    stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);
    // Serial.print("%s ControlerBoxes[%u].stationIP = ", __ui16BoxIndex);Serial.println(ControlerBoxes[__ui16BoxIndex].stationIP);
  }

  // set the ui16NodeName
  ui16NodeName = _obj["NNa"];
  // Serial.printf("%s ControlerBoxes[%u].ui16NodeName = %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].ui16NodeName);

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  if (_obj.containsKey("boxDefstate")) {
    const short int __senderBoxActiveState = _obj["actSt"];
    const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
    setBoxActiveState(__senderBoxActiveState, __uiSenderBoxActiveStateStartTime);
  }
  
  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  if (_obj.containsKey("boxDefstate")) {
    const short int __senderBoxDefaultState = _obj["boxDefstate"];
    setBoxDefaultState(__senderBoxDefaultState);
  }

  if (_obj["action"] == "usi" && _obj["key"] == "IR") {
    thisBox.setBoxIRTimes(_obj["time"].as<uint32_t>());
  }

  // Print out the updated properties
  if (MY_DEBUG == true) {
    Serial.printf("%s Updated box index %u. Calling printProperties().\n", _subName, __ui16BoxIndex);
    printProperties(__ui16BoxIndex);
  }
  Serial.println("%s Ending");
}






void ControlerBox::updateConnectedBoxCount(short int newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  connectedBoxesCount = newConnectedBoxesCount;
}





void ControlerBox::deleteBox() {
  const char * _subName = "ControlerBox::deleteBox():";
  Serial.printf("%s Starting\n", _subName);
  // Serial.printf("%s Received _ui16BoxIndex %u\n", );
  nodeId = 0;
  Serial.printf("%s NodeId reset to 0\n", _subName);
  APIP = {0,0,0,0};
  Serial.printf("%s APIP reset to 0.0.0.0\n", _subName);
  stationIP = {0,0,0,0};
  Serial.printf("%s stationIP reset to 0.0.0.0\n", _subName);
  ui16NodeName = 254;
  Serial.printf("%s ui16NodeName reset to 0\n", _subName);

  i16BoxActiveState = -1;
  Serial.printf("%s i16BoxActiveState reset to -1\n", _subName);
  ui32BoxActiveStateStartTime = 0;
  Serial.printf("%s ui32BoxActiveStateStartTime reset to 0\n", _subName);
  boxActiveStateHasBeenSignaled = true;
  Serial.printf("%s ui32BoxActiveStateStartTime reset to true\n", _subName);

  Serial.printf("%s ui32lastRecPirHighTime reset to 0\n", _subName);
  ui32lastRecPirHighTime = 0;

  isNewBoxHasBeenSignaled = true;
  Serial.printf("%s isNewBoxHasBeenSignaled reset to true\n", _subName);
  boxDeletionHasBeenSignaled = false;
  Serial.printf("%s boxDeletionHasBeenSignaled set to FALSE\n", _subName);

  ui16MasterBoxName = gui8DefaultMasterNodeName;
  Serial.printf("%s ui16MasterBoxName set to %u\n", _subName, ui16MasterBoxName);
  bMasterBoxNameChangeHasBeenSignaled = true;
  Serial.printf("%s bMasterBoxNameChangeHasBeenSignaled set to true\n", _subName);
  bMasterBoxNameChangeHasBeenTakenIntoAccount = true;
  Serial.printf("%s bMasterBoxNameChangeHasBeenTakenIntoAccount set to true\n", _subName);

  sBoxDefaultState = gi16BoxDefaultState;
  Serial.printf("%s sBoxDefaultState set to %i\n", _subName, sBoxDefaultState);
  sBoxDefaultStateChangeHasBeenSignaled = true;
  Serial.printf("%s sBoxDefaultStateChangeHasBeenSignaled set to true\n", _subName);

  updateConnectedBoxCount(connectedBoxesCount - 1);
  Serial.printf("%s updated ConnectedBoxCount\n", _subName);
  Serial.printf("%s Ending\n", _subName);
}






void ControlerBox::deleteBoxByNodeId(uint32_t _ui32nodeId) {
  Serial.println("ControlerBox::deleteBoxByNodeId(): Starting");
  for (uint16_t __it = 0; __it < gui16BoxesCount; __it++) {
    if (ControlerBoxes[__it].nodeId == _ui32nodeId) {
      ControlerBoxes[__it].deleteBox();
      break;
    }
  }
  Serial.println("ControlerBox::deleteBoxByNodeId(): Ending");
}




Task ControlerBox::tReboot(3000, TASK_ONCE, NULL, NULL/*&mns::myScheduler*/, false, NULL, &_reboot);

void ControlerBox::_reboot() {
  if (MY_DG_WS) { Serial.printf("ControlerBox::tReboot(): About to reboot.\n"); }
  ESP.restart();
}





////////////////////////////////////////////////////////////////////////////////
// PRIVATE
