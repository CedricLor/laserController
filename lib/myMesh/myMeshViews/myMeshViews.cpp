/*
  myMeshViews.cpp - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.

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
#include "myMeshViews.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// myMeshViews: message sender for controller boxes
// myMeshViews::myMeshViews()
/*
   Upon instantiation, the constructor updates the properties of this ControlerBox
   in the array of ControlerBoxes (APIP, StationIP, NodeName).
   After having instantiating it, you have to call a message builder, that will:
   2. create a structured String with the params you passed it on;
   3. passe this String to the sendBroadcast or sendSingle methods of painlessMesh.
   painlessMesh takes care of the broadcast/sending.
*/



// Tasks
// Task myMeshViews::tSendBoxStateToNewBox((gui8MyIndexInCBArray * 1000), 1, NULL, &userScheduler, false, NULL, _odtcbSendBoxStateToNewBox);
//
// void myMeshViews::_odtcbSendBoxStateToNewBox() {
//   for (short int _boxIndex = 1; _boxIndex < sBoxesCount; _boxIndex++) {
//     if (ControlerBoxes[_boxIndex].nodeId != 0) {
//       if (ControlerBoxes[_boxIndex].isNewBoxHasBeenSignaled == false) {
//         myMeshViews __myMeshViews;
//         __myMeshViews.statusMsg(ControlerBoxes[_boxIndex].nodeId);
//         ControlerBoxes[_boxIndex].isNewBoxHasBeenSignaled = true;
//       }
//     }
//   }
// }






// Constructor
myMeshViews::myMeshViews()
{
  ControlerBoxes[gui8MyIndexInCBArray].updateThisBoxProperties();
}





void myMeshViews::statusMsg(uint32_t destNodeId) {
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::statusMsg(): Starting.");
  }
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"boxDefstate":5;"NNa":"201";"APIP":"...";"StIP":"..."}

  const int _capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<_capacity> _jDoc;
  JsonObject _joMsg = _jDoc.to<JsonObject>();

  // load the JSON document with values
  _joMsg["actSt"] = ControlerBoxes[gui8MyIndexInCBArray].boxActiveState;
  _joMsg["actStStartT"] = ControlerBoxes[gui8MyIndexInCBArray].uiBoxActiveStateStartTime; // gets the recorded mesh time
  _joMsg["boxDefstate"] = ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState;
  _joMsg["action"] = "s";

  // send to the sender
  _sendMsg(_joMsg, destNodeId);

  // I signaled my boxState change.
  // => set my own boxActiveStateHasBeenSignaled to true
  ControlerBoxes[gui8MyIndexInCBArray].boxActiveStateHasBeenSignaled = true;

  if (MY_DG_MESH) {
    Serial.println("myMeshViews::statusMsg(): Ending.");
  }
}






void myMeshViews::changeBoxRequest(JsonObject& _obj, bool _bBroadcast) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (MY_DG_MESH) {
    Serial.print("myMeshViews::changeBoxRequest(): Starting.\n");
  }

  // broadcast or send the message
  if (_bBroadcast) {
    _sendMsg(_obj);
  } else {
    // get the destination nodeId
    uint32_t _destNodeId = ControlerBoxes[_obj["lb"].as<uint8_t>()].nodeId;
    _sendMsg(_obj, _destNodeId);
  }
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

  if (MY_DG_MESH) {
    Serial.println("myMeshViews::changeBoxRequest(): Ending.");
  }
}





void myMeshViews::changedBoxConfirmation(JsonObject& obj) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 2} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::changedBoxConfirmation(): Starting.");
  }

  // broadcast confirmation
  _sendMsg(obj);

  if (MY_DG_MESH) {
    Serial.println("myMeshViews::changedMasterBoxConfirmation(): Ending.");
  }
}










void myMeshViews::_sendMsg(JsonObject& _joMsg, uint32_t destNodeId) {
  if (MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): Starting.");
    // Serial.println("myMeshViews::_sendMsg(): about to allocate ControlerBoxes[gui8MyIndexInCBArray].bNodeName to _joMsg[\"senderNodeName\"]");
  }


  // adding my nodeName to the JSON to be sent to other boxes
  _joMsg["NNa"] = ControlerBoxes[gui8MyIndexInCBArray].bNodeName;
  // if (MY_DG_MESH) {
  //  Serial.println("myMeshViews::_sendMsg(): about to allocate APIP to _joMsg[\"senderAPIP\"]");
  // }


  // adding the APIP and the StationIP to the JSON to be sent to other boxes
  if (_joMsg.containsKey("APIP") && _joMsg.containsKey("StIP")) {
    for (short _i = 0; _i < 4; _i++) {
      _joMsg["APIP"][_i] = ControlerBoxes[gui8MyIndexInCBArray].APIP[_i];
      _joMsg["StIP"][_i] = ControlerBoxes[gui8MyIndexInCBArray].stationIP[_i];
    }
  } else {
    JsonArray _APIP = _joMsg.createNestedArray("APIP");
    JsonArray _StIP = _joMsg.createNestedArray("StIP");
    for (short _i = 0; _i < 4; _i++) {
      _APIP.add(ControlerBoxes[gui8MyIndexInCBArray].APIP[_i]);
      _StIP.add(ControlerBoxes[gui8MyIndexInCBArray].stationIP[_i]);
    }
  }
  // if (MY_DG_MESH) {
  //  Serial.println("myMeshViews::_sendMsg(): added IPs to the JSON object before sending");
  // }


  // JSON serialization
  int size_buff = 254;
  char output[size_buff];

  // if (MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): about to serialize JSON object");
  // }
  serializeJson(_joMsg, output, size_buff);
  // if (MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): JSON object serialized");
  // }


  // JSON conversion to String for painlessMesh
  // if (MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): About to convert serialized object to String");
  // }
  String str;
  str = output;
  // if (MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): About to send message as String");
  // }

  // diffusion
  if (destNodeId) {
    laserControllerMesh.sendSingle(destNodeId, str);
  } else {
    laserControllerMesh.sendBroadcast(str);
  }


  if (MY_DG_MESH) {
    Serial.print("myMeshViews:_sendMsg(): done. Sent message: ");Serial.println(str);
  }
}










// Helper functions
// serialization of message to be sent
// JsonObject myMeshViews::_createJsonobject() {
//   const int capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
//   StaticJsonDocument<capacity> doc;
//
//   JsonObject msg = doc.to<JsonObject>();
//   return msg;
// }
