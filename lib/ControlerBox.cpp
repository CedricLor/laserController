/*
  ControlerBox.cpp - Library to replace box_type struct - handles the ControlerBox attributes
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "ControlerBox.h"

// STATIC VARIABLES
bool ControlerBox::valPir = LOW;
short int ControlerBox::valMesh = -1;

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
  iNodeName = I_NODE_NAME;
  // valPir is updated by pirController (which sets it HIGH) and boxState (which sets it LOW)
  // valMesh is updated by myMesh (--> update to be moved to myMeshController) and boxState
  if (MY_DEBUG == true) {ControlerBoxes[MY_INDEX_IN_CB_ARRAY].printProperties();};
}

void ControlerBox::printProperties() {
  Serial.println("myMesh::receivedCallback(): ControlerBoxes[0] updated.");
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[MY_INDEX_IN_CB_ARRAY].nodeId:");Serial.println(nodeId);
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[MY_INDEX_IN_CB_ARRAY].APIP:");Serial.println(APIP);
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[MY_INDEX_IN_CB_ARRAY].stationIP:");Serial.println(stationIP);
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[MY_INDEX_IN_CB_ARRAY].iNodeName:");Serial.println(iNodeName);
}

void ControlerBox::updateOtherBoxProperties(uint32_t senderNodeId, JsonDocument& doc) {
  short int __iNodeName = doc["senderNodeName"]; // ex. 201
  short int __boxIndex = __iNodeName - I_CONTROLLER_BOX_PREFIX; // 201 - 200 = 1
  ControlerBoxes[__boxIndex].nodeId = senderNodeId;
  ControlerBoxes[__boxIndex].APIP = _parseIpStringToIPAddress(doc, "senderAPIP");
  ControlerBoxes[__boxIndex].stationIP = _parseIpStringToIPAddress(doc, "senderStationIP");
  ControlerBoxes[__boxIndex].iNodeName = __iNodeName;
  // need to add the sender boxState so that each box knows in which state are the other boxes
  // need to send via myMeshViews and add to ControlerBox the time at which the sender boxState was changed
  // need to send via myMeshViews and add to ControlerBox the time for which the new sender boxState shall apply
}

////////////////////////////////////////////////////////////////////////////////
// PRIVATE
IPAddress ControlerBox::_parseIpStringToIPAddress(JsonDocument& root, String rootKey) {
  const char* ipStr = root[rootKey];
  byte ip[4];
  _parseCharArrayToBytes(ipStr, '.', ip, 4, 10);
  return ip;
}

void ControlerBox::_parseCharArrayToBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  // see https://stackoverflow.com/questions/35227449/convert-ip-or-mac-address-from-string-to-byte-array-arduino-or-c
  /*  Call it this way:
        const char* ipStr = "50.100.150.200";
        byte ip[4];
        _parseBytesToCharArray(ipStr, '.', ip, 4, 10);
   */
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);  // Convert byte
    str = strchr(str, sep);               // Find next separator
    if (str == NULL || *str == '\0') {
        break;                            // No more separators, exit
    }
    str++;                                // Point to next character after separator
  }
}
