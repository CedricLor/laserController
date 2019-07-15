/*
  myMeshViews.cpp - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.

  |--main.cpp
  |  |
  |  |--myMesh.cpp
  |  |  |--myMesh.h
  |  |  |
  |  |  |--myMeshControler.cpp (private - called only from my Mesh)
  |  |  |  |--myMeshControler.h
  |  |  |  |--MasterSlaveBox.cpp
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
  |  |  |  |--ControlerBox.cpp
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
*/

#include "Arduino.h"
#include "myMeshViews.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// myMeshViews: message sender for controller boxes
// myMeshViews::myMeshViews(const char* state)
/* To broadcast a message, you just have to instantiate the class. The sender is located in the constructor.
   Upon instantiation, the constructor updates the properties of the representation of this box
   in the array of ControlerBoxes (APIP, StationIP, NodeName).
   After having called the constructor, you have to call a message builder, that will:
   2. create a structured String with the params you passed it on;
   3. passe this String to the sendBroadcast method of painlessMesh.
   painlessMesh takes care of the broadcast.
   Messages from controller boxes are not adressed to any other box in particular; all the other boxes
   receive the message and, as the case may be, react accordingly.
*/



// Tasks
Task myMeshViews::tSendBoxStateToNewBox(myIndexInCBArray * 1000, 1, NULL, &userScheduler, false, NULL, _odtcbSendBoxStateToNewBox);

void myMeshViews::_odtcbSendBoxStateToNewBox() {
  for (short int _boxIndex = 1; _boxIndex < sBoxesCount; _boxIndex++) {
    if (ControlerBoxes[_boxIndex].nodeId != 0) {
      if (ControlerBoxes[_boxIndex].isNewBoxHasBeenSignaled == false) {
        myMeshViews __myMeshViews;
        __myMeshViews.statusMsg(ControlerBoxes[_boxIndex].nodeId);
        ControlerBoxes[_boxIndex].isNewBoxHasBeenSignaled = true;
      }
    }
  }
}






// Constructor
myMeshViews::myMeshViews()
{
  ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();
}





void myMeshViews::relayWSChangeRequest(const int8_t _i8RequestedChange, const char& _cChangeKey, const int8_t _i8BoxIndexInCB) {
    if (MY_DG_MESH) {
      Serial.printf("myMeshViews::relayWSChangeRequest(): Starting. _i8RequestedChange = %i, _cChangeKey = %c, _i8BoxIndexInCB (dest index nb) = %i\n", _i8RequestedChange, _cChangeKey, _i8BoxIndexInCB);
    }
    /* expected JSON string: {
      "NNa":"001";
      "APIP":"...";
      "StIP":"...";
      "action":"changeBox";
      "boxState": 0; // "boxDefstate": 4; // "masterbox":201}
    */
    const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
    StaticJsonDocument<capacity> doc;
    JsonObject msg = doc.to<JsonObject>();

    // load the JSON document with values
    msg[&_cChangeKey] = _i8RequestedChange;
    msg["action"] = "changeBox";

    // msg["receiverTargetState"] = _i8BoxTargetState;
    // // msg["receiverBoxName"] = _i8NodeName;
    // msg["boxIndex"] = _i8BoxIndexInCB;
    // msg["action"] = "c";

    // msg["ms"] = _i8MasterBox;
    // msg["action"] = "m";

    // msg["receiverDefaultState"] = _i8BoxDefaultState;
    // // msg["receiverBoxName"] = _i8NodeName;
    // msg["boxIndex"] = _i8BoxIndexInCB;
    // msg["action"] = "d";

    // get the destination nodeId
    uint32_t _destNodeId = ControlerBoxes[_i8BoxIndexInCB].nodeId;

    _sendMsg(msg, _destNodeId);

    if (MY_DG_MESH) {
      Serial.println("myMeshViews::relayWSChangeRequest(): Ending.");
    }
}





