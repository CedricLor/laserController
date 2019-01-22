/*
  myMeshViews.cpp - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.
*/

#include "Arduino.h"
#include "myMeshViews.h"


const short myMeshViews::BOXES_I_PREFIX = 201; // this is the iNodeName of the node in the mesh, that has the lowest iNodeName of the network // NETWORK BY NETWORK

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Message Sender
// void broadcastStatusOverMesh(const char* state)
/* broadcastStatusOverMesh is the main mesh message sender module.
   After updating the properties of this box (APIP, StationIP, NodeName),
   it calls _createMeshMessage to create a String.
   Such String is then broadcasted over the mesh via the sendBroadcast function of painlessMesh
*/

myMeshViews::myMeshViews(const char* state)
{
  Serial.printf("MESH: broadcastStatusOverMesh(const char* state): starting with state = %s\n", state);
  ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].updateProperties();
  String str = _createMeshMessage(state);
  Serial.print("MESH: broadcastStatusOverMesh(): about to call mesh.sendBroadcast(str) with str = ");Serial.println(str);
  laserControllerMesh.sendBroadcast(str);   // MESH SENDER
}

// String _createMeshMessage(const char* myStatus);
/* _createMeshMessage is a helper function for broadcastStatusOverMesh.
   It creates a string out of a Json message to be sent over the network.
   NOTE: This is weird. I had understood that painlessMesh was sending Json messages...
*/
String myMeshViews::_createMeshMessage(const char* myStatus) {
  Serial.printf("MESH: _createMeshMessage(const char* myStatus) starting with myStatus = %s\n", myStatus);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();

  msg["senderNodeName"] = _nodeNameBuilder();
  msg["senderAPIP"] = (ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].APIP).toString();
  msg["senderStationIP"] = (ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].stationIP).toString();
  msg["senderStatus"] = myStatus;

  String str;
  msg.printTo(str);
  Serial.print("MESH: CreateMeshJsonMessage(...) done. Returning String: ");Serial.println(str);
  return str;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions

char myMeshViews::_nodeNameBuf[4];

char* myMeshViews::_nodeNameBuilder() {
  String _sNodeName = String(I_NODE_NAME);
  _sNodeName.toCharArray(_nodeNameBuf, 4);
  return _nodeNameBuf;
}
