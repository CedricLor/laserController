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
bool ControlerBox::bValFromPir = LOW;
uint32_t ControlerBox::ui32SettingTimeOfValFromPir = 0;
short int ControlerBox::valFromWeb = -1;
short int ControlerBox::connectedBoxesCount = 1;
short int ControlerBox::previousConnectedBoxesCount = 1;


// PUBLIC
// Instance Methods

// Constructor
ControlerBox::ControlerBox()
{
  nodeId = 0;
  APIP = {0,0,0,0};
  stationIP = {0,0,0,0};
  ui16NodeName = 0;

  i16BoxActiveState = -1;
  ui32BoxActiveStateStartTime = 0;
  boxActiveStateHasBeenSignaled = true; // to the browser by the interface
  boxActiveStateHasBeenTakenIntoAccount = false; // by the boxState class

  isNewBoxHasBeenSignaled = true;
  boxDeletionHasBeenSignaled = true;

  // this->ui16MasterBoxName
  // setters:
  // - here; -> from the global.UI8_DEFAULT_MASTER_NODE_NAME
  // - in updateMasterBoxName() below. updateMasterBoxName() is called from:
  //    - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface)
  //    - myMeshController constructor (on the interface, because on receiving an action "mc" (master changed confirmation) message)
  // - in mySavedPrefs::loadPreferences()
  // tested or used in:
  // - boxState class (on the laser boxes)
  // - here (printProperties)
  // - in myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) to send various messages
  // - in mySavedPrefs::savePreferences()
  this->ui16MasterBoxName = UI8_DEFAULT_MASTER_NODE_NAME;

  // bMasterBoxNameChangeHasBeenSignaled
  // setters:
  // - here
  // - in updateMasterBoxName() below. updateMasterBoxName() is called from:
  //    - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface)
  //    - myMeshController constructor (on the interface, because on receiving an action "mc" (master changed confirmation) message)
  // - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface); setting it to true
  // - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) (setting it to true, once a message has been sent to the browser)
  // testers:
  // - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) (tests whether a change has been made and whether it needs to inform the browser)
  bMasterBoxNameChangeHasBeenSignaled = true;

  sBoxDefaultState = S_BOX_DEFAULT_STATE;
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
    ControlerBoxes[gui16MyIndexInCBArray].printProperties(gui16MyIndexInCBArray);
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
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveStateHasBeenTakenIntoAccount: %i\n", __ui16BoxIndex, boxActiveStateHasBeenTakenIntoAccount);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].isNewBoxHasBeenSignaled: %i\n", __ui16BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxDeletionHasBeenSignaled: %i\n", __ui16BoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].ui16MasterBoxName: %u\n", __ui16BoxIndex, ui16MasterBoxName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxNameChangeHasBeenSignaled: %i\n", __ui16BoxIndex, bMasterBoxNameChangeHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultState: %u\n", __ui16BoxIndex, sBoxDefaultState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultStateChangeHasBeenSignaled: %i\n", __ui16BoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}

void ControlerBox::updateMasterBoxName(const uint16_t _ui16MasterBoxName) {
  ui16MasterBoxName = _ui16MasterBoxName;  // see in constructor for information on where this variable is set and read
  bMasterBoxNameChangeHasBeenSignaled = false; // see in constructor for information on where this variable is set and read
}



// Static Methods
uint16_t ControlerBox::findIndexByNodeId(uint32_t _ui32nodeId) {
  Serial.printf("ControlerBox::findIndexByNodeId(): looking for ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  for (uint16_t _i = 0; _i < gui16BoxesCount; _i++) {
    if (ControlerBoxes[_i].nodeId == _ui32nodeId) {
      Serial.printf("ControlerBox::findIndexByNodeId(): found ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
      Serial.printf("ControlerBox::findIndexByNodeId(): ControlerBox with _ui32nodeId %u has index: %u\n", _ui32nodeId, _i);
      return _i;
    }
  }
  Serial.printf("ControlerBox::findIndexByNodeId(): did not find ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  return 254;
}

uint16_t ControlerBox::findIndexByNodeName(uint16_t _ui16NodeName) {
  Serial.printf("ControlerBox::findIndexByNodeName(): looking for ControlerBox with uint16_t ui16NodeName = %u\n", _ui16NodeName);
  for (uint16_t _i = 0; _i < gui16BoxesCount; _i++) {
    if (ControlerBoxes[_i].nodeId == _ui16NodeName) {
      Serial.printf("ControlerBox::findIndexByNodeName(): found ControlerBox with _ui16NodeName = %u\n", _ui16NodeName);
      Serial.printf("ControlerBox::findIndexByNodeName(): ControlerBox with _ui16NodeName %u has index: %u\n", _ui16NodeName, _i);
      return _i;
    }
  }
  Serial.printf("ControlerBox::findIndexByNodeName(): did not find ControlerBox with _ui16NodeName = %u\n", _ui16NodeName);
  return 254;
}

// updater of the properties of the other boxes in the mesh
// called from myMeshController
void ControlerBox::updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex) {
  Serial.println("ControlerBox::updateOtherBoxProperties(): Starting");

  // set the nodeId
  if (nodeId == 0) {
    updateConnectedBoxCount(connectedBoxesCount + 1);
    isNewBoxHasBeenSignaled = false;
    Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].isNewBoxHasBeenSignaled = %i\n", __ui16BoxIndex, isNewBoxHasBeenSignaled);
  }
  nodeId = _ui32SenderNodeId;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].nodeId = %u\n", __ui16BoxIndex, _ui32SenderNodeId);

  // set the IPs
  APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].APIP = ", __ui16BoxIndex);Serial.println(ControlerBoxes[__ui16BoxIndex].APIP);
  stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);
  // Serial.print("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].stationIP = ", __ui16BoxIndex);Serial.println(ControlerBoxes[__ui16BoxIndex].stationIP);

  // set the ui16NodeName
  ui16NodeName = _obj["NNa"];
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].ui16NodeName = %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].ui16NodeName);

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxActiveState = _obj["actSt"];
  const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
  setBoxActiveState(__senderBoxActiveState, __uiSenderBoxActiveStateStartTime);

  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxDefaultState = _obj["boxDefstate"];
  setBoxDefaultState(__senderBoxDefaultState);

  // Print out the updated properties
  if (MY_DEBUG == true) {
    Serial.printf("ControlerBox::updateOtherBoxProperties(): Updated box index %u. Calling printProperties().\n", __ui16BoxIndex);
    printProperties(__ui16BoxIndex);
  }
  Serial.println("ControlerBox::updateOtherBoxProperties(): Ending");
}




