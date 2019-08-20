/*
myMeshController.cpp - Library to handle mesh controller related functions.
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
#include "myMeshController.h"




myMeshController::myMeshController(uint32_t _ui32SenderNodeId, String &_msg) 
: _ui32SenderNodeId(_ui32SenderNodeId)
{
  if (MY_DG_MESH) Serial.printf("myMeshController::myMeshController(): starting. &_msg == %s \n", _msg.c_str());

  // Convert the document to an object
  _nsobj = _nsdoc.to<JsonObject>();
  
  // deserialize the message _msg received from the mesh into the StaticJsonDocument _nsdoc
  DeserializationError _err = deserializeJson(_nsdoc, _msg);
  if (MY_DG_MESH) Serial.print("myMeshController::myMeshController(): DeserializationError = ");Serial.print(_err.c_str());Serial.print("\n");
  
  // parse and act
  _main();
}







void myMeshController::_main()
{
  if (MY_DG_MESH) {
    Serial.print("myMeshController::_main: Starting\n");
  }
  
  // read the action field
  const char* _action = _nsobj["action"];

  // if debug, serial print the action field
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_main: _action = %s\n", _action);
  }




  // STATUS MESSAGE (received by all, sent by LBs only).
  /* Upon receiving a status message from another box,
     read and save the state of the other boxes */
  const char* _s = "s";
  if (strcmp(_action, _s) == 0) {

    _statusMessage();
    return;
  }




  // CHANGEBOX REQUEST AND CONFIRMATION (received by the destination laser boxes only on request and by all on confirmation)
  const char* _actionChangeBox = "changeBox";
  if (strcmp(_action, _actionChangeBox) == 0) {           // action 'changeBox' for this message relates to a change in active state, default state or master node number, that this box should update as the case may be
    // Serial.println("------------------------------ DETECTED A \"changeBox\" MESSAGE ---------------------------");
    _changeBox();
    return;
  }
}






// STATUS MESSAGE (received by all, sent by LBs only).
void myMeshController::_statusMessage() {
  /*
    action 's': the boxState of another box has changed and is being
    signalled to the mesh. This box shall update its ControlerBoxes[] array
    with the values received from the other box.
  */

  // Setting nodeName, nodeId and IP properties
  // extract the index of the relevant box from its senderNodeName in the JSON
  uint16_t __ui16NodeName = _nsobj["NNa"]; // ex. 201
  Serial.printf("myMeshController::_statusMessage(): __ui16NodeName = %u\n", __ui16NodeName);
  uint16_t __ui16BoxIndex = ControlerBox::findIndexByNodeName(__ui16NodeName);

  // update the box properties in my CB array
  __ui16BoxIndex = ((__ui16BoxIndex == 254) ? ControlerBox::connectedBoxesCount : __ui16BoxIndex);
  ControlerBoxes[__ui16BoxIndex].updateOtherBoxProperties(_ui32SenderNodeId, _nsobj, __ui16BoxIndex);
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
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  // ON CONFIRMATION:
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
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






// CHANGEBOX REQUEST: emitted by the IF and received by the laser boxes only
void myMeshController::_changeBoxRequest() {

  // if this is a change active state request
  // _nsobj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (_nsobj["key"] == "boxState") {
    // Serial.println("------------------------------ THIS IS A CHANGE BOXSTATE REQUEST ---------------------------");
    _updateMyValFromWeb();
    return;
  }

  // if this is a change master box request
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  if (_nsobj["key"] == "masterbox") {
    // Serial.println("------------------------------ THIS IS A CHANGE MASTERBOX REQUEST ---------------------------");
    _updateMyMasterBoxName();
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
  if ((_nsobj["key"] == "save") && (_nsobj["val"] == "wifi")) {
    Serial.println("------------------------------ THIS IS A SAVE WIFI REQUEST ---------------------------");
    _saveWifi();
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
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // get the index number of the sender
  uint16_t __ui16BoxIndex = _nsobj["lb"];

  // if this is a "change master box request" confirmation
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  if (_nsobj["key"] == "masterbox") {
    // Serial.println("----------------- THIS A MASTERBOX CONFIRMATION ---------------");
    ControlerBoxes[__ui16BoxIndex].updateMasterBoxNameFromWeb(_nsobj["val"].as<uint16_t>());
    return;
  }

  // if this is a "change default state request" confirmation
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  if (_nsobj["key"] == "boxDefstate") {
    // Serial.println("----------------- THIS A DEFAULT STATE CONFIRMATION ---------------");
    ControlerBoxes[__ui16BoxIndex].setBoxDefaultState(_nsobj["val"].as<uint16_t>());
    return;
  }

  // if this is a "reboot" confirmation
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1; save: 1, st: 2} // boxDefstate // ancient 9
  if ((_nsobj["key"] == "reboot") || (_nsobj["key"] == "dropped")) {
    // Serial.println("----------------- THIS A REBOOT CONFIRMATION ---------------");
    ControlerBoxes[__ui16BoxIndex].deleteBox();
    // only decrease the MeshSize by one if it is a dropped connection message
    // note: dropped connection message are sent by the box which first detected
    // the dropped box. Such box has already updated its uiMeshSize in the
    // dropped connection callback, in myMesh class
    if (_nsobj["key"] == "dropped"){
      uiMeshSize = uiMeshSize - 1;
    }
    return;
  }

  // if this is a "save" confirmation
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1; save: 1, st: 2} // boxDefstate // ancient 9
  if (_nsobj["key"] == "save") {
    // Serial.println("----------------- THIS A SAVE CONFIRMATION ---------------");
    myWSSender _myWSSender;
    _myWSSender.sendWSData(_nsobj);
    return;
  }
}




