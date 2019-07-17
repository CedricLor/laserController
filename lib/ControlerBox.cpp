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
  // - here; -> from the global.B_DEFAULT_MASTER_NODE_NAME
  // - in updateMasterBoxName() below. updateMasterBoxName() is called from:
  //    - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface)
  //    - myMeshController constructor (on the interface, because on receiving an action "mc" (master changed confirmation) message)
  // - in mySavedPrefs::loadPreferences()
  // tested or used in:
  // - boxState class (on the laser boxes)
  // - here (printProperties)
  // - in myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) to send various messages
  // - in mySavedPrefs::savePreferences()
  this->bMasterBoxName = B_DEFAULT_MASTER_NODE_NAME;

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
  bNodeName = gbNodeName;
  // For this box, boxActiveState, boxActiveStateHasBeenSignaled and uiBoxActiveStateStartTime are updated
  // by a call to setBoxActiveState from boxState
  if (MY_DEBUG == true) {
    Serial.println("ControlerBox::updateThisBoxProperties(): Updated myself. Calling printProperties().");
    ControlerBoxes[myIndexInCBArray].printProperties(myIndexInCBArray);
  };
}



void ControlerBox::printProperties(const byte bBoxIndex) {
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].nodeId: %u\n", bBoxIndex, nodeId);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].APIP:", bBoxIndex);Serial.println(APIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].stationIP:", bBoxIndex);Serial.println(stationIP.toString());
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bNodeName: %i\n", bBoxIndex, this->bNodeName);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveState: %i\n", bBoxIndex, boxActiveState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].uiBoxActiveStateStartTime: %u\n", bBoxIndex, uiBoxActiveStateStartTime);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveStateHasBeenSignaled: %i\n", bBoxIndex, boxActiveStateHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveStateHasBeenTakenIntoAccount: %i\n", bBoxIndex, boxActiveStateHasBeenTakenIntoAccount);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].isNewBoxHasBeenSignaled: %i\n", bBoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxDeletionHasBeenSignaled: %i\n", bBoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bMasterBoxName: %i\n", bBoxIndex, bMasterBoxName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bMasterBoxNameChangeHasBeenSignaled: %i\n", bBoxIndex, bMasterBoxNameChangeHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].sBoxDefaultState: %i\n", bBoxIndex, sBoxDefaultState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].sBoxDefaultStateChangeHasBeenSignaled: %i\n", bBoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}

void ControlerBox::updateMasterBoxName(const byte _bMasterBoxName) {
  bMasterBoxName = _bMasterBoxName;  // see in constructor for information on where this variable is set and read
  bMasterBoxNameChangeHasBeenSignaled = false; // see in constructor for information on where this variable is set and read
}



// Static Methods

// updater of the properties of the other boxes in the mesh
// called from myMeshController
void ControlerBox::updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  Serial.println("ControlerBox::updateOtherBoxProperties(): Starting");

  // Setting nodeName, nodeId and IP properties
  // extract the index of the relevant box from its senderNodeName in the JSON
  byte __bNodeName = _obj["NNa"]; // ex. 201
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __bNodeName = %i\n", __bNodeName);
  byte __bBoxIndex = __bNodeName - bControllerBoxPrefix; // 201 - 200 = 1

  // set the nodeId
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __bBoxIndex = %i\n", __bBoxIndex);
  if (ControlerBoxes[__bBoxIndex].nodeId == 0) {
    updateConnectedBoxCount(connectedBoxesCount + 1);
    ControlerBoxes[__bBoxIndex].isNewBoxHasBeenSignaled = false;
    Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__bBoxIndex].isNewBoxHasBeenSignaled = %i\n", ControlerBoxes[__bBoxIndex].isNewBoxHasBeenSignaled);
  }
  ControlerBoxes[__bBoxIndex].nodeId = _ui32SenderNodeId;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__bBoxIndex].nodeId = %i\n", ControlerBoxes[__bBoxIndex]._ui32SenderNodeId);

  // set the IPs
  ControlerBoxes[__bBoxIndex].APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);; // _parseIpStringToIPAddress(_obj, "APIP");
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].APIP = ", __bBoxIndex);Serial.println(ControlerBoxes[__bBoxIndex].APIP);
  ControlerBoxes[__bBoxIndex].stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);; // _parseIpStringToIPAddress(_obj, "APIP");
  // Serial.print("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].stationIP = ", __bBoxIndex);Serial.println(ControlerBoxes[__bBoxIndex].stationIP);

  // set the bNodeName
  ControlerBoxes[__bBoxIndex].bNodeName = __bNodeName;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].bNodeName = %i\n", __bBoxIndex, ControlerBoxes[__bBoxIndex].bNodeName);

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxActiveState = _obj["actSt"];
  const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
  setBoxActiveState(__bBoxIndex, __senderBoxActiveState, __uiSenderBoxActiveStateStartTime);

  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  const short int __senderBoxDefaultState = _obj["boxDefstate"];
  setBoxDefaultState(__bBoxIndex, __senderBoxDefaultState);

  // Print out the updated properties
  if (MY_DEBUG == true) {
    Serial.printf("ControlerBox::updateOtherBoxProperties(): Updated box index %i. Calling printProperties().", __bBoxIndex);
    ControlerBoxes[__bBoxIndex].printProperties(__bBoxIndex);
  }
  Serial.println("ControlerBox::updateOtherBoxProperties(): Ending");
}




