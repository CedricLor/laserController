/*
myMeshController.cpp - Library that decodes the requests coming from the mesh 
and executes them.
Created by Cedric Lor, January 22, 2019.
*/

#include "Arduino.h"
#include "myMeshController.h"




/** *meshInit param: 
 *  1. trying to pass the static method myMesh::init() to myMeshController 
 *  so that myMeshController can call myMesh::init() to restart the mesh.
 *  2. trying to save the method as non static member of myMesh ->
 *   _meshInit(meshInit) */
myMeshController::myMeshController(uint32_t _ui32SenderNodeId, String &_msg/*, void (*meshInit)()*/)
: /*_meshInit(meshInit),*/ _ui32SenderNodeId(_ui32SenderNodeId)
{
  if (globalBaseVariables.MY_DG_MESH) Serial.printf("myMeshController::myMeshController(): starting. &_msg == %s \n", _msg.c_str());

  // Convert the document to an object
  _nsobj = _nsdoc.to<JsonObject>();
  
  // deserialize the message _msg received from the mesh into the StaticJsonDocument _nsdoc
  DeserializationError _err = deserializeJson(_nsdoc, _msg);
  if (globalBaseVariables.MY_DG_MESH) Serial.print("myMeshController::myMeshController(): DeserializationError = ");Serial.print(_err.c_str());Serial.print("\n");
  
  // parse and act
  _main();
}







void myMeshController::_main()
{
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.print("myMeshController::_main: starting\n");
  }
  
  // read the action field
  const char* _action = _nsobj["action"];

  // if debug, serial print the action field
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.printf("myMeshController::_main: _action = %s\n", _action);
  }




  /** STATUS MESSAGE (received by all, sent by LBs only).
   *  The boxState of another box has changed and is being
   *  signalled to the mesh.
   *  
   *  Upon receiving a status message from another box,
   *  read and save the relevant information (on the other
   *  box) in my controller boxes' array. */
  if (_nsobj["action"] == "s") {
    const uint16_t __ui16BoxIndex = thisControllerBox.thisSignalHandler.ctlBxColl._updateOrCreate(_ui32SenderNodeId, _nsobj);
    if (thisControllerBox.globBaseVars.hasLasers) {
      thisControllerBox.thisSignalHandler.checkImpactOfChangeInActiveStateOfOtherBox(__ui16BoxIndex);
    }
    return;
  }




  /** USI MESSAGE (received by all, sent by LBs only).
   *  Another box is broadcasting "usi" (i.e. upstream 
   *  information) to the mesh.
   * 
   *  For the moment, "usi" are limited to IR High 
   *  information.
   *  
   *  Upon receiving a "usi" message from another box,
   *  read and save the relevant information (on the other
   *  box) in my controller boxes' array. */
  if (_nsobj["action"] == "usi") {
    const uint16_t __ui16BoxIndex = thisControllerBox.thisSignalHandler.ctlBxColl._updateOrCreate(_ui32SenderNodeId, _nsobj);
    if (thisControllerBox.globBaseVars.hasLasers) {
      thisControllerBox.thisSignalHandler.checkImpactOfUpstreamInformationOfOtherBox(__ui16BoxIndex);
    }
    return;
  }




  /** CHANGEBOX REQUEST AND CONFIRMATION:
   *  
   *  Two scenarii: 
   *  (i) request: coming from the web, forwarded by the IF to the 
   *  relevant LB only or broadcasted to all the LBs;
   *  (ii) confirmation: always broadcasted to all the boxes (including
   *  root or IF).
   *  
   *  All the messages always relates to requested or confirmed changes in
   *  the active state or the default state of one or several boxes.
   * 
   *  Upon reception of such messages, this box should update the corresponding values 
   *  in the controller boxes' array. 
   * 
   *  If the recipient (i.e. this box) is the IF, this change will be detected by 
   *  myWSSender and a message will be sent to the browser.
   * 
   *  If the recipient (i.e. this box) is another LB, this change might be detected by 
   *  boxStates and trigger a state change of this box. */
  if (_nsobj["action"] == "changeBox") {
    // Serial.println("------------------------------ DETECTED A \"changeBox\" MESSAGE ---------------------------");
    _changeBox();
    return;
  }
}






/** CHANGEBOX REQUEST AND CONFIRMATION */
/* myMeshController::_changeBox(): called upon receiving a message marked with _nsobj["action"] == "changeBox".
 * 
 * Switches between changeBox requests and changeBox confirmations.
 * 
 * "changeBox requests" are sent by the IF to the laser controllers ONLY.
 * "changeBox confirmations" are sent by the laser controllers and are received (and acted upon) by 
 *  BOTH the IF and the laser controllers. */
