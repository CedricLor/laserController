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
  laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed
  laserControllerMesh.onDroppedConnection(&droppedConnectionCallback);                                   // Might not be needed
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Network Callbacks
void myMesh::receivedCallback(uint32_t from, String &msg ) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  }
  ControlerBoxes[gui16MyIndexInCBArray].updateThisBoxProperties();
  _decodeRequest(from, msg);
}









void myMesh::newConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::newConnectionCallback(): New Connection, nodeId = %u\n", nodeId);
    Serial.println("++++++++++++++++++++++++ NEW CONNECTION +++++++++++++++++++++++++++");
    const uint16_t _uiNewMeshSize = laserControllerMesh.getNodeList().size();
    Serial.printf("myMesh::droppedConnectionCallback(): previous uiMeshSize %u\n", uiMeshSize);
    Serial.printf("myMesh::droppedConnectionCallback(): new _uiNewMeshSize %u\n", _uiNewMeshSize);
  }
}







// Task myMesh::_tSendNotifOnDroppedConnection(0, 1, &_tcbSendNotifOnDroppedConnection, &userScheduler, false, &_oetcbSendNotifOnDroppedConnection, &_odtcbSendNotifOnDroppedConnection);


// bool myMesh::_oetcbSendNotifOnDroppedConnection() {
//   // const std::list<unsigned int> &_nodeList = laserControllerMesh.getNodeList();
//   // for (auto it = _nodeList.cbegin(); it != _nodeList.cend(); it++) {
//   // }
//   uint16_t _ui16droppedNodeName = ControlerBox::findByNodeId(nodeId);
//   if (_ui16droppedNodeName != 254) {
//     return true;
//   }
//   return false;
// };


// void myMesh::_tcbSendNotifOnDroppedConnection() {
//     myMeshViews __myMeshViews;
//     __myMeshViews.droppedNodeNotif(_ui16droppedNodeName);
//     if (MY_DG_MESH) {
//       Serial.printf("myMesh::droppedConnectionCallback(): Broadcasted a message: %s\n",laserControllerMesh.subConnectionJson().c_str());
//       Serial.printf("myMesh::droppedConnectionCallback(): Deleting the dropper %i ControlerBoxes.\n", _ui16droppedNodeName);
//     }
//     ControlerBox::deleteBox(_ui16droppedNodeName);
// };


// void myMesh::_odtcbSendNotifOnDroppedConnection() {


// };


void myMesh::droppedConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection for nodeId %u\n", nodeId);
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection %s\n",laserControllerMesh.subConnectionJson().c_str());
    Serial.println("--------------------- DROPPED CONNECTION --------------------------");
  }

  // check the mesh size
  Serial.printf("myMesh::droppedConnectionCallback(): previous uiMeshSize %u\n", uiMeshSize);
  const uint16_t _uiNewMeshSize = laserControllerMesh.getNodeList().size();
  Serial.printf("myMesh::droppedConnectionCallback(): new _uiNewMeshSize %u\n", _uiNewMeshSize);
  // find the dropper in the ControlerBox
  uint16_t _ui16droppedIndex = ControlerBox::findByNodeId(nodeId);

  // if the dropped node is my ControlerBoxes array, send a notification to the mesh
  // and delete it from the box
  if (_ui16droppedIndex != 0) {
    // 1. Send a notification to the mesh (if I am not alone)
    if (_uiNewMeshSize > 1) {
      myMeshViews __myMeshViews;
      __myMeshViews.droppedNodeNotif(_ui16droppedIndex);
    }

    // 2. Delete the dropper from the ControlerBoxes
    if (MY_DG_MESH) {
      Serial.printf("myMesh::droppedConnectionCallback(): Broadcasted a message: %s\n",laserControllerMesh.subConnectionJson().c_str());
      Serial.printf("myMesh::droppedConnectionCallback(): Deleting the dropper %u from ControlerBoxes[].\n", _ui16droppedIndex);
    }
    ControlerBox::deleteBox(_ui16droppedIndex);
    if (MY_DG_MESH) {
      Serial.println("myMesh::droppedConnectionCallback(): Dropper deleted.");
    }
  }


  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): New mesh size = %u. Saving it.\n", _uiNewMeshSize);
  }
  uiMeshSize = _uiNewMeshSize;
  if (MY_DG_MESH) {
    Serial.println("myMesh::changedConnectionCallback(): Ending.");
  }
}









// This Task broadcasts all other boxes this boxState, when there is a
// changedConnection
Task myMesh::_tSendStatusOnChangeConnection((3000 + gui16MyIndexInCBArray * 250), 1, &_tcbSendStatusOnChangeConnection, &userScheduler, false, &_oetcbSendStatusOnChangeConnection, &_odtcbSendStatusOnChangeConnection);

