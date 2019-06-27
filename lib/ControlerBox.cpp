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
short int ControlerBox::valFromMesh = -1;
uint32_t ControlerBox::uiSettingTimeOfValFromMesh = 0;
short int ControlerBox::valFromWeb = -1;

// PUBLIC
ControlerBox::ControlerBox()
{
  nodeId = 0;
  boxActiveState = -1;
}

void ControlerBox::updateThisBoxProperties() {
  nodeId = laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  APIP = laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  stationIP = laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  bNodeName = B_NODE_NAME;
  // valFromPir is updated by pirController (which sets it HIGH) and boxState (which sets it LOW)
  // valFromMesh is updated by myMesh (--> update to be moved to myMeshController) and boxState
  if (MY_DEBUG == true) {ControlerBoxes[MY_INDEX_IN_CB_ARRAY].printProperties(MY_INDEX_IN_CB_ARRAY);};
}

void ControlerBox::printProperties(const byte bBoxIndex) {
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i] updated.\n", bBoxIndex);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].nodeId: %u\n", bBoxIndex, nodeId);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].APIP:", bBoxIndex);Serial.println(APIP);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].stationIP:", bBoxIndex);Serial.println(stationIP);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].bNodeName: %i\n", bBoxIndex, bNodeName);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].boxActiveState: %i\n", bBoxIndex, boxActiveState);
  Serial.printf("ControlerBox::printProperties(): ControlerBoxes[%i].uiBoxActiveStateStartTime: %u\n", bBoxIndex, uiBoxActiveStateStartTime);
}

void ControlerBox::updateOtherBoxProperties(uint32_t senderNodeId, JsonDocument& doc) {
  Serial.println("ControlerBox::updateOtherBoxProperties(): Starting");
  byte __bNodeName = doc["senderNodeName"]; // ex. 201
  Serial.printf("ControlerBox::updateOtherBoxProperties(): __bNodeName = %i\n", __bNodeName);
  byte __bBoxIndex = __bNodeName - B_CONTROLLER_BOX_PREFIX; // 201 - 200 = 1
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): __bBoxIndex = %i\n", __bBoxIndex);
  ControlerBoxes[__bBoxIndex].nodeId = senderNodeId;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[__bBoxIndex].nodeId = %i", senderNodeId);
  ControlerBoxes[__bBoxIndex].APIP = _parseIpStringToIPAddress(doc, "senderAPIP");
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].APIP = ", __bBoxIndex);Serial.println(ControlerBoxes[__bBoxIndex].APIP);
  ControlerBoxes[__bBoxIndex].stationIP = _parseIpStringToIPAddress(doc, "senderStIP");
  // Serial.print("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].stationIP = ", __bBoxIndex);Serial.println(ControlerBoxes[__bBoxIndex].stationIP);
  ControlerBoxes[__bBoxIndex].bNodeName = __bNodeName;
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].bNodeName = %i", __bBoxIndex, ControlerBoxes[__bBoxIndex].bNodeName);
  ControlerBoxes[__bBoxIndex].boxActiveState = doc["senderBoxActiveState"];
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].boxActiveState: %i\n", boxIndex, boxActiveState);
  ControlerBoxes[__bBoxIndex].uiBoxActiveStateStartTime = doc["senderBoxActiveStateStartTime"];
  // Serial.printf("ControlerBox::updateOtherBoxProperties(): ControlerBoxes[%i].uiBoxActiveStateStartTime: %i\n", boxIndex, boxActiveState);
  // need to send via myMeshViews and add to ControlerBox the time for which the new sender boxState shall apply
  if (MY_DEBUG == true) {ControlerBoxes[__bBoxIndex].printProperties(__bBoxIndex);};
  Serial.println("ControlerBox::updateOtherBoxProperties(): Ending");
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
