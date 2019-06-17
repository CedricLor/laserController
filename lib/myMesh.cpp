/*
  myMesh.cpp - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.
*/

/* Relies on the existence of:
- ControlerBox
*/

#include "Arduino.h"
#include "myMesh.h"

// #include "myMeshController.cpp"

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

  ControlerBoxes[0].updateThisBoxProperties();

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
  ControlerBoxes[0].updateThisBoxProperties();
  _decodeRequest(from, msg);
}

void myMesh::newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  ControlerBoxes[0].updateThisBoxProperties();
}

void myMesh::changedConnectionCallback() {
  Serial.printf("Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
  ControlerBoxes[0].updateThisBoxProperties();
}

void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(),offset);
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
  StaticJsonDocument<capacity> doc;
  Serial.print("myMesh::_decodeRequest(...): jsonDocument created\n");
  deserializeJson(doc, msg.c_str());
  Serial.print("myMesh::_decodeRequest(...): message msg deserialized into JsonDocument doc\n");
  const short iSenderNodeName = doc["senderNodeName"];
  Serial.printf("myMesh::_decodeRequest(...) %u alloted from doc[\"senderNodeName\"] to iSenderNodeName \n", iSenderNodeName);

  Serial.println(iMasterNodeName);
  Serial.println(iSenderNodeName);
  Serial.println(!(iSenderNodeName == iMasterNodeName));
  Serial.println(iInterfaceNodeName);
  Serial.println(!(iSenderNodeName == iInterfaceNodeName));
  Serial.println("(!(iSenderNodeName == iMasterNodeName)&&!(iSenderNodeName == iInterfaceNodeName))");
  Serial.println((!(iSenderNodeName == iMasterNodeName)&&!(iSenderNodeName == iInterfaceNodeName)));
  // Is the message addressed to me?
  if (!(iSenderNodeName == iMasterNodeName)&&!(iSenderNodeName == iInterfaceNodeName)) {   // do not react to broadcast message if message not sent by relevant sender
    return;
  }

  // If the message is addressed to me, act depending on the sender status
  // myMeshController myMeshController(doc);
  short int _i;
  _i = doc["senderStatus"];
  ControlerBox::valMesh = _i;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
char* myMesh::_apSsidBuilder(char _apSsidBuf[8]) {
  strcat(_apSsidBuf, _PREFIX_AP_SSID);
  char _nodeName[4];
  itoa(I_NODE_NAME, _nodeName, 10);
  strcat(_apSsidBuf, _nodeName);
  return _apSsidBuf;
}

short myMesh::_jsonToInt(JsonDocument root, String rootKey) {
  short iValue = 0;
  const char* sValue = root[rootKey];
  iValue = atoi(sValue);
  return iValue;
}
