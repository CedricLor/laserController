/*
  mySavedPrefs.cpp - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "mySavedPrefs.h"

mySavedPrefs::mySavedPrefs()
{
}

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", /*read only = */false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putShort("savedSettings", preferences.getShort("savedSettings", 0) + 1);
  // preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);
  preferences.putShort("bMasterNName", bMasterNodeName);

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
      bMasterNodeName = (byte)preferences.getShort("bMasterNName", bMasterNodeName);
      Serial.printf("SETUP: loadPreferences(). bMasterNodeName set to: %i\n", bMasterNodeName);
    }
  } else {
    Serial.printf("SETUP: loadPreferences(): \"savedSettingsNS\" does not exist. global bMasterNodeName will keep the default B_DEFAULT_MASTER_NODE_NAME = %i\n", bMasterNodeName);
  }

  preferences.end();
  Serial.print("SETUP: loadPreferences(): done\n");
}
