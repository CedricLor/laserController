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






myMeshController::myMeshController(uint32_t _ui32SenderNodeId, JsonObject& _obj)
{
  if (MY_DG_MESH) {
    Serial.print("myMeshController::myMeshController: Starting\n");
  }

  // read the action field
  const char* _action = _obj["action"];

  // if debug, serial print the action field
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::myMeshController: _action = %s\n", _action);
  }




  // STATUS MESSAGE (received by all, sent by LBs only).
  /* Upon receiving a status message from another box,
     read and save the state of the other boxes */
  const char* _s = "s";
  if (strcmp(_action, _s) == 0) {

    _statusMessage(_ui32SenderNodeId, _obj);
    return;
  }




  // CHANGEBOX REQUEST AND CONFIRMATION (received by the destination laser boxes only on request and by all on confirmation)
  const char* _actionChangeBox = "changeBox";
  if (strcmp(_action, _actionChangeBox) == 0) {           // action 'changeBox' for this message relates to a change in active state, default state or master node number, that this box should update as the case may be
    // Serial.println("------------------------------ DETECTED A \"changeBox\" MESSAGE ---------------------------");
    _changeBox(_ui32SenderNodeId, _obj);
    return;
  }

}






// STATUS MESSAGE (received by all, sent by LBs only).
void myMeshController::_statusMessage(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  /*
    action 's': the boxState of another box has changed and is being
    signalled to the mesh. This box shall update its ControlerBoxes[] array
    with the values received from the other box.
  */

  // get the index number of the sender
  int8_t __i8BoxIndex = _getSenderBoxIndexNumber(_obj);

  // if the sender is a newly connected box
  if (ControlerBoxes[__i8BoxIndex].nodeId == 0) { //
    // Enable a Task to send this new box my current boxState.
    myMeshViews::tSendBoxStateToNewBox.restartDelayed();
  }

  // update the box properties in my CB array
  ControlerBox::updateOtherBoxProperties(_ui32SenderNodeId, _obj);
}





// CHANGEBOX REQUEST AND CONFIRMATION
// (received by the laser boxes only on request and by the interface and all the lbs on confirmation)
void myMeshController::_changeBox(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  // ON REQUEST:
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  // ON CONFIRMATION:
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // if this is a change request
  if (_obj["st"].as<int8_t>() == 1) {
      // Serial.println("------------------------------ THIS IS A CHANGE REQUEST ---------------------------");
      _changeBoxRequest(_ui32SenderNodeId, _obj);

    return;
  }

  // if this is a change confirmation
  if (_obj["st"].as<int8_t>() == 2) {
      // Serial.println("------------------------------ THIS IS A CHANGE CONFIRMATION ---------------------------");
      _changedBoxConfirmation(_ui32SenderNodeId, _obj);

    return;
  }
}






// CHANGEBOX REQUEST (received by the laser boxes only)
void myMeshController::_changeBoxRequest(uint32_t _ui32SenderNodeId, JsonObject& _obj) {

  // if this is a change active state request
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (_obj["key"] == "boxState") {
    // Serial.println("------------------------------ THIS IS A CHANGE BOXSTATE REQUEST ---------------------------");
    _updateMyValFromWeb(_obj);
    return;
  }

  // if this is a change master box request
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  if (_obj["key"] == "masterbox") {
    // Serial.println("------------------------------ THIS IS A CHANGE MASTERBOX REQUEST ---------------------------");
    _updateMyMasterBoxName(_obj);
    return;
  }

  // if this is a change default state request
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (_obj["key"] == "boxDefstate") {
    // Serial.println("------------------------------ THIS IS A CHANGE DEFSTATE REQUEST ---------------------------");
    _updateMyDefaultState(_obj);
    return;
  }

  // if this is a reboot  request
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (_obj["key"] == "reboot") {
    Serial.println("------------------------------ THIS IS A REBOOT REQUEST ---------------------------");
    _rebootEsp(_obj);
    return;
  }

  // if this is a save request
  // _obj = {action: "changeBox"; key: "save"; lb: 1, val: "all"} // save all the values
  if (_obj["key"] == "save") {
    Serial.println("------------------------------ THIS IS A SAVE REQUEST ---------------------------");
    _save(_obj);
    return;
  }
}







// CHANGED BOX CONFIRMATION (received by the interface and all the other boxes)
void myMeshController::_changedBoxConfirmation(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // get the index number of the sender
  int8_t __i8BoxIndex = _obj["lb"];

  // if this is a "change master box request" confirmation
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  if (_obj["key"] == "masterbox") {
    // Serial.println("----------------- THIS A MASTERBOX CONFIRMATION ---------------");
    _updateSenderMasterBox(__i8BoxIndex, _obj);
    return;
  }

  // if this is a "change default state request" confirmation
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  if (_obj["key"] == "boxDefstate") {
    // Serial.println("----------------- THIS A DEFAULT STATE CONFIRMATION ---------------");
    _updateSenderDefaultState(__i8BoxIndex, _obj);
    return;
  }

}





