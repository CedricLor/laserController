/*
  myMesh.cpp - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.

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
#include "myMesh.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH constant /////////////////////////////////////////////////////////////////////////////////////////////////
#define   MESH_PREFIX     "laser_boxes"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

const char* myMesh::STATION_SSID = ssid;
const char* myMesh::STATION_PASSWORD = pass;
const uint8_t* myMesh::STATION_IP = station_ip;

const char myMesh::_PREFIX_AP_SSID[5] = "box_";
char myMesh::_myApSsidBuf[8];


myMesh::myMesh()
{
}

void myMesh::meshSetup() {
  if ( MY_DEBUG == true ) {
    // laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP |/*MESH_STATUS |*/ CONNECTION |/* SYNC |*/ COMMUNICATION /* | GENERAL | MSG_TYPES | REMOTE */);
    laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);
  }

  laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL );

  if ((IS_INTERFACE == true) && (IS_STATION_MANUAL == true)) {
    // laserControllerMesh.stationManual(STATION_SSID, STATION_PASSWORD, MESH_PORT, STATION_IP);
    laserControllerMesh.stationManual(STATION_SSID, STATION_PASSWORD);
  }

  laserControllerMesh.setHostname(_apSsidBuilder(_myApSsidBuf));
  if (MESH_ROOT == true) {
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    laserControllerMesh.setRoot(true);
  }
  // This and all other mesh should ideally know that the mesh contains a root
  laserControllerMesh.setContainsRoot(true);

  // Serial.println("myMesh::meshSetup(): About to call updateThisBoxProperties:");
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();

  laserControllerMesh.onReceive(&receivedCallback);
  laserControllerMesh.onNewConnection(&newConnectionCallback);
  laserControllerMesh.onChangedConnections(&changedConnectionCallback);
  laserControllerMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Network Callbacks
void myMesh::receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("myMesh::receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
  _decodeRequest(from, msg);
}

void myMesh::newConnectionCallback(uint32_t nodeId) {
  Serial.printf("myMesh::newConnectionCallback(): New Connection, nodeId = %u\n", nodeId);
  // Serial.printf("myMesh::newConnectionCallback(): laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.println("++++++++++++++++++++++++ NEW CONNECTION +++++++++++++++++++++++++++");
  // _updateConnectedBoxCount();
  if (IS_INTERFACE == false) {
    // following line commented out; a call to updateThisBoxProperties will be done in myMeshViews, before broadcasting
    // ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties(); // does not update the boxState related fields (boxActiveState, boxActiveStateHasChanged and uiBoxActiveStateStartTime)
    myMeshViews __myMeshViews;
    __myMeshViews.statusMsg(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState);
  } else {
    Serial.println("myMesh::newConnectionCallback(): I am the interface. About to call updateThisBoxProperties()");
    ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties(); // does not update the boxState related fields (boxActiveState, boxActiveStateHasBeenSignaled and uiBoxActiveStateStartTime)
  }
}

void myMesh::changedConnectionCallback() {
  Serial.printf("myMesh::changedConnectionCallback(): Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.println("--------------------- CHANGED CONNECTION --------------------------");
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
  // _updateConnectedBoxCount();
}

void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("myMesh::nodeTimeAdjustedCallback(): Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(), offset);
}

void myMesh::delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Decode Request
/* Mesh controller is a controller in the same meaning as in Model-View-Controller pattern in Ruby.
    It is called from the receivedCallback callback.
    It reads the JSON string received via the Mesh communication protocol.
    It determines whether the messages is sent by my Master Node or by the Interface Node.
    If so, it calls the meshController
*/
void myMesh::_decodeRequest(uint32_t senderNodeId, String &msg) {
  Serial.printf("myMesh::_decodeRequest(uint32_t senderNodeId, String &msg) starting with senderNodeId == %u and &msg == %s \n", senderNodeId, msg.c_str());
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<capacity> doc;
  Serial.print("myMesh::_decodeRequest(...): jsonDocument created\n");

  // deserialize the message msg received from the mesh into the StaticJsonDocument doc
  DeserializationError err = deserializeJson(doc, msg.c_str());
  Serial.print("myMesh::_decodeRequest(...): message msg deserialized into JsonDocument doc\n");
  Serial.print("myMesh::_decodeRequest(...): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");

  // pass the deserialized doc and the senderNodeId to the controller
  myMeshController myMeshController(doc, senderNodeId);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
char* myMesh::_apSsidBuilder(char _apSsidBuf[8]) {
  // strcat(destination, source); source is appended destination, replacing the null character of destination by the first character of source
  strcat(_apSsidBuf, _PREFIX_AP_SSID);
  // after the preceeding line, _apSsidBuf shall be equal to "box_"
  char _cNodeName[4];
  itoa(B_NODE_NAME, _cNodeName, 10);
  // itoa convert B_NODE_NAME (201, 202, etc.) from into to char array, the resulting char array being stored into
  strcat(_apSsidBuf, _cNodeName);
  // strcat appends 201, 202, etc. to box_
  // _apSsidBuf shall equal to something like box_201, box_202, etc.
  return _apSsidBuf;
}

// short myMesh::_jsonToInt(JsonDocument root, String rootKey) {
//   short iValue = 0;
//   const char* sValue = root[rootKey];
//   iValue = atoi(sValue);
//   return iValue;
// }

// void myMesh::_updateConnectedBoxCount() {
//   StaticJsonDocument<256> doc;
//   DeserializationError err = deserializeJson(doc, laserControllerMesh.subConnectionJson());
//   if (err) {
//     Serial.print(F("myMesh::_updateConnectedBoxCount(): deserializeJson() failed with code "));
//     Serial.println(err.c_str());
//   }
//   const char* _containsSub = obj["subs"];
//   if (_containsSub != nullptr) {
//     ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateConnectedBoxCount(2); // for the time being, a number of 2 means "2 or more"
//   } else {
//     ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateConnectedBoxCount(1); // If subs does not exist, I am alone in the mesh
//   }
// }
