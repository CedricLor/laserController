/*
  MasterSlaveBox.cpp -
  Library to handle relationship of this box with other controller box related functions.
  This library is not a class to be instantiated.
  This library does not contain any static variable.
  It is just a container for a couple of functions to change:
  - this boxes' master controller; and
  - this boxes' reactions to signals of the master controller.
  Created by Cedric Lor, January 3, 2019.
*/

#include "Arduino.h"
#include "MasterSlaveBox.h"

MasterSlaveBox::MasterSlaveBox()
{
}


/* changeGlobalMasterBoxAndSlaveReaction changes the master node number (name) and
  the reaction to master node signals received via the mesh.
  changeGlobalMasterBoxAndSlaveReaction is called from:
  - the webControler; and
  - the meshController.
*/

void MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(const byte bMasterBoxNumber, const char* action) {
  _changeTheMasterBoxId(bMasterBoxNumber);
  _changeSlaveReaction(action);
  mySavedPrefs::savePrefsWrapper();
}



void MasterSlaveBox::_changeTheMasterBoxId(const byte bMasterBoxNumber) {
  // Serial.printf("MasterSlaveBox::_changeTheMasterBoxId(const byte bMasterBoxNumber): Starting with bMasterBoxNumber = %u\n", bMasterBoxNumber);
  thisBox.ui16MasterBoxName = bMasterBoxNumber;
  // Serial.print("MasterSlaveBox::_changeTheMasterBoxId(const byte bMasterBoxNumber): Done\n");
}



void MasterSlaveBox::_changeSlaveReaction(const char* action) {
  // Serial.printf("MasterSlaveBox::_changeSlaveReaction(const char* action): starting with action (char argument) =%s\n", action);
  for (short i=0; i < 4; i++) {
    // Serial.print("MasterSlaveBox::_changeSlaveReaction(const char* action): looping over the slaveReactionHtml[] array\n");
    if (strcmp(slaveReactionHtml[i], action) > 0) {
      // Serial.print("MasterSlaveBox::_changeSlaveReaction(const char* action): saving iSlaveOnOffReaction\n");
      iSlaveOnOffReaction = i;  // iSlaveOnOffReaction has been defined in the global singleton
      break; // break for
    }
  }
  // Serial.print("MasterSlaveBox::_changeSlaveReaction(const char* action): done\n");
}
