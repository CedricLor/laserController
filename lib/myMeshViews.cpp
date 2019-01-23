/*
  myMeshViews.cpp - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.
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
  ControlerBoxes[0].updateThisBoxProperties();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Views (or messages)
void myMeshViews::manualSwitchMsg(const short targetOnOffState) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"u";"ts":"0"}
  JsonObject& msg = _createJsonobject('u');
  msg["ts"] = targetOnOffState;
  _broadcastMsg(msg);
}

void myMeshViews::inclusionIRMsg(const short targetIrState) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"i";"ts":"0"}
  JsonObject& msg = _createJsonobject('i');
  msg["ts"] = targetIrState;
  _broadcastMsg(msg);
}

void myMeshViews::blinkingIntervalMsg(const unsigned long targetBlinkingInterval) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"b";"ti":"5000"}
  JsonObject& msg = _createJsonobject('b');
  msg["ti"] = targetBlinkingInterval;
  _broadcastMsg(msg);
}

void myMeshViews::changeMasterBoxMsg(const int newMasterNodeId, const char* newReaction) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"m";"ms":"201";"react":"syn"}
  JsonObject& msg = _createJsonobject('m');
  msg["ms"] = newMasterNodeId;
  msg["react"] = newReaction;
  _broadcastMsg(msg);
}

void myMeshViews::statusMsg(const char* myStatus) {
  // expected JSON string: {"senderNodeName":"201";"senderAPIP":"...";"senderStationIP":"...";"action":"s";"senderStatus":"on"}
  JsonObject& msg = _createJsonobject('s');
  msg["senderStatus"] = myStatus;
  _broadcastMsg(msg);
}

void myMeshViews::pinPairingMsg(const short sTargetPairingType) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"p";"pt":"0"}
  JsonObject& msg = _createJsonobject('p');
  msg["senderStatus"] = sTargetPairingType;
  _broadcastMsg(msg);
}

void myMeshViews::dataRequestMsg() {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"d"}
  JsonObject& msg = _createJsonobject('d');
  _broadcastMsg(msg);
}

void myMeshViews::dataRequestResponse() {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStationIP":"...";"action":"r";"response":{A DETERMINER}}
  JsonObject& msg = _createJsonobject('r');
  // msg["senderStatus"] = myStatus;
  _broadcastMsg(msg);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
JsonObject& myMeshViews::_createJsonobject(const char action) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();

  msg["senderNodeName"] = _nodeNameBuilder();
  msg["senderAPIP"] = (ControlerBoxes[0].APIP).toString();
  msg["senderStationIP"] = (ControlerBoxes[0].stationIP).toString();
  msg["action"] = action;
  return msg;
}

void myMeshViews::_broadcastMsg(JsonObject& msg) {
  String str;
  msg.printTo(str);
  laserControllerMesh.sendBroadcast(str);
  Serial.print("MESH: _broadcastMsg(...) done. Broadcasted message: ");Serial.println(str);
}

char myMeshViews::_nodeNameBuf[4];

char* myMeshViews::_nodeNameBuilder() {
  String _sNodeName = String(I_NODE_NAME);
  _sNodeName.toCharArray(_nodeNameBuf, 4);
  return _nodeNameBuf;
}
