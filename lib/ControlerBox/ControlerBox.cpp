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
bool ControlerBox::valFromPir = LOW;
uint32_t ControlerBox::uiSettingTimeOfValFromPir = 0;
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
  uiBoxActiveStateStartTime = 0;
  boxActiveStateHasBeenSignaled = true; // to the browser by the interface
  boxActiveStateHasBeenTakenIntoAccount = false; // by the boxState class

  isNewBoxHasBeenSignaled = true;
  boxDeletionHasBeenSignaled = true;

  // this->bMasterBoxName
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
  this->bMasterBoxName = UI8_DEFAULT_MASTER_NODE_NAME;

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
  // For this box, i16BoxActiveState, boxActiveStateHasBeenSignaled and uiBoxActiveStateStartTime are updated
  // by a call to setBoxActiveState from boxState
  if (MY_DEBUG == true) {
    Serial.println("ControlerBox::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    ControlerBoxes[gui16MyIndexInCBArray].printProperties(gui16MyIndexInCBArray);
  };
}



void ControlerBox::printProperties(const uint8_t __ui16BoxIndex) {
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].nodeId: %u\n", __ui16BoxIndex, nodeId);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].APIP:", __ui16BoxIndex);Serial.println(APIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].stationIP:", __ui16BoxIndex);Serial.println(stationIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].ui16NodeName: %u\n", __ui16BoxIndex, ui16NodeName);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, i16BoxActiveState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].uiBoxActiveStateStartTime: %u\n", __ui16BoxIndex, uiBoxActiveStateStartTime);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, boxActiveStateHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveStateHasBeenTakenIntoAccount: %i\n", __ui16BoxIndex, boxActiveStateHasBeenTakenIntoAccount);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].isNewBoxHasBeenSignaled: %i\n", __ui16BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxDeletionHasBeenSignaled: %i\n", __ui16BoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxName: %u\n", __ui16BoxIndex, bMasterBoxName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxNameChangeHasBeenSignaled: %i\n", __ui16BoxIndex, bMasterBoxNameChangeHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultState: %u\n", __ui16BoxIndex, sBoxDefaultState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultStateChangeHasBeenSignaled: %i\n", __ui16BoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}

void ControlerBox::updateMasterBoxName(const byte _bMasterBoxName) {
  bMasterBoxName = _bMasterBoxName;  // see in constructor for information on where this variable is set and read
  bMasterBoxNameChangeHasBeenSignaled = false; // see in constructor for information on where this variable is set and read
}



// Static Methods
uint16_t ControlerBox::findByNodeId(uint32_t _ui32nodeId) {
  Serial.printf("ControlerBox::findByNodeId(): looking for ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  for (uint16_t _i = 0; _i < gui16BoxesCount; _i++) {
    if (ControlerBoxes[_i].nodeId == _ui32nodeId) {
      Serial.printf("ControlerBox::findByNodeId(): found ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
      Serial.printf("ControlerBox::findByNodeId(): ControlerBox with _ui32nodeId %u has index: %u\n", _ui32nodeId, _i);
      return _i;
    }
  }
  Serial.printf("ControlerBox::findByNodeId(): did not find ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  return 254;
}


// updater of the properties of the other boxes in the mesh
// called from myMeshController
void ControlerBox::updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  Serial.println("ControlerBox::updateOtherBoxProperties(): Starting");

  // Setting nodeName, nodeId and IP properties
  // extract the index of the relevant box from its senderNodeName in the JSON
  uint16_t __ui16NodeName = _obj["NNa"]; // ex. 201
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __ui16NodeName = %u\n", __ui16NodeName);
  uint16_t __ui16BoxIndex = __ui16NodeName - gui16ControllerBoxPrefix; // 201 - 200 = 1

  // set the nodeId
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __ui16BoxIndex = %u\n", __ui16BoxIndex);
  if (ControlerBoxes[__ui16BoxIndex].nodeId == 0) {
    updateConnectedBoxCount(connectedBoxesCount + 1);
    ControlerBoxes[__ui16BoxIndex].isNewBoxHasBeenSignaled = false;
    Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__ui16BoxIndex].isNewBoxHasBeenSignaled = %i\n", ControlerBoxes[__ui16BoxIndex].isNewBoxHasBeenSignaled);
  }
  ControlerBoxes[__ui16BoxIndex].nodeId = _ui32SenderNodeId;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__ui16BoxIndex].nodeId = %u\n", ControlerBoxes[__ui16BoxIndex]._ui32SenderNodeId);

  // set the IPs
  ControlerBoxes[__ui16BoxIndex].APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);; // _parseIpStringToIPAddress(_obj, "APIP");
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].APIP = ", __ui16BoxIndex);Serial.println(ControlerBoxes[__ui16BoxIndex].APIP);
  ControlerBoxes[__ui16BoxIndex].stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);; // _parseIpStringToIPAddress(_obj, "APIP");
  // Serial.print("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].stationIP = ", __ui16BoxIndex);Serial.println(ControlerBoxes[__ui16BoxIndex].stationIP);

  // set the ui16NodeName
  ControlerBoxes[__ui16BoxIndex].ui16NodeName = __ui16NodeName;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].ui16NodeName = %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].ui16NodeName);

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxActiveState = _obj["actSt"];
  const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
  setBoxActiveState(__ui16BoxIndex, __senderBoxActiveState, __uiSenderBoxActiveStateStartTime);

  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxDefaultState = _obj["boxDefstate"];
  setBoxDefaultState(__ui16BoxIndex, __senderBoxDefaultState);

  // Print out the updated properties
  if (MY_DEBUG == true) {
    Serial.printf("ControlerBox::updateOtherBoxProperties(): Updated box index %u. Calling printProperties().\n", __ui16BoxIndex);
    ControlerBoxes[__ui16BoxIndex].printProperties(__ui16BoxIndex);
  }
  Serial.println("ControlerBox::updateOtherBoxProperties(): Ending");
}




