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
  this->bNodeName = 0;

  boxActiveState = -1;
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
  bNodeName = gui8NodeName;
  // For this box, boxActiveState, boxActiveStateHasBeenSignaled and uiBoxActiveStateStartTime are updated
  // by a call to setBoxActiveState from boxState
  if (MY_DEBUG == true) {
    Serial.println("ControlerBox::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    ControlerBoxes[gui8MyIndexInCBArray].printProperties(gui8MyIndexInCBArray);
  };
}



void ControlerBox::printProperties(const uint8_t _ui8BoxIndex) {
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].nodeId: %u\n", _ui8BoxIndex, nodeId);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].APIP:", _ui8BoxIndex);Serial.println(APIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].stationIP:", _ui8BoxIndex);Serial.println(stationIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bNodeName: %u\n", _ui8BoxIndex, this->bNodeName);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveState: %u\n", _ui8BoxIndex, boxActiveState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].uiBoxActiveStateStartTime: %u\n", _ui8BoxIndex, uiBoxActiveStateStartTime);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", _ui8BoxIndex, boxActiveStateHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxActiveStateHasBeenTakenIntoAccount: %i\n", _ui8BoxIndex, boxActiveStateHasBeenTakenIntoAccount);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].isNewBoxHasBeenSignaled: %i\n", _ui8BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].boxDeletionHasBeenSignaled: %i\n", _ui8BoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxName: %u\n", _ui8BoxIndex, bMasterBoxName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].bMasterBoxNameChangeHasBeenSignaled: %i\n", _ui8BoxIndex, bMasterBoxNameChangeHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultState: %u\n", _ui8BoxIndex, sBoxDefaultState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%u].sBoxDefaultStateChangeHasBeenSignaled: %i\n", _ui8BoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}

void ControlerBox::updateMasterBoxName(const byte _bMasterBoxName) {
  bMasterBoxName = _bMasterBoxName;  // see in constructor for information on where this variable is set and read
  bMasterBoxNameChangeHasBeenSignaled = false; // see in constructor for information on where this variable is set and read
}



// Static Methods
int8_t ControlerBox::findByNodeId(uint32_t _ui32nodeId) {
  for (uint8_t _i = 0; _i < gui8BoxesCount; _i++) {
    if (ControlerBoxes[_i].nodeId == _ui32nodeId) {
      return _i;
    }
  }
  return 254;
}


// updater of the properties of the other boxes in the mesh
// called from myMeshController
void ControlerBox::updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  Serial.println("ControlerBox::updateOtherBoxProperties(): Starting");

  // Setting nodeName, nodeId and IP properties
  // extract the index of the relevant box from its senderNodeName in the JSON
  uint8_t __ui8NodeName = _obj["NNa"]; // ex. 201
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __ui8NodeName = %u\n", __ui8NodeName);
  uint8_t __ui8BoxIndex = __ui8NodeName - gui8ControllerBoxPrefix; // 201 - 200 = 1

  // set the nodeId
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __ui8BoxIndex = %u\n", __ui8BoxIndex);
  if (ControlerBoxes[__ui8BoxIndex].nodeId == 0) {
    updateConnectedBoxCount(connectedBoxesCount + 1);
    ControlerBoxes[__ui8BoxIndex].isNewBoxHasBeenSignaled = false;
    Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__ui8BoxIndex].isNewBoxHasBeenSignaled = %i\n", ControlerBoxes[__ui8BoxIndex].isNewBoxHasBeenSignaled);
  }
  ControlerBoxes[__ui8BoxIndex].nodeId = _ui32SenderNodeId;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__ui8BoxIndex].nodeId = %u\n", ControlerBoxes[__ui8BoxIndex]._ui32SenderNodeId);

  // set the IPs
  ControlerBoxes[__ui8BoxIndex].APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);; // _parseIpStringToIPAddress(_obj, "APIP");
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].APIP = ", __ui8BoxIndex);Serial.println(ControlerBoxes[__ui8BoxIndex].APIP);
  ControlerBoxes[__ui8BoxIndex].stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);; // _parseIpStringToIPAddress(_obj, "APIP");
  // Serial.print("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].stationIP = ", __ui8BoxIndex);Serial.println(ControlerBoxes[__ui8BoxIndex].stationIP);

  // set the bNodeName
  ControlerBoxes[__ui8BoxIndex].bNodeName = __ui8NodeName;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].bNodeName = %u\n", __ui8BoxIndex, ControlerBoxes[__ui8BoxIndex].bNodeName);

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxActiveState = _obj["actSt"];
  const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
  setBoxActiveState(__ui8BoxIndex, __senderBoxActiveState, __uiSenderBoxActiveStateStartTime);

  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxDefaultState = _obj["boxDefstate"];
  setBoxDefaultState(__ui8BoxIndex, __senderBoxDefaultState);

  // Print out the updated properties
  if (MY_DEBUG == true) {
    Serial.printf("ControlerBox::updateOtherBoxProperties(): Updated box index %u. Calling printProperties().\n", __ui8BoxIndex);
    ControlerBoxes[__ui8BoxIndex].printProperties(__ui8BoxIndex);
  }
  Serial.println("ControlerBox::updateOtherBoxProperties(): Ending");
}




