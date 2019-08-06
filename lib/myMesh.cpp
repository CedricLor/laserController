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

/*  
    Variables for use case "Interface on soft AP"
    The following variables are used when providing the interface on the soft AP,
    instead of the having the interface connecting to an external gateway (as 
    recommended by the developpers of painlessMesh).
 */
const char* myMesh::_soft_ap_ssid     = "ESP32-Access-Point";
const char* myMesh::_soft_ap_password = "123456789";
IPAddress myMesh::_soft_ap_my_ip            = IPAddress(192, 168, 5, 1);
IPAddress myMesh::_soft_ap_me_as_gateway_ip = IPAddress(192, 168, 5, 1);
IPAddress myMesh::_soft_ap_netmask          = IPAddress(255, 255, 255, 0);
/*  
    Variables for use case: stationManual / "Interface on STATION"
    The following variables are used when providing the interface on the soft AP,
    instead of the having the interface connecting to an external gateway. 
    This is the use case recommended by the developpers of painlessMesh.
    But this only works with my own refactored version of painlessMesh.
 */
IPAddress myMesh::_fixed_ip                 = IPAddress(192, 168, 43, 50);
IPAddress myMesh::_fixed_netmask            = IPAddress(255, 255, 255, 0);



myMesh::myMesh()
{
}



void myMesh::meshSetup() {
  if ( MY_DEBUG == true ) {
    // laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP |/*MESH_STATUS |*/ CONNECTION |/* SYNC |*/ COMMUNICATION /* | GENERAL | MSG_TYPES | REMOTE */);
    laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION /* | GENERAL */ | MSG_TYPES | REMOTE );
  }

  _initAndConfigureMesh();

  _tPrintMeshTopo.enable();
  _tSaveNodeListAndMap.restart();

  // Serial.println("myMesh::meshSetup(): About to call updateThisBoxProperties:");
  ControlerBoxes[gui16MyIndexInCBArray].updateThisBoxProperties();

  _setupMdns();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup Helpers

void myMesh::_initAndConfigureMesh() {
  // 1. Init mesh
  _initMesh();

  // 2. Init station manual
  _initStationManual();

  // 3. Root the mesh
  _rootTheMesh();

  // 4. Set the callbacks
  _setMeshCallbacks();
}

/* _initMesh()
   Either init the mesh with interface/bridge on AP or interface on STATION
*/
void myMesh::_initMesh() {
  if (isInterface && !(isRoot)) {
    // Special init for case of physically mobile interface (interface on AP)
    _interfaceOnAPInit();
  } else {
    // All the other mesh nodes  (whether root or non-root, interface on STATION,
    // ControlerBoxes, relays, IR, etc. ) share the same init function. (no ad hoc 
    // config of the Soft AP is required).
    laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, ui8WifiChannel);
  }
}

/* _initStationManual()
  If the mesh is interface and isRoot, the STATION shall try to connect to an external 
  network and the web users will have access to the STATION through their browser. 
  The other mesh nodes will connect on the AP. (This is the recommended use case
  by the devs of painlessMesh.)
*/
void myMesh::_initStationManual() {
  if (isInterface && isRoot) {
    laserControllerMesh.stationManual(ssid, pass, ui16GatewayPort, gatewayIP, _fixed_ip, _fixed_netmask);
    // laserControllerMesh.stationManual(ssid, pass);
  }
}

/* _rootTheMesh()
  One root node per mesh is recommanded. Once a node has been set as root,
  it and all other mesh member should know that the mesh contains a root.
*/
void myMesh::_rootTheMesh() {
  if (isRoot) {
    laserControllerMesh.setRoot(true);
  }
  laserControllerMesh.setContainsRoot(true);
}

