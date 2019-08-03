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

const char* myMesh::_ap_ssid     = "ESP32-Access-Point";
const char* myMesh::_ap_password = "123456789";

myMesh::myMesh()
{
}



void myMesh::meshSetup() {
  if ( MY_DEBUG == true ) {
    // laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP |/*MESH_STATUS |*/ CONNECTION |/* SYNC |*/ COMMUNICATION /* | GENERAL | MSG_TYPES | REMOTE */);
    laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);
  }

  if (interfaceInAP) {
    laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_STA, ui8WifiChannel);
  } else {
    laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, ui8WifiChannel);
  }

  if (isInterface == true) {
  // Two scenarii:
    if (interfaceInAP) {
      // A. you can use the interface as STATION scanning of other 
      // mesh nodes. Web users have to connect on the AP.
      if(!WiFi.softAPConfig(IPAddress(192, 168, 5, 1), IPAddress(192, 168, 5, 1), IPAddress(255, 255, 255, 0))){
        Serial.println("AP Config Failed");
      }
      if (WiFi.softAP(_ap_ssid, _ap_password, 
                      ui8WifiChannel/*, false /* int ssid_hidden *\/,*/ 
                      /*10 /* int max_connection */)){
        Serial.println("");
        IPAddress myIP = WiFi.softAPIP();
        Serial.println("Network " + String(_ap_ssid) + " running");
        Serial.print("AP IP address: ");Serial.println(myIP);
      } else {
        Serial.println("Starting AP failed.");
      }
    } else {
      // B. you can use the interface as an AP for the other mesh nodes. 
      // Web users have to connect on the STATION.
      laserControllerMesh.stationManual(ssid, pass, ui16GatewayPort, gatewayIP);
    // laserControllerMesh.stationManual(ssid, pass);
    }
  }

  snprintf(gcHostnamePrefix, 10, "%s%u", gcHostnamePrefix, (uint32_t)gui16NodeName);
  // laserControllerMesh.setHostname(gcHostnamePrefix);
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "ESP32_200.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(gcHostnamePrefix)) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");


  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("arduino", "tcp", 3232);

  if (isInterface == true) {
      if (interfaceInAP) {
        laserControllerMesh.setRoot(false);
      } else {
        // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
        laserControllerMesh.setRoot(true);
      }
  }
  // This and all other mesh member should ideally know that the mesh contains a root
  laserControllerMesh.setContainsRoot(true);

  // Serial.println("myMesh::meshSetup(): About to call updateThisBoxProperties:");
  ControlerBoxes[gui16MyIndexInCBArray].updateThisBoxProperties();

  laserControllerMesh.onReceive(&receivedCallback);
  laserControllerMesh.onNewConnection(&newConnectionCallback);
  laserControllerMesh.onChangedConnections(&changedConnectionCallback);
  laserControllerMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);
  laserControllerMesh.onDroppedConnection(&droppedConnectionCallback);
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helpers
void myMesh::_updateNodeListSize() {
  Serial.printf("myMesh::_updateNodeListSize(): previous uiMeshSize %u\n", uiMeshSize);
  uiMeshSize = laserControllerMesh.getNodeList().size();
  Serial.printf("myMesh::_updateNodeListSize(): new uiMeshSize %u\n", uiMeshSize);
  _printNodeListAndTopology();
  Serial.printf("myMesh::_updateNodeListSize(): ending\n");
}

void myMesh::_printNodeListAndTopology() {
  Serial.printf("myMesh::_printNodeListAndTopology(): mesh topology %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("myMesh::_printNodeListAndTopology(): Node list size: %i\n", laserControllerMesh.getNodeList().size());
  int16_t _i = 0;
  for (int n : laserControllerMesh.getNodeList()) {
    Serial.printf("myMesh::_printNodeListAndTopology(): node [%i] id: %u\n", _i++, n);
  }
  Serial.printf("myMesh::_printNodeListAndTopology(): ending -------\n");
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Network Callbacks
void myMesh::receivedCallback(uint32_t from, String &msg ) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  }
  ControlerBoxes[gui16MyIndexInCBArray].updateThisBoxProperties();
  _decodeRequest(from, msg);

  Serial.printf("myMesh::receivedCallback(): ending\n");
}












