/*
  controllerBoxNetworkData.cpp - Library to handle the ControlerBox network data attributes.
  Created by Cedric Lor, October 26, 2019. 
*/

#include "Arduino.h"
#include "controllerBoxNetworkData.h"


// PUBLIC
// Instance Methods

// Constructor
controllerBoxNetworkData::controllerBoxNetworkData():
  nodeId(0),
  stationIP{0,0,0,0},
  APIP{0,0,0,0}  
{ }




// void controllerBoxNetworkData::printProperties(const uint16_t __ui16BoxIndex) {
//   const char * _methodName = "controllerBoxNetworkData::printProperties(): Box n. ";
//   Serial.printf("%s[%u] -> nodeId: %u\n", _methodName, __ui16BoxIndex, nodeId);
//   Serial.printf("%s[%u] -> APIP: ", _methodName, __ui16BoxIndex);Serial.println(APIP.toString());
//   Serial.printf("%s[%u] -> stationIP: ", _methodName, __ui16BoxIndex);Serial.println(stationIP.toString());
// }



/**void controllerBoxNetworkData::update(uint32_t _ui32SenderNodeId, JsonObject& _obj)
 * 
 *  Updater of the network properties of the (other) controller boxes in the mesh.
 *  
 *  Called from ControlerBox exclusively
*/
void controllerBoxNetworkData::update(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  const char * _subName = "controllerBoxNetworkData::update():";
  Serial.printf("%s starting\n", _subName);

  nodeId = _ui32SenderNodeId;
  // Serial.printf("%s ControlerBoxes[%u].nodeId = %u\n", __ui16BoxIndex, _ui32SenderNodeId);

  // set the IPs
  if ( _obj.containsKey("APIP")){ 
    APIP = IPAddress(_obj["APIP"][0], _obj["APIP"][1], _obj["APIP"][2], _obj["APIP"][3]);
  }
  if ( _obj.containsKey("StIP")) {
    stationIP = IPAddress(_obj["StIP"][0], _obj["StIP"][1], _obj["StIP"][2], _obj["StIP"][3]);
  }

  Serial.printf("%s over\n", _subName);
}
