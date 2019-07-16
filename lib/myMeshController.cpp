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

  // change masterBox confirmation (received by the interface only)
  const char* _mc = "mc";
  if (strcmp(_action, _mc) == 0) {           // action 'm' for this message relates to a master node number, that this box should update as the case may be

    byte __bMasterBoxName = _obj["ms"];
    if (MY_DG_MESH) {
      Serial.printf("myMeshController::myMeshController: _action = %s, __bMasterBoxName = %i\n", _action, __bMasterBoxName);
    }
    // reduce it from [e.g. 205] to [e.g. 5] send less data via the web
    __bMasterBoxName = __bMasterBoxName - bControllerBoxPrefix;
    if (MY_DG_MESH) {
      Serial.printf("myMeshController::myMeshController: _action = %s, __bMasterBoxName - bControllerBoxPrefix = %i\n", _action, (__bMasterBoxName - bControllerBoxPrefix));
    }

    // get the index as an int:
    // _obj["NNa"] is a char -> cast it as int
    // bControllerBoxPrefix is a byte -> cast it as int
    short int __sSlaveBoxIndexNumber = (int)_obj["NNa"] - (int)bControllerBoxPrefix;
    // if (MY_DG_MESH) {
    //   Serial.printf("myMeshController::myMeshController: _action = %s, __sSlaveBoxIndexNumber = %i\n", _action, __sSlaveBoxIndexNumber);
    // }

    // set the new master box number in the relevant ControlerBox (on the interface)
    // set the bool announcing that the change has not been signaled, to have it caught by the webServerTask
    ControlerBoxes[__sSlaveBoxIndexNumber].updateMasterBoxName(__bMasterBoxName);
    if (MY_DG_MESH) {
      Serial.printf("myMeshController::myMeshController: _action = %s, ControlerBoxes[%i].bMasterBoxName has been updated to %i\n", _action, __sSlaveBoxIndexNumber, ControlerBoxes[__sSlaveBoxIndexNumber].bMasterBoxName);
      Serial.printf("myMeshController::myMeshController: ending on _action = %s\n", _action);
    }
    return;
  }


  // changeBox request (received by the laser boxes only)
  const char* _actionChangeBox = "changeBox";

  // if (MY_DG_MESH) {
  //   // Serial.printf("myMeshController::myMeshController: _action = %s\n", _action);
  //   // Serial.printf("myMeshController::myMeshController: _actionChangeBox = %s\n", _actionChangeBox);
  //   // Serial.printf("myMeshController::myMeshController: (strcmp(_action, _actionChangeBox) == 0) =  %i\n", (strcmp(_action, _actionChangeBox) == 0));
  //   Serial.printf("myMeshController::myMeshController: _obj.containsKey(\"boxState\") = %i\n", (_obj.containsKey("boxState")));
  //   Serial.printf("myMeshController::myMeshController: _obj.containsKey(\"masterbox\") = %i\n", (_obj.containsKey("masterbox")));
  //   Serial.printf("myMeshController::myMeshController: _obj.containsKey(\"boxDefstate\") = %i\n", (_obj.containsKey("boxDefstate")));
  // }

  if (strcmp(_action, _actionChangeBox) == 0) {           // action 'changeBox' for this message relates to a change in active state, default state or master node number, that this box should update as the case may be

    // "boxState": 0; // "masterbox":201 // "boxDefstate": 4

    // if this is a change active state request
    if (_obj.containsKey("boxState")) {
      // if the message comes from the interface, this is a relayed message coming from the web
      if (_ui32SenderNodeId == ControlerBoxes[0].nodeId) {
        if (MY_DG_MESH) {
          Serial.printf("myMeshController::myMeshController: will change my target state to %i", _obj["boxState"].as<int8_t>());
        }
        // set the valFromWeb
        ControlerBox::valFromWeb = _obj["boxState"].as<int8_t>();
      } // else it might be a message coming from some other box,
      // but I shall not react. Reactions to changes in the mesh are
      // detected via status messages
      return;
    }

    // if this is a change master box request
    if (_obj.containsKey("masterbox")) {
      // update bMasterBoxName and bMasterBoxNameChangeHasBeenSignaled for my box
      ControlerBoxes[myIndexInCBArray].updateMasterBoxName(_obj["masterbox"].as<int8_t>() + bControllerBoxPrefix);
      // send a message to the IF telling it that I have taken the change into account
      _obj["action"].set("changedBx");
      myMeshViews __myMeshViews;
      __myMeshViews.changedBoxConfirmation(_obj);
      // __myMeshViews.changedMasterBoxConfirmation(_obj["masterbox"].as<int8_t>() + bControllerBoxPrefix);
      // mark the change as signaled
      ControlerBoxes[myIndexInCBArray].bMasterBoxNameChangeHasBeenSignaled = true;
      return;
    }

    // if this is a change default state request
    if (_obj.containsKey("boxDefstate")) {
      // if (MY_DG_MESH) {
      //   // Serial.printf("myMeshController::myMeshController: ----------------------------------------------------------\n");
      //   Serial.printf("myMeshController::myMeshController: _ui32SenderNodeId = %u\n", _ui32SenderNodeId);
      //   Serial.printf("myMeshController::myMeshController: ControlerBoxes[0].nodeId = %u\n", ControlerBoxes[0].nodeId);
      //   Serial.printf("myMeshController::myMeshController: (_ui32SenderNodeId == ControlerBoxes[0].nodeId) = %i\n", (_ui32SenderNodeId == ControlerBoxes[0].nodeId));
      // }
      // if the message comes from the interface,
      // this is a relayed message coming from the web
      if (_ui32SenderNodeId == ControlerBoxes[0].nodeId) {
        // if (MY_DG_MESH) {
        //   Serial.printf("myMeshController::myMeshController: will change my default state to %i\n", _obj["boxDefstate"].as<int>());
        // }
        ControlerBoxes[myIndexInCBArray].sBoxDefaultState = _obj["boxDefstate"].as<int8_t>();

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

        // mark the change as signaled
        ControlerBoxes[myIndexInCBArray].sBoxDefaultStateChangeHasBeenSignaled = true;
      } // else it might be a message coming from some other box,
      // but I shall not react. Reactions to changes in the mesh are
      // detected via status messages
      return;
    }

    return;
  }


  // change masterBox request (received by the laser boxes only)
  // const char* _m = "m";
  // if (strcmp(_action, _m) == 0) {           // action 'm' for this message relates to a master node number, that this box should update as the case may be
  //   byte __bMasterBoxName = _obj["ms"];
  //   __bMasterBoxName = __bMasterBoxName + bControllerBoxPrefix;
  //
  //   // update bMasterBoxName and bMasterBoxNameChangeHasBeenSignaled for my box
  //   ControlerBoxes[myIndexInCBArray].updateMasterBoxName(__bMasterBoxName);
  //   // send a message to the IF telling it that I have taken the change into account
  //   myMeshViews __myMeshViews;
  //   __myMeshViews.changedMasterBoxConfirmation(__bMasterBoxName);
  //   // mark the change has signaled
  //   ControlerBoxes[myIndexInCBArray].bMasterBoxNameChangeHasBeenSignaled = true;
  //
  //   return;
  // }


  // status message
  // upon receiving a status message from other boxes,
  // read and save the state of the other boxes
  const char* _s = "s";
  if (strcmp(_action, _s) == 0) {
    // action 's': the boxState of another box has changed and is being signalled to the mesh
    // update the ControlerBoxes[] array with the values received from the other box
    // if the sender box is not the interface

    byte __bSenderNodeName = _obj["NNa"];
    if (MY_DG_MESH) {Serial.print("myMeshController::myMeshController: __bSenderNodeName = ");Serial.println(__bSenderNodeName);}

    // Update the sender box properties

    // calculate the box index of the sending box
    byte __boxIndex = __bSenderNodeName - bControllerBoxPrefix;
    if (MY_DG_MESH) {Serial.print("myMeshController::myMeshController: __boxIndex = ");Serial.println(__boxIndex);}

    // if the sender is a newly connected box
    if (ControlerBoxes[__boxIndex].nodeId == 0) { //
      // Enable a Task to send this new box my current boxState.
      myMeshViews::tSendBoxStateToNewBox.restartDelayed();
    }

    // update the box properties
    ControlerBox::updateOtherBoxProperties(_ui32SenderNodeId, _obj);

    return;
  }


  // Receiving a "change this boxState" request
  // It may only be received on laser boxes
  // This is a signal sent from the web and relayed by the mesh
  // const char* _c = "c";
  // if (strcmp(_action, _c) == 0) {
  //   // action 'c': this message orders to change the boxTargetState
  //   byte __bSenderNodeName = _obj["NNa"];
  //   if (MY_DG_MESH) {
  //     Serial.print("myMeshController::myMeshController: __bSenderNodeName = ");Serial.println(__bSenderNodeName);
  //     Serial.print("myMeshController::myMeshController: bInterfaceNodeName = ");Serial.println(bInterfaceNodeName);
  //     Serial.print("myMeshController::myMeshController: __bSenderNodeName == bInterfaceNodeName = ");Serial.println(__bSenderNodeName == bInterfaceNodeName);
  //   }
  //
  //   // if the message comes from the interface,
  //   // this is a relayed message coming from the web
  //   if ((__bSenderNodeName == bInterfaceNodeName)) {
  //     if (MY_DG_MESH) {
  //       Serial.printf("myMeshController::myMeshController: will change my target state to %i", _obj["receiverTargetState"].as<int>());
  //     }
  //     ControlerBox::valFromWeb = _obj["receiverTargetState"];
  //   } // else it might be a message coming from some other box,
  //   // but I shall not react. Reactions to changes in the mesh are
  //   // detected via status messages
  //   return;
  // }



  // change this defaultBoxState request
  // This is a signal sent from the web and relayed by the mesh
  // const char* _d = "d";
  // if (strcmp(_action, _d) == 0) {
  //   // action 'c': this message orders to change the boxDefaultState
  //   byte __bSenderNodeName = (byte)_obj["NNa"].as<int>();
  //   if (MY_DG_MESH) {
  //     Serial.print("myMeshController::myMeshController: __bSenderNodeName = ");Serial.println(__bSenderNodeName);
  //     Serial.print("myMeshController::myMeshController: bInterfaceNodeName = ");Serial.println(bInterfaceNodeName);
  //     Serial.print("myMeshController::myMeshController: __bSenderNodeName == bInterfaceNodeName = ");Serial.println(__bSenderNodeName == bInterfaceNodeName);
  //   }
  //
  //   // if the message comes from the interface,
  //   // this is a relayed message coming from the web
  //   if ((__bSenderNodeName == bInterfaceNodeName)) {
  //     if (MY_DG_MESH) {Serial.printf("myMeshController::myMeshController: will change my default state to %i", _obj["receiverDefaultState"].as<int>());}
  //     byte __bSenderIndexInCB = _obj["receiverBoxName"];
  //     __bSenderIndexInCB = __bSenderIndexInCB - bControllerBoxPrefix;
  //     ControlerBoxes[(int)__bSenderIndexInCB].sBoxDefaultState = _obj["receiverDefaultState"].as<short>();
  //     // send a message to the IF telling it that I have taken the change into account
  //     myMeshViews __myMeshViews;
  //     __myMeshViews.changedBoxDefaultStateConfirmation(ControlerBoxes[(int)__bSenderIndexInCB].sBoxDefaultState);
  //     // mark the change has signaled
  //     ControlerBoxes[(int)__bSenderIndexInCB].sBoxDefaultStateChangeHasBeenSignaled = false;
  //   } // else it might be a message coming from some other box,
  //   // but I shall not react. Reactions to changes in the mesh are
  //   // detected via status messages
  //   return;
  // }


  // defaultBoxState change confirmation
  // This is a signal received by the IF from a laser controller
  const char* _dc = "dc";
  if (strcmp(_action, _dc) == 0) {
    // action 'dc': this message confirms a change the boxDefaultState for a given box
    short int __sSenderIndexInCB  = _obj["NNa"].as<short>() - (int)bControllerBoxPrefix;
    short int __sDefaultState = _obj["ds"].as<short>();
    if (MY_DG_MESH) {
      Serial.print("myMeshController::myMeshController: __sSenderIndexInCB = ");Serial.println(__sSenderIndexInCB);
      Serial.print("myMeshController::myMeshController: __sDefaultState = ");Serial.println(__sDefaultState);
    }

    // set the new default state in the relevant ControlerBox (on the interface)
    // set the bool announcing that the change has not been signaled, to have it caught by the webServerTask
    ControlerBoxes[__sSenderIndexInCB].sBoxDefaultState = __sDefaultState;
    // mark the change has unsignaled
    ControlerBoxes[__sSenderIndexInCB].sBoxDefaultStateChangeHasBeenSignaled = false;

    return;
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