// Setter for the activeState and associated variables
// Called only from this class (for the other boxes) and by
// boxState (when an effective update has been made).
void ControlerBox::setBoxActiveState(const byte bBoxIndex, const short _sBoxActiveState, const uint32_t _uiBoxActiveStateStartTime) {
  Serial.println("ControlerBox::setBoxActiveState(): Starting");

  ControlerBoxes[bBoxIndex].boxActiveState = _sBoxActiveState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveState: %i\n", bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveState);

  ControlerBoxes[bBoxIndex].boxActiveStateHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveStateHasBeenSignaled: %i\n", bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveStateHasBeenSignaled);
  // setters:
  // - by default to true upon init (controlerBox constructor);
  // - to false here (usefull for the IF, for the received states of other boxes);
  // - to true in myMeshViews (for this box only, upon sending a statusMsg);
  // - to true and false in myWebServerBase (by the IF, for the other boxes) --> tracing if it has sent an update to the browser
  // used by the interface mostly

  ControlerBoxes[bBoxIndex].uiBoxActiveStateStartTime = _uiBoxActiveStateStartTime;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].uiBoxActiveStateStartTime: %i\n", __bBoxIndex, ControlerBoxes[__bBoxIndex].uiBoxActiveStateStartTime);

  ControlerBoxes[bBoxIndex].boxActiveStateHasBeenTakenIntoAccount = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveStateHasBeenTakenIntoAccount: %i\n", __bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveStateHasBeenTakenIntoAccount);
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
void ControlerBox::setBoxDefaultState(const byte bBoxIndex, const short _sBoxDefaultState) {
  Serial.println("ControlerBox::setBoxDefaultState(): Starting");

  ControlerBoxes[bBoxIndex].sBoxDefaultState = _sBoxDefaultState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveState: %i\n", bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveState);

  ControlerBoxes[bBoxIndex].sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveStateHasBeenSignaled: %i\n", bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveStateHasBeenSignaled);

  Serial.println("ControlerBox::setBoxDefaultState(): Ending");
}





void ControlerBox::updateConnectedBoxCount(short int newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  connectedBoxesCount = newConnectedBoxesCount;
}



void ControlerBox::deleteBox(uint32_t nodeId) {
  Serial.println("ControlerBox::deleteBox(): Starting");
  short int __boxIndex;
  for (short int __it = 0; __it < sBoxesCount; __it++) {
    if (ControlerBoxes[__it].nodeId == nodeId) {
      __boxIndex = __it;
      ControlerBoxes[__boxIndex].nodeId = 0;
      ControlerBoxes[__boxIndex].APIP = {0,0,0,0};
      ControlerBoxes[__boxIndex].stationIP = {0,0,0,0};
      ControlerBoxes[__boxIndex].bNodeName = 0;

      ControlerBoxes[__boxIndex].boxActiveState = -1;
      ControlerBoxes[__boxIndex].uiBoxActiveStateStartTime = 0;
      ControlerBoxes[__boxIndex].boxActiveStateHasBeenSignaled = true;
      ControlerBoxes[__boxIndex].boxActiveStateHasBeenTakenIntoAccount = true;

      ControlerBoxes[__boxIndex].isNewBoxHasBeenSignaled = true;
      ControlerBoxes[__boxIndex].boxDeletionHasBeenSignaled = false;

      ControlerBoxes[__boxIndex].bMasterBoxName = B_DEFAULT_MASTER_NODE_NAME;
      ControlerBoxes[__boxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
      ControlerBoxes[__boxIndex].boxActiveStateHasBeenTakenIntoAccount = true;

      updateConnectedBoxCount(connectedBoxesCount - 1);
      break;
    }
  }
  Serial.println("ControlerBox::deleteBox(): Ending");
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
