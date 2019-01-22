/*
myMeshController.cpp - Library to handle mesh controller related functions.
Created by Cedric Lor, January 22, 2019.
*/

#include "Arduino.h"
#include "myMeshController.h"


myMeshController::myMeshController()
{
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

void myMeshController::AutoSwitchRelaysController(const char* senderStatus) {
  /*
      Explanation of index numbers in the array of boolean arrays B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0 or 1]:
      const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
      - First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
        It is set via the iSlaveOnOffReaction variable. It is itself set either:
        - at startup and equal to the global constant I_DEFAULT_SLAVE_ON_OFF_REACTION (in the global variables definition);
        - via the changeSlaveReaction sub (in case the user has decided to change it via a web control).
      - Second index number is the reaction to the "on" state of the master box if 0, to its "off" state if 1.
  */
  Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(senderStatus) starting.\n senderStatus = %s.\n", senderStatus);
  const char* myFutureState = _B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0] == LOW ? "on" : "off";
  if (strstr(senderStatus, "on")  > 0) {                              // if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(...): Turning myself to %s.\n", myFutureState);
    LaserPinsArray::autoSwitchAllRelays(_B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0]);
  } else if (strstr(senderStatus, "off")  > 0) {
    Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(...): Turning myself to %s.\n", myFutureState);
    LaserPinsArray::autoSwitchAllRelays(_B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1]);
  }
  Serial.print("MESH: autoSwitchAllRelaysMeshWrapper(...): done\n");
}