// void myMeshViews::changeBoxTargetState(const int8_t _i8BoxTargetState, const int8_t _i8BoxIndexInCB/*_i8NodeName *//* const short _sBoxTargetState, const short _sBoxName */) {
//   // prepare the JSON string to be sent via the mesh
//   // expected JSON string: {"receiverTargetState":3;"action":"c";"receiverBoxName":201;"NNa":"200";"APIP":"...";"StIP":"..."}
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshViews::changeBoxTargetState(): _i8BoxTargetState = %i, _i8BoxIndexInCB = %i\n", _i8BoxTargetState, _i8BoxIndexInCB);
//   }
//
//   const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
//   StaticJsonDocument<capacity> doc;
//   JsonObject msg = doc.to<JsonObject>();
//
//   msg["receiverTargetState"] = _i8BoxTargetState;
//   // msg["receiverBoxName"] = _i8NodeName;
//   msg["boxIndex"] = _i8BoxIndexInCB;
//   msg["action"] = "c";
//
//   // get the destination nodeId
//   uint32_t _destNodeId = ControlerBoxes[_i8BoxIndexInCB].nodeId;
//
//   _sendMsg(msg, _destNodeId);
//
//   if (MY_DG_MESH) {
//     Serial.println("myMeshViews::changeBoxTargetState(): Ending.");
//   }
// }
//
//
//
//
// // This function is called exclusively from the webserver -- the interface
// void myMeshViews::changeMasterBox(const int8_t _i8MasterBox /*newMasterNodeName*/, const int8_t _i8BoxIndexInCB) {
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshViews::changeMasterBox(): Starting. _i8MasterBox = %i, _i8BoxIndexInCB (dest index nb) = %i\n", _i8MasterBox, _i8BoxIndexInCB);
//   }
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"m";"ms":"201";"react":"syn"}
//   const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
//   StaticJsonDocument<capacity> doc;
//   JsonObject msg = doc.to<JsonObject>();
//
//   // load the JSON document with values
//   msg["ms"] = _i8MasterBox;
//   msg["action"] = "m";
//
//   // get the destination nodeId
//   uint32_t _destNodeId = ControlerBoxes[_i8BoxIndexInCB].nodeId;
//
//   _sendMsg(msg, _destNodeId);
//
//   if (MY_DG_MESH) {
//     Serial.println("myMeshViews::changeMasterBox(): Ending.");
//   }
// }
//
//
//
//
//
// void myMeshViews::changeBoxDefaultState(const int8_t _i8BoxDefaultState, const int8_t _i8BoxIndexInCB/*_i8BoxName*/ /*const char *boxTargetState, const char *boxName*/) {
//   // prepare the JSON string to be sent via the mesh
//   // expected JSON string: {"receiverDefaultState":3;"action":"c";"receiverBoxName":201;"NNa":"200";"APIP":"...";"StIP":"..."}
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshViews::changeBoxDefaultState(): _i8BoxDefaultState = %i, _i8BoxIndexInCB = %i\n", _i8BoxDefaultState, _i8BoxIndexInCB);
//   }
//
//   const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
//   StaticJsonDocument<capacity> doc;
//   JsonObject msg = doc.to<JsonObject>();
//
//   msg["receiverDefaultState"] = _i8BoxDefaultState;
//   // msg["receiverBoxName"] = _i8NodeName;
//   msg["boxIndex"] = _i8BoxIndexInCB;
//   msg["action"] = "d";
//
//   // get the destination nodeId
//   uint32_t _destNodeId = ControlerBoxes[_i8BoxIndexInCB].nodeId;
//
//   _sendMsg(msg, _destNodeId);
//
//   if (MY_DG_MESH) {
//     Serial.println("myMeshViews::changeBoxDefaultState(): Ending.");
//   }
// }





void myMeshViews::statusMsg(uint32_t destNodeId) {
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::statusMsg(): Starting.");
  }
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"boxDefstate":5;"NNa":"201";"APIP":"...";"StIP":"..."}

  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  // load the JSON document with values
  msg["actSt"] = ControlerBoxes[myIndexInCBArray].boxActiveState;
  msg["actStStartT"] = ControlerBoxes[myIndexInCBArray].uiBoxActiveStateStartTime; // gets the recorded mesh time
  msg["boxDefstate"] = ControlerBoxes[myIndexInCBArray].sBoxDefaultState;
  msg["action"] = "s";

  // send to the sender
  _sendMsg(msg, destNodeId);

  // I signaled my boxState change.
  // => set my own boxActiveStateHasBeenSignaled to true
  ControlerBoxes[myIndexInCBArray].boxActiveStateHasBeenSignaled = true;

  if (MY_DG_MESH) {
    Serial.println("myMeshViews::statusMsg(): Ending.");
  }
}






// This function is called exclusively from the laser controllers -- not the interface
void myMeshViews::changedMasterBoxConfirmation(const int8_t _i8MasterBox) {
  if (MY_DG_MESH) {
    Serial.printf("myMeshViews::changedMasterBoxConfirmation(): Starting. _i8MasterBox = %i\n", _i8MasterBox);
  }
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"m";"ms":"201"}
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  // load the JSON document with values
  msg["ms"] = _i8MasterBox;
  msg["action"] = "mc";

  _sendMsg(msg, ControlerBoxes[0].nodeId);

  if (MY_DG_MESH) {
    Serial.println("myMeshViews::changedMasterBoxConfirmation(): Ending.");
  }
}





