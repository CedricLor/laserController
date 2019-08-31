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






myMesh::myMesh()
{
}

void myMesh::init() {
  myMeshStarter _myMeshStarter;
  _myMeshStarter.myMeshSetup();

  tPrintMeshTopo.enable();
  tSaveNodeMap.restart();

  laserControllerMesh.onReceive(&receivedCallback);
  laserControllerMesh.onNewConnection(&newConnectionCallback);
  laserControllerMesh.onChangedConnections(&changedConnectionCallback);
  // laserControllerMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  // laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);
  laserControllerMesh.onDroppedConnection(&droppedConnectionCallback);

  myMeshStarter::hasBeenStarted = true;
}






/** myMesh::start()
 *  
 *  This method is the initial mesh starter. Its function is:
 *  1. to call myMesh::init() which effectively setups and starts the mesh;
 *  2. to set the main callback of Task myMeshStarter::tRestart, by passing it
 *     a lambda with the local context. The reason for doing this is that:
 *  (a) myMeshStarter does not have access to myMesh (myMesh includes
 *      myMeshStarter, so myMeshStarter cannot include myMesh);
 *  (b) the painlessmesh callbacks are located in myMesh;
 *  (c) myMesh cannot be included into myMeshController and/or myWebReceiverWS 
 *      where the mesh restart callers would be used/needed;
 *  (d) myMeshStarter contains most of the procs required for a mesh restart;
 *  (e) myMeshStarter can easily get included into (i) myMesh, (ii) myMeshController
 *      and (iii) myWebReceiverWS.
 *  By passing the context in a lambda to set the callback of 
 *  Task myMeshStarter::tRestart, I am giving access to myMeshStarter
 *  to the Task starters located in myMesh. */