void myMesh::_tcbSendStatusOnChangeConnection() {
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();
}

bool myMesh::_oetcbSendStatusOnChangeConnection() {
    Serial.println("myMesh::_oetcbSendStatusOnChangeConnection(): starting");
    // Serial.printf("myMesh::_oetcbSendStatusOnChangeConnection(): starting. Time: %lu\n", millis());
    // Serial.printf("myMesh::_oetcbSendStatusOnChangeConnection(): task enabled? %i\n", _tSendStatusOnChangeConnection.isEnabled());
    // Serial.printf("myMesh::_oetcbSendStatusOnChangeConnection(): task interval: %lu\n", _tSendStatusOnChangeConnection.getInterval());
    // Serial.print("myMesh::_oetcbSendStatusOnChangeConnection(): task iterations: ");Serial.println(_tSendStatusOnChangeConnection.getIterations());
    // Serial.print("myMesh::_oetcbSendStatusOnChangeConnection(): time until next iteration: ");Serial.println(userScheduler.timeUntilNextIteration(_tSendStatusOnChangeConnection));
    return true;
}

void myMesh::_odtcbSendStatusOnChangeConnection() {
    Serial.println("--------------------- CHANGED CONNECTION TASK DISABLE --------------------------");
    Serial.printf("myMesh::_odtcbSendStatusOnChangeConnection(): starting. Time: %lu\n", millis());
}

void myMesh::changedConnectionCallback() {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
    Serial.println("--------------------- CHANGED CONNECTION --------------------------");
    Serial.println("myMesh::changedConnectionCallback(): Checking whether new comer or dropper.");
  }

  const uint16_t _uiNewMeshSize = laserControllerMesh.getNodeList().size();
  Serial.printf("myMesh::droppedConnectionCallback(): previous uiMeshSize %u\n", uiMeshSize);
  Serial.printf("myMesh::droppedConnectionCallback(): new _uiNewMeshSize %u\n", _uiNewMeshSize);

  if (_uiNewMeshSize > uiMeshSize && _uiNewMeshSize > 0) {    
    // if the new node is not in my ControlerBoxes array, send it my status
    // if (ControlerBox::findByNodeId(nodeId) != 254) {
      if (MY_DG_MESH) {
        Serial.println("myMesh::changedConnectionCallback(): New member has joined.");
        Serial.println("myMesh::changedConnectionCallback(): About to send them my data.");
        // Serial.printf("myMesh::changedConnectionCallback(): gui16MyIndexInCBArray: %u\n", gui16MyIndexInCBArray);
        // Serial.printf("myMesh::changedConnectionCallback(): task enabled? %i\n", _tSendStatusOnChangeConnection.isEnabled());
        // Serial.printf("myMesh::changedConnectionCallback(): task interval: %lu\n", _tSendStatusOnChangeConnection.getInterval());
        // Serial.print("myMesh::changedConnectionCallback(): task iterations: ");Serial.println(_tSendStatusOnChangeConnection.getIterations());
        // Serial.print("myMesh::changedConnectionCallback(): time until next iteration: ");Serial.println(userScheduler.timeUntilNextIteration(_tSendStatusOnChangeConnection));
      }
      _tSendStatusOnChangeConnection.restartDelayed();
      if (MY_DG_MESH) {
        Serial.printf("myMesh::changedConnectionCallback(): gui16MyIndexInCBArray: %u\n", gui16MyIndexInCBArray);
        Serial.printf("myMesh::changedConnectionCallback(): task enabled? %i\n", _tSendStatusOnChangeConnection.isEnabled());
        Serial.printf("myMesh::changedConnectionCallback(): task interval: %lu\n", _tSendStatusOnChangeConnection.getInterval());
        Serial.print("myMesh::changedConnectionCallback(): task iterations: ");Serial.println(_tSendStatusOnChangeConnection.getIterations());
        Serial.print("myMesh::changedConnectionCallback(): time until next iteration: ");Serial.println(userScheduler.timeUntilNextIteration(_tSendStatusOnChangeConnection));
      }
    // }
  }

  uiMeshSize = _uiNewMeshSize;
}





void myMesh::nodeTimeAdjustedCallback(int32_t offset) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::nodeTimeAdjustedCallback(): Adjusted time %u. Offset = %d\n", laserControllerMesh.getNodeTime(), offset);
  }
}





void myMesh::delayReceivedCallback(uint32_t from, int32_t delay) {
  if (MY_DG_MESH) {
    Serial.printf("Delay to node %u is %d us\n", from, delay);
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
