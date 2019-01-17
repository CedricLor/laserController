/*
  ControlerBox.cpp - Library to replace box_type struct - handles the ControlerBox attributes
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "ControlerBox.h"

ControlerBox::ControlerBox()
{
  // pinMode(pin, OUTPUT);
  // _pin = pin;
}

void ControlerBox::boxTypeSelfUpdate(ControlerBox *ControlerBoxes, const short I_NODE_NAME, const short BOXES_I_PREFIX) {
  ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].APIP = myMesh.getAPIP();      // store this boxes APIP in the array of boxes pertaining to the mesh
  ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].stationIP = myMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].nodeId = myMesh.getNodeId();  // store this boxes nodeId in the array of boxes pertaining to the mesh
}

// void ControlerBox::boxTypeUpdate(ControlerBox *ControlerBoxes, uint32_t iSenderNodeName, uint32_t senderNodeId, JsonObject& root, const short I_NODE_NAME, const short BOXES_I_PREFIX) {
//   ControlerBoxes[iSenderNodeName - BOXES_I_PREFIX].APIP = ControlerBox::parseIpString(root, "senderAPIP");
//   ControlerBoxes[iSenderNodeName - BOXES_I_PREFIX].stationIP = ControlerBox::parseIpString(root, "senderStationIP");
//   ControlerBoxes[iSenderNodeName - BOXES_I_PREFIX].nodeId = senderNodeId;
// }

IPAddress ControlerBox::parseIpString(JsonObject& root, String rootKey) {
  const char* ipStr = root[rootKey];
  byte ip[4];
  parseBytes(ipStr, '.', ip, 4, 10);
  return ip;
}

void ControlerBox::parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
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
