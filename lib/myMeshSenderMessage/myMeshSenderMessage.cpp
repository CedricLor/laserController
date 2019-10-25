/**
  myMeshSenderMessage.cpp - Library to provide a common way for other libraries to 
  pass messages to myMeshViews, which will be in charge of sending them over the mesh.
  Created by Cedric Lor, Octobre 25, 2019.
 * */
#include "Arduino.h"
#include "myMeshSenderMessage.h"



///////////////////////////////////////////////////////////////////////////////////
/** myMeshSenderMessage::myMeshSenderMessage():
 * 
 * */
myMeshSenderMessage::myMeshSenderMessage()
{ }




// void myMeshSenderMessage::statusMsg(uint32_t destNodeId) {
//   Serial.println("myMeshSenderMessage::statusMsg(): starting.");
  
//   // prepare the JSON string to be sent via the mesh
//   // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"boxDefstate":5;"NNa":"201";"APIP":"...";"StIP":"..."}

//   _joMsg = _jDoc.to<JsonObject>();

//   // load the JSON document with values
//   // _joMsg["actSt"] = _ctlBxColl.controllerBoxesArray.at(0).i16BoxActiveState;
//   // _joMsg["actStStartT"] = _ctlBxColl.controllerBoxesArray.at(0).ui32BoxActiveStateStartTime; // gets the recorded mesh time
//   // _joMsg["boxDefstate"] = _ctlBxColl.controllerBoxesArray.at(0).sBoxDefaultState;
//   // _joMsg["action"] = "s";

//   // send to the sender
//   // _sendMsg(_joMsg, destNodeId);

//   // I signaled my boxState change.
//   // => set my own boxActiveStateHasBeenSignaled to true
//   // _ctlBxColl.controllerBoxesArray.at(0).boxActiveStateHasBeenSignaled = true;

//   Serial.println("myMeshSenderMessage::statusMsg(): over.");
// }





// void myMeshSenderMessage::_droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB) {
//   constexpr int _capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
//   StaticJsonDocument<_capacity> _jDoc;
//   JsonObject _joMsg = _jDoc.to<JsonObject>();

//   // load the JSON document with values
//   _joMsg["action"] = "changeBox";
//   _joMsg["key"] = "dropped";
//   _joMsg["lb"] = _ui16droppedNodeIndexInCB;
//   _joMsg["st"] = 2;

//   _sendMsg(_joMsg);
// }





// /** _changeBoxRequest(JsonObject& _obj, bool _bBroadcast) is  */
// void myMeshSenderMessage::_changeBoxRequest(JsonObject& _obj, bool _bBroadcast) {
//   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
//   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
//   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.print("myMeshSenderMessage::_changeBoxRequest(): starting.\n");
//   }

//   // broadcast or send the message
//   if (_bBroadcast) {
//     _sendMsg(_obj);
//   } else {
//     // get the destination nodeId
//     uint32_t _destNodeId = _ctlBxColl.controllerBoxesArray.at(_obj["lb"].as<uint8_t>()).nodeId;
//     _sendMsg(_obj, _destNodeId);
//   }
//   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
//   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
//   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.println("myMeshSenderMessage::_changeBoxRequest(): over.");
//   }
// }





// void myMeshSenderMessage::_changedBoxConfirmation(JsonObject& obj) {
//   // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 2} // boxState // ancient 4
//   // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
//   // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.println("myMeshSenderMessage::_changedBoxConfirmation(): starting.");
//   }

//   // broadcast confirmation
//   _sendMsg(obj);

//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.println("myMeshSenderMessage::changedMasterBoxConfirmation(): over.");
//   }
// }





// void myMeshSenderMessage::_IRHighMsg() {
//   constexpr int _capacity = JSON_OBJECT_SIZE(6);
//   StaticJsonDocument<_capacity> _jDoc;
//   JsonObject _joMsg = _jDoc.to<JsonObject>();

//   // load the JSON document with values
//   _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
//   _joMsg["key"] = "IR";
//   _joMsg["time"] = _ctlBxColl.controllerBoxesArray.at(0).ui32lastRecPirHighTime;
//   _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();

//   // broadcast IR high message
//   _sendMsg(_joMsg);
// }





// void myMeshSenderMessage::_sendMsg(JsonObject& _joMsg, uint32_t destNodeId) {
//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.println("myMeshSenderMessage::_sendMsg(): starting.");
//     // Serial.println("myMeshSenderMessage::_sendMsg(): about to allote _ctlBxColl.controllerBoxesArray.at(0).ui16NodeName to _joMsg[\"senderNodeName\"]");
//   }

//   // adding my nodeName to the JSON to be sent to other boxes
//   _joMsg["NNa"] = _ctlBxColl.controllerBoxesArray.at(0).ui16NodeName;

//   // JSON serialization
//   int size_buff = 254;
//   char output[size_buff];

//   serializeJson(_joMsg, output, size_buff);

//   // JSON conversion to String for painlessMesh
//   String str;
//   str = output;

//   // diffusion
//   if (destNodeId) {
//     globalBaseVariables.laserControllerMesh.sendSingle(destNodeId, str);
//   } else {
//     globalBaseVariables.laserControllerMesh.sendBroadcast(str);
//   }


//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.print("myMeshViews:_sendMsg(): done. Sent message: ");Serial.println(str);
//   }
// }