// This function is called exclusively from the laser controllers -- not the interface
void myMeshViews::changedBoxDefaultStateConfirmation(const int8_t _i8BoxDefaultState) {
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"dc";"ds":3}
  if (MY_DG_MESH) {
    Serial.printf("myMeshViews::changeBoxDefaultStateConfirmation(): _i8BoxDefaultState = %i\n", _i8BoxDefaultState);
  }

  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  msg["ds"] = _i8BoxDefaultState;
  msg["action"] = "dc";

  _sendMsg(msg);
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::changedBoxDefaultStateConfirmation(): Ending.");
  }
}





// Helper functions
// serialization of message to be sent
JsonObject myMeshViews::_createJsonobject() {
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;

  JsonObject msg = doc.to<JsonObject>();
  return msg;
}




void myMeshViews::_sendMsg(JsonObject& msg, uint32_t destNodeId) {
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): Starting.");
  }
  // Serial.println("myMeshViews::_sendMsg(): about to allocate ControlerBoxes[myIndexInCBArray].bNodeName to msg[\"senderNodeName\"]");
  msg["NNa"] = ControlerBoxes[myIndexInCBArray].bNodeName;
  // Serial.println("myMeshViews::_sendMsg(): about to allocate APIP to msg[\"senderAPIP\"]");
  JsonArray _APIP = msg.createNestedArray("APIP");
  for (short _i = 0; _i < 4; _i++) {
    _APIP.add(ControlerBoxes[myIndexInCBArray].APIP[_i]);
  }
  // Serial.println("myMeshViews::_sendMsg(): about to allocate stationIP to msg[\"senderStIP\"]");
  JsonArray _StIP = msg.createNestedArray("StIP");
  for (short _i = 0; _i < 4; _i++) {
    _StIP.add(ControlerBoxes[myIndexInCBArray].stationIP[_i]);
  }
  // Serial.println("myMeshViews::_sendMsg(): added IPs to the JSON object before sending");

  // JSON serialization
  int size_buff = 254;
  char output[size_buff];

  if (MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): about to serialize JSON object");
  }
  serializeJson(msg, output, size_buff);
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): JSON object serialized");
  }
  // JSON serialization conversion for painlessMesh
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): About to convert serialized object to String");
  }
  String str;
  str = output;
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): About to send message as String");
  }

  // diffusion
  if (destNodeId) {
    laserControllerMesh.sendSingle(destNodeId, str);
  } else {
    laserControllerMesh.sendBroadcast(str);
  }
  if (MY_DG_MESH) {
    Serial.print("myMeshViews:_sendMsg(): done. Broadcasted message: ");Serial.println(str);
  }
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Views (or messages)
// void myMeshViews::manualSwitchMsg(const short targetOnOffState) {
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"u";"ts":"0"}
//   JsonObject msg = _createJsonobject();
//   msg["ts"] = targetOnOffState;
//   _sendMsg(msg, 'u');
// }
//
// void myMeshViews::inclusionIRMsg(const short targetIrState) {
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"i";"ts":"0"}
//   JsonObject msg = _createJsonobject();
//   msg["ts"] = targetIrState;
//   _sendMsg(msg, 'i');
// }
//
// void myMeshViews::blinkingIntervalMsg(const unsigned long targetBlinkingInterval) {
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"b";"ti":"5000"}
//   JsonObject msg = _createJsonobject();
//   msg["ti"] = targetBlinkingInterval;
//   _sendMsg(msg, 'b');
// }
//
// void myMeshViews::pinPairingMsg(const short sTargetPairingType) {
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"p";"pt":"0"}
//   JsonObject msg = _createJsonobject();
//   msg["pinPairingType"] = sTargetPairingType;
//   _sendMsg(msg, 'p');
// }
//
// void myMeshViews::dataRequestMsg() {
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"d"}
//   JsonObject msg = _createJsonobject();
//   _sendMsg(msg, 'd');
// }
//
// void myMeshViews::dataRequestResponse() {
//   // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"r";"response":{A DETERMINER}}
//   JsonObject msg = _createJsonobject();
//   // msg["response"] = {A DETERMINER};
//   _sendMsg(msg, 'r');
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