/* _interfaceOnAPInit()
   If the interface is on the AP, web users have access to it on the AP. They have to
   connect just like on any ESP32 AP.  Accordingly, the AP shall be configured as any
   AP on an ESP32.
   This node will use its STATION to scanning for other mesh nodes and access to
   the mesh (accordingly, scanStationManual shall not be called; scanning shall 
   remain under the full control of painlessMesh).
*/
void myMesh::_interfaceOnAPInit() {
  // 1. init the mesh in station only
  laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_STA, ui8WifiChannel);
  // 2. configure the soft AP
  if(!WiFi.softAPConfig(_soft_ap_my_ip, _soft_ap_me_as_gateway_ip, _soft_ap_netmask)){
    Serial.println("\nAP Config Failed\n");
  }
  if (WiFi.softAP(_soft_ap_ssid, _soft_ap_password, 
                  ui8WifiChannel, 0 /* int ssid_hidden*/, 
                  10 /*int max_connection */)){
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("Network " + String(_soft_ap_ssid) + " running");
    Serial.println("AP IP address:" + String(myIP) +"");
  } else {
    Serial.println("\nStarting AP failed.\n");
  }
}


void myMesh::_setMeshCallbacks() {
  laserControllerMesh.onReceive(&receivedCallback);
  laserControllerMesh.onNewConnection(&newConnectionCallback);
  laserControllerMesh.onChangedConnections(&changedConnectionCallback);
  // laserControllerMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  // laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);
  laserControllerMesh.onDroppedConnection(&droppedConnectionCallback);
}

void myMesh::_setupMdns() {
  snprintf(gcHostnamePrefix, 10, "%s%u", gcHostnamePrefix, (uint32_t)gui16NodeName);
  // laserControllerMesh.setHostname(gcHostnamePrefix);
  // begin mDNS responder: argument is the name to broadcast. In this example
  // "ESP32_200". It will be broadcasted as ESP32_200.local.
  if (!MDNS.begin(gcHostnamePrefix)) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("arduino", "tcp", 3232);
}













//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Network Callbacks
void myMesh::receivedCallback(uint32_t from, String &msg ) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  }

  _tcbDecodeRequest(from, msg);

  Serial.println(F("myMesh::receivedCallback(): ending"));
}












void myMesh::_tcbSendStatusOnNewConnection() {
  Serial.println(F("myMesh::_tcbSendStatusOnNewConnection(): starting"));
  if (IamAlone()) {
    return; 
    Serial.println(F("myMesh::_tcbSendStatusOnNewConnection(): I am alone. ending."));
  }
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();
  Serial.println(F("myMesh::_tcbSendStatusOnNewConnection(): sent status message. ending."));
}

void myMesh::newConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::newConnectionCallback(): New Connection, nodeId = %u\n", nodeId);
    _printNodeListAndTopology();
    Serial.println(F("++++++++++++++++++++++++ NEW CONNECTION +++++++++++++++++++++++++++"));
  }
}








void myMesh::_tcbSendNotifOnDroppedConnection(uint32_t nodeId) {
  Serial.printf("myMesh::_tcbSendNotifOnDroppedConnection: starting. Dropper nodeID:%u \n", nodeId);
  // 1. delete the ControlerBox
  uint16_t _ui16droppedIndex = _deleteBoxFromCBArray(nodeId);
  if (_ui16droppedIndex == 0) { return; }

  // 2. send a dropped box notification
  if (IamAlone()) { 
    if (MY_DEEP_DG_MESH) Serial.println(F("myMesh::_tcbSendNotifOnDroppedConnection(): I am alone. ending."));
    return; 
  }
  myMeshViews __myMeshViews;
  __myMeshViews.droppedNodeNotif(_ui16droppedIndex);
  Serial.println(F("myMesh::_tcbSendNotifOnDroppedConnection(): sent message. ending."));
};


