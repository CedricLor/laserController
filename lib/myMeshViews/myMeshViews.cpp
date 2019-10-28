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
myMeshViews::myMeshViews(ControlerBox & __thisCtrlerBox):
  _thisCtrlerBox(__thisCtrlerBox)
{ }




void myMeshViews::statusMsg(uint32_t destNodeId) {
  Serial.println("myMeshViews::statusMsg(): starting.");
  
  // prepare the JSON string to be sent via the mesh
  // expected JSON string: {"actSt":3;"action":"s";"actStStartT":6059117;"boxDefstate":5;"NNa":"201";"APIP":"...";"StIP":"..."}

  _jDoc.clear();

  // load the JSON document with values
  _joMsg["actSt"] = _thisCtrlerBox.i16BoxActiveState;
  _joMsg["actStStartT"] = _thisCtrlerBox.ui32BoxActiveStateStartTime; // gets the recorded mesh time
  _joMsg["boxDefstate"] = _thisCtrlerBox.sBoxDefaultState;
  _joMsg["action"] = "s";

  // send to the sender
  _sendMsg(_joMsg, destNodeId);

  // I signaled my boxState change.
  // => set my own boxActiveStateHasBeenSignaled to true
  _thisCtrlerBox.boxActiveStateHasBeenSignaled = true;

  Serial.println("myMeshViews::statusMsg(): over.");
}





void myMeshViews::sendBoxState(const int16_t _i16BoxStateIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "bs";
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  _joMsg["bs"] = _i16BoxStateIdNbr;
}





void myMeshViews::sendSequence(const int16_t _i16SequenceIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "sq";
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  _joMsg["bs"] = _i16SequenceIdNbr;
}





void myMeshViews::sendBar(const int16_t _i16BarIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "ba";
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  _joMsg["bs"] = _i16BarIdNbr;
}





void myMeshViews::sendNote(const uint16_t _ui16LaserToneIdNbr, const uint16_t _ui16LaserNote) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "nt";
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  _joMsg["tn"] = _ui16LaserToneIdNbr;
  _joMsg["nt"] = _ui16LaserNote;
}





void myMeshViews::sendTone(const uint16_t _ui16LaserToneIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "tn";
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  _joMsg["bs"] = _ui16LaserToneIdNbr;
}





void myMeshViews::_droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB) {
  _jDoc.clear();

  // load the JSON document with values
  _joMsg["action"] = "changeBox";
  _joMsg["key"] = "dropped";
  _joMsg["lb"] = _ui16droppedNodeIndexInCB;
  _joMsg["st"] = 2;

  _sendMsg(_joMsg);
}




/** void myMeshViews::_changedBoxConfirmation(JsonObject& obj)
 * 
 *  Broadcasts a message to the mesh, with particular purpose to the IF,
 *  confirming that this box has executed its change box request.
 * 
 *  The param JsonObject& __obj is the change box message sent by the IF.
*/
void myMeshViews::_changedBoxConfirmation(JsonObject& __obj) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 2} // boxState // ancient 4
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  Serial.println("myMeshViews::_changedBoxConfirmation(): starting.");

  // change the "st" key of the received JSON object from 1 (request forwarded) to 2 (request executed)
  __obj["st"] = 2;
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: _obj[\"st\"] = %u\n", __obj["st"].as<uint16_t>());
  }

  /** if the message was a "changeNet" request, it was broadcasted and
   *  its "lb" field was equal to either "LBs" or "all";
   * 
   *  replace it with thix box's index number so that the controller boxes' array
   *  be properly updated from the _changedBoxConfirmation method of the receiving box's
   *  meshController. */
  if ((__obj["lb"] == "LBs") || (__obj["lb"] == "all")) {
    __obj["lb"] = _thisCtrlerBox.ui16NodeName - globalBaseVariables.gui16ControllerBoxPrefix;
  }

  // broadcast confirmation
  _sendMsg(__obj);

  Serial.println("myMeshViews::_changedBoxConfirmation(): over.");
}





void myMeshViews::_IRHighMsg(uint32_t _ui32IRHighTime) {
  _jDoc.clear();

  // load the JSON document with values
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "IR";
  _joMsg["time"] = _ui32IRHighTime;
  _joMsg["now"] = globalBaseVariables.laserControllerMesh.getNodeTime();

  // broadcast IR high message
  _sendMsg(_joMsg);
}





void myMeshViews::_sendMsg(JsonObject& _joMsg, uint32_t destNodeId) {
  Serial.println("myMeshViews::_sendMsg(): starting.");

  // adding my nodeName to the JSON to be sent to other boxes
  _joMsg["NNa"] = _thisCtrlerBox.ui16NodeName;

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