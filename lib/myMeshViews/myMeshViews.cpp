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
{ }




void myMeshViews::statusMsg(uint32_t destNodeId) {
  Serial.println("myMeshViews::statusMsg(): starting.");
  
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"boxDefstate":5;"NNa":"201";"APIP":"...";"StIP":"..."}

  myMeshSenderMessage _msg;

  // load the JSON document with values
  _msg._joMsg["actSt"] = _ctlBxColl.controllerBoxesArray.at(0).i16BoxActiveState;
  _msg._joMsg["actStStartT"] = _ctlBxColl.controllerBoxesArray.at(0).ui32BoxActiveStateStartTime; // gets the recorded mesh time
  _msg._joMsg["boxDefstate"] = _ctlBxColl.controllerBoxesArray.at(0).sBoxDefaultState;
  _msg._joMsg["action"] = "s";

  // send to the sender
  _sendMsg(_msg._joMsg, destNodeId);

  // I signaled my boxState change.
  // => set my own boxActiveStateHasBeenSignaled to true
  _ctlBxColl.controllerBoxesArray.at(0).boxActiveStateHasBeenSignaled = true;

  Serial.println("myMeshViews::statusMsg(): over.");
}





void myMeshViews::_droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB) {
  myMeshSenderMessage _msg;

  // load the JSON document with values
  _msg._joMsg["action"] = "changeBox";
  _msg._joMsg["key"] = "dropped";
  _msg._joMsg["lb"] = _ui16droppedNodeIndexInCB;
  _msg._joMsg["st"] = 2;

  _sendMsg(_msg._joMsg);
}





/** _changeBoxRequest(JsonObject& _obj, bool _bBroadcast) is  */
void myMeshViews::_changeBoxRequest(JsonObject& _obj, bool _bBroadcast) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  Serial.print("myMeshViews::_changeBoxRequest(): starting.\n");

  // broadcast or send the message
  if (_bBroadcast) {
    _sendMsg(_obj);
  } else {
    // get the destination nodeId
    uint32_t _destNodeId = _ctlBxColl.controllerBoxesArray.at(_obj["lb"].as<uint8_t>()).nodeId;
    _sendMsg(_obj, _destNodeId);
  }
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

  Serial.println("myMeshViews::_changeBoxRequest(): over.");
}





void myMeshViews::_changedBoxConfirmation(JsonObject& obj) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 2} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  Serial.println("myMeshViews::_changedBoxConfirmation(): starting.");

  // broadcast confirmation
  _sendMsg(obj);

  Serial.println("myMeshViews::changedMasterBoxConfirmation(): over.");
}





void myMeshViews::_IRHighMsg() {
  myMeshSenderMessage _msg;

  // load the JSON document with values
  _msg._joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _msg._joMsg["key"] = "IR";
  _msg._joMsg["time"] = _ctlBxColl.controllerBoxesArray.at(0).ui32lastRecPirHighTime;
  _msg._joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();

  // broadcast IR high message
  _sendMsg(_msg._joMsg);
}





void myMeshViews::_sendMsg(JsonObject& _joMsg, uint32_t destNodeId) {
  Serial.println("myMeshViews::_sendMsg(): starting.");

  // adding my nodeName to the JSON to be sent to other boxes
  _joMsg["NNa"] = _ctlBxColl.controllerBoxesArray.at(0).ui16NodeName;

  // JSON serialization
  int size_buff = 254;
  char output[size_buff];

  serializeJson(_joMsg, output, size_buff);

  // JSON conversion to String for painlessMesh
  String str;
  str = output;

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