void myMesh::droppedConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection for nodeId: %u\n", nodeId);
    _printNodeListAndTopology();
    Serial.println(F("--------------------- DROPPED CONNECTION --------------------------"));
  }

  // 1. disable Task _tChangedConnection (enabled in changedConnectionCallback)
  _tChangedConnection.disable();
  _tChangedConnection.setInterval(0);

  // 2. If nodeId < UINT16_MAX, this is a false signal, just return.
  if (nodeId < UINT16_MAX) {
    if (MY_DEEP_DG_MESH) Serial.printf("myMesh::droppedConnectionCallback(): nodeId == %u. False signal. About to return.\n", nodeId);
    _tChangedConnection.setCallback(NULL);
    return;
  }

  // 3. Else, the previously detected changeConnection was indeed a dropper.
  // Accordingly, reset Task _tChangedConnection to send a dropped notification
  // to the other mesh nodes to which this node is connected.
  if (MY_DEEP_DG_MESH) Serial.printf("myMesh::droppedConnectionCallback(): nodeId == %u. Setting _tChangedConnection to notify the mesh and delete the box in ControlerBox[].\n", nodeId);
  _tChangedConnection.setCallback( [nodeId]() { _tcbSendNotifOnDroppedConnection(nodeId); } );

  // 4. Restart the Task _tChangedConnection, for execution without delay
  if (MY_DEEP_DG_MESH) Serial.println(F("myMesh::droppedConnectionCallback(): restarting _tChangedConnection."));
  _tChangedConnection.restart();

  if (MY_DG_MESH) {
    Serial.println(F("myMesh::droppedConnectionCallback(): Ending."));
  }
}









/*  
    This Task broadcasts this box's boxState to all the other boxes when there is a
    changedConnection.
    If the onDroppedConnection callback is called immediately after the onChangedConnection
    callback, this Task is reset by the onDroppedConnection callback to send a
    dropped box notification instead.
*/
Task myMesh::_tChangedConnection(0, 1, NULL, &userScheduler, false, NULL, &_odtcbChangedConnection);

void myMesh::_odtcbChangedConnection() {
    if (MY_DEEP_DG_MESH) Serial.println("--------------------- CHANGED CONNECTION TASK DISABLE --------------------------");
}

void myMesh::changedConnectionCallback() {
  // 1. Enabling the Task _tUpdateCDOnChangedConnections
  _tUpdateCDOnChangedConnections.enable();

  // 2. Printing the mesh topology
  if (MY_DEEP_DG_MESH) {
    Serial.println("myMesh::changedConnectionCallback(): printing the current nodeList (no update):");
    _printNodeListAndTopology();
  }
  Serial.println("--------------------- CHANGED CONNECTION --------------------------");

  // 3. Checking if I am alone
  if (IamAlone()) {
    Serial.printf("myMesh::changedConnectionCallback(): I am alone. Not sending any message.\n");
    return;
  }

  // 4. If I am not alone, trigger the Task _tChangedConnection which will send my status
  // in case a new comer is joining or be interrupted by the droppedConnection callback
  // if the changedConnectionCallback was triggered by a direct dropper for this box.
  Serial.printf("myMesh::changedConnectionCallback(): I am not alone. Sending my status.\n");
  _tChangedConnection.setInterval((2900 + gui16MyIndexInCBArray * 100));
  _tChangedConnection.setCallback(_tcbSendStatusOnNewConnection);
  _tChangedConnection.restartDelayed();

  if (MY_DEEP_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): gui16MyIndexInCBArray: %u\n", gui16MyIndexInCBArray);
    Serial.printf("myMesh::changedConnectionCallback(): task enabled? %i\n", _tChangedConnection.isEnabled());
    Serial.printf("myMesh::changedConnectionCallback(): task interval: %lu\n", _tChangedConnection.getInterval());
    Serial.print("myMesh::changedConnectionCallback(): task iterations: ");Serial.println(_tChangedConnection.getIterations());
    Serial.print("myMesh::changedConnectionCallback(): time until next iteration: ");Serial.println(userScheduler.timeUntilNextIteration(_tChangedConnection));
  }
}






// void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
//   if (MY_DG_MESH) {
//     Serial.printf("myMesh::nodeTimeAdjustedCallback(): Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(), offset);
//   }
// }





// void myMesh::delayReceivedCallback(uint32_t from, int32_t delay) {
//   if (MY_DG_MESH) {
//     Serial.printf("Delay to node %u is %d ms\n", from, delay);
//   }
// }





