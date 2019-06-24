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
|  |  |
|  |  |--myMeshViews.cpp
|  |  |  |--myMeshViews.h
|  |  |  |--boxState.cpp
|  |  |  |  |--boxState.h
|  |  |  |--//LaserGroupedUnitsArray.cpp
|  |  |  |  |--//LaserGroupedUnitsArray.h
*/

#include "Arduino.h"
#include "myMeshController.h"

myMeshController::myMeshController(JsonDocument root)
{
  char _action = root["action"];
  //////// Manual mode
  if (_action == 'u') {           // action 'u' for this message requests that the laserPin be turned into or out of User controlled mode
    _manualSwitch(root);
    return;
  }
  //////// IR mode
  if (_action == 'i') {           // action 'i' for this message requests that the laserPin be turned into or out of IR control mode
    _changeInclusionIR(root);
    return;
  }
  //////// Blinking interval
  if (_action == 'b') {           // action 'b' for this message relates to a blinking interval, that this box should update as the case may be
    _changeBlinkingInterval(root);
    return;
  }
  //////// masterBox and reaction to masterBox status change
  if (_action == 'm') {           // action 'm' for this message relates to a master node number, that this box should update as the case may be
    _changeMasterBox(root);
    return;
  }
  if (_action == 's') {           // action 's' for status of master box has changed
    _slaveBoxSwitch(root);
    return;
  }
  if (_action == 'p') {           // action 't' for this message orders the pairing
    _pinPairing(root);            // (either of type unpairing, twin pairing or cooperative)
    return;                       // of the pins, that this box should update as the case may be
  }
  if (_action == 'd') {           // action 'd' for this message requests that this box returns Data on it current states
    _dataRequest();
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

void myMeshController::_manualSwitch(JsonDocument _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"u";"ts":"0"}
  short _iTargetState;
  const char* _sTargetState = _root["ts"];
  _iTargetState = atoi(_sTargetState);
  // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_iTargetState);
  // LaserGroupedUnitsArray::setTargetState(4);      // 4 means turn the state of the LaserGroupedUnitsArray to manual
}

void myMeshController::_changeInclusionIR(JsonDocument _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"i";"ts":"0"}
  short _iTargetState;
  const char* _sTargetState = _root["ts"];
  _iTargetState = atoi(_sTargetState);
  // LaserGroupedUnitsArray::setTargetPirState(_iTargetState /*0 for false = out of IR control; 1 for true = under IR control */);
}

void myMeshController::_changeBlinkingInterval(JsonDocument _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"b";"ti":"5000"}
  unsigned long _ulTargetBlinkingInterval;
  const char* _sTargetBlinkingInterval = _root["ti"];
  _ulTargetBlinkingInterval = atoi(_sTargetBlinkingInterval);
  // LaserGroupedUnitsArray::setTargetBlinkingInterval(_ulTargetBlinkingInterval);
}

void myMeshController::_changeMasterBox(JsonDocument _root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"m";"ms":"201";"react":"syn"}
  short iNewMasterBoxNumber;
  const char* sNewMasterBoxNumber = _root["ms"];
  iNewMasterBoxNumber = atoi(sNewMasterBoxNumber);
  const char* sNewReaction = _root["react"];
  // const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};
  MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(iNewMasterBoxNumber, sNewReaction);
}

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

void myMeshController::_slaveBoxSwitch(JsonDocument root) {
  // expected JSON string: {"senderNodeName":"201";"senderAPIP":"...";"senderStIP":"...";"action":"s";"senderStatus":"on"}
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
  const char* cSenderStatus = root["senderStatus"];
  Serial.printf("myMeshController::_slaveBoxSwitch() %s alloted from root[\"senderStatus\"] to senderStatus \n", cSenderStatus);

  // The following line has for sole purpose to provide data to the Serial.printfs below
  const char* myFutureState = _B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0] == LOW ? "on" : "off";

  if (strstr(cSenderStatus, "on")  > 0) {
    // if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    Serial.printf("myMeshController::_slaveBoxSwitch(): Turning myself to %s.\n", myFutureState);
    // LaserGroupedUnitsArray::setTargetState(3);  /*3 means turn the LaserGroupedUnitsArray state machine to the state: in slave box mode, with on status */
    // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0]);
  } else if (strstr(cSenderStatus, "off")  > 0) {
    // else if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    Serial.printf("myMeshController::_slaveBoxSwitch(): Turning myself to %s.\n", myFutureState);
    // LaserGroupedUnitsArray::setTargetState(LaserGroupedUnitsArray::previousState);  /*3 means turn the LaserGroupedUnitsArray state machine to the state: in slave box mode, with off status */
    // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1]);
  }
  Serial.print("myMeshController::_slaveBoxSwitch(): done\n");
}

void myMeshController::_pinPairing(JsonDocument root) {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"p";"pt":"0"}
  short _iTargetPairingType;
  const char* _sTargetPairingType = root["ts"];
  _iTargetPairingType = atoi(_sTargetPairingType);
  // LaserGroupedUnitsArray::pairUnpairAllPins(_iTargetPairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/);
}

void myMeshController::_dataRequest() {
  // expected JSON string: {"senderNodeName":"001";"senderAPIP":"...";"senderStIP":"...";"action":"d"}
  // ----------------------
}
