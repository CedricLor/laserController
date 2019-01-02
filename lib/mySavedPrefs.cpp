/*
  mySavedPrefs.cpp - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2007.
*/

#include "Arduino.h"
#include "mySavedPrefs.h"

mySavedPrefs::mySavedPrefs()
{
}

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putUInt("savedSettings", preferences.getUInt("savedSettings", 0) + 1);
  preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);
  preferences.putShort("iMasterNName", iMasterNodeName);
  preferences.putULong("pinBlinkInt", pinBlinkingInterval);

  preferences.end();
  Serial.print("WEB CONTROLLER: savePreferences(): done\n");
}

void mySavedPrefs::loadPreferences() {
  Serial.print("\nSETUP: loadPreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", true);        // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).
  unsigned int savedSettings = preferences.getUInt("savedSettings", 0);
  if (savedSettings > 0) {
    Serial.print("SETUP: loadPreferences(). NVS has saved settings. Loading values.\n");
    iSlaveOnOffReaction = preferences.getShort("iSlavOnOffReac", I_DEFAULT_SLAVE_ON_OFF_REACTION);
    Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", iSlaveOnOffReaction);
    iMasterNodeName = preferences.getShort("iMasterNName", iMasterNodeName);
    Serial.printf("SETUP: loadPreferences(). iMasterNodeName set to: %u\n", iMasterNodeName);
    pinBlinkingInterval = preferences.getULong("pinBlinkInt", pinBlinkingInterval);
    Serial.print("SETUP: loadPreferences(). pinBlinkingInterval set to: ");Serial.println(pinBlinkingInterval);
  }
  preferences.end();
  Serial.print("SETUP: loadPreferences(): done\n");
}