// Setter for the activeState and associated variables
// Called only from this class (for the other boxes) and by
// boxState (when an effective update has been made).
void ControlerBox::setBoxActiveState(const uint8_t __ui16BoxIndex, const short _sBoxActiveState, const uint32_t _uiBoxActiveStateStartTime) {
  // Serial.println("ControlerBox::setBoxActiveState(): Starting");

  ControlerBoxes[__ui16BoxIndex].i16BoxActiveState = _sBoxActiveState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].i16BoxActiveState);

  ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenSignaled);
  // setters:
  // - by default to true upon init (controlerBox constructor);
  // - to false here (usefull for the IF, for the received states of other boxes);
  // - to true in myMeshViews (for this box only, upon sending a statusMsg);
  // - to true and false in myWebServerBase (by the IF, for the other boxes) --> tracing if it has sent an update to the browser
  // used by the interface mostly

  ControlerBoxes[__ui16BoxIndex].uiBoxActiveStateStartTime = _uiBoxActiveStateStartTime;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].uiBoxActiveStateStartTime: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].uiBoxActiveStateStartTime);

  ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenTakenIntoAccount = false;
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
void ControlerBox::setBoxDefaultState(const uint8_t __ui16BoxIndex, const short _sBoxDefaultState) {
  // Serial.println("ControlerBox::setBoxDefaultState(): Starting");

  ControlerBoxes[__ui16BoxIndex].sBoxDefaultState = _sBoxDefaultState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].i16BoxActiveState);

  ControlerBoxes[__ui16BoxIndex].sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenSignaled);

  // Serial.println("ControlerBox::setBoxDefaultState(): Ending");
}





void ControlerBox::updateConnectedBoxCount(short int newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  connectedBoxesCount = newConnectedBoxesCount;
}





