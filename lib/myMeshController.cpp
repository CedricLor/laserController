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




  // CHANGEBOX REQUEST (received by the laser boxes only)
  const char* _actionChangeBox = "changeBox";
  if (strcmp(_action, _actionChangeBox) == 0) {           // action 'changeBox' for this message relates to a change in active state, default state or master node number, that this box should update as the case may be

    _changeBox(_ui32SenderNodeId, _obj);
    return;
  }




  // CHANGED BOX CONFIRMATION (received by all the boxes)
  const char* _actionChangedBx = "changedBx";
  if (strcmp(_action, _actionChangedBx) == 0) {           // action 'changedBx' for this message relates to a change in active state, default state or master node number, that this box should update as the case may be

    _changedBx(_ui32SenderNodeId, _obj);
    return;
  }




  // Temporarily commented out
  //////// Manual mode
  // const char* _u = "u";
  // if (strcmp(_action, _u) == 0) {           // action 'u' for this message requests that the laserPin be turned into or out of User controlled mode
  //   _manualSwitch(_obj);
  //   return;
  // }
  //////// IR mode
  // const char* _i = "i";
  // if (strcmp(_action, _i) == 0) {           // action 'i' for this message requests that the laserPin be turned into or out of IR control mode
  //   _changeInclusionIR(_obj);
  //   return;
  // }
  //////// Blinking interval
  // const char* _b = "b";
  // if (strcmp(_action, _b) == 0) {           // action 'b' for this message relates to a blinking interval, that this box should update as the case may be
  //   _changeBlinkingInterval(_obj);
  //   return;
  // }

}






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

  // update the box properties
  ControlerBox::updateOtherBoxProperties(_ui32SenderNodeId, _obj);
}







void myMeshController::_changeBox(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  // looking for "boxState": 0; // "masterbox":201 // "boxDefstate": 4

  // if this is a change active state request
  if (_obj.containsKey("boxState")) {

    _updateMyValFromWeb(_obj);
    return;
  }

  // if this is a change master box request
  if (_obj.containsKey("masterbox")) {

    _updateMyMasterBoxName(_obj);
    return;
  }

  // if this is a change default state request
  if (_obj.containsKey("boxDefstate")) {

    _updateMyDefaultState(_obj);
    return;
  }
}




// helper functions to myMeshController::_changeBox
void myMeshController::_updateMyValFromWeb(JsonObject& _obj) {
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyValFromWeb: will change my target state to %i", _obj["boxState"].as<int8_t>());
  }

  // update the valFromWeb
  ControlerBox::valFromWeb = _obj["boxState"].as<int8_t>();
}



void myMeshController::_updateMyMasterBoxName(JsonObject& _obj) {
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyMasterBoxName: will change my master to %i", _obj["masterbox"].as<int8_t>() + bControllerBoxPrefix);
  }

  // update bMasterBoxName and bMasterBoxNameChangeHasBeenSignaled for my box
  ControlerBoxes[myIndexInCBArray].updateMasterBoxName(_obj["masterbox"].as<int8_t>() + bControllerBoxPrefix);

  // send confirmation message
  _changeBoxSendConfirmationMsg(_obj);

  // mark the change as signaled
  ControlerBoxes[myIndexInCBArray].bMasterBoxNameChangeHasBeenSignaled = true;
}



void myMeshController::_updateMyDefaultState(JsonObject& _obj) {
  if (MY_DG_MESH) {
    Serial.printf("myMeshController::_updateMyDefaultState: will change my default state to %i", _obj["boxDefstate"].as<int8_t>());
  }

  ControlerBoxes[myIndexInCBArray].sBoxDefaultState = _obj["boxDefstate"].as<int8_t>();

  // send confirmation message
  _changeBoxSendConfirmationMsg(_obj);

  // mark the change as signaled
  ControlerBoxes[myIndexInCBArray].sBoxDefaultStateChangeHasBeenSignaled = true;
}



void myMeshController::_changeBoxSendConfirmationMsg(JsonObject& _obj) {
  // send a message to the IF telling it that I have taken the change into account
  _obj["action"] = "changedBx";
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: _obj[\"action\"] = %s\n", _obj["action"].as<const char*>());
  //   Serial.printf("myMeshController::myMeshController: _bHasChangedAction = %i\n", _bHasChangedAction);
  // }

  myMeshViews __myMeshViews;
  __myMeshViews.changedBoxConfirmation(_obj);
  // if (MY_DG_MESH) {
  //   Serial.printf("myMeshController::myMeshController: just called my mesh views\n");
  // }
}






void myMeshController::_changedBx(uint32_t _ui32SenderNodeId, JsonObject& _obj) {
  // lloking for "boxState": 0; // "masterbox":201 // "boxDefstate": 4

  // get the index number of the sender
  int8_t __i8BoxIndex = _getSenderBoxIndexNumber(_obj);

  // if this is a "change master box request" confirmation
  if (_obj.containsKey("masterbox")) {

    // get the new masterBoxName from the JSON
    int8_t __i8MasterBoxName = _obj["masterbox"].as<int8_t>() + bControllerBoxPrefix;
    if (MY_DG_MESH) {
      Serial.printf("myMeshController::_changedBx: __i8MasterBoxName = %i\n",  __i8MasterBoxName);
    }

    // set the new master box number in the relevant ControlerBox (on the interface)
    // set the bool announcing that the change has not been signaled, to have it caught by the webServerTask
    ControlerBoxes[__i8BoxIndex].updateMasterBoxName(__i8MasterBoxName);
    if (MY_DG_MESH) {
      Serial.printf("myMeshController::_changedBx: ControlerBoxes[%i].bMasterBoxName has been updated to %i\n", __i8BoxIndex, ControlerBoxes[__i8BoxIndex].bMasterBoxName);
    }

    return;
  }

  // if this is a "change default state request" confirmation
  if (_obj.containsKey("boxDefstate")) {

    // get the new defaultState from the JSON
    int8_t __i8DefaultState = _obj["boxDefstate"].as<int8_t>();
    if (MY_DG_MESH) {
      Serial.printf("myMeshController::myMeshController: __i8DefaultState = %i\n", __i8DefaultState);
    }

    // set the new default state in the relevant ControlerBox (on the interface)
    // set the bool announcing that the change has not been signaled, to have it caught by the webServerTask
    ControlerBoxes[__i8BoxIndex].sBoxDefaultState = __sDefaultState;
    // mark the change has unsignaled
    ControlerBoxes[__i8BoxIndex].sBoxDefaultStateChangeHasBeenSignaled = false;

    return;
  }

}






// _changedBox and _statusMessage helper function
int8_t myMeshController::_getSenderBoxIndexNumber(JsonObject& _obj) {
  // get the nodeName number of the sender
  int8_t __i8SenderNodeName = _obj["NNa"];
  if (MY_DG_MESH) {Serial.print("myMeshController::_getSenderBoxIndexNumber: __i8SenderNodeName = ");Serial.println(__i8SenderNodeName);}

  // get index number of the sender
  int8_t __i8BoxIndex = __i8SenderNodeName - bControllerBoxPrefix;
  if (MY_DG_MESH) {Serial.print("myMeshController::_getSenderBoxIndexNumber: __i8BoxIndex = ");Serial.println(__i8BoxIndex);}

  return __i8BoxIndex;
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
