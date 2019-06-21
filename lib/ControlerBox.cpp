/*
  ControlerBox.cpp - Library to replace box_type struct - handles the ControlerBox attributes
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "ControlerBox.h"

// PUBLIC

ControlerBox::ControlerBox()
{
  valPir = LOW;
  valMesh = -1;
  activeBoxState = -1;
}

void ControlerBox::updateProperties() {
  // to be drafted
}

void ControlerBox::updateThisBoxProperties() {
  nodeId = laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  APIP = laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  stationIP = laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  iNodeName = I_NODE_NAME;
  // valPir is updated by pirController (which sets it HIGH) and boxState (which sets it LOW)
  // valMesh is updated by myMesh (--> update to be moved to myMeshController) and boxState
  if (MY_DEBUG == true) {ControlerBoxes[0].printProperties();};
}

void ControlerBox::printProperties() {
  Serial.println("myMesh::receivedCallback(): ControlerBoxes[0] updated.");
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[0].nodeId:");Serial.println(nodeId);
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[0].APIP:");Serial.println(APIP);
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[0].stationIP:");Serial.println(stationIP);
  Serial.print("myMesh::receivedCallback(): ControlerBoxes[0].iNodeName:");Serial.println(iNodeName);
}

void ControlerBox::updateOtherBoxProperties() {

}
////////////////////////////////////////////////////////////////////////////////
// PRIVATE
IPAddress ControlerBox::_parseIpString(JsonObject& root, String rootKey) {
  const char* ipStr = root[rootKey];
  byte ip[4];
  _parseBytes(ipStr, '.', ip, 4, 10);
  return ip;
}

void ControlerBox::_parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  // see https://stackoverflow.com/questions/35227449/convert-ip-or-mac-address-from-string-to-byte-array-arduino-or-c
  /*  Call it this way:
        const char* ipStr = "50.100.150.200";
        byte ip[4];
        parseBytes(ipStr, '.', ip, 4, 10);
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
