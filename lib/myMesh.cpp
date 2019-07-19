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

// const uint8_t* myMesh::STATION_IP = station_ip;

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

  laserControllerMesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, ui8WifiChannel);

  // _IpStringToIPAddress(gatewayIP)

  if (isInterface == true) {
    laserControllerMesh.stationManual(ssid, pass, ui16GatewayPort, gatewayIP);
    // laserControllerMesh.stationManual(ssid, pass);
  }

  laserControllerMesh.setHostname(_apSsidBuilder(_myApSsidBuf));
  if (isInterface == true) {
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    laserControllerMesh.setRoot(true);
  }
  // This and all other mesh member should ideally know that the mesh contains a root
  laserControllerMesh.setContainsRoot(true);

  // Serial.println("myMesh::meshSetup(): About to call updateThisBoxProperties:");
  ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();

  laserControllerMesh.onReceive(&receivedCallback);
  laserControllerMesh.onNewConnection(&newConnectionCallback);
  laserControllerMesh.onChangedConnections(&changedConnectionCallback);
  laserControllerMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  laserControllerMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed
  laserControllerMesh.onDroppedConnection(&droppedConnectionCallback);                                   // Might not be needed
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mesh Network Callbacks
void myMesh::receivedCallback( uint32_t from, String &msg ) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  }
  ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();
  _decodeRequest(from, msg);
}


// This Task broadcasts all other boxes this boxState, when this box newly connects to the Mesh
// after a delay of 4500 ms (before such delay, message are not send to the mesh)
Task myMesh::_tSendStatusOnNewConnection((100 + myIndexInCBArray * 250), 1, &_tcbSendStatusOnNewConnection, &userScheduler, false);

void myMesh::_tcbSendStatusOnNewConnection() {
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();
}




void myMesh::newConnectionCallback(uint32_t nodeId) {
    if (MY_DG_MESH) {
      Serial.printf("myMesh::newConnectionCallback(): New Connection, nodeId = %u\n", nodeId);
      // Serial.printf("myMesh::newConnectionCallback(): laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
      Serial.println("++++++++++++++++++++++++ NEW CONNECTION +++++++++++++++++++++++++++");
      Serial.println("myMesh::newConnectionCallback(): I am going to send them my data.");
    }
    // if the new node is not in my ControlerBoxes array, send it my status
    if (ControlerBox::findByNodeId(nodeId) != 254) {
      if (MY_DG_MESH) {
        Serial.println("myMesh::newConnectionCallback(): About to send them my data.");
      }
      _tSendStatusOnNewConnection.restartDelayed();
      if (MY_DG_MESH) {
        Serial.println("myMesh::newConnectionCallback(): Enabled task _tSendStatusOnNewConnection.");
      }
    }
    // else, we were already connected to the mesh: do nothing; we will send it a statusMsg when it will have identified itself
    // else {
    //   Serial.println("myMesh::newConnectionCallback(): A new box has joined the existing mesh. About to send it my data.");
    //   _tSendStatusOnNewConnection.enableDelayed(myIndexInCBArray * 1000);
    //   Serial.println("myMesh::newConnectionCallback(): Enabled task _tSendStatusOnNewConnection.");
    // }
}





void myMesh::droppedConnectionCallback(uint32_t nodeId) {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection for nodeId %i\n",nodeId);
    Serial.printf("myMesh::droppedConnectionCallback(): Dropped connection %s\n",laserControllerMesh.subConnectionJson().c_str());
    Serial.println("--------------------- DROPPED CONNECTION --------------------------");
  }
  ControlerBox::deleteBox(nodeId);
}





void myMesh::changedConnectionCallback() {
  if (MY_DG_MESH) {
    Serial.printf("myMesh::changedConnectionCallback(): Changed connections %s\n",laserControllerMesh.subConnectionJson().c_str());
    Serial.println("--------------------- CHANGED CONNECTION --------------------------");
  }
  // ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();
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





//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
IPAddress myMesh::_IpStringToIPAddress(const char* _cIpCharArray) {
  // see https://stackoverflow.com/questions/35227449/convert-ip-or-mac-address-from-string-to-byte-array-arduino-or-c
  /*  Call it this way:
        const char* ipStr = "50.100.150.200";
        byte ip[4];
        _parseBytesToCharArray(ipStr, '.', ip, 4, 10);
   */
  byte _ip[4];
  for (int i = 0; i < 4; i++) {
    _ip[i] = strtoul(_cIpCharArray, NULL, 10);  // Convert to byte
    _cIpCharArray = strchr(_cIpCharArray, '.');             // Find next separator
    if (_cIpCharArray == NULL || *_cIpCharArray == '\0') {
        break;                            // No more separators, exit
    }
    _cIpCharArray++;                                // Point to next character after separator
  }
  return _ip;
}



char* myMesh::_apSsidBuilder(char _apSsidBuf[8]) {
  // strcat(destination, source); source is appended destination, replacing the null character of destination by the first character of source
  strcat(_apSsidBuf, _PREFIX_AP_SSID);
  // after the preceeding line, _apSsidBuf shall be equal to "box_"
  char _cNodeName[4];
  itoa((int)gui8NodeName, _cNodeName, 10);
  // itoa convert gui8NodeName (201, 202, etc.) into to char array, the resulting char array being stored into _cNodeName
  strcat(_apSsidBuf, _cNodeName);
  // strcat appends 201, 202, etc. to box_
  // _apSsidBuf shall equal to something like box_201, box_202, etc.
  return _apSsidBuf;
}
