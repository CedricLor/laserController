/*
  myMesh.cpp - Library to handle web server controller related functions.
  Created by Cedric Lor, January 2, 2019.
*/

/* Relies on the existence of:
- ControlerBox
*/

#include "Arduino.h"
#include "myMesh.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH constant /////////////////////////////////////////////////////////////////////////////////////////////////
#define   MESH_PREFIX     "laser_boxes"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

const short myMesh::BOXES_I_PREFIX = 201; // this is the iNodeName of the node in the mesh, that has the lowest iNodeName of the network // NETWORK BY NETWORK
const bool myMesh::B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES FOR REACTION TO NETWORK REQUESTS
///////////////////////////////
// The following variable, at start up, corresponds to the default reaction of this box, when it receives the status of its parent box.
// It is a number from 0 to 3. This number corresponds to the index in the following array (const bool B_SLAVE_ON_OFF_REACTIONS[4][2])
// short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;       // saves the index in the B_SLAVE_ON_OFF_REACTIONS bool array of the choosen reaction to the master states
// const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
// HIGH, LOW = reaction if master is on = HIGH; reaction if master is off = LOW;  // Synchronous (index 0): When the master box is on, turn me on AND when the master box is off, turn me off
// LOW, HIGH = reaction if master is on = LOW; reaction if master is off = HIGH;  // Opposed  (index 1): When the master box is on, turn me off AND when the master box is off, turn me on
// HIGH, HIGH = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always off  (index 2): When the master box is on, turn me off AND when the master box is off, turn me off
// LOW, LOW = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always on  (index 3): When the master box is on, turn me off AND when the master box is off, turn me off
// !!!!!! IMPORTANT: If changing the structure of B_SLAVE_ON_OFF_REACTIONS above, update slaveReaction
// in class myWebServerViews and slaveReactionHtml in global.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const char myMesh::PREFIX_AP_SSID[5] = "box_";
char myMesh::myApSsidBuf[8];

myMesh::myMesh()
{
}

void myMesh::meshSetup() {
  laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP |/*MESH_STATUS |*/ CONNECTION |/* SYNC |*/ COMMUNICATION /* | GENERAL | MSG_TYPES | REMOTE */);

  laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );

  //laserControllerMesh.stationManual(STATION_SSID, STATION_PASSWORD, STATION_PORT, station_ip);
  laserControllerMesh.setHostname(apSsidBuilder(I_NODE_NAME, myApSsidBuf));
  if (MESH_ROOT == true) {
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    laserControllerMesh.setRoot(true);
    // This and all other mesh should ideally now the mesh contains a root
    laserControllerMesh.setContainsRoot(true);
  }

  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);

  laserControllerMesh.onReceive(&receivedCallback);
  laserControllerMesh.onNewConnection(&newConnectionCallback);
  laserControllerMesh.onChangedConnections(&changedConnectionCallback);
  laserControllerMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
}

void myMesh::broadcastStatusOverMesh(const char* state) {
  Serial.printf("MESH: broadcastStatusOverMesh(const char* state): starting with state = %s\n", state);
  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);
  String str = createMeshMessage(state);
  Serial.print("MESH: broadcastStatusOverMesh(): about to call mesh.sendBroadcast(str) with str = ");Serial.println(str);
  laserControllerMesh.sendBroadcast(str);   // MESH SENDER
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP: Mesh Network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// void sendMessage() {
  // broadcastStatusOverMesh("on");
  /*
  JsonObject& msg = createMeshJsonMessage("on");
  String str = createMeshStrMessage(msg);
  if (logServerId == 0) // If we don't know the logServer yet
    mesh.sendBroadcast(str);
  else
    mesh.sendSingle(logServerId, str);

  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast(msg);
  Serial.printf("Sending message: %s\n", msg.c_str());
  mesh.sendSingle(destServerId, msg);
  */
  // taskSendMessage.setInterval(TASK_SECOND * 1);
// }

// Needed for painless library
void myMesh::receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("MESH CALLBACK: receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  meshController(from, msg);
}

void myMesh::newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);
  broadcastStatusOverMesh("na");
}

void myMesh::changedConnectionCallback() {
  Serial.printf("Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);
  broadcastStatusOverMesh("na");
}

void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(),offset);
}

void myMesh::delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}


