/**
 * myMeshViews.cpp - Library to handle functions related to the JSON messages 
 * sent by the box over the mesh.
 * 
 * Created by Cedric Lor, January 22, 2019.
 * */
#include "Arduino.h"
#include "myMeshViews.h"



///////////////////////////////////////////////////////////////////////////////////
/** myMeshViews::myMeshViews(): 
 * 
 *  message sender for controller boxes
 * 
 *  myMeshViews needs to be instantiated by the method that will use it.
 * 
 *  Once instantiated it, the user has to call one of the provided message builders, that will:
 *  2. populate the JsonDocument/JsonObject with the passed-on parameters;
 *  3. passe this JsonObject to the _sendMsg method, which will call the send methods of painlessMesh.
 * 
 *  painlessMesh takes care of the broadcast/sending.
 * */
myMeshViews::myMeshViews(ControlerBox & __thisCtrlerBox):
  _thisCtrlerBox(__thisCtrlerBox)
{ }




///////////////////////////////////////////////////////////////////////////////////
/** ["action"] = "usi" -- upstream information ***********************************/
/////////////////////////////////////////////////////////////////////////////////// 
/** void myMeshViews::sendStatus(uint32_t destNodeId)
 * 
 *  Sends the current active boxState, the time at which it was set and the time at which such boxState
 *  was started.
 * */
void myMeshViews::sendStatus(uint32_t destNodeId) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "status";
  _joMsg["bs"] = _thisCtrlerBox.i16BoxActiveState;
  _joMsg["time"] = _thisCtrlerBox.ui32BoxActiveStateStartTime; // gets the recorded mesh time
  // _joMsg["dur"] = TO BE COMPLETED
  _joMsg["bds"] = _thisCtrlerBox.sBoxDefaultState;

  _sendMsg(_joMsg, destNodeId);
}





void myMeshViews::sendBoxState(const int16_t _i16BoxStateId) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "bs";
  _joMsg["bs"] = _i16BoxStateId;
  _joMsg["time"] = _thisCtrlerBox.ui32BoxActiveStateStartTime;
  // _joMsg["dur"] = TO BE COMPLETED

  _sendMsg(_joMsg);
}





void myMeshViews::sendSequence(const int16_t _i16SequenceIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "sq";
  /** to set "time", we need to save the time at which the sequence started 
   *  and pass it as an argument to this sendSequence() method. */
  _joMsg["time"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  /** to set "dur" (duration), we need to pass this function the duration of the sequence calculated
   *  in the sequence class. */
  // _joMsg["dur"] = TO BE COMPLETED;
  /** to set _joBeat, we need to pass this function the beat at which the sequence is being played. */
  JsonObject _joBeat = _joMsg["beat"].createNestedObject();
  // _joBeat["bpm"] = TO BE COMPLETED;
  // _joBeat["bnb"] = TO BE COMPLETED;
  _joMsg["sq"] = _i16SequenceIdNbr;

  _sendMsg(_joMsg);
}





void myMeshViews::sendBar(const int16_t _i16BarIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "ba";
  /** to set "time", we need to save the time at which the bar started 
   *  and pass it as an argument to this sendBar() method. */
  _joMsg["time"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  /** to set "dur" (duration), we need to pass this function the duration of the bar calculated
   *  in the bar class. */
  // _joMsg["dur"] = TO BE COMPLETED;
  /** to set _joBeat, we need to pass this function the beat at which the bar is being played. */
  JsonObject _joBeat = _joMsg["beat"].createNestedObject();
  // _joBeat["bpm"] = TO BE COMPLETED;
  // _joBeat["bnb"] = TO BE COMPLETED;
  _joMsg["ba"] = _i16BarIdNbr;

  _sendMsg(_joMsg);
}





void myMeshViews::sendNote(const uint16_t _ui16LaserToneIdNbr, const uint16_t _ui16LaserNote) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "nt";
  /** to set "time", we need to save the time at which the bar started 
   *  and pass it as an argument to this sendBar() method. */
  _joMsg["time"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  /** to set "dur" (duration), we need to pass this function the duration of the bar calculated
   *  in the bar class. */
  // _joMsg["dur"] = TO BE COMPLETED;
  /** to set _joBeat, we need to pass this function the beat at which the bar is being played. */
  JsonObject _joBeat = _joMsg["beat"].createNestedObject();
  // _joBeat["bpm"] = TO BE COMPLETED;
  // _joBeat["bnb"] = TO BE COMPLETED;
  _joMsg["tn"] = _ui16LaserToneIdNbr;
  _joMsg["nt"] = _ui16LaserNote;

  _sendMsg(_joMsg);
}





void myMeshViews::sendTone(const uint16_t _ui16LaserToneIdNbr) {
  _jDoc.clear();
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "tn";
  _joMsg["time"] = globalBaseVariables.laserControllerMesh.getNodeTime();
  _joMsg["tn"] = _ui16LaserToneIdNbr;

  _sendMsg(_joMsg);
}





void myMeshViews::sendIRHigh(uint32_t _ui32IRHighTime) {
  _jDoc.clear();

  // load the JSON document with values
  _joMsg["action"] = "usi"; // "usi" for upstream information (from the ControlerBox to the Mesh)
  _joMsg["key"] = "IR";
  _joMsg["time"] = _ui32IRHighTime;

  // broadcast IR high message
  _sendMsg(_joMsg);
}





///////////////////////////////////////////////////////////////////////////////////
/** ["action"] = "changeBox" -- upstream information *****************************/
/////////////////////////////////////////////////////////////////////////////////// 
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
  Serial.println("myMeshViews::_changedBoxConfirmation(): starting");

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

  Serial.println("myMeshViews::_changedBoxConfirmation(): over");
}





void myMeshViews::_sendMsg(JsonObject& _joMsg, uint32_t destNodeId) {
  Serial.println("myMeshViews::_sendMsg(): starting");

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