void myMesh::_tcbSendStatusOnNewConnection() {
  Serial.printf("myMesh::_tcbSendStatusOnNewConnection(): starting\n");
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();
  Serial.printf("myMesh::_tcbSendStatusOnNewConnection(): ending\n");
}

bool myMesh::_oetcbSendStatusOnNewConnection() {
    Serial.println("myMesh::_oetcbSendStatusOnNewConnection(): starting");
    // Serial.printf("myMesh::_oetcbSendStatusOnNewConnection(): starting. Time: %lu\n", millis());
    // Serial.printf("myMesh::_oetcbSendStatusOnNewConnection(): task enabled? %i\n", _tChangedConnection.isEnabled());
    // Serial.printf("myMesh::_oetcbSendStatusOnNewConnection(): task interval: %lu\n", _tChangedConnection.getInterval());
    // Serial.print("myMesh::_oetcbSendStatusOnNewConnection(): task iterations: ");Serial.println(_tChangedConnection.getIterations());
    // Serial.print("myMesh::_oetcbSendStatusOnNewConnection(): time until next iteration: ");Serial.println(userScheduler.timeUntilNextIteration(_tChangedConnection));
    Serial.printf("myMesh::_oetcbSendStatusOnNewConnection(): ending\n");
    return true;
}

void myMesh::newConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::newConnectionCallback(): New Connection, nodeId = %u\n", nodeId);
    Serial.println("++++++++++++++++++++++++ NEW CONNECTION +++++++++++++++++++++++++++");
  }
}










bool myMesh::_oetcbSendNotifOnDroppedConnection(uint32_t nodeId) {
  // A. check if the box is listed in my ControlerBoxes[]
  uint16_t _ui16droppedIndex = ControlerBox::findByNodeId(nodeId);
  // B. if the box index is different than 254, it exists
  if (_ui16droppedIndex != 254) {
    // a. set the main callback of the Task
    _tChangedConnection.setCallback(
      // pass a lambda as main callback for the Task _tChangedConnection and _ui16droppedIndex as contextual param
      [_ui16droppedIndex]() {
        droppedConnectionCallback(_ui16droppedIndex);
      }
    ); // end of setCallback(
    // b. return true so that the main callback of Task _tSendNotifOnDroppedConnection
    // be executed
    return true;
  } // end of (_ui16droppedIndex != 254)
  // C. if the box index is 254, it does not exist -> disable the Task by returning false
  return false;
};

void myMesh::_tcbSendNotifOnDroppedConnection(uint16_t _ui16droppedIndex) {
  // 1. send a dropped box notification
  myMeshViews __myMeshViews;
  __myMeshViews.droppedNodeNotif(_ui16droppedIndex);
  // 2. delete the box from ControlerBox[]
  ControlerBox::deleteBox(_ui16droppedIndex);
};

void myMesh::droppedConnectionCallback(uint32_t nodeId) {
  _tChangedConnection.disable();
  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection for nodeId %u\n", nodeId);
    Serial.printf("myMesh::droppedConnectionCallback(): printing the current nodeList (no update):\n");
    _printNodeListAndTopology();
    Serial.println("--------------------- DROPPED CONNECTION --------------------------");
  }

  // 1. disable Task _tChangedConnection (enabled in changedConnectionCallback)
  // If the droppedConnectionCallback is triggered, the changedConnectionCallback has been triggered
  // before. Upon a changedConnectionCallback event, Task _tChangedConnection is enabled
  // with a delay.
  // Obviously, if the dropped connection callback is triggered, it means that changedConnectionCallback
  // was triggered by a dropped connection, not by a new connection.

  // 2. If nodeId == 0, this is the case where a box a shortly has failed (power on reset or so) and 
  // has rebooted and is trying to join the mesh as a station before its dropping has been detected
  // In this case, do nothing, just return.
  if (nodeId == 0) {
    Serial.printf("myMesh::droppedConnectionCallback(): nodeId == 0. False signal. About to return.\n");
    return;
  }
  Serial.printf("myMesh::droppedConnectionCallback(): nodeId == %u. Setting _tChangedConnection to notify the mesh and delete the box in ControlerBox[].\n", nodeId);

  // 3. set the Task _tChangedConnection to send notification to the
  // mesh and delete the box from the ControlerBoxes[].
  // Here, the nodeId != 0; we have dropped nodeId => this is a real dropped connection callback
  _tChangedConnection.setOnEnable(
    // pass a lambda as onOnEnable callback for the Task _tChangedConnection and nodeId as contextual param
    [nodeId]() { return _oetcbSendNotifOnDroppedConnection(nodeId); } 
  );

  // 4. Enable the Task _tChangedConnection, for execution without delay
  _tChangedConnection.setInterval(0);
  _tChangedConnection.enable();

  if (MY_DG_MESH) {
    Serial.println("myMesh::droppedConnectionCallback(): Ending.");
  }
}