void myMeshController::_changeBox() {
  // ON REQUEST:
  // _nsobj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  // ON CONFIRMATION:
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // if this is a change request
  if (_nsobj["st"].as<uint8_t>() == 1) {
      // Serial.println("------------------------------ THIS IS A CHANGE REQUEST ---------------------------");
      _changeBoxRequest();

    return;
  }

  // if this is a change confirmation
  if (_nsobj["st"].as<uint8_t>() == 2) {
      // Serial.println("------------------------------ THIS IS A CHANGE CONFIRMATION ---------------------------");
      _changedBoxConfirmation();

    return;
  }
}






/** CHANGEBOX REQUEST: 
 *  
 *  Correspond to changes requested by the user through the web interface.
 * 
 *  In the mesh, changeBox request (_obj["action"] == "changeBox") are:
 *  - emitted by the IF only; 
 *  - received by the laser boxes only. */
void myMeshController::_changeBoxRequest() {

  // if this is a change active state request
  // _nsobj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (_nsobj["key"] == "boxState") {
    // Serial.println("------------------------------ THIS IS A CHANGE BOXSTATE REQUEST ---------------------------");
    _updateMyValFromWeb();
    return;
  }

  // if this is a change default state request
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (_nsobj["key"] == "boxDefstate") {
    // Serial.println("------------------------------ THIS IS A CHANGE DEFSTATE REQUEST ---------------------------");
    _updateMyDefaultState();
    return;
  }

  // if this is a reboot  request
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (_nsobj["key"] == "reboot") {
    Serial.println("------------------------------ THIS IS A REBOOT REQUEST ---------------------------");
    _rebootEsp();
    return;
  }

  // if this is a save request
  // _nsobj = {action: "changeBox"; key: "save"; lb: 1, val: "all"} // save all the values
  if ((_nsobj["key"] == "save") && (_nsobj["val"] == "all")) {
    Serial.println("------------------------------ THIS IS A SAVE REQUEST ---------------------------");
    _save();
    return;
  }

  // if this is a save wifi values request
  // _nsobj = {action: "changeBox"; key: "save"; lb: 1, val: "wifi"} // save the external wifi values
  if (( (_nsobj["key"] == "save") || (_nsobj["key"] == "apply") ) && ((_nsobj["val"] == "wifi") || (_nsobj["val"] == "softAP") || (_nsobj["val"] == "mesh") || (_nsobj["val"] == "RoSet") || (_nsobj["val"] == "IFSet") )) {
    Serial.printf("------------------------------ THIS IS A SAVE %s REQUEST ---------------------------\n", _nsobj["val"].as<const char*>());
    _specificSave();
    if (_nsobj["key"] == "apply") {
      myMeshStarter::tRestart.restartDelayed();
    }
    return;
  }

  // if this is a "OTA save and reboot" request
  // _nsobj = {action: "changeBox"; key: "save"; lb: 1, val: "gi8RequestedOTAReboots"}
  if ((_nsobj["key"] == "save") && (_nsobj["val"] == "gi8RequestedOTAReboots")) {
    Serial.println("------------------------------ THIS IS AN OTA SAVE AND REBOOT REQUEST ---------------------------");
    _savegi8RequestedOTAReboots();
    return;
  }
}







// CHANGED BOX CONFIRMATION (received by the interface and all the other boxes)
void myMeshController::_changedBoxConfirmation() {
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // get the index number of the sender
  uint16_t __ui16BoxIndex = _nsobj["lb"];

  // if this is a "change default state request" confirmation
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  if (_nsobj["key"] == "boxDefstate") {
    // Serial.println("----------------- THIS A DEFAULT STATE CONFIRMATION ---------------");
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(__ui16BoxIndex)._setBoxDefaultState(_nsobj["val"].as<uint16_t>());
    return;
  }

  // if this is a "reboot" confirmation
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1; save: 1, st: 2} // boxDefstate // ancient 9
  if ((_nsobj["key"] == "reboot") || (_nsobj["key"] == "dropped")) {
    // Serial.println("----------------- THIS A REBOOT CONFIRMATION ---------------");
    thisControllerBox.thisSignalHandler.ctlBxColl.deleteBoxByBoxIndex(__ui16BoxIndex);
    // only decrease the MeshSize by one if it is a dropped connection message
    // note: dropped connection message are sent by the box which first detected
    // the dropped box. Such box has already updated its globalBaseVariables.uiMeshSize in the
    // dropped connection callback, in myMesh class
    if (_nsobj["key"] == "dropped"){
      globalBaseVariables.uiMeshSize = globalBaseVariables.uiMeshSize - 1;
    }
    return;
  }

  // if this is a "save" confirmation
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1; save: 1, st: 2} // boxDefstate // ancient 9
  if (_nsobj["key"] == "save") {
    // Serial.println("----------------- THIS A SAVE CONFIRMATION ---------------");
    myWSSender _myWSSender(_myWebServer.getAsyncWebSocketInstance());
    _myWSSender.sendWSData(_nsobj);
    return;
  }
}




/////////////////
// HELPERS
////////////////