void myMesh::_tcbDecodeRequest(uint32_t _ui32SenderNodeId, String &_msg) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::_tcbDecodeRequest(): starting. _ui32SenderNodeId == %u; &_msg == %s \n", _ui32SenderNodeId, _msg.c_str());
  }
  constexpr int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<capacity> _doc;
  // Convert the document to an object
  JsonObject _obj = _doc.to<JsonObject>();

  if (MY_DG_MESH) {
    Serial.print("myMesh::_tcbDecodeRequest: empty jsonDocument created\n");
  }

  // deserialize the message _msg received from the mesh into the StaticJsonDocument doc
  DeserializationError _err = deserializeJson(_doc, _msg);
  if (MY_DG_MESH) {
    Serial.print("myMesh::_tcbDecodeRequest: message _msg deserialized into JsonDocument doc\n");
    Serial.print("myMesh::_tcbDecodeRequest: DeserializationError = ");Serial.print(_err.c_str());Serial.print("\n");
  }

  myMeshController myMeshController(_ui32SenderNodeId, _obj);
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
/* IamAlone
   Checks whether the node is alone, upon mesh events or before sending
   messages through the mesh.
*/
bool myMesh::IamAlone() {
  Serial.printf("myMesh::IamAlone(): Starting\n");
  if (
    laserControllerMesh.getNodeList().size() < 2 && 
    0 == *laserControllerMesh.getNodeList().rbegin()
    ) {
    Serial.printf("myMesh::IamAlone(): Yes\n");
    if ((!(isInterface)) && (!(_tIamAloneTimeOut.isEnabled()))) {
      Serial.println("\nmyMesh::IamAlone(): Enabling _tIamAloneTimeOut.\n");
    }
    return true;
  }
  Serial.printf("myMesh::IamAlone(): No\n");
  Serial.println("\nmyMesh::IamAlone(): Disabling _tIamAloneTimeOut.\n");
  _tIamAloneTimeOut.disable();
  return false;
}

/* _tIamAloneTimeOut
   Task enabled when the node detects that it is alone.
   If after 20 seconds, it is still alone, it will restart the mesh.
*/
Task myMesh::_tIamAloneTimeOut(20*TASK_SECOND, 1, &_tcbIamAloneTimeOut, &userScheduler, false);

/* _tcbIamAloneTimeOut()
   Restarts the mesh if the node is no longer connected.
*/
void myMesh::_tcbIamAloneTimeOut() {
  Serial.println("myMesh::_tcbIamAloneTimeOut(): Starting.");
  if (IamAlone()) {
    Serial.println("myMesh::_tcbIamAloneTimeOut(): Restarting the mesh.");
    laserControllerMesh.stop();
    meshSetup();
  }
}






/* _tPrintMeshTopo
   Used for debug purposes.
   Calls _printNodeListAndTopology() every 30 seconds.
*/
Task myMesh::_tPrintMeshTopo(60*TASK_SECOND, TASK_FOREVER, &_printNodeListAndTopology, &userScheduler, false);

