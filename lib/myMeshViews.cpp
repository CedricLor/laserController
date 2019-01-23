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
   Upon instantiation, the constructor:
   1. updates the properties of the representation of this box in the array of ControlerBoxes (APIP, StationIP, NodeName);
   2. calls _createMeshMessage to create a structured String;
   3. passes this String to the sendBroadcast method of painlessMesh.
   painlessMesh takes care of the broadcast.
   Messages from controller boxes are not adressed to any other box in particular;
   all the other boxes receive the message and react accordingly.
   To modify the code all through the project, look for "myMeshViews myMeshViews(".
   Currently in use in (i) myMesh class and (ii) pirController class.
*/

myMeshViews::myMeshViews(const char* state)
{
  Serial.printf("myMeshViews::myMeshViews(const char* state): starting with state = %s\n", state);
  ControlerBoxes[I_NODE_NAME - I_NODE_NUMBER_PREFIX].updateProperties();
  //String str = _createStatusMsg(state);
  //Serial.print("myMeshViews::myMeshViews(): about to call laserControllerMesh.sendBroadcast(str) with str = ");Serial.println(str);
  //laserControllerMesh.sendBroadcast(str);   // MESH SENDER
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String _createStatusMessage(const char* myStatus);
/* _createMeshMessage is the helper function that creates the String to be passed to the sendBroadcast method of painlessMesh.
   It creates a String out of a Json message to be sent over the network.
   NOTE: This is weird. I had understood that painlessMesh was sending Json messages...
*/
// String myMeshViews::_createManualSwitchMsg(const char* myStatus) {
//   JsonObject& msg = _createJsonobject();
//
// }

String myMeshViews::_createStatusMsg(const char* myStatus) {
  JsonObject& msg = _createJsonobject('s');
  msg["senderStatus"] = myStatus;
  return _createReturnString(msg);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
JsonObject& myMeshViews::_createJsonobject(const char action) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();

  msg["senderNodeName"] = _nodeNameBuilder();
  msg["senderAPIP"] = (ControlerBoxes[I_NODE_NAME - I_NODE_NUMBER_PREFIX].APIP).toString();
  msg["senderStationIP"] = (ControlerBoxes[I_NODE_NAME - I_NODE_NUMBER_PREFIX].stationIP).toString();
  msg["action"] = action;
  return msg;
}

String myMeshViews::_createReturnString(JsonObject& msg) {
  String str;
  msg.printTo(str);
  Serial.print("MESH: _createReturnString(...) done. Returning String: ");Serial.println(str);
  return str;
}

char myMeshViews::_nodeNameBuf[4];

char* myMeshViews::_nodeNameBuilder() {
  String _sNodeName = String(I_NODE_NAME);
  _sNodeName.toCharArray(_nodeNameBuf, 4);
  return _nodeNameBuf;
}
