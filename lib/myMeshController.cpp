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






myMeshController::myMeshController(uint32_t senderNodeId, JsonDocument& root)
{
  // read the action field
  const char* _action = root["action"];

  // if debug, serial print the action field
  if (MY_DEBUG) {
    Serial.print("myMeshController::myMeshController: _action = ");Serial.println(_action);
  }

  // Temporarily commented out
  //////// Manual mode
  // const char* _u = "u";
  // if (strcmp(_action, _u) == 0) {           // action 'u' for this message requests that the laserPin be turned into or out of User controlled mode
  //   _manualSwitch(root);
  //   return;
  // }
  //////// IR mode
  // const char* _i = "i";
  // if (strcmp(_action, _i) == 0) {           // action 'i' for this message requests that the laserPin be turned into or out of IR control mode
  //   _changeInclusionIR(root);
  //   return;
  // }
  //////// Blinking interval
  // const char* _b = "b";
  // if (strcmp(_action, _b) == 0) {           // action 'b' for this message relates to a blinking interval, that this box should update as the case may be
  //   _changeBlinkingInterval(root);
  //   return;
  // }


  // change masterBox request
  const char* _m = "m";
  if (strcmp(_action, _m) == 0) {           // action 'm' for this message relates to a master node number, that this box should update as the case may be
    byte __masterBoxName = root["ms"];
    __masterBoxName = __masterBoxName + B_CONTROLLER_BOX_PREFIX;

    ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName = __masterBoxName;
    ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxNameChangeHasBeenSignaled = false;

    return;
  }


  // status message
  // upon receiving a status message from other boxes,
  // read and save the state of the other boxes
  const char* _s = "s";
  if (strcmp(_action, _s) == 0) {
    // action 's': the boxState of another box has changed and is being signalled to the mesh
    // update the ControlerBoxes[] array with the values received from the other box
    // if the sender box is not the interface

    byte __bSenderNodeName = root["senderNodeName"];
    if (MY_DEBUG) {Serial.print("myMeshController::myMeshController: __bSenderNodeName = ");Serial.println(__bSenderNodeName);}

    // Update the sender box properties

    // calculate the box index of the sending box
    byte __boxIndex = __bSenderNodeName - B_CONTROLLER_BOX_PREFIX;
    if (MY_DEBUG) {Serial.print("myMeshController::myMeshController: __boxIndex = ");Serial.println(__boxIndex);}

    // if the sender is a newly connected box
    if (ControlerBoxes[__boxIndex].nodeId == 0) { //
      // Enable a Task to send this new box my current boxState.
      myMeshViews::tSendBoxStateToNewBox.restartDelayed();
    }

    // update the box properties
    ControlerBox::updateOtherBoxProperties(senderNodeId, root);

    return;
  }


  // change this boxState request
  // This is a signal sent from the web and relayed by the mesh
  const char* _c = "c";
  if (strcmp(_action, _c) == 0) {
    // action 'c': this message orders to change the boxTargetState
    byte __bSenderNodeName = root["senderNodeName"];
    if (MY_DEBUG) {Serial.print("myMeshController::myMeshController: __bSenderNodeName = ");Serial.println(__bSenderNodeName);}

    // if the message comes from the interface, this is a relayed message coming from the web
    if ((__bSenderNodeName == bInterfaceNodeName)) {
      if (MY_DEBUG) {Serial.printf("myMeshController::myMeshController: will change my target state to %i", ControlerBox::valFromWeb);}
      ControlerBox::valFromWeb = root["receiverTargetState"];
    }

    return;
  }



  // Temporarily commented out
  // const char* _p = "p";
  // if (strcmp(_action, _p) == 0) {           // action 'p' for this message orders the pairing
  //   _pinPairing(root);            // (either of type unpairing, twin pairing or cooperative)
  //   return;                       // of the pins, that this box should update as the case may be
  // }
  // const char* _d = "d";
  // if (strcmp(_action, _d) == 0) {           // action 'd' for this message requests that this box returns Data on it current states
  //   _dataRequest();
  //   return;
  // }
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

// void myMeshController::_manualSwitch(JsonDocument& _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"u";"ts":"0"}
  // short _iTargetState;
  // const char* _sTargetState = _root["ts"];
  // _iTargetState = atoi(_sTargetState);
  // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_iTargetState);
  // LaserGroupedUnitsArray::setTargetState(4);      // 4 means turn the state of the LaserGroupedUnitsArray to manual
// }

// void myMeshController::_changeInclusionIR(JsonDocument& _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"i";"ts":"0"}
  // short _iTargetState;
  // const char* _sTargetState = _root["ts"];
  // _iTargetState = atoi(_sTargetState);
  // LaserGroupedUnitsArray::setTargetPirState(_iTargetState /*0 for false = out of IR control; 1 for true = under IR control */);
// }

// void myMeshController::_changeBlinkingInterval(JsonDocument& _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"b";"ti":"5000"}
  // unsigned long _ulTargetBlinkingInterval;
  // const char* _sTargetBlinkingInterval = _root["ti"];
  // _ulTargetBlinkingInterval = atoi(_sTargetBlinkingInterval);
  // LaserGroupedUnitsArray::setTargetBlinkingInterval(_ulTargetBlinkingInterval);
// }

// void myMeshController::_changeMasterBox(JsonDocument& _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"m";"ms":"201";"react":"syn"}
  // byte bNewMasterBoxNumber;
  // const char* sNewMasterBoxNumber = _root["ms"];
  // bNewMasterBoxNumber = atoi(sNewMasterBoxNumber);
  // const char* sNewReaction = _root["react"];
  // // const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};
  // MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(bNewMasterBoxNumber, sNewReaction);
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

// void myMeshController::_slaveBoxSwitch(JsonDocument& root) {
  // expected JSON string: {"senderNodeName":"201";"senderAPIP":"...";"senderStIP":"...";"action":"s";"senderBoxActiveState":"on"}
  /*
      Explanation of index numbers in the array of boolean arrays B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0 or 1]:
      const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
      - The first index number (iSlaveOnOffReaction) selects one of the pair of HIGH/LOW reactions listed in the upper dimension of the array.
        It is set via the iSlaveOnOffReaction variable. It is itself set either:
        - at startup; equal to the global constant I_DEFAULT_SLAVE_ON_OFF_REACTION (in the global variables definition);
        - via the changeSlaveReaction sub (in case the user has decided to change it via a web control).
      - The second index number (0 or 1) indicates the current status of the masterBox and selects, within the HIGH/LOW pair, the reaction of the slaveBox.
  */
  // extract the root[rootKey]
  // const char* __cSenderBoxActiveState = root["senderBoxActiveState"];
  // Serial.printf("myMeshController::_slaveBoxSwitch() %s alloted from root[\"senderBoxActiveState\"] to senderBoxActiveState \n", __cSenderBoxActiveState);

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

// void myMeshController::_pinPairing(JsonDocument& root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"p";"pt":"0"}
  // short _iTargetPairingType;
  // const char* _sTargetPairingType = root["ts"];
  // _iTargetPairingType = atoi(_sTargetPairingType);
  // LaserGroupedUnitsArray::pairUnpairAllPins(_iTargetPairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/);
// }

// void myMeshController::_dataRequest() {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"d"}
  // ----------------------
// }