/////////////////
// HELPERS
////////////////









// HELPER FUNCTIONS _changeBoxRequest
void myMeshController::_updateMyValFromWeb() {
// _nsobj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyValFromWeb: will change my target state to [%u]\n", (_nsobj["val"].as<uint16_t>()));
  }

  // update the valFromWeb
  ControlerBox::valFromWeb = _nsobj["val"].as<uint8_t>();
}



void myMeshController::_updateMyMasterBoxName() {
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyMasterBoxName: will change my master to [%u] + the prefix \n", _nsobj["val"].as<uint16_t>());
  }

  // update ui16MasterBoxName and bMasterBoxNameChangeHasBeenSignaled for my box
  thisBox.updateMasterBoxNameFromWeb(_nsobj["val"].as<uint16_t>());

  // send confirmation message
  _changeBoxSendConfirmationMsg();

  // mark the change as signaled
  thisBox.bMasterBoxNameChangeHasBeenSignaled = true;
}



void myMeshController::_updateMyDefaultState() {
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyDefaultState: will change my default state to [%u]\n", _nsobj["val"].as<uint16_t>());
  }

  thisBox.sBoxDefaultState = _nsobj["val"].as<uint16_t>();

  // send confirmation message
  _changeBoxSendConfirmationMsg();

  // mark the change as signaled
  thisBox.sBoxDefaultStateChangeHasBeenSignaled = true;
}



void myMeshController::_rebootEsp() {
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _nsobj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_rebootEsp: about to reboot\n");
  }

  // save preferences if requested
  if (_nsobj["save"] == 1) {
    _save();
  }

  // broadcast confirmation message (_changeBoxSendConfirmationMsg()
  // called without _ui32SenderNodeId param)
  Serial.println("------------------------------ CONFIRMING REBOOT ---------------------------");
  _changeBoxSendConfirmationMsg();

  // reboot
  Serial.println("------------------------------ ABOUT TO REBOOT ---------------------------");
  ControlerBox::tReboot.enableDelayed();
}





void myMeshController::_save() {
  // {action: "changeBox", key: "save", val: "all", lb: 1} // save all the values that can be saved
  mySavedPrefs::savePrefsWrapper();

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE ---------------------------");
  _changeBoxSendConfirmationMsg();
}


void myMeshController::_saveWifi() {
  // {action: "changeBox", key: "save", val: "wifi", lb: 1} // save external wifi values

  // if this is an "OTA save and reboot" or a "wifi save" request
    mySavedPrefs::saveFromNetRequest(_nsobj);

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE WIFI ---------------------------");
  _changeBoxSendConfirmationMsg();
}