// Setter for the activeState and associated variables
// Called only from this class (for the other boxes) and by
// boxState (when an effective update has been made).
void ControlerBox::setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
  // Serial.println("ControlerBox::setBoxActiveState(): Starting");

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

  boxActiveStateHasBeenTakenIntoAccount = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenTakenIntoAccount: %i\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenTakenIntoAccount);
  // setters:
  // - by default at true upon init (controlerBox constructor);
  // - to false here (useful so that the boxState can check if a boxState change request has come);
  // - to true (for this box only) by boxState.
  // This variable has effect only in the laser box / boxState stack (i.e. not in the interface).
  // It is used when the laserBox receives an order to change active state from the interface.

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





void ControlerBox::updateConnectedBoxCount(short int newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  connectedBoxesCount = newConnectedBoxesCount;
}





void ControlerBox::deleteBox() {
  Serial.println("ControlerBox::deleteBox(): Starting");
  // Serial.printf("ControlerBox::deleteBox(): Received _ui16BoxIndex %u\n", );
  nodeId = 0;
  Serial.println("ControlerBox::deleteBox(): NodeId reset to 0");
  APIP = {0,0,0,0};
  Serial.println("ControlerBox::deleteBox(): APIP reset to 0.0.0.0");
  stationIP = {0,0,0,0};
  Serial.println("ControlerBox::deleteBox(): stationIP reset to 0.0.0.0");
  ui16NodeName = 1;
  Serial.println("ControlerBox::deleteBox(): ui16NodeName reset to 1");
  ui16NodeName = 0;
  Serial.println("ControlerBox::deleteBox(): ui16NodeName reset to 0");

  i16BoxActiveState = -1;
  Serial.println("ControlerBox::deleteBox(): i16BoxActiveState reset to -1");
  ui32BoxActiveStateStartTime = 0;
  Serial.println("ControlerBox::deleteBox(): ui32BoxActiveStateStartTime reset to 0");
  boxActiveStateHasBeenSignaled = true;
  Serial.println("ControlerBox::deleteBox(): ui32BoxActiveStateStartTime reset to true");
  boxActiveStateHasBeenTakenIntoAccount = true;
  Serial.println("ControlerBox::deleteBox(): boxActiveStateHasBeenTakenIntoAccount reset to true");

  isNewBoxHasBeenSignaled = true;
  Serial.println("ControlerBox::deleteBox(): isNewBoxHasBeenSignaled reset to true");
  boxDeletionHasBeenSignaled = false;
  Serial.println("ControlerBox::deleteBox(): boxDeletionHasBeenSignaled set to FALSE");

  ui16MasterBoxName = UI8_DEFAULT_MASTER_NODE_NAME;
  Serial.printf("ControlerBox::deleteBox(): ui16MasterBoxName set to %u\n", UI8_DEFAULT_MASTER_NODE_NAME);
  bMasterBoxNameChangeHasBeenSignaled = true;
  Serial.printf("ControlerBox::deleteBox(): bMasterBoxNameChangeHasBeenSignaled set to true\n");

  updateConnectedBoxCount(connectedBoxesCount - 1);
  Serial.printf("ControlerBox::deleteBox(): updated ConnectedBoxCount\n");
  Serial.println("ControlerBox::deleteBox(): Ending");
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




Task ControlerBox::tReboot(3000, TASK_ONCE, NULL, &userScheduler, false, NULL, &_reboot);

void ControlerBox::_reboot() {
  if (MY_DG_WS) { Serial.printf("ControlerBox::tReboot(): About to reboot.\n"); }
  ESP.restart();
}





////////////////////////////////////////////////////////////////////////////////
// PRIVATE
