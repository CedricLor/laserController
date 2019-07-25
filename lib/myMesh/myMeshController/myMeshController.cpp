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

    _statusMessage(_obj, _ui32SenderNodeId);
    return;
  }




  // CHANGEBOX REQUEST AND CONFIRMATION (received by the destination laser boxes only on request and by all on confirmation)
  const char* _actionChangeBox = "changeBox";
  if (strcmp(_action, _actionChangeBox) == 0) {           // action 'changeBox' for this message relates to a change in active state, default state or master node number, that this box should update as the case may be
    // Serial.println("------------------------------ DETECTED A \"changeBox\" MESSAGE ---------------------------");
    _changeBox(_obj, _ui32SenderNodeId);
    return;
  }
}






// STATUS MESSAGE (received by all, sent by LBs only).
void myMeshController::_statusMessage(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  /*
    action 's': the boxState of another box has changed and is being
    signalled to the mesh. This box shall update its ControlerBoxes[] array
    with the values received from the other box.
  */

  // update the box properties in my CB array
  ControlerBox::updateOtherBoxProperties(_ui32SenderNodeId, _obj);
}





// CHANGEBOX REQUEST AND CONFIRMATION
// (received by the laser boxes only on request and by the interface and all the lbs on confirmation)
void myMeshController::_changeBox(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // ON REQUEST:
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  // ON CONFIRMATION:
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // if this is a change request
  if (_obj["st"].as<uint8_t>() == 1) {
      // Serial.println("------------------------------ THIS IS A CHANGE REQUEST ---------------------------");
      _changeBoxRequest(_obj, _ui32SenderNodeId);

    return;
  }

  // if this is a change confirmation
  if (_obj["st"].as<uint8_t>() == 2) {
      // Serial.println("------------------------------ THIS IS A CHANGE CONFIRMATION ---------------------------");
      _changedBoxConfirmation(_obj, _ui32SenderNodeId);

    return;
  }
}






// CHANGEBOX REQUEST (received by the laser boxes only)
void myMeshController::_changeBoxRequest(JsonObject& _obj, uint32_t _ui32SenderNodeId) {

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
    _updateMyMasterBoxName(_obj, _ui32SenderNodeId);
    return;
  }

  // if this is a change default state request
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (_obj["key"] == "boxDefstate") {
    // Serial.println("------------------------------ THIS IS A CHANGE DEFSTATE REQUEST ---------------------------");
    _updateMyDefaultState(_obj, _ui32SenderNodeId);
    return;
  }

  // if this is a reboot  request
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (_obj["key"] == "reboot") {
    Serial.println("------------------------------ THIS IS A REBOOT REQUEST ---------------------------");
    _rebootEsp(_obj, _ui32SenderNodeId);
    return;
  }

  // if this is a save request
  // _obj = {action: "changeBox"; key: "save"; lb: 1, val: "all"} // save all the values
  if (_obj["key"] == "save") {
    Serial.println("------------------------------ THIS IS A SAVE REQUEST ---------------------------");
    _save(_obj, _ui32SenderNodeId);
    return;
  }
}







// CHANGED BOX CONFIRMATION (received by the interface and all the other boxes)
void myMeshController::_changedBoxConfirmation(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // get the index number of the sender
  uint16_t __ui16BoxIndex = _obj["lb"];

  // if this is a "change master box request" confirmation
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  if (_obj["key"] == "masterbox") {
    // Serial.println("----------------- THIS A MASTERBOX CONFIRMATION ---------------");
    _updateSenderMasterBox(__ui16BoxIndex, _obj);
    return;
  }

  // if this is a "change default state request" confirmation
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  if (_obj["key"] == "boxDefstate") {
    // Serial.println("----------------- THIS A DEFAULT STATE CONFIRMATION ---------------");
    _updateSenderDefaultState(__ui16BoxIndex, _obj);
    return;
  }

  // if this is a "reboot" confirmation
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1; save: 1, st: 2} // boxDefstate // ancient 9
  if ((_obj["key"] == "reboot") || (_obj["key"] == "dropped")) {
    // Serial.println("----------------- THIS A REBOOT CONFIRMATION ---------------");
    ControlerBox::deleteBox(__ui16BoxIndex);
    // only decrease the MeshSize by one if it is a dropped connection message
    // note: dropped connection message are sent by the box which first detected
    // the dropped box. Such box has already updated its uiMeshSize in the
    // dropped connection callback, in myMesh class
    if (_obj["key"] == "dropped"){
      uiMeshSize = uiMeshSize - 1;
    }
    return;
  }

}




