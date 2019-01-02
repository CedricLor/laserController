/*
  mySavedPrefs.cpp - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2007.
*/

#include "Arduino.h"
#include "mySavedPrefs.h"

mySavedPrefs::mySavedPrefs(short iSlaveOnOffReaction, short iMasterNodeName, unsigned long pinBlinkingInterval, const short I_DEFAULT_SLAVE_ON_OFF_REACTION)
{
  _iSlaveOnOffReaction = iSlaveOnOffReaction;
  _iMasterNodeName = iMasterNodeName;
  _pinBlinkingInterval = pinBlinkingInterval;
  _I_DEFAULT_SLAVE_ON_OFF_REACTION = I_DEFAULT_SLAVE_ON_OFF_REACTION;
}

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putUInt("savedSettings", preferences.getUInt("savedSettings", 0) + 1);
  preferences.putShort("iSlavOnOffReac", _iSlaveOnOffReaction);
  preferences.putShort("iMasterNName", _iMasterNodeName);
  preferences.putULong("pinBlinkInt", _pinBlinkingInterval);

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
    _iSlaveOnOffReaction = preferences.getShort("iSlavOnOffReac", _I_DEFAULT_SLAVE_ON_OFF_REACTION);
    Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", _iSlaveOnOffReaction);
    _iMasterNodeName = preferences.getShort("iMasterNName", _iMasterNodeName);
    Serial.printf("SETUP: loadPreferences(). iMasterNodeName set to: %u\n", _iMasterNodeName);
    _pinBlinkingInterval = preferences.getULong("pinBlinkInt", _pinBlinkingInterval);
    Serial.print("SETUP: loadPreferences(). pinBlinkingInterval set to: ");Serial.println(_pinBlinkingInterval);
  }
  preferences.end();
  Serial.print("SETUP: loadPreferences(): done\n");
}
