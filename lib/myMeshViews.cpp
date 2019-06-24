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
  |  |  |  |--boxState.cpp
  |  |  |  |  |--boxState.h
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
   To modify the code all through the project, look for "myMeshViews myMeshViews(".
   Currently in use in (i) myMesh class and (ii) pirController class.
*/

myMeshViews::myMeshViews()
{
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Views (or messages)
// void myMeshViews::manualSwitchMsg(const short targetOnOffState) {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"u";"ts":"0"}
//   JsonObject msg = _createJsonobject();
//   msg["ts"] = targetOnOffState;
//   _broadcastMsg(msg, 'u');
// }
//
// void myMeshViews::inclusionIRMsg(const short targetIrState) {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"i";"ts":"0"}
//   JsonObject msg = _createJsonobject();
//   msg["ts"] = targetIrState;
//   _broadcastMsg(msg, 'i');
// }
//
// void myMeshViews::blinkingIntervalMsg(const unsigned long targetBlinkingInterval) {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"b";"ti":"5000"}
//   JsonObject msg = _createJsonobject();
//   msg["ti"] = targetBlinkingInterval;
//   _broadcastMsg(msg, 'b');
// }
//
// void myMeshViews::changeMasterBoxMsg(const int newMasterNodeName, const char* newReaction) {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"m";"ms":"201";"react":"syn"}
//   JsonObject msg = _createJsonobject();
//   msg["ms"] = newMasterNodeName;
//   msg["react"] = newReaction;
//   _broadcastMsg(msg, 'm');
// }

void myMeshViews::statusMsg(const short int myBoxState) {
  // update this boxes BoxActiveStateStartTime
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].uiBoxActiveStateStartTime = laserControllerMesh.getNodeTime();
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"senderBoxActiveState":3;"action":"s";"mTime":6059117;"senderNodeName":"201";"senderAPIP":"...";"senderStIP":"..."}
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<capacity> doc;
  JsonObject msg = doc.to<JsonObject>();
  msg["senderBoxActiveState"] = myBoxState;
  msg["action"] = "s";
  msg["senderBoxActiveStateStartTime"] = ControlerBoxes[MY_INDEX_IN_CB_ARRAY].uiBoxActiveStateStartTime; // mesh time
  _broadcastMsg(msg);
}

// void myMeshViews::pinPairingMsg(const short sTargetPairingType) {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"p";"pt":"0"}
//   JsonObject msg = _createJsonobject();
//   msg["pinPairingType"] = sTargetPairingType;
//   _broadcastMsg(msg, 'p');
// }
//
// void myMeshViews::dataRequestMsg() {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"d"}
//   JsonObject msg = _createJsonobject();
//   _broadcastMsg(msg, 'd');
// }
//
// void myMeshViews::dataRequestResponse() {
//   // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"r";"response":{A DETERMINER}}
//   JsonObject msg = _createJsonobject();
//   // msg["response"] = {A DETERMINER};
//   _broadcastMsg(msg, 'r');
// }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
// serialization of message to be sent
JsonObject myMeshViews::_createJsonobject() {
  const int capacity = JSON_OBJECT_SIZE(10);
  StaticJsonDocument<capacity> doc;

  JsonObject msg = doc.to<JsonObject>();
  return msg;
}

void myMeshViews::_broadcastMsg(JsonObject msg) {
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
  char _cNodeName[4];
  itoa(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].iNodeName, _cNodeName, 10);
  msg["senderNodeName"] = _cNodeName;
  msg["senderAPIP"] = (ControlerBoxes[MY_INDEX_IN_CB_ARRAY].APIP).toString();
  msg["senderStIP"] = (ControlerBoxes[MY_INDEX_IN_CB_ARRAY].stationIP).toString();
  int size_buff = 254;
  char output[size_buff];
  serializeJson(msg, output, size_buff);
  String str;
  str = output;
  laserControllerMesh.sendBroadcast(str);
  Serial.print("myMeshViews:_broadcastMsg(...) done. Broadcasted message: ");Serial.println(str);
}
