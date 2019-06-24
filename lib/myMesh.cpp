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
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
  |  |  |  |--boxState.cpp
  |  |  |  |  |--boxState.h
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
    laserControllerMesh.stationManual(STATION_SSID, STATION_PASSWORD, MESH_PORT, STATION_IP);
    //laserControllerMesh.stationManual(STATION_SSID, STATION_PASSWORD);
  }

  laserControllerMesh.setHostname(_apSsidBuilder(_myApSsidBuf));
  if (MESH_ROOT == true) {
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    laserControllerMesh.setRoot(true);
  }
  // This and all other mesh should ideally know that the mesh contains a root
  laserControllerMesh.setContainsRoot(true);

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
  Serial.printf("MESH CALLBACK: receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
  _decodeRequest(from, msg);
}

void myMesh::newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
}

void myMesh::changedConnectionCallback() {
  Serial.printf("Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].updateThisBoxProperties();
}

void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(), offset);
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

  const short __iSenderNodeName = doc["senderNodeName"];
  Serial.printf("myMesh::_decodeRequest(...) %u alloted from doc[\"senderNodeName\"] to __iSenderNodeName \n", __iSenderNodeName);

  // auto __APIP = doc["APIP"].as<const char*>();
  // Serial.print("myMesh::_decodeRequest(...): __APIP = ");Serial.println(__APIP);
  // auto __StationIP = doc["senderStIP"].as<const char*>();
  // Serial.print("myMesh::_decodeRequest(...): __StationIP = ");Serial.println(__StationIP);

  // update the ControlerBoxes[] array with the values received from the other box
  // if the sender box is not the interface
  if (!(__iSenderNodeName == iInterfaceNodeName)) {
    ControlerBox::updateOtherBoxProperties(senderNodeId, doc);
  }

  // Serial.println(iMasterNodeName);
  // Serial.println(__iSenderNodeName);
  // Serial.println(!(__iSenderNodeName == iMasterNodeName));
  // Serial.println(iInterfaceNodeName);
  // Serial.println(!(__iSenderNodeName == iInterfaceNodeName));
  // Serial.println("(!(__iSenderNodeName == iMasterNodeName)&&!(__iSenderNodeName == iInterfaceNodeName))");
  // Serial.println((!(__iSenderNodeName == iMasterNodeName)&&!(__iSenderNodeName == iInterfaceNodeName)));

  // If the message is not addressed to me, discard it
  if (!(__iSenderNodeName == iMasterNodeName)&&!(__iSenderNodeName == iInterfaceNodeName)) {   // do not react to broadcast message if message not sent by relevant sender
    return;
  }

  // If the message is addressed to me, act depending on the sender status
  // myMeshController myMeshController(doc);
  // short int _i;
  // _i = doc["senderStatus"];
  // ControlerBox::valMesh = _i;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
char* myMesh::_apSsidBuilder(char _apSsidBuf[8]) {
  // strcat(destination, source); source is appended destination, replacing the null character of destination by the first character of source
  strcat(_apSsidBuf, _PREFIX_AP_SSID);
  // after the preceeding line, _apSsidBuf shall be equal to "box_"
  char _cNodeName[4];
  itoa(I_NODE_NAME, _cNodeName, 10);
  // itoa convert I_NODE_NAME (201, 202, etc.) from into to char array, the resulting char array being stored into
  strcat(_apSsidBuf, _cNodeName);
  // strcat appends 201, 202, etc. to box_
  // _apSsidBuf shall equal to something like box_201, box_202, etc.
  return _apSsidBuf;
}

short myMesh::_jsonToInt(JsonDocument root, String rootKey) {
  short iValue = 0;
  const char* sValue = root[rootKey];
  iValue = atoi(sValue);
  return iValue;
}