void myMesh::start() {
  Serial.println("myMesh::start: starting");  
  // 1. call myMesh::init() to effectively start the mesh
  init();
  Serial.println("myMesh::start: mesh started. Setting restart callback.");
  // 1. set the main callback for myMeshStarter::tRestart by passing it a lambda
  // with the context in the capture initializer
  myMeshStarter::tRestart.setCallback(
    [&](){
      Serial.println("myMeshStarter::tRestart: mainCallback: starting");
      if (myMeshStarter::hasBeenStarted == true) {
        // disable all the tasks depending on network functions
        tChangedConnection.disable();
        tIamAloneTimeOut.disable();
        tPrintMeshTopo.disable();
        tUpdateCBArrayOnChangedConnections.disable();
        tSaveNodeMap.disable();
        myWSSender::tSendWSDataIfChangeStationIp.disable();
        myWSSender::tSendWSDataIfChangeBoxState.disable();

        // end the MDNS server
        MDNS.end();

        // restart the mesh
        laserControllerMesh.stop();
        myMesh::init();

        // reset the node map and the controller boxes array
        _saveNodeMap();
        _tcbUpdateCBOnChangedConnections();

        // enable the Task sending changes in ControlerBoxes states to connected browsers
        if (isInterface) {
          myWSSender::tSendWSDataIfChangeBoxState.enable();
        }
      }
      Serial.println("myMeshStarter::tRestart: mainCallback: ending");
    }
  );
  Serial.println("myMesh::start: mesh started. Restart callback has been set.");
  Serial.println("myMesh::start: ending");  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Network Callbacks
void myMesh::receivedCallback(uint32_t from, String &msg ) {
  if (MY_DG_MESH) {
    Serial.printf("\nmyMesh::receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  }

  /** &init params: trying to pass the static method init to myMeshController 
   *  so that myMeshController can call init from there to restart the mesh.  */
  myMeshController myMeshController(from, msg/*, &init*/);

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
  __myMeshViews._droppedNodeNotif(_ui16droppedIndex);
  Serial.println(F("myMesh::_tcbSendNotifOnDroppedConnection(): sent message. ending."));
};


void myMesh::droppedConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection for nodeId: %u\n", nodeId);
    _printNodeListAndTopology();
    Serial.println(F("--------------------- DROPPED CONNECTION --------------------------"));
  }

  // 1. disable Task tChangedConnection (enabled in changedConnectionCallback)
  tChangedConnection.disable();
  tChangedConnection.setInterval(0);

  // 2. If nodeId < UINT16_MAX, this is a false signal, just return.
  if (nodeId < UINT16_MAX) {
    if (MY_DEEP_DG_MESH) Serial.printf("myMesh::droppedConnectionCallback(): nodeId == %u. False signal. About to return.\n", nodeId);
    tChangedConnection.setCallback(NULL);
    return;
  }

  // 3. Else, the previously detected changeConnection was indeed a dropper.
  // Accordingly, reset Task tChangedConnection to send a dropped notification
  // to the other mesh nodes to which this node is connected.
  if (MY_DEEP_DG_MESH) Serial.printf("myMesh::droppedConnectionCallback(): nodeId == %u. Setting tChangedConnection to notify the mesh and delete the box in ControlerBox[].\n", nodeId);
  tChangedConnection.setCallback( [nodeId]() { _tcbSendNotifOnDroppedConnection(nodeId); } );

  // 4. Restart the Task tChangedConnection, for execution without delay
  if (MY_DEEP_DG_MESH) Serial.println(F("myMesh::droppedConnectionCallback(): restarting tChangedConnection."));
  tChangedConnection.restart();

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
Task myMesh::tChangedConnection(0, 1, NULL, NULL/*&mns::myScheduler*/, false, NULL, &_odtcbChangedConnection);

void myMesh::_odtcbChangedConnection() {
    if (MY_DEEP_DG_MESH) Serial.println("--------------------- CHANGED CONNECTION TASK DISABLE --------------------------");
}

void myMesh::changedConnectionCallback() {
  // 1. Enabling the Task tUpdateCBArrayOnChangedConnections
  tUpdateCBArrayOnChangedConnections.enable();

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

  // 4. If I am not alone, trigger the Task tChangedConnection which will send my status
  // in case a new comer is joining or be interrupted by the droppedConnection callback
  // if the changedConnectionCallback was triggered by a direct dropper for this box.
  Serial.printf("myMesh::changedConnectionCallback(): I am not alone. Sending my status.\n");
  tChangedConnection.setInterval(2900 + gui16NodeName);
  tChangedConnection.setCallback(_tcbSendStatusOnNewConnection);
  tChangedConnection.restartDelayed();

  // 5. If I do not know the number of the root node, try and figure out whether the mesh knows it
  if (!(ui32RootNodeId)) {
    // {"nodeId":2760139053,"root":true}
    char* _ptr = strstr(laserControllerMesh.subConnectionJson().c_str(), ",\"root\":true");
    char _cRootNodeId[12];
    if (_ptr != nullptr) {
      strncpy(_cRootNodeId, _ptr - 10, 11);
      ui32RootNodeId = strtoul(_cRootNodeId, NULL, 10);
    }
  }

  if (MY_DEEP_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): thisBoxes' index in CB array: %u\n", thisBox.findIndexByNodeName(thisBox.ui16NodeName));
    Serial.printf("myMesh::changedConnectionCallback(): task enabled? %i\n", tChangedConnection.isEnabled());
    Serial.printf("myMesh::changedConnectionCallback(): task interval: %lu\n", tChangedConnection.getInterval());
    Serial.print("myMesh::changedConnectionCallback(): task iterations: ");Serial.println(tChangedConnection.getIterations());
    // Serial.print("myMesh::changedConnectionCallback(): time until next iteration: ");Serial.println(myScheduler.timeUntilNextIteration(tChangedConnection));
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












//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
/** IamAlone
 *  Checks whether the node is alone, upon mesh events or before sending
 *  messages through the mesh. */
bool myMesh::IamAlone() {
  Serial.printf("myMesh::IamAlone(): Starting\n");
  /** Tests:
   *  (i) is the size of the nodeList < 2 (=> equal to 1 or 0)?
   *  (ii) is the last item of the nodeList equal to 0? <-- this second test is suspect. */
  if (
    laserControllerMesh.getNodeList().size() < 2 && 
    0 == *laserControllerMesh.getNodeList().rbegin()
    ) {
    Serial.printf("myMesh::IamAlone(): Yes\n");
    /** Tests:
     *  (i) whether this node is NOT an interface?
     *  (ii) the task tIamAloneTimeOut is not enabled?
     *  
     *  This should be moved somewhere else? It is not answering to the 
     *  question "Am I alone?" but to the question "When I am alone, have I already 
     *  planned steps to find other nodes?"
     *  */
    if ((!(isInterface)) && (!(tIamAloneTimeOut.isEnabled()))) {
      Serial.println("\nmyMesh::IamAlone(): Enabling tIamAloneTimeOut.\n");
    }
    return true;
  }
  Serial.printf("myMesh::IamAlone(): No\n");
  Serial.println("\nmyMesh::IamAlone(): Disabling tIamAloneTimeOut.\n");
  tIamAloneTimeOut.disable();
  return false;
}

/* tIamAloneTimeOut
   Task enabled when the node detects that it is alone.
   If after 20 seconds, it is still alone, it will restart the mesh.
*/
Task myMesh::tIamAloneTimeOut(20*TASK_SECOND, 1, &_tcbIamAloneTimeOut, NULL/*&mns::myScheduler*/, false, NULL, NULL);

/* _tcbIamAloneTimeOut()
   Restarts the mesh if the node is no longer connected.
*/
void myMesh::_tcbIamAloneTimeOut() {
  Serial.println("myMesh::_tcbIamAloneTimeOut(): Starting.");
  if (IamAlone()) {
    Serial.println("myMesh::_tcbIamAloneTimeOut(): Restarting the mesh.");
    // myMeshStarter::tRestart.restartDelayed();
  }
}






/* tPrintMeshTopo
   Used for debug purposes.
   Calls _printNodeListAndTopology() every 30 seconds.
*/
Task myMesh::tPrintMeshTopo(60*TASK_SECOND, TASK_FOREVER, &_printNodeListAndTopology, NULL/*&mns::myScheduler*/, false, NULL, NULL);

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
Task myMesh::tUpdateCBArrayOnChangedConnections(10*TASK_SECOND, 1, &_tcbUpdateCBOnChangedConnections, NULL, false, &_oetcbUpdateCBOnChangedConnections, &_odtcbUpdateCBOnChangedConnections);


bool myMesh::_oetcbUpdateCBOnChangedConnections() {
  // Disable the Task tSaveNodeMap (just in case)
  if (MY_DEEP_DG_MESH) Serial.println("\nmyMesh::_oetcbUpdateCBOnChangedConnections(): tSaveNodeMap disabled.\n");
  tSaveNodeMap.disable();
  return true;
}


void myMesh::_odtcbUpdateCBOnChangedConnections() {
  // Enable the Task tSaveNodeMap
  if (MY_DEEP_DG_MESH) Serial.println("\nmyMesh::_odtcbUpdateCBOnChangedConnections(): tSaveNodeMap restarted.\n");
  tSaveNodeMap.restart();
}


void myMesh::_tcbUpdateCBOnChangedConnections() {
  if (MY_DG_MESH) Serial.println("\nmyMesh::_tcbUpdateCBOnChangedConnections(): Starting. Time: " + String(millis()));
  // 1. Disable the Task tSaveNodeMap (just in case)
  tSaveNodeMap.disable();

  // 2. Create a _newNodeList containing the new mesh layout
  // std::list<uint32_t> _newNodeList = laserControllerMesh.getNodeList();
  // _newNodeList.remove(0);
  // _newNodeList.sort();
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
  for (uint32_t _newNode : laserControllerMesh.getNodeList()) {
    if (_newNode == (uint32_t)0) continue;
    std::map<uint32_t, uint16_t>::iterator _nodeInMap = _nodeMap.find(_newNode);
    if (_nodeInMap != _nodeMap.end()) {
      _nodeInMap->second = 1; // now existed and is still existing
      continue;
    }
    _nodeMap.emplace(_newNode, 2); // new node
  }
  // if (MY_DEBUG) {
  //   uint16_t _it = 0;
  //   Serial.printf("\nmyMesh::_tcbUpdateCBOnChangedConnections(): Printing out the map:\n");
  //   for (std::pair<std::uint32_t, uint16_t> _node : _nodeMap) {
  //     Serial.printf("myMesh::_tcbUpdateCBOnChangedConnections(): node [%u]: %u\n", _it, _node.first);
  //     _it++;
  //   }
  //   Serial.printf("myMesh::_tcbUpdateCBOnChangedConnections(): ---\n\n");
  // }
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
    if (_node.second == (uint16_t)0) {  // node marked as 0: existed but does not exist anymore
      _deleteBoxFromCBArray(_node.first);
    }
  }
}

uint16_t myMesh::_deleteBoxFromCBArray(uint32_t nodeId) {
  // 1. check if the box is listed in my ControlerBoxes[]
  uint16_t _ui16droppedIndex = ControlerBox::findIndexByNodeId(nodeId);

  // 2. if the box index is different than 254, it exists
  if (_ui16droppedIndex == 254) {
    if (MY_DEEP_DG_MESH) Serial.println(F("myMesh::_deleteBoxFromCBArray(): ending (box not found in ControlerBoxes[])."));
    return 0;
  }

  // 3. delete the box from ControlerBoxes[]
  ControlerBoxes[_ui16droppedIndex].deleteBox();

  return _ui16droppedIndex;
}







/* tSaveNodeMap
  This Task is called on various occasions, to keep an 
  up-to-date vision of the mesh layout.
*/
Task myMesh::tSaveNodeMap(10 * TASK_SECOND, 2, &_tcbSaveNodeMap, NULL/*&mns::myScheduler*/, false, NULL, NULL);

void myMesh::_tcbSaveNodeMap() {
  if (MY_DEEP_DG_MESH) Serial.println("\nmyMesh::_tcbSaveNodeMap(): remaining iterations: " + String(tSaveNodeMap.getIterations()));
  if (tUpdateCBArrayOnChangedConnections.isEnabled()) {
    if (MY_DEEP_DG_MESH) Serial.println("myMesh::_tcbSaveNodeMap(): tUpdateCBArrayOnChangedConnections is enabled. Passing this iteration.");
    tSaveNodeMap.setIterations(tSaveNodeMap.getIterations() + 1);
    return;
  }
  tSaveNodeMap.setInterval(20 * TASK_SECOND);
  if (MY_DG_MESH) Serial.println("myMesh::_tcbSaveNodeMap(): tUpdateCBArrayOnChangedConnections is not enabled. Updating mesh topo map.");
  _saveNodeMap();
}

std::map<uint32_t, uint16_t> myMesh::_nodeMap;

void myMesh::_saveNodeMap() {
  // Serial.println("myMesh::_saveNodeMap(): Starting. Time: " + String(millis()));
  _nodeMap.clear();
  // Serial.println("myMesh::_saveNodeMap(): Before iteration. Time: " + String(millis()));
  // for (uint32_t _savedNode : _savedNodeList) {
  for (uint32_t _nodeFromList : laserControllerMesh.getNodeList()) {
    if (_nodeFromList == (uint32_t)0) continue;
    _nodeMap.emplace(_nodeFromList, 1);
  }

  // if (MY_DEBUG) {
  //   uint16_t _it = 0;
  //   Serial.printf("\nmyMesh::_saveNodeMap(): Printing out the map:\n");
  //   for (std::pair<std::uint32_t, uint16_t> _node : _nodeMap) {
  //     Serial.printf("myMesh::_saveNodeMap(): node [%u]: %u\n", _it, _node.first);
  //     _it++;
  //   }
  //   Serial.printf("myMesh::_saveNodeMap(): ---\n\n");
  // }
  // Serial.println("myMesh::_saveNodeMap(): Ending. Time: " + String(millis()));
}


