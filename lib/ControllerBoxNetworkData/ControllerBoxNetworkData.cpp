/*
  ControllerBoxNetworkData.cpp - Library to handle the ControlerBox network data attributes.
  Created by Cedric Lor, October 26, 2019.
*/

#include "Arduino.h"
#include "ControllerBoxNetworkData.h"


// PUBLIC
// Instance Methods

// Constructor
ControllerBoxNetworkData::ControllerBoxNetworkData():
  nodeId(0),
  stationIP{0,0,0,0},
  APIP{0,0,0,0}  
{ }




/**void ControllerBoxNetworkData::update(uint32_t _ui32SenderNodeId, JsonObject& _obj)
 * 
 *  Updater of the properties of the controller boxes in the mesh
 *  (including this one, at index 0 of the controller boxes array).
 *  
 *  Called from controllerBoxesCollection::_updateOrCreate() exclusively
*/
void ControllerBoxNetworkData::update(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  const char * _subName = "ControlerBox::update():";
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

  Serial.println("%s over");
}
