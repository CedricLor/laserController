/**
 * myMeshViews.cpp - Library to handle functions related to the JSON messages 
 * sent by the box over the mesh.
 * 
 * Created by Cedric Lor, January 22, 2019.
 * */
#include "Arduino.h"
#include "myMeshViews.h"



///////////////////////////////////////////////////////////////////////////////////
/** myMeshViews::myMeshViews(): message sender for controller boxes
 * 
 * Upon instantiation, the constructor updates the properties of this ControlerBox
 * in the array of ControlerBoxes (APIP, StationIP, NodeName).
 * After having instantiating it, you have to call a message builder, that will:
 * 2. create a structured String with the params you passed it on;
 * 3. passe this String to the sendBroadcast or sendSingle methods of painlessMesh.
 * painlessMesh takes care of the broadcast/sending.
 * */
myMeshViews::myMeshViews(controllerBoxesCollection & __ctlBxColl):
  _ctlBxColl(__ctlBxColl)
{
  _ctlBxColl.controllerBoxesArray.at(0).updateThisBoxProperties();
}




void myMeshViews::statusMsg(uint32_t destNodeId) {
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.println("myMeshViews::statusMsg(): starting.");
  }
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"boxDefstate":5;"NNa":"201";"APIP":"...";"StIP":"..."}

  constexpr int _capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<_capacity> _jDoc;
  JsonObject _joMsg = _jDoc.to<JsonObject>();

  // load the JSON document with values
  _joMsg["actSt"] = cntrllerBoxesCollection.controllerBoxesArray.at(0).i16BoxActiveState;
  _joMsg["actStStartT"] = cntrllerBoxesCollection.controllerBoxesArray.at(0).ui32BoxActiveStateStartTime; // gets the recorded mesh time
  _joMsg["boxDefstate"] = cntrllerBoxesCollection.controllerBoxesArray.at(0).sBoxDefaultState;
  _joMsg["action"] = "s";

  // send to the sender
  _sendMsg(_joMsg, destNodeId);

  // I signaled my boxState change.
  // => set my own boxActiveStateHasBeenSignaled to true
  cntrllerBoxesCollection.controllerBoxesArray.at(0).boxActiveStateHasBeenSignaled = true;

  if (globalBaseVariables.MY_DG_MESH) {
    Serial.println("myMeshViews::statusMsg(): over.");
  }
}





void myMeshViews::_droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB) {
  constexpr int _capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<_capacity> _jDoc;
  JsonObject _joMsg = _jDoc.to<JsonObject>();

  // load the JSON document with values
  _joMsg["action"] = "changeBox";
  _joMsg["key"] = "dropped";
  _joMsg["lb"] = _ui16droppedNodeIndexInCB;
  _joMsg["st"] = 2;

  _sendMsg(_joMsg);
}





/** _changeBoxRequest(JsonObject& _obj, bool _bBroadcast) is  */
void myMeshViews::_changeBoxRequest(JsonObject& _obj, bool _bBroadcast) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.print("myMeshViews::_changeBoxRequest(): starting.\n");
  }

  // broadcast or send the message
  if (_bBroadcast) {
    _sendMsg(_obj);
  } else {
    // get the destination nodeId
    uint32_t _destNodeId = cntrllerBoxesCollection.controllerBoxesArray.at(_obj["lb"].as<uint8_t>()).nodeId;
    _sendMsg(_obj, _destNodeId);
  }
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

  if (globalBaseVariables.MY_DG_MESH) {
    Serial.println("myMeshViews::_changeBoxRequest(): over.");
  }
}





void myMeshViews::_changedBoxConfirmation(JsonObject& obj) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 2} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.println("myMeshViews::_changedBoxConfirmation(): starting.");
  }

  // broadcast confirmation
  _sendMsg(obj);

  if (globalBaseVariables.MY_DG_MESH) {
    Serial.println("myMeshViews::changedMasterBoxConfirmation(): over.");
  }
}





void myMeshViews::_IRHighMsg() {
  constexpr int _capacity = JSON_OBJECT_SIZE(6);
  StaticJsonDocument<_capacity> _jDoc;
  JsonObject _joMsg = _jDoc.to<JsonObject>();

  // load the JSON document with values
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "IR";
  _joMsg["time"] = cntrllerBoxesCollection.controllerBoxesArray.at(0).ui32lastRecPirHighTime;
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();

  // broadcast IR high message
  _sendMsg(_joMsg);
}





/** _addIps(JsonObject& _joMsg) left unused for the moment. */
void myMeshViews::_addIps(JsonObject& _joMsg) {
  // adding the APIP and the StationIP to the JSON to be sent to other boxes
  if (_joMsg.containsKey("APIP") && _joMsg.containsKey("StIP")) {
    for (short _i = 0; _i < 4; _i++) {
      _joMsg["APIP"][_i] = cntrllerBoxesCollection.controllerBoxesArray.at(0).APIP[_i];
      _joMsg["StIP"][_i] = cntrllerBoxesCollection.controllerBoxesArray.at(0).stationIP[_i];
    }
  } else {
    JsonArray _APIP = _joMsg.createNestedArray("APIP");
    JsonArray _StIP = _joMsg.createNestedArray("StIP");
    for (short _i = 0; _i < 4; _i++) {
      _APIP.add(cntrllerBoxesCollection.controllerBoxesArray.at(0).APIP[_i]);
      _StIP.add(cntrllerBoxesCollection.controllerBoxesArray.at(0).stationIP[_i]);
    }
  }
  // if (globalBaseVariables.MY_DG_MESH) {
  //  Serial.println("myMeshViews::_sendMsg(): added IPs to the JSON object before sending");
  // }
}






void myMeshViews::_sendMsg(JsonObject& _joMsg, uint32_t destNodeId) {
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.println("myMeshViews::_sendMsg(): starting.");
    // Serial.println("myMeshViews::_sendMsg(): about to allote cntrllerBoxesCollection.controllerBoxesArray.at(0).ui16NodeName to _joMsg[\"senderNodeName\"]");
  }

  // adding my nodeName to the JSON to be sent to other boxes
  _joMsg["NNa"] = cntrllerBoxesCollection.controllerBoxesArray.at(0).ui16NodeName;
  // if (globalBaseVariables.MY_DG_MESH) {
  //  Serial.println("myMeshViews::_sendMsg(): about to allocate APIP to _joMsg[\"senderAPIP\"]");
  // }

  // JSON serialization
  int size_buff = 254;
  char output[size_buff];

  // if (globalBaseVariables.MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): about to serialize JSON object");
  // }
  serializeJson(_joMsg, output, size_buff);
  // if (globalBaseVariables.MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): JSON object serialized");
  // }


  // JSON conversion to String for painlessMesh
  // if (globalBaseVariables.MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): About to convert serialized object to String");
  // }
  String str;
  str = output;
  // if (globalBaseVariables.MY_DG_MESH) {
  //   Serial.println("myMeshViews::_sendMsg(): About to send message as String");
  // }

  // diffusion
  if (destNodeId) {
    globalBaseVariables.laserControllerMesh.sendSingle(destNodeId, str);
  } else {
    globalBaseVariables.laserControllerMesh.sendBroadcast(str);
  }


  if (globalBaseVariables.MY_DG_MESH) {
    Serial.print("myMeshViews:_sendMsg(): done. Sent message: ");Serial.println(str);
  }
}