/* _printNodeListAndTopology()
   Used for debug purposes.
   Prints the mesh topology as JSON and
   iterates over the nodeList to print their values.
*/
void myMesh::_printNodeListAndTopology() {
  Serial.printf("myMesh::_printNodeListAndTopology(): Mesh topology: %s\n", laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("myMesh::_printNodeListAndTopology(): Node list size: %i\n", laserControllerMesh.getNodeList().size());
  int16_t _i = 0;
  for (int n : laserControllerMesh.getNodeList()) {
    Serial.printf("myMesh::_printNodeListAndTopology(): node [%i] id: %u\n", _i++, n);
  }
  Serial.printf("myMesh::_printNodeListAndTopology(): ending -------\n");
}







/* _tUpdateCDOnDroppedConnections
  In case a connection dropped, we need to update the DB. This is easy for the dropper,
  as it is signaled.
  However, the sub nodes which may be in the ControlerBoxes[] array are not signaled as dropped.
  Each remaining node should know how to delete the subs.
  Upon such events, this Task does the job after 10 seconds.
*/
Task myMesh::_tUpdateCDOnChangedConnections(10*TASK_SECOND, 1, &_tcbUpdateCBOnChangedConnections, &userScheduler, false, &_oetcbUpdateCBOnChangedConnections, &_odtcbUpdateCBOnChangedConnections);


bool myMesh::_oetcbUpdateCBOnChangedConnections() {
  // Disable the Task _tSaveNodeListAndMap (just in case)
  if (MY_DEEP_DG_MESH) Serial.println("\nmyMesh::_oetcbUpdateCBOnChangedConnections(): _tSaveNodeListAndMap disabled.\n");
  _tSaveNodeListAndMap.disable();
  return true;
}


void myMesh::_odtcbUpdateCBOnChangedConnections() {
  // Enable the Task _tSaveNodeListAndMap
  if (MY_DEEP_DG_MESH) Serial.println("\nmyMesh::_odtcbUpdateCBOnChangedConnections(): _tSaveNodeListAndMap restarted.\n");
  _tSaveNodeListAndMap.restart();
}


void myMesh::_tcbUpdateCBOnChangedConnections() {
  if (MY_DG_MESH) Serial.println("\nmyMesh::_tcbUpdateCBOnChangedConnections(): Starting. Time: " + String(millis()));
  // 1. Disable the Task _tSaveNodeListAndMap (just in case)
  _tSaveNodeListAndMap.disable();

  // 2. Create a _newNodeList containing the new mesh layout
  std::list<uint32_t> _newNodeList = laserControllerMesh.getNodeList();
  _newNodeList.remove(0);
  _newNodeList.sort();
  // auto _newListNode = _newNodeList.begin();

  // 3. Set all the values of the _nodeMap to 0
  //    (box for deletion because no longer available).
  // Serial.println("myMesh::_tcbUpdateCBOnChangedConnections(): Before iteration over the map. Time: " + String(millis()));
  for (std::pair<std::uint32_t, uint16_t> _node : _nodeMap) {
    _node.second = 0;
  }
  // Serial.println("myMesh::_tcbUpdateCBOnChangedConnections(): Before iteration over the map. Time: " + String(millis()));

  // 3. Iterate through the _newNodeList containing the new mesh layout.
  //    Look for each box in the _nodeMap. If any is found, 
  //    set its value in the _nodeMap to 1 (box is still here).
  //    The boxes which have not been set to 1 or 2 will remain 
  //    marked as 0 -> for deletion.
  // Serial.println("myMesh::_tcbUpdateCBOnChangedConnections(): Before iteration over the list. Time: " + String(millis()));
  for (uint32_t _newNode : _newNodeList) {
    std::map<uint32_t, uint16_t>::iterator _nodeInMap = _nodeMap.find(_newNode);
    if (_nodeInMap != _nodeMap.end()) {
      _nodeInMap->second = 1;
      continue;
    }
    _nodeMap.emplace(_newNode, 2);
  }
  // Serial.println("myMesh::_tcbUpdateCBOnChangedConnections(): After iteration over the list. Time: " + String(millis()));

  // 4. Delete the boxes marked as 0 from the ControlerBoxes[] array
  _deleteBoxesFromCBArray(_nodeMap);

  Serial.println("myMesh::_tcbUpdateCBOnChangedConnections(): Ending. Time: " + String(millis()));

  // This code was when iterating over the _savedNodeList
  // and comparing it with the _newNodeList
  // for (uint32_t n : _savedNodeList) {
  //   if (*_newListNode == n) {
  //     // the node is still here
  //     _nodeMap.emplace(n, 1);
  //     _newListNode = std::next(_newListNode);
  //     _newNodeList.remove(*(std::prev(_newListNode)));
  //     continue;
  //   }
  //   if (*_newListNode > n) {
  //     // the node has been disconnected
  //     _nodeMap.emplace(n, 0);
  //     continue;
  //   }
  //   while (*_newListNode < n) {
  //     // this is a new node
  //     _nodeMap.emplace(*_newListNode, 2);
  //     _newListNode = std::next(_newListNode);
  //     _newNodeList.remove(*(std::prev(_newListNode)));
  //   }
  // }

  // if (_newNodeList.size() > 0) {
  //   for (uint32_t _newNode : _newNodeList) {
  //     _nodeMap.emplace(_newNode, 2);
  //   }
  // }
}

void myMesh::_deleteBoxesFromCBArray(std::map<uint32_t, uint16_t> &_nodeMap) {
  for (std::pair<std::uint32_t, uint16_t> _node : _nodeMap) {
    if (_node.second == (uint16_t)0) {
      _deleteBoxFromCBArray(_node.first);
    }
  }
}

uint16_t myMesh::_deleteBoxFromCBArray(uint32_t nodeId) {
  // 1. check if the box is listed in my ControlerBoxes[]
  uint16_t _ui16droppedIndex = ControlerBox::findByNodeId(nodeId);

  // 2. if the box index is different than 254, it exists
  if (_ui16droppedIndex == 254) {
    if (MY_DEEP_DG_MESH) Serial.println(F("myMesh::_deleteBoxFromCBArray(): ending (box not found in ControlerBoxes[])."));
    return 0;
  }

  // 3. delete the box from ControlerBox[]
  ControlerBox::deleteBox(_ui16droppedIndex);

  return _ui16droppedIndex;
}







/* _tSaveNodeListAndMap
  This Task is called on various occasions, to keep an 
  up-to-date vision of the mesh layout.
*/
Task myMesh::_tSaveNodeListAndMap(10 * TASK_SECOND, 3, &_tcbSaveNodeListAndMap, &userScheduler, false);

void myMesh::_tcbSaveNodeListAndMap() {
  if (MY_DEEP_DG_MESH) Serial.println("\nmyMesh::_tcbSaveNodeListAndMap(): remaining iterations: " + String(_tSaveNodeListAndMap.getIterations()));
  if (_tUpdateCDOnChangedConnections.isEnabled()) {
    if (MY_DEEP_DG_MESH) Serial.println("myMesh::_tcbSaveNodeListAndMap(): _tUpdateCDOnChangedConnections is enabled. Passing this iteration.");
    _tSaveNodeListAndMap.setIterations(_tSaveNodeListAndMap.getIterations() + 1);
    return;
  }
  _tSaveNodeListAndMap.setInterval(10 * TASK_SECOND + ((_tSaveNodeListAndMap.getRunCounter() - 1) / (_tSaveNodeListAndMap.getIterations() + 1)) * (20 / (_tSaveNodeListAndMap.getIterations() + 1)));
  if (MY_DG_MESH) Serial.println("myMesh::_tcbSaveNodeListAndMap(): _tUpdateCDOnChangedConnections is not enabled. Updating mesh topo list and map.");
  _saveNodeList();
  _saveNodeMap();
}

std::list<uint32_t> myMesh::_savedNodeList = laserControllerMesh.getNodeList();

void myMesh::_saveNodeList() {
  // Serial.println("myMesh::_saveNodeList(): Starting. Time: " + String(millis()));
  _savedNodeList = laserControllerMesh.getNodeList();
  // Serial.println("myMesh::_saveNodeList(): Before remove(0). Time: " + String(millis()));
  _savedNodeList.remove(0);
  // Serial.println("myMesh::_saveNodeList(): Before sort. Time: " + String(millis()));
  _savedNodeList.sort();
  // Serial.println("myMesh::_saveNodeList(): Ending. Time: " + String(millis()));
}

std::map<uint32_t, uint16_t> myMesh::_nodeMap;

void myMesh::_saveNodeMap() {
  // Serial.println("myMesh::_saveNodeMap(): Starting. Time: " + String(millis()));
  _nodeMap.clear();
  // Serial.println("myMesh::_saveNodeMap(): Before iteration. Time: " + String(millis()));
  for (uint32_t _savedNode : _savedNodeList) {
    _nodeMap.emplace(_savedNode, 1);
  }
  // Serial.println("myMesh::_saveNodeMap(): Ending. Time: " + String(millis()));
}


