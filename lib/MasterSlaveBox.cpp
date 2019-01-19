/*
  MasterSlaveBox.cpp - Library to handle relationship of this box with other controller box related functions.
  Created by Cedric Lor, January 3, 2019.
*/

#include "Arduino.h"
#include "MasterSlaveBox.h"

MasterSlaveBox::MasterSlaveBox()
{
}

// REDRAFT AND ADD COMMENTS TO THE CODE OF THE WHOLE BLOCK
void MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action) {
  changeTheMasterBoxId(masterBoxNumber);
  changeSlaveReaction(action);
  mySavedPrefs::savePreferences();
}

void MasterSlaveBox::changeTheMasterBoxId(const short masterBoxNumber) {
  // Serial.printf("WEB CONTROLLER: changeTheMasterBoxId(const short masterBoxNumber): Starting with masterBoxNumber = %u\n", masterBoxNumber);
  iMasterNodeName = I_MASTER_NODE_PREFIX + masterBoxNumber;
  // Serial.print("WEB CONTROLLER: changeTheMasterBoxId(const short masterBoxNumber): Done\n");
}

void MasterSlaveBox::changeSlaveReaction(const char* action) {
  // Serial.printf("WEB CONTROLLER: changeSlaveReaction(char *action): starting with action (char argument) =%s\n", action);
  for (short i=0; i < 4; i++) {
    // Serial.print("WEB CONTROLLER: changeSlaveReaction(): looping over the slaveReactionHtml[] array\n");
    if (strcmp(slaveReactionHtml[i], action) > 0) {
      // Serial.print("WEB CONTROLLER: changeSlaveReaction(): saving iSlaveOnOffReaction\n");
      short t = i;
      iSlaveOnOffReaction = t;
      break; // break for
    }
  }
  // Serial.print("WEB CONTROLLER: changeSlaveReaction(): done\n");
}