/////////////////
// HELPERS
////////////////


// _statusMessage HELPER FUNCTION
int8_t myMeshController::_getSenderBoxIndexNumber(JsonObject& _obj) {
  // get the nodeName number of the sender
  int8_t __i8SenderNodeName = _obj["NNa"];
  if (MY_DG_MESH) {Serial.print("myMeshController::_getSenderBoxIndexNumber: __i8SenderNodeName = ");Serial.println(__i8SenderNodeName);}

  // get index number of the sender
  int8_t __i8BoxIndex = __i8SenderNodeName - bControllerBoxPrefix;
  if (MY_DG_MESH) {Serial.print("myMeshController::_getSenderBoxIndexNumber: __i8BoxIndex = ");Serial.println(__i8BoxIndex);}

  return __i8BoxIndex;
}










// HELPER FUNCTIONS _changeBoxRequest
void myMeshController::_updateMyValFromWeb(JsonObject& _obj) {
// _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyValFromWeb: will change my target state to %i\n", (_obj["val"].as<int8_t>()));
  }

  // update the valFromWeb
  ControlerBox::valFromWeb = _obj["val"].as<int8_t>();
}



void myMeshController::_updateMyMasterBoxName(JsonObject& _obj) {
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyMasterBoxName: will change my master to %i\n", _obj["val"].as<int8_t>() + bControllerBoxPrefix);
  }

  // update bMasterBoxName and bMasterBoxNameChangeHasBeenSignaled for my box
  ControlerBoxes[myIndexInCBArray].updateMasterBoxName(_obj["val"].as<int8_t>() + bControllerBoxPrefix);

  // send confirmation message
  _changeBoxSendConfirmationMsg(_obj);

  // mark the change as signaled
  ControlerBoxes[myIndexInCBArray].bMasterBoxNameChangeHasBeenSignaled = true;
}



void myMeshController::_updateMyDefaultState(JsonObject& _obj) {
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyDefaultState: will change my default state to %i\n", _obj["val"].as<int8_t>());
  }

  ControlerBoxes[myIndexInCBArray].sBoxDefaultState = _obj["val"].as<int8_t>();

  // send confirmation message
  _changeBoxSendConfirmationMsg(_obj);

  // mark the change as signaled
  ControlerBoxes[myIndexInCBArray].sBoxDefaultStateChangeHasBeenSignaled = true;
}



void myMeshController::_rebootEsp(JsonObject& _obj) {
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_rebootEsp: about to reboot\n");
  }

  // save if necessary
  if (_obj["save"] == 1) {
    _save(_obj);
  }

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING REBOOT ---------------------------");
  _changeBoxSendConfirmationMsg(_obj);

  // reboot
  Serial.println("------------------------------ ABOUT TO REBOOT ---------------------------");
  ControlerBox::tReboot.enableDelayed();
}


void myMeshController::_save(JsonObject& _obj) {
  // save preferences
  mySavedPrefs::savePreferences();

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE ---------------------------");
  _changeBoxSendConfirmationMsg(_obj);
}

// PROTOTYPE FOR A MORE ABSTRACT CHANGE PROPERTY HANDLER
// void myMeshController::_updateMyProperty(char& _cPropertyKey, JsonObject& _obj) {
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateMyProperty: will change my property %s to %i\n", _obj[_cPropertyKey].as<int8_t>());
//   }
//
//   // update property and propertyChangeHasBeenSignaled for my box
//   _updatePropertyForBox(_cPropertyKey, myIndexInCBArray, _obj)
//
//   // send confirmation message
//   _changeBoxSendConfirmationMsg(_obj);
//
//   // mark the change as signaled
//   ControlerBoxes[_i8BoxIndex]._cPropertyKey = true;
// }


// attempt to abstract even more the process
// void myMeshController::_updatePropertyForBox(char& _cPropertyKey, int8_t _i8BoxIndex, JsonObject& _obj) {
//   // get the new property from the JSON
//   int8_t __i8PropertyValue = _obj[_cPropertyKey].as<int8_t>();
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: %s = %i\n", _cPropertyKey, __i8PropertyValue);
//   }
//
//   // 1. set the new property of the relevant ControlerBox in the
//   // ControlerBoxes array
//   // 2. set the bool announcing that the change has not been signaled,
//   // to have it caught by the webServerTask (on the interface).
//   // TODO:
//   // a. ControlerBoxes[_i8BoxIndex].updateProperty needs to be drafted
//   // b. in ControlerBoxes[_i8BoxIndex].updateProperty, if it is a master nodeName
//   // change, add the bControllerBoxPrefix.
//   ControlerBoxes[_i8BoxIndex].updateProperty(_cPropertyKey, __i8PropertyValue);
//
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: ControlerBoxes[%i], property %s has been updated to %i\n", _i8BoxIndex, _cPropertyKey, __i8PropertyValue);
//   }
// }