/////////////////
// HELPERS
////////////////


// _statusMessage HELPER FUNCTION
uint16_t myMeshController::_getSenderBoxIndexNumber(JsonObject& _obj) {
  // get the nodeName number of the sender
  uint16_t __ui16SenderNodeName = _obj["NNa"];
  if (MY_DG_MESH) {Serial.print("myMeshController::_getSenderBoxIndexNumber: __ui16SenderNodeName = ");Serial.println(__ui16SenderNodeName);}

  // get index number of the sender
  uint16_t __ui16BoxIndex = __ui16SenderNodeName - gui16ControllerBoxPrefix;
  if (MY_DG_MESH) {Serial.print("myMeshController::_getSenderBoxIndexNumber: __ui16BoxIndex = ");Serial.println(__ui16BoxIndex);}

  return __ui16BoxIndex;
}










// HELPER FUNCTIONS _changeBoxRequest
void myMeshController::_updateMyValFromWeb(JsonObject& _obj) {
// _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyValFromWeb: will change my target state to %u\n", (_obj["val"].as<uint16_t>()));
  }

  // update the valFromWeb
  ControlerBox::valFromWeb = _obj["val"].as<uint8_t>();
}



void myMeshController::_updateMyMasterBoxName(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyMasterBoxName: will change my master to %u\n", _obj["val"].as<uint16_t>() + gui16ControllerBoxPrefix);
  }

  // update bMasterBoxName and bMasterBoxNameChangeHasBeenSignaled for my box
  ControlerBoxes[gui16MyIndexInCBArray].updateMasterBoxName(_obj["val"].as<uint16_t>() + gui16ControllerBoxPrefix);

  // send confirmation message
  _changeBoxSendConfirmationMsg(_obj);

  // mark the change as signaled
  ControlerBoxes[gui16MyIndexInCBArray].bMasterBoxNameChangeHasBeenSignaled = true;
}



void myMeshController::_updateMyDefaultState(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyDefaultState: will change my default state to %u\n", _obj["val"].as<uint16_t>());
  }

  ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState = _obj["val"].as<uint16_t>();

  // send confirmation message
  _changeBoxSendConfirmationMsg(_obj);

  // mark the change as signaled
  ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultStateChangeHasBeenSignaled = true;
}



void myMeshController::_rebootEsp(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0, st: 1} // reboot without saving
  // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1, st: 1} // reboot and save
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_rebootEsp: about to reboot\n");
  }

  // save preferences if requested
  if (_obj["save"] == 1) {
    _save(_obj, _ui32SenderNodeId);
  }

  // broadcast confirmation message (_changeBoxSendConfirmationMsg(_obj)
  // called without _ui32SenderNodeId param)
  Serial.println("------------------------------ CONFIRMING REBOOT ---------------------------");
  _changeBoxSendConfirmationMsg(_obj);

  // reboot
  Serial.println("------------------------------ ABOUT TO REBOOT ---------------------------");
  ControlerBox::tReboot.enableDelayed();
}





void myMeshController::_save(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // save preferences
  mySavedPrefs::savePrefsWrapper();

  // send confirmation message
  Serial.println("------------------------------ CONFIRMING SAVE ---------------------------");
  _changeBoxSendConfirmationMsg(_obj, _ui32SenderNodeId);
}

// PROTOTYPE FOR A MORE ABSTRACT CHANGE PROPERTY HANDLER
// void myMeshController::_updateMyProperty(char& _cPropertyKey, JsonObject& _obj) {
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateMyProperty: will change my property %s to %u\n", _obj[_cPropertyKey].as<uint8_t>());
//   }
//
//   // update property and propertyChangeHasBeenSignaled for my box
//   _updatePropertyForBox(_cPropertyKey, gui16MyIndexInCBArray, _obj)
//
//   // send confirmation message
//   _changeBoxSendConfirmationMsg(_obj);
//
//   // mark the change as signaled
//   ControlerBoxes[_ui16BoxIndex]._cPropertyKey = true;
// }