void ControlerBox::deleteBox(uint16_t _ui16BoxIndex) {
  Serial.println("ControlerBox::deleteBox(): Starting");
  Serial.printf("ControlerBox::deleteBox(): Received _ui16BoxIndex %u\n", _ui16BoxIndex);
  ControlerBoxes[_ui16BoxIndex].nodeId = 0;
  Serial.println("ControlerBox::deleteBox(): NodeId reset to 0");
  ControlerBoxes[_ui16BoxIndex].APIP = {0,0,0,0};
  Serial.println("ControlerBox::deleteBox(): APIP reset to 0.0.0.0");
  ControlerBoxes[_ui16BoxIndex].stationIP = {0,0,0,0};
  Serial.println("ControlerBox::deleteBox(): stationIP reset to 0.0.0.0");
  ControlerBoxes[_ui16BoxIndex].ui16NodeName = 1;
  Serial.println("ControlerBox::deleteBox(): ui16NodeName reset to 1");
  ControlerBoxes[_ui16BoxIndex].ui16NodeName = 0;
  Serial.println("ControlerBox::deleteBox(): ui16NodeName reset to 0");

  ControlerBoxes[_ui16BoxIndex].i16BoxActiveState = -1;
  Serial.println("ControlerBox::deleteBox(): i16BoxActiveState reset to -1");
  ControlerBoxes[_ui16BoxIndex].uiBoxActiveStateStartTime = 0;
  Serial.println("ControlerBox::deleteBox(): uiBoxActiveStateStartTime reset to 0");
  ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenSignaled = true;
  Serial.println("ControlerBox::deleteBox(): uiBoxActiveStateStartTime reset to true");
  ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenTakenIntoAccount = true;
  Serial.println("ControlerBox::deleteBox(): boxActiveStateHasBeenTakenIntoAccount reset to true");

  ControlerBoxes[_ui16BoxIndex].isNewBoxHasBeenSignaled = true;
  Serial.println("ControlerBox::deleteBox(): isNewBoxHasBeenSignaled reset to true");
  ControlerBoxes[_ui16BoxIndex].boxDeletionHasBeenSignaled = false;
  Serial.println("ControlerBox::deleteBox(): boxDeletionHasBeenSignaled set to FALSE");

  ControlerBoxes[_ui16BoxIndex].bMasterBoxName = UI8_DEFAULT_MASTER_NODE_NAME;
  Serial.printf("ControlerBox::deleteBox(): bMasterBoxName set to %u\n", UI8_DEFAULT_MASTER_NODE_NAME);
  ControlerBoxes[_ui16BoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
  Serial.printf("ControlerBox::deleteBox(): bMasterBoxNameChangeHasBeenSignaled set to true\n");
  ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenTakenIntoAccount = true;
  Serial.printf("ControlerBox::deleteBox(): boxActiveStateHasBeenTakenIntoAccount set to true\n");

  updateConnectedBoxCount(connectedBoxesCount - 1);
  Serial.printf("ControlerBox::deleteBox(): updated ConnectedBoxCount\n");
  Serial.println("ControlerBox::deleteBox(): Ending");
}






void ControlerBox::deleteBoxByNodeId(uint32_t _ui32nodeId) {
  Serial.println("ControlerBox::deleteBoxByNodeId(): Starting");
  for (uint16_t __it = 0; __it < gui16BoxesCount; __it++) {
    if (ControlerBoxes[__it].nodeId == _ui32nodeId) {
      deleteBox(__it);
      break;
    }
  }
  Serial.println("ControlerBox::deleteBoxByNodeId(): Ending");
}




Task ControlerBox::tReboot(3000, TASK_ONCE, NULL, &myTaskScheduler, false, NULL, &_reboot);

void ControlerBox::_reboot() {
  if (MY_DG_WS) { Serial.printf("ControlerBox::tReboot(): About to reboot.\n"); }
  ESP.restart();
}





////////////////////////////////////////////////////////////////////////////////
// PRIVATE
IPAddress ControlerBox::_parseIpStringToIPAddress(JsonDocument& root, const char* rootKey/*String& rootKey*/) {
  Serial.println("ControlerBox::_parseIpStringToIPAddress: Starting");
  // Serial.print("ControlerBox::_parseIpStringToIPAddress: rootKey = ");Serial.println(rootKey);
  const char* ipStr = root[rootKey];
  // Serial.print("ControlerBox::_parseIpStringToIPAddress: ipStr = ");Serial.println(ipStr);
  byte ip[4];
  _parseCharArrayToBytes(ipStr, '.', ip, 4, 10);
  Serial.println("ControlerBox::_parseIpStringToIPAddress: Ending");
  return ip;
}

void ControlerBox::_parseCharArrayToBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  // see https://stackoverflow.com/questions/35227449/convert-ip-or-mac-address-from-string-to-byte-array-arduino-or-c
  /*  Call it this way:
        const char* ipStr = "50.100.150.200";
        byte ip[4];
        _parseBytesToCharArray(ipStr, '.', ip, 4, 10);
   */
  Serial.println("ControlerBox::_parseCharArrayToBytes: Starting");
  // Serial.print("ControlerBox::_parseCharArrayToBytes: str = ");Serial.println(str);
  for (int i = 0; i < maxBytes; i++) {
    // Serial.println("ControlerBox::_parseCharArrayToBytes: In for loop");
    // Serial.print("ControlerBox::_parseCharArrayToBytes: i = ");Serial.println(i);
    bytes[i] = strtoul(str, NULL, base);  // Convert byte
    // Serial.print("ControlerBox::_parseCharArrayToBytes: bytes[i] = ");Serial.println(bytes[i]);
    str = strchr(str, sep);               // Find next separator
    // Serial.print("ControlerBox::_parseCharArrayToBytes: str = ");Serial.println(str);
    if (str == NULL || *str == '\0') {
        // Serial.println("ControlerBox::_parseCharArrayToBytes: in if (str == NULL || *str == '\0')");
        break;                            // No more separators, exit
    }
    str++;                                // Point to next character after separator
  }
  Serial.println("ControlerBox::_parseCharArrayToBytes: Ending");
}


IPAddress _IpStringToIPAddress(const char* _cIpCharArray) {
  // see https://stackoverflow.com/questions/35227449/convert-ip-or-mac-address-from-string-to-byte-array-arduino-or-c
  /*  Call it this way:
    _IpStringToIPAddress("192.168.25.34");
   */
  byte _ip[4];
  for (int i = 0; i < 4; i++) {
    _ip[i] = strtoul(_cIpCharArray, NULL, 10);  // Convert to byte
    _cIpCharArray = strchr(_cIpCharArray, '.');             // Find next separator
    if (_cIpCharArray == NULL || *_cIpCharArray == '\0') {
        break;                            // No more separators, exit
    }
    _cIpCharArray++;                                // Point to next character after separator
  }
  return _ip;
}