void myMeshController::_changeBoxSendConfirmationMsg(JsonObject& _obj) {
  // send a message to the IF telling it that I have executed its request
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

  // change the "st" key of the received JSON object from 1 (request forwarded) to 2 (request executed)
  _obj["st"] = 2;
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: _obj[\"st\"] = %i\n", _obj["st"].as<int8_t>());
  // }
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // if the message was a "changeNet" request, it was broadcasted and
  // its "lb" field was equal to either "LBs" or "all";
  // replace it with thix box's index number so that the ControlerBoxes array
  // be properly updated in _changedBoxConfirmation
  if ((_obj["lb"] == "LBs") || (_obj["lb"] == "all")) {
    _obj["lb"] = myIndexInCBArray;
  }

  // send back the received JSON object with its amended "st" key
  myMeshViews __myMeshViews;
  __myMeshViews.changedBoxConfirmation(_obj);
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: just called my mesh views\n");
  // }
}








// _changedBoxConfirmation HELPER FUNCTIONS
void myMeshController::_updateSenderMasterBox(int8_t _i8BoxIndex, JsonObject& _obj) {
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // get the new masterBoxName from the JSON
  int8_t __i8MasterBoxName = _obj["val"];
  __i8MasterBoxName = __i8MasterBoxName + bControllerBoxPrefix;
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderMasterBox: __i8MasterBoxName = %i\n",  __i8MasterBoxName);
  }

  // 1. set the new master box number of the relevant ControlerBox in the
  // ControlerBoxes array
  // 2. set the bool announcing that the change has not been signaled,
  // to have it caught by the webServerTask (on the interface)
  ControlerBoxes[_i8BoxIndex].updateMasterBoxName(__i8MasterBoxName);

  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderMasterBox: ControlerBoxes[%i].bMasterBoxName has been updated to %i\n", _i8BoxIndex, ControlerBoxes[_i8BoxIndex].bMasterBoxName);
  }
}







void myMeshController::_updateSenderDefaultState(int8_t _i8BoxIndex, JsonObject& _obj) {
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  // get the new defaultState from the JSON
  int8_t __i8DefaultState = _obj["val"];
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderDefaultState: __i8DefaultState = %i\n", __i8DefaultState);
  }

  // 1. set the new default state of the relevant ControlerBox in the
  // ControlerBoxes array
  // 2. set the bool announcing that the change has not been signaled,
  // to have it caught by the webServerTask (on the interface)
  ControlerBox::setBoxDefaultState(_i8BoxIndex, __i8DefaultState);

  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderDefaultState: ControlerBoxes[%i].sBoxDefaultState has been updated to %i\n", _i8BoxIndex, ControlerBoxes[_i8BoxIndex].sBoxDefaultState);
  }
}







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

// void myMeshController::_manualSwitch(JsonDocument& _obj) {
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"u";"ts":"0"}
  // short _iTargetState;
  // const char* _sTargetState = _obj["ts"];
  // _iTargetState = atoi(_sTargetState);
  // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_iTargetState);
  // LaserGroupedUnitsArray::setTargetState(4);      // 4 means turn the state of the LaserGroupedUnitsArray to manual
// }

// void myMeshController::_changeInclusionIR(JsonDocument& _obj) {
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"i";"ts":"0"}
  // short _iTargetState;
  // const char* _sTargetState = _obj["ts"];
  // _iTargetState = atoi(_sTargetState);
  // LaserGroupedUnitsArray::setTargetPirState(_iTargetState /*0 for false = out of IR control; 1 for true = under IR control */);
// }

// void myMeshController::_changeBlinkingInterval(JsonDocument& _obj) {
  // expected JSON string: {"NNa":"001";"APIP":"...";"StIP":"...";"action":"b";"ti":"5000"}
  // unsigned long _ulTargetBlinkingInterval;
  // const char* _sTargetBlinkingInterval = _obj["ti"];
  // _ulTargetBlinkingInterval = atoi(_sTargetBlinkingInterval);
  // LaserGroupedUnitsArray::setTargetBlinkingInterval(_ulTargetBlinkingInterval);
// }

const bool myMeshController::_B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
// const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};
// const char* _slaveReaction[4] = {"synchronous: on - on & off - off", "opposed: on - off & off - on", "always on: off - on & on - on", "always off: on - off & off - off"};

// struct slaveReactionStruct {
//   bool slaveReactionCore[2];
//   char* slaveReactionJson;
//   char* slaveReactionText;
// };
//
// slaveReactionStruct slaveReactionStructsArray[4];

// void myMeshController::_slaveBoxSwitch(JsonDocument& _obj) {
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
  // extract the _obj[rootKey]
  // const char* __cSenderBoxActiveState = _obj["senderBoxActiveState"];
  // Serial.printf("myMeshController::_slaveBoxSwitch() %s alloted from _obj[\"senderBoxActiveState\"] to senderBoxActiveState \n", __cSenderBoxActiveState);

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