void myMeshController::_savegi8RequestedOTAReboots() {
  // {action: "changeBox", key: "save", lb: 1, val: "gi8RequestedOTAReboots"}

  // if this is an "OTA save and reboot" request
  mySavedPrefs::saveFromNetRequest(_nsobj);

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE OTA ---------------------------");
  _changeBoxSendConfirmationMsg();

  // reboot
  Serial.println("------------------------------ ABOUT TO REBOOT ---------------------------");
  ControlerBox::tReboot.enableDelayed();
}


// Task myMeshController::_tChangeBoxSendConfirmationMsg;

// sends a message to the IF telling it that this box has executed its request
void myMeshController::_changeBoxSendConfirmationMsg() {
  // _nsobj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox 
  // _nsobj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate 
  if (MY_DG_MESH) Serial.printf("\nmyMeshController::_changeBoxSendConfirmationMsg: Starting\n");

  // change the "st" key of the received JSON object from 1 (request forwarded) to 2 (request executed)
  _nsobj["st"] = 2;
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: _obj[\"st\"] = %u\n", _nsobj["st"].as<uint16_t>());
  }

  // if the message was a "changeNet" request, it was broadcasted and
  // its "lb" field was equal to either "LBs" or "all";
  // replace it with thix box's index number so that the ControlerBoxes array
  // be properly updated in _changedBoxConfirmation
  if ((_nsobj["lb"] == "LBs") || (_nsobj["lb"] == "all")) {
    _nsobj["lb"] = thisBox.ui16NodeName - gui16ControllerBoxPrefix;
  }

  myMeshViews __myMeshViews;
  __myMeshViews.changedBoxConfirmation(_nsobj);
  // send back the received JSON object with its amended "st" key
  // if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: About to set the Task _tChangeBoxSendConfirmationMsg\n");
  // _tChangeBoxSendConfirmationMsg.setInterval(0);
  // if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: About to set the callback for _tChangeBoxSendConfirmationMsg \n");
  // _tChangeBoxSendConfirmationMsg.setCallback(
  //   [&_obj]() {
  //     if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: Inside the lambda \n");
  //     myMeshViews __myMeshViews;
  //     __myMeshViews.changedBoxConfirmation(_obj);
  //     if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: Message passed to myMeshViews \n");
  //   }
  // );
  // if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: Set the iterations\n");
  // _tChangeBoxSendConfirmationMsg.setIterations(1);
  // if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: Adding the Task to the Scheduler\n");
  // userScheduler.addTask(_tChangeBoxSendConfirmationMsg);
  // if (MY_DG_MESH) Serial.printf("myMeshController::_changeBoxSendConfirmationMsg: Restarting the Task _tChangeBoxSendConfirmationMsg\n");
  // _tChangeBoxSendConfirmationMsg.restart();
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: just called my mesh views\n");
  // }
}








// _changedBoxConfirmation HELPER FUNCTIONS



// PROTOTYPE FOR A MORE ABSTRACT CHANGE PROPERTY HANDLER
// void myMeshController::_updateMyProperty(char& _cPropertyKey, JsonObject& _nsobj) {
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateMyProperty: will change my property %s to %u\n", _nsobj[_cPropertyKey].as<uint8_t>());
//   }
//
//   // update property and propertyChangeHasBeenSignaled for my box
//   _updatePropertyForBox(_cPropertyKey, gui16MyIndexInCBArray, _nsobj)
//
//   // send confirmation message
//   _changeBoxSendConfirmationMsg();
//
//   // mark the change as signaled
//   ControlerBoxes[_ui16BoxIndex]._cPropertyKey = true;
// }


// attempt to abstract even more the process
// void myMeshController::_updatePropertyForBox(char& _cPropertyKey, uint8_t _ui16BoxIndex, JsonObject& _nsobj) {
//   // get the new property from the JSON
//   int8_t __i8PropertyValue = _nsobj[_cPropertyKey].as<uint8_t>();
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: %s = %i\n", _cPropertyKey, __i8PropertyValue);
//   }
//
//   // 1. set the new property of the relevant ControlerBox in the
//   // ControlerBoxes array
//   // 2. set the bool announcing that the change has not been signaled,
//   // to have it caught by the webServerTask (on the interface).
//   // TODO:
//   // a. ControlerBoxes[_ui16BoxIndex].updateProperty needs to be drafted
//   ControlerBoxes[_ui16BoxIndex].updateProperty(_cPropertyKey, __i8PropertyValue);
//
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: ControlerBoxes[%u], property %s has been updated to %i\n", _ui16BoxIndex, _cPropertyKey, __i8PropertyValue);
//   }
// }