// This Task broadcasts this box's boxState all the other boxes, when there is a
// changedConnection. It is reset the onDroppedConnection callback to send a
// dropped box notification instead, if the onDroppedConnection callback is called
// immediately after the onChangedConnection callback.
Task myMesh::_tChangedConnection(0, 1, &_tcbSendStatusOnNewConnection, &userScheduler, false);

void myMesh::_odtcbChangedConnection() {
    Serial.println("--------------------- CHANGED CONNECTION TASK DISABLE --------------------------");
    Serial.printf("myMesh::_odtcbSendStatusOnNewConnection(): starting. Time: %lu\n", millis());
}

void myMesh::changedConnectionCallback() {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): printing the current nodeList (no update):\n");
    _printNodeListAndTopology();
    Serial.println("--------------------- CHANGED CONNECTION --------------------------");
  }

  if (IamAlone) {
    Serial.printf("myMesh::changedConnectionCallback(): I am alone. Not sending any message.\n");
    return;
  }

  _tChangedConnection.setOnEnable(_oetcbSendStatusOnNewConnection);
  _tChangedConnection.setInterval((3000 + gui16MyIndexInCBArray * 100));
  _tChangedConnection.setCallback(_tcbSendStatusOnNewConnection);
  _tChangedConnection.restartDelayed();

  if (MY_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): gui16MyIndexInCBArray: %u\n", gui16MyIndexInCBArray);
    Serial.printf("myMesh::changedConnectionCallback(): task enabled? %i\n", _tChangedConnection.isEnabled());
    Serial.printf("myMesh::changedConnectionCallback(): task interval: %lu\n", _tChangedConnection.getInterval());
    Serial.print("myMesh::changedConnectionCallback(): task iterations: ");Serial.println(_tChangedConnection.getIterations());
    Serial.print("myMesh::changedConnectionCallback(): time until next iteration: ");Serial.println(userScheduler.timeUntilNextIteration(_tChangedConnection));
  }
}

bool myMesh::IamAlone() {
  if ((laserControllerMesh.getNodeList()).size() < 2 && 0 == (*(laserControllerMesh.getNodeList()).end())--) {
    return true;
  }
  return false;
}



void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::nodeTimeAdjustedCallback(): Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(), offset);
  }
}





void myMesh::delayReceivedCallback(uint32_t from, int32_t delay) {
  if (MY_DG_MESH) {
    Serial.printf("Delay to node %u is %d ms\n", from, delay);
  }
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Decode Request
/* Mesh controller is a controller in the same meaning as in Model-View-Controller pattern in Ruby.
    It is called from the receivedCallback callback.
    It reads the JSON string received via the Mesh communication protocol.
    It determines whether the messages is sent by my Master Node or by the Interface Node.
    If so, it calls the meshController
*/
void myMesh::_decodeRequest(uint32_t _ui32SenderNodeId, String &_msg) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::_decodeRequest(uint32_t _ui32SenderNodeId, String &_msg) starting. _ui32SenderNodeId == %u; &_msg == %s \n", _ui32SenderNodeId, _msg.c_str());
  }
  const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<capacity> _doc;
  // Convert the document to an object
  JsonObject _obj = _doc.to<JsonObject>();

  if (MY_DG_MESH) {
    Serial.print("myMesh::_decodeRequest(...): jsonDocument created\n");
  }

  // deserialize the message _msg received from the mesh into the StaticJsonDocument doc
  DeserializationError _err = deserializeJson(_doc, _msg.c_str());
  if (MY_DG_MESH) {
    Serial.print("myMesh::_decodeRequest(...): message _msg deserialized into JsonDocument doc\n");
    Serial.print("myMesh::_decodeRequest(...): DeserializationError = ");Serial.print(_err.c_str());Serial.print("\n");
  }

  // pass the deserialized doc and the _ui32SenderNodeId to the controller
  myMeshController myMeshController(_ui32SenderNodeId, _obj);
}
