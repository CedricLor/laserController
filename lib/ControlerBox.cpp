/*
  ControlerBox.cpp - Library to replace box_type struct - handles the ControlerBox attributes
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "ControlerBox.h"

bool ControlerBox::valPir = LOW;
short int ControlerBox::valMesh = -1;

ControlerBox::ControlerBox()
{
  // pinMode(pin, OUTPUT);
  // _pin = pin;
}

void ControlerBox::updateProperties() {
  // nodeId = laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  // APIP = laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  // stationIP = laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
}

void ControlerBox::updateThisBoxProperties() {
  nodeId = laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  APIP = laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
  stationIP = laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  iNodeName = I_NODE_NAME;
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
