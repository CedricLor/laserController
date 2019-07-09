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


myMeshViews::myMeshViews()
{
  ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();
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

// This function is called exclusively from the laser controllers -- not the interface
void myMeshViews::changedMasterBoxConfirmation(const byte newMasterNodeName) {
  Serial.printf("myMeshViews::changedMasterBoxConfirmation(): Starting. newMasterNodeName = %i\n", newMasterNodeName);
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"m";"ms":"201";"react":"syn"}
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  // load the JSON document with values
  msg["ms"] = (int)newMasterNodeName;
  msg["action"] = "mc";

  _sendMsg(msg, ControlerBoxes[0].nodeId);

  Serial.println("myMeshViews::changedMasterBoxConfirmation(): Ending.");
}

// This function is called exclusively from the webserver -- the interface
void myMeshViews::changeMasterBoxMsg(const int newMasterNodeName, const int iBoxName) {
  Serial.printf("myMeshViews::changeMasterBoxMsg(): Starting. newMasterNodeName = %i, iBoxName (dest index nb) = %i\n", newMasterNodeName, iBoxName);
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"m";"ms":"201";"react":"syn"}
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  // load the JSON document with values
  msg["ms"] = newMasterNodeName;
  msg["action"] = "m";

  // get the destination nodeId
  uint32_t _destNodeId = ControlerBoxes[iBoxName].nodeId;

  _sendMsg(msg, _destNodeId);

  Serial.println("myMeshViews::changeMasterBoxMsg(): Ending.");
}

void myMeshViews::statusMsg(uint32_t destNodeId) {
  Serial.println("myMeshViews::statusMsg(): Starting.");
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"defBxSt":5;"NNa":"201";"APIP":"...";"StIP":"..."}

  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  // load the JSON document with values
  msg["actSt"] = ControlerBoxes[myIndexInCBArray].boxActiveState;
  msg["actStStartT"] = ControlerBoxes[myIndexInCBArray].uiBoxActiveStateStartTime; // gets the recorded mesh time
  msg["defBxSt"] = ControlerBoxes[myIndexInCBArray].sBoxDefaultState;
  msg["action"] = "s";

  // send to the sender
  _sendMsg(msg, destNodeId);

  // I signaled my boxState change.
  // => set my own boxActiveStateHasBeenSignaled to true
  ControlerBoxes[myIndexInCBArray].boxActiveStateHasBeenSignaled = true;

  Serial.println("myMeshViews::statusMsg(): Ending.");
}

void myMeshViews::changeBoxTargetState(const char *boxTargetState, const char *boxName) {
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"receiverTargetState":3;"action":"c";"receiverBoxName":201;"NNa":"200";"APIP":"...";"StIP":"..."}
  if (MY_DEBUG) {
    Serial.printf("myMeshViews::changeBoxTargetState(): boxTargetState = %s, boxName = %s\n", boxTargetState, boxName);
  }

  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();

  msg["receiverTargetState"] = boxTargetState;
  msg["receiverBoxName"] = boxName;
  msg["action"] = "c";

  _sendMsg(msg);
}

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
// Helper functions
// serialization of message to be sent
JsonObject myMeshViews::_createJsonobject() {
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;

  JsonObject msg = doc.to<JsonObject>();
  return msg;
}

void myMeshViews::_sendMsg(JsonObject& msg, uint32_t destNodeId) {
  Serial.println("myMeshViews::_sendMsg(): Starting.");

  // Serial.println("myMeshViews::_sendMsg(): about to allocate ControlerBoxes[myIndexInCBArray].bNodeName to msg[\"senderNodeName\"]");
  msg["NNa"] = ControlerBoxes[myIndexInCBArray].bNodeName;
  // Serial.println("myMeshViews::_sendMsg(): about to allocate APIP to msg[\"senderAPIP\"]");
  msg["APIP"] = (ControlerBoxes[myIndexInCBArray].APIP).toString();
  // Serial.println("myMeshViews::_sendMsg(): about to allocate stationIP to msg[\"senderStIP\"]");
  msg["StIP"] = (ControlerBoxes[myIndexInCBArray].stationIP).toString();
  // Serial.println("myMeshViews::_sendMsg(): added IPs to the JSON object before sending");

  // JSON serialization
  int size_buff = 254;
  char output[size_buff];

  Serial.println("myMeshViews::_sendMsg(): about to serialize JSON object");
  serializeJson(msg, output, size_buff);
  Serial.println("myMeshViews::_sendMsg(): JSON object serialized");

  // JSON serialization conversion for painlessMesh
  Serial.println("myMeshViews::_sendMsg(): About to convert serialized object to String");
  String str;
  str = output;
  Serial.println("myMeshViews::_sendMsg(): About to send message as String");

  // diffusion
  if (destNodeId) {
    laserControllerMesh.sendSingle(destNodeId, str);
  } else {
    laserControllerMesh.sendBroadcast(str);
  }
  Serial.print("myMeshViews:_sendMsg(): done. Broadcasted message: ");Serial.println(str);
}