void myMesh::meshController(uint32_t senderNodeId, String &msg) {
  // React depending on wether the remote is myMaster and if so, on its on or off status

  Serial.printf("MESH CONTROLLER: meshController(uint32_t senderNodeId, String &msg) starting with senderNodeId == %u and &msg == %s \n", senderNodeId, msg.c_str());
  StaticJsonBuffer<250> jsonBuffer;
  Serial.print("MESH CONTROLLER: meshController(...): jsonBuffer created\n");
  JsonObject& root = jsonBuffer.parseObject(msg.c_str());
  Serial.print("MESH CONTROLLER: meshController(...): jsonBuffer parsed into JsonObject& root\n");

  const short iSenderNodeName = jsonToInt(root, "senderNodeName");
  Serial.printf("MESH CONTROLLER: meshController(...) %u alloted from root[\"senderNodeName\"] to iSenderNodeName \n", iSenderNodeName);

  const char* cSenderStatus = root["senderStatus"];
  Serial.printf("MESH CONTROLLER: meshController(...) %s alloted from root[\"senderStatus\"] to senderStatus \n", cSenderStatus);

  // Is the message addressed to me?
  if (!(iSenderNodeName == iMasterNodeName)) {                 // do not react to broadcast message if message not sent by relevant sender
    return;
  }

  // If the message is addressed to me, act depending on the sender status
  autoSwitchAllRelaysMeshWrapper(cSenderStatus, iSenderNodeName);
}

void myMesh::autoSwitchAllRelaysMeshWrapper(const char* senderStatus, const short iSenderNodeName) {
  /*
      Explanation of index numbers in the array of boolean arrays B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0 or 1]:
      const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
      - First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
        It is set via the iSlaveOnOffReaction variable. It is itself set either:
        - at startup and equal to the global constant I_DEFAULT_SLAVE_ON_OFF_REACTION (in the global variables definition);
        - via the changeSlaveReaction sub (in case the user has decided to change it via a web control).
      - Second index number is the reaction to the on state of the master box if 0, to its off state if 1.
  */
  Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(senderStatus, iSenderNodeName) starting.\niSenderNodeName = %u\n. senderStatus = %s.\n", iSenderNodeName, senderStatus);
  const char* myFutureState = B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0] == LOW ? "on" : "off";
  if (strstr(senderStatus, "on")  > 0) {                              // if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(...): Turning myself to %s.\n", myFutureState);
    LaserPinsArray::autoSwitchAllRelays(B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0]);
  } else if (strstr(senderStatus, "off")  > 0) {
    Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(...): Turning myself to %s.\n", myFutureState);
    LaserPinsArray::autoSwitchAllRelays(B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1]);
  }
  Serial.print("MESH: autoSwitchAllRelaysMeshWrapper(...): done\n");
}


String myMesh::createMeshMessage(const char* myStatus) {
  Serial.printf("MESH: createMeshMessage(const char* myStatus) starting with myStatus = %s\n", myStatus);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();

  msg["senderNodeName"] = nodeNameBuilder(I_NODE_NAME, nodeNameBuf);
  msg["senderAPIP"] = (ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].APIP).toString();
  msg["senderStationIP"] = (ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].stationIP).toString();
  msg["senderStatus"] = myStatus;

  String str;
  msg.printTo(str);
  Serial.print("MESH: CreateMeshJsonMessage(...) done. Returning String: ");Serial.println(str);
  return str;
}

char* myMesh::apSsidBuilder(const short _I_NODE_NAME, char _apSsidBuf[8]) {
  strcat(_apSsidBuf, PREFIX_AP_SSID);
  char _nodeName[4];
  itoa(I_NODE_NAME, _nodeName, 10);
  strcat(_apSsidBuf, _nodeName);
  return _apSsidBuf;
}

char myMesh::nodeNameBuf[4];
char* myMesh::nodeNameBuilder(const short _I_NODE_NAME, char _nodeNameBuf[4]) {
  String _sNodeName = String(_I_NODE_NAME);
  _sNodeName.toCharArray(_nodeNameBuf, 4);
  return _nodeNameBuf;
}



short myMesh::jsonToInt(JsonObject& root, String rootKey) {
  short iValue;
  const char* sValue = root[rootKey];
  iValue = atoi(sValue);
  return iValue;
}