/** HELPER FUNCTIONS for _changeBoxRequest (on _obj["action"] = "changeBox"):
 *  _updateMyValFromWeb();
 *  _updateMyDefaultState();
 *  _rebootEsp();
 *  _save();
 *  _specificSave();
 *  _savegi8RequestedOTAReboots(); */
void myMeshController::_updateMyValFromWeb() {
// _nsobj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyValFromWeb: will change my target state to [%i]\n", (_nsobj["val"].as<int16_t>()));
  }

  // update variable i16boxStateRequestedFromWeb in the signal class
  if (thisControllerBox.globBaseVars.hasLasers) {
    thisControllerBox.thisSignalHandler.setBoxActiveStateFromWeb(_nsobj["val"].as<int8_t>());
  }
  /** not sending any confirmation, as boxState will send an automatic
   *  status message. */
}



void myMeshController::_updateMyDefaultState() {
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyDefaultState: will change my default state to [%u]\n", _nsobj["val"].as<uint16_t>());
  }

  thisControllerBox.thisCtrlerBox.sBoxDefaultState = _nsobj["val"].as<uint16_t>();

  // send confirmation message
  thisControllerBox.thisMeshViews._changedBoxConfirmation(_nsobj);

  // mark the change as signaled
  thisControllerBox.thisCtrlerBox.sBoxDefaultStateChangeHasBeenSignaled = true;
}



void myMeshController::_rebootEsp() {
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (globalBaseVariables.MY_DG_MESH) {
    Serial.printf("myMeshController::_rebootEsp: about to reboot\n");
  }

  // save preferences if requested
  if (_nsobj["save"] == 1) {
    _save();
  }

  // broadcast confirmation message
  Serial.println("------------------------------ CONFIRMING REBOOT ---------------------------");
  thisControllerBox.thisMeshViews._changedBoxConfirmation(_nsobj);

  // reboot
  Serial.println("------------------------------ ABOUT TO REBOOT ---------------------------");
  thisControllerBox.tReboot.restartDelayed();
}



void myMeshController::_save() {
  // {action: "changeBox", key: "save", val: "all", lb: 1} // save all the values that can be saved
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.savePrefsWrapper();

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE ---------------------------");
    // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate 
  thisControllerBox.thisMeshViews._changedBoxConfirmation(_nsobj);
}



void myMeshController::_specificSave() {
  // {action: "changeBox", key: "save", val: "wifi", lb: 1} // save external wifi values

  // if this is an "OTA save and reboot" or a "wifi save" request
  mySavedPrefs::saveFromNetRequest(_nsobj);

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE WIFI ---------------------------");
  thisControllerBox.thisMeshViews._changedBoxConfirmation(_nsobj);
}



void myMeshController::_savegi8RequestedOTAReboots() {
  // {action: "changeBox", key: "save", lb: 1, val: "gi8RequestedOTAReboots"}

  // if this is an "OTA save and reboot" request
  mySavedPrefs::saveFromNetRequest(_nsobj);

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE OTA ---------------------------");
  thisControllerBox.thisMeshViews._changedBoxConfirmation(_nsobj);

  // reboot
  Serial.println("------------------------------ ABOUT TO REBOOT ---------------------------");
  thisControllerBox.tReboot.restartDelayed();
}










// _changedBoxConfirmation HELPER FUNCTIONS



// PROTOTYPE FOR A MORE ABSTRACT CHANGE PROPERTY HANDLER
// void myMeshController::_updateMyProperty(char& _cPropertyKey, JsonObject& _nsobj) {
//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateMyProperty: will change my property %s to %u\n", _nsobj[_cPropertyKey].as<uint8_t>());
//   }
//
//   // update property and propertyChangeHasBeenSignaled for my box
//   _updatePropertyForBox(_cPropertyKey, gui16MyIndexInCBArray, _nsobj)
//
//   // send confirmation message
//   thisControllerBox.thisMeshViews._changedBoxConfirmation(_nsobj);
//
//   // mark the change as signaled
//   thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(__ui16BoxIndex)._cPropertyKey = true;
// }


// attempt to abstract even more the process
// void myMeshController::_updatePropertyForBox(char& _cPropertyKey, uint8_t _ui16BoxIndex, JsonObject& _nsobj) {
//   // get the new property from the JSON
//   int8_t __i8PropertyValue = _nsobj[_cPropertyKey].as<uint8_t>();
//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: %s = %i\n", _cPropertyKey, __i8PropertyValue);
//   }
//
//   // 1. set the new property of the relevant Controler Box in 
//   // the controller boxes's array
//   // 2. set the bool announcing that the change has not been signaled,
//   // to have it caught by the webServerTask (on the interface).
//   // TODO:
//   // a. thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).updateProperty needs to be drafted
//   thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).updateProperty(_cPropertyKey, __i8PropertyValue);
//
//   if (globalBaseVariables.MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%u), property %s has been updated to %i\n", _ui16BoxIndex, _cPropertyKey, __i8PropertyValue);
//   }
// }

