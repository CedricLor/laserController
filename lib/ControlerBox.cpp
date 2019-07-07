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
  // - myMeshController constructor (on the laser box, because receiving an action "m" (change master box request) from the interface)
  // - myMeshController constructor (on the interface, because on receiving an action "mc" (master changed confirmation) message); setting it to false
  // - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) (setting it to true, once a message has been sent to the browser)
  // testers:
  // - myWebServerBase::_tcbSendWSDataIfChangeBoxState (on the interface) (tests whether a change has been made and whether it needs to inform the browser)
  bMasterBoxNameChangeHasBeenSignaled = true;
}


void ControlerBox::updateThisBoxProperties() {
  nodeId = laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  APIP = laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  stationIP = laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  this->bNodeName = gbNodeName;
  // For this box, boxActiveState, boxActiveStateHasBeenSignaled and uiBoxActiveStateStartTime are updated
  // by a call to setBoxActiveState from boxState
  if (MY_DEBUG == true) {ControlerBoxes[MY_INDEX_IN_CB_ARRAY].printProperties(MY_INDEX_IN_CB_ARRAY);};
}


void ControlerBox::printProperties(const byte bBoxIndex) {
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].nodeId: %u\n", bBoxIndex, nodeId);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].APIP:", bBoxIndex);Serial.println(APIP);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].stationIP:", bBoxIndex);Serial.println(stationIP);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bNodeName: %i\n", bBoxIndex, this->bNodeName);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveState: %i\n", bBoxIndex, boxActiveState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].uiBoxActiveStateStartTime: %u\n", bBoxIndex, uiBoxActiveStateStartTime);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveStateHasBeenSignaled: %i\n", bBoxIndex, boxActiveStateHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveStateHasBeenTakenIntoAccount: %i\n", bBoxIndex, boxActiveStateHasBeenTakenIntoAccount);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].isNewBoxHasBeenSignaled: %i\n", bBoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxDeletionHasBeenSignaled: %i\n", bBoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bMasterBoxName: %i\n", bBoxIndex, bMasterBoxName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bMasterBoxNameChangeHasBeenSignaled: %i\n", bBoxIndex, bMasterBoxNameChangeHasBeenSignaled);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveStateHasBeenTakenIntoAccount: %i\n", bBoxIndex, boxActiveStateHasBeenTakenIntoAccount);
}

void ControlerBox::updateMasterBoxName(const byte _bMasterBoxName) {
  this->bMasterBoxName = _bMasterBoxName;
  bMasterBoxNameChangeHasBeenSignaled = false;
}



// Static Methods

// updater of the properties of the other boxes in the mesh
// called from myMeshController
void ControlerBox::updateOtherBoxProperties(uint32_t senderNodeId, JsonDocument& doc) {
  Serial.println("ControlerBox::updateOtherBoxProperties(): Starting");

  // Setting nodeName, nodeId and IP properties
  // extract the index of the relevant box from its senderNodeName in the JSON
  byte __bNodeName = doc["senderNodeName"]; // ex. 201
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __bNodeName = %i\n", __bNodeName);
  byte __bBoxIndex = __bNodeName - B_CONTROLLER_BOX_PREFIX; // 201 - 200 = 1

  // set the nodeId
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __bBoxIndex = %i\n", __bBoxIndex);
  if (ControlerBoxes[__bBoxIndex].nodeId == 0) {
    updateConnectedBoxCount(connectedBoxesCount + 1);
    ControlerBoxes[__bBoxIndex].isNewBoxHasBeenSignaled = false;
    Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__bBoxIndex].isNewBoxHasBeenSignaled = %i\n", ControlerBoxes[__bBoxIndex].isNewBoxHasBeenSignaled);
  }
  ControlerBoxes[__bBoxIndex].nodeId = senderNodeId;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__bBoxIndex].nodeId = %i\n", ControlerBoxes[__bBoxIndex].senderNodeId);

  // set the IPs
  ControlerBoxes[__bBoxIndex].APIP = _parseIpStringToIPAddress(doc, "senderAPIP");
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].APIP = ", __bBoxIndex);Serial.println(ControlerBoxes[__bBoxIndex].APIP);
  ControlerBoxes[__bBoxIndex].stationIP = _parseIpStringToIPAddress(doc, "senderStIP");
  // Serial.print("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].stationIP = ", __bBoxIndex);Serial.println(ControlerBoxes[__bBoxIndex].stationIP);

  // set the bNodeName
  ControlerBoxes[__bBoxIndex].bNodeName = __bNodeName;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].bNodeName = %i\n", __bBoxIndex, ControlerBoxes[__bBoxIndex].bNodeName);

  // Setting activeState stack
  // extract the __senderBoxActiveState from the JSON
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  int __senderBoxActiveState = doc["senderBoxActiveState"];
  setBoxActiveState(__bBoxIndex, __senderBoxActiveState);

  // Print out the updated properties
  if (MY_DEBUG == true) {ControlerBoxes[__bBoxIndex].printProperties(__bBoxIndex);};
  Serial.println("ControlerBox::updateOtherBoxProperties(): Ending");
}




// Setter for the activeState and associated variables
// Called only from this class (for the other boxes) and by
// boxState (when an effective update has been made).
void ControlerBox::setBoxActiveState(const byte bBoxIndex, const int senderBoxActiveState) {
  Serial.println("ControlerBox::setBoxActiveState(): Starting");

  ControlerBoxes[bBoxIndex].boxActiveState = senderBoxActiveState;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveState: %i\n", bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveState);

  ControlerBoxes[bBoxIndex].boxActiveStateHasBeenSignaled = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveStateHasBeenSignaled: %i\n", bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveStateHasBeenSignaled);

  ControlerBoxes[bBoxIndex].uiBoxActiveStateStartTime = laserControllerMesh.getNodeTime();
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].uiBoxActiveStateStartTime: %i\n", __bBoxIndex, ControlerBoxes[__bBoxIndex].uiBoxActiveStateStartTime);

  ControlerBoxes[bBoxIndex].boxActiveStateHasBeenTakenIntoAccount = false;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveStateHasBeenTakenIntoAccount: %i\n", __bBoxIndex, ControlerBoxes[bBoxIndex].boxActiveStateHasBeenTakenIntoAccount);

  Serial.println("ControlerBox::setBoxActiveState(): Ending");
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
