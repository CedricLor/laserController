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
#include "./myMeshViews/myMeshViews.cpp"
#include "./myMeshController/myMeshController.cpp"

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
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
      laserControllerMesh.setRoot(true);
    } else {
      laserControllerMesh.setRoot(false);
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
  }
}





void myMesh::droppedConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection for nodeId %u\n",nodeId);
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection %s\n",laserControllerMesh.subConnectionJson().c_str());
    Serial.println("--------------------- DROPPED CONNECTION --------------------------");
  }
  // if the new node is not in my ControlerBoxes array, send it my status

  uint16_t _ui16droppedNodeName = ControlerBox::findByNodeId(nodeId);
  if (_ui16droppedNodeName != 254) {
    myMeshViews __myMeshViews;
    __myMeshViews.droppedNodeNotif(_ui16droppedNodeName);
    ControlerBox::deleteBox(_ui16droppedNodeName);
  }
  uiMeshSize = laserControllerMesh.getNodeList().size();
}





// This Task broadcasts all other boxes this boxState, when there is a
// changedConnection
Task myMesh::_tSendStatusOnChangeConnection((3000 + gui16MyIndexInCBArray * 250), 1, &_tcbSendStatusOnChangeConnection, &userScheduler, false);

void myMesh::_tcbSendStatusOnChangeConnection() {
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();
}





void myMesh::changedConnectionCallback() {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
    Serial.println("--------------------- CHANGED CONNECTION --------------------------");
    Serial.println("myMesh::changedConnectionCallback(): Checking whether new comer or dropper.");
  }

  uint16_t _uiNewMeshSize = laserControllerMesh.getNodeList().size();

  if (_uiNewMeshSize > uiMeshSize) {
    // if the new node is not in my ControlerBoxes array, send it my status
    // if (ControlerBox::findByNodeId(nodeId) != 254) {
      if (MY_DG_MESH) {
        Serial.println("myMesh::changedConnectionCallback(): New member has joined.");
        Serial.println("myMesh::changedConnectionCallback(): About to send them my data.");
        Serial.println(millis());
      }
      _tSendStatusOnChangeConnection.restartDelayed();
      if (MY_DG_MESH) {
        Serial.println("myMesh::changedConnectionCallback(): Enabled task _tSendStatusOnChangeConnection.");
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

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<1000> _doc;
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