///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES FOR REACTION TO NETWORK REQUESTS
///////////////////////////////
// The following variable, at start up, corresponds to the default reaction of this box, when it receives the status of its parent box.
// It is a number from 0 to 3. This number corresponds to the index in the following array (const bool B_SLAVE_ON_OFF_REACTIONS[4][2])
// short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;       // saves the index in the B_SLAVE_ON_OFF_REACTIONS bool array of the choosen reaction to the master states
// const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
// HIGH, LOW = reaction if master is on = HIGH; reaction if master is off = LOW;  // Synchronous (index 0): When the master box is on, turn me on AND when the master box is off, turn me off
// LOW, HIGH = reaction if master is on = LOW; reaction if master is off = HIGH;  // Opposed  (index 1): When the master box is on, turn me off AND when the master box is off, turn me on
// HIGH, HIGH = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always off  (index 2): When the master box is on, turn me off AND when the master box is off, turn me off
// LOW, LOW = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always on  (index 3): When the master box is on, turn me off AND when the master box is off, turn me off
// !!!!!! IMPORTANT: If changing the structure of B_SLAVE_ON_OFF_REACTIONS above, update _slaveReaction
// in class myWebServerViews and slaveReactionHtml in global.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// void myMeshController::_slaveBoxSwitch(JsonDocument& _nsobj) {
  // expected JSON string: {"NNa":"201";"APIP":"...";"StIP":"...";"action":"s";"senderBoxActiveState":"on"}
  /*
      Explanation of index numbers in the array of boolean arrays B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0 or 1]:
      const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
      - The first index number (iSlaveOnOffReaction) selects one of the pair of HIGH/LOW reactions listed in the upper dimension of the array.
        It is set via the iSlaveOnOffReaction variable. It is itself set either:
        - at startup; equal to the global constant I_DEFAULT_SLAVE_ON_OFF_REACTION (in the global variables definition);
        - via the changeSlaveReaction sub (in case the user has decided to change it via a web control).
      - The second index number (0 or 1) indicates the current status of the masterBox and selects, within the HIGH/LOW pair, the reaction of the slaveBox.
  */
  // extract the _nsobj[rootKey]
  // const char* __cSenderBoxActiveState = _nsobj["senderBoxActiveState"];
  // Serial.printf("myMeshController::_slaveBoxSwitch() %s alloted from _nsobj[\"senderBoxActiveState\"] to senderBoxActiveState \n", __cSenderBoxActiveState);

  // The following line has for sole purpose to provide data to the Serial.printfs below
  // const char* myFutureState = _B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0] == LOW ? "on" : "off";
  //
  // if (strstr(__cSenderBoxActiveState, "on")  > 0) {
  //   // if senderBoxActiveState contains "on", it means that the master box (the mesh sender) is turned on.
  //   Serial.printf("myMeshController::_slaveBoxSwitch(): Turning myself to %s.\n", myFutureState);
  //   // LaserGroupedUnitsArray::setTargetState(3);  /*3 means turn the LaserGroupedUnitsArray state machine to the state: in slave box mode, with on status */
  //   // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0]);
  // } else if (strstr(__cSenderBoxActiveState, "off")  > 0) {
  //   // else if senderBoxActiveState contains "on", it means that the master box (the mesh sender) is turned on.
  //   Serial.printf("myMeshController::_slaveBoxSwitch(): Turning myself to %s.\n", myFutureState);
  //   // LaserGroupedUnitsArray::setTargetState(LaserGroupedUnitsArray::previousState);  /*3 means turn the LaserGroupedUnitsArray state machine to the state: in slave box mode, with off status */
  //   // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1]);
  // }
  // Serial.print("myMeshController::_slaveBoxSwitch(): done\n");
// }
