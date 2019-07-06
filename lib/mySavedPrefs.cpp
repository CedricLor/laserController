/*
  mySavedPrefs.cpp - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "mySavedPrefs.h"

mySavedPrefs::mySavedPrefs()
{
}

  /* variables to be saved in NVS:
  *  short int boxState::_boxDefaultState = 5;
  *  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName;
  *  global::iSlaveOnOffReaction;
  */

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", /*read only = */false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putShort("savedSettings", preferences.getShort("savedSettings", 0) + 1);
  // preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);
  preferences.putShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);

  preferences.end();
  Serial.print("PREFERENCES: savePreferences(): done\n");
}

void mySavedPrefs::loadPreferences() {
  Serial.print("\nSETUP: loadPreferences(): starting\n");
  // Instanciate preferences
  Preferences preferences;
  if (preferences.begin("savedSettingsNS", /*read only = */true) == true){       // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).
    unsigned int savedSettings = preferences.getUInt("savedSettings", 0);
    if (savedSettings > 0) {
      Serial.print("SETUP: loadPreferences(). NVS has saved settings. Loading values.\n");

      // iSlaveOnOffReaction = preferences.getShort("iSlavOnOffReac", iSlaveOnOffReaction);
      // Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", iSlaveOnOffReaction);

      // If there is a value saved for bMasterNName, reset
      // ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName
      // which is set by default to B_DEFAULT_MASTER_NODE_NAME
      // in the ControlerBox constructor. Else, the value of
      // ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName
      // will stay unchanged.
      ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName = (byte)preferences.getShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
      Serial.printf("SETUP: loadPreferences(). ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName set to: %i\n", ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
    }
  } else {
    Serial.printf("SETUP: loadPreferences(): \"savedSettingsNS\" does not exist. ControlerBoxes[%i].bMasterBoxName will keep the default B_DEFAULT_MASTER_NODE_NAME = %i\n", MY_INDEX_IN_CB_ARRAY, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  }

  preferences.end();
  Serial.print("SETUP: loadPreferences(): done\n");
}