// attempt to abstract even more the process
// void myMeshController::_updatePropertyForBox(char& _cPropertyKey, uint8_t _ui16BoxIndex, JsonObject& _obj) {
//   // get the new property from the JSON
//   int8_t __i8PropertyValue = _obj[_cPropertyKey].as<uint8_t>();
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
//   // b. in ControlerBoxes[_ui16BoxIndex].updateProperty, if it is a master nodeName
//   // change, add the gui16ControllerBoxPrefix.
//   ControlerBoxes[_ui16BoxIndex].updateProperty(_cPropertyKey, __i8PropertyValue);
//
//   if (MY_DG_MESH) {
//     Serial.printf("myMeshController::_updateSenderProperty: ControlerBoxes[%u], property %s has been updated to %i\n", _ui16BoxIndex, _cPropertyKey, __i8PropertyValue);
//   }
// }


void myMeshController::_changeBoxSendConfirmationMsg(JsonObject& _obj, uint32_t _ui32SenderNodeId) {
  // send a message to the IF telling it that I have executed its request
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 1} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

  // change the "st" key of the received JSON object from 1 (request forwarded) to 2 (request executed)
  _obj["st"] = 2;
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: _obj[\"st\"] = %u\n", _obj["st"].as<uint8_t>());
  // }
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9

  // if the message was a "changeNet" request, it was broadcasted and
  // its "lb" field was equal to either "LBs" or "all";
  // replace it with thix box's index number so that the ControlerBoxes array
  // be properly updated in _changedBoxConfirmation
  if ((_obj["lb"] == "LBs") || (_obj["lb"] == "all")) {
    _obj["lb"] = gui16MyIndexInCBArray;
  }

  // send back the received JSON object with its amended "st" key
  myMeshViews __myMeshViews;
  __myMeshViews.changedBoxConfirmation(_obj);
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: just called my mesh views\n");
  // }
}








// _changedBoxConfirmation HELPER FUNCTIONS
void myMeshController::_updateSenderMasterBox(uint16_t _ui16BoxIndex, JsonObject& _obj) {
  // _obj = {action: "changeBox", key: "masterbox"; lb: 1, val: 4, st: 2} // masterbox // ancient 8
  // get the new masterBoxName from the JSON
  uint16_t __ui16MasterBoxName = _obj["val"].as<uint16_t>() + gui16ControllerBoxPrefix;
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderMasterBox: __ui16MasterBoxName = %u\n",  __ui16MasterBoxName);
  }

  // 1. set the new master box number of the relevant ControlerBox in the
  // ControlerBoxes array
  // 2. set the bool announcing that the change has not been signaled,
  // to have it caught by the webServerTask (on the interface)
  ControlerBoxes[_ui16BoxIndex].updateMasterBoxName(__ui16MasterBoxName);

  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderMasterBox: ControlerBoxes[%u].bMasterBoxName has been updated to %u\n", _ui16BoxIndex, ControlerBoxes[_ui16BoxIndex].bMasterBoxName);
  }
}







void myMeshController::_updateSenderDefaultState(uint16_t _ui16BoxIndex, JsonObject& _obj) {
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 2} // boxDefstate // ancient 9
  // get the new defaultState from the JSON
  uint16_t __ui16DefaultState = _obj["val"];
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderDefaultState: __ui8DefaultState = %u\n", __ui16DefaultState);
  }

  // 1. set the new default state of the relevant ControlerBox in the
  // ControlerBoxes array
  // 2. set the bool announcing that the change has not been signaled,
  // to have it caught by the webServerTask (on the interface)
  ControlerBox::setBoxDefaultState(_ui16BoxIndex, __ui16DefaultState);

  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateSenderDefaultState: ControlerBoxes[%u].sBoxDefaultState has been updated to %u\n", _ui16BoxIndex, ControlerBoxes[_ui16BoxIndex].sBoxDefaultState);
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