// Setter for the activeState and associated variables
// Called only from this class (for the other boxes) and by
// boxState (when an effective update has been made).
void ControlerBox::setBoxActiveState(const uint8_t _ui8BoxIndex, const short _sBoxActiveState, const uint32_t _uiBoxActiveStateStartTime) {
  Serial.println("ControlerBox::setBoxActiveState(): Starting");

  ControlerBoxes[_ui8BoxIndex].boxActiveState = _sBoxActiveState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveState: %u\n", _ui8BoxIndex, ControlerBoxes[_ui8BoxIndex].boxActiveState);

  ControlerBoxes[_ui8BoxIndex].boxActiveStateHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", _ui8BoxIndex, ControlerBoxes[_ui8BoxIndex].boxActiveStateHasBeenSignaled);
  // setters:
  // - by default to true upon init (controlerBox constructor);
  // - to false here (usefull for the IF, for the received states of other boxes);
  // - to true in myMeshViews (for this box only, upon sending a statusMsg);
  // - to true and false in myWebServerBase (by the IF, for the other boxes) --> tracing if it has sent an update to the browser
  // used by the interface mostly

  ControlerBoxes[_ui8BoxIndex].uiBoxActiveStateStartTime = _uiBoxActiveStateStartTime;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].uiBoxActiveStateStartTime: %u\n", __ui8BoxIndex, ControlerBoxes[__ui8BoxIndex].uiBoxActiveStateStartTime);

  ControlerBoxes[_ui8BoxIndex].boxActiveStateHasBeenTakenIntoAccount = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenTakenIntoAccount: %i\n", __ui8BoxIndex, ControlerBoxes[_ui8BoxIndex].boxActiveStateHasBeenTakenIntoAccount);
  // setters:
  // - by default at true upon init (controlerBox constructor);
  // - to false here (useful so that the boxState can check if a boxState change request has come);
  // - to true (for this box only) by boxState.
  // This variable has effect only in the laser box / boxState stack (i.e. not in the interface).
  // It is used when the laserBox receives an order to change active state from the interface.

  Serial.println("ControlerBox::setBoxActiveState(): Ending");
}





// Setter for the defaultState and associated variables
// Called only from this class (for the other boxes).
void ControlerBox::setBoxDefaultState(const uint8_t _ui8BoxIndex, const short _sBoxDefaultState) {
  Serial.println("ControlerBox::setBoxDefaultState(): Starting");

  ControlerBoxes[_ui8BoxIndex].sBoxDefaultState = _sBoxDefaultState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveState: %u\n", _ui8BoxIndex, ControlerBoxes[_ui8BoxIndex].boxActiveState);

  ControlerBoxes[_ui8BoxIndex].sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", _ui8BoxIndex, ControlerBoxes[_ui8BoxIndex].boxActiveStateHasBeenSignaled);

  Serial.println("ControlerBox::setBoxDefaultState(): Ending");
}





void ControlerBox::updateConnectedBoxCount(short int newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  connectedBoxesCount = newConnectedBoxesCount;
}



void ControlerBox::deleteBox(uint32_t _ui32nodeId) {
  Serial.println("ControlerBox::deleteBox(): Starting");
  for (uint8_t __it = 0; __it < gui8BoxesCount; __it++) {
    if (ControlerBoxes[__it].nodeId == _ui32nodeId) {
      ControlerBoxes[__it].nodeId = 0;
      ControlerBoxes[__it].APIP = {0,0,0,0};
      ControlerBoxes[__it].stationIP = {0,0,0,0};
      ControlerBoxes[__it].bNodeName = 0;

      ControlerBoxes[__it].boxActiveState = -1;
      ControlerBoxes[__it].uiBoxActiveStateStartTime = 0;
      ControlerBoxes[__it].boxActiveStateHasBeenSignaled = true;
      ControlerBoxes[__it].boxActiveStateHasBeenTakenIntoAccount = true;

      ControlerBoxes[__it].isNewBoxHasBeenSignaled = true;
      ControlerBoxes[__it].boxDeletionHasBeenSignaled = false;

      ControlerBoxes[__it].bMasterBoxName = UI8_DEFAULT_MASTER_NODE_NAME;
      ControlerBoxes[__it].bMasterBoxNameChangeHasBeenSignaled = true;
      ControlerBoxes[__it].boxActiveStateHasBeenTakenIntoAccount = true;

      updateConnectedBoxCount(connectedBoxesCount - 1);
      break;
    }
  }
  Serial.println("ControlerBox::deleteBox(): Ending");
}




Task ControlerBox::tReboot(3000, TASK_ONCE, NULL, &userScheduler, false, NULL, &_reboot);

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
