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
  *  global::iSlaveOnOffReaction; or I_DEFAULT_SLAVE_ON_OFF_REACTION

  *  global::I_DEFAULT_INTERFACE_NODE_ID; or iInterfaceNodeId
  *  global::bInterfaceNodeName or B_DEFAULT_INTERFACE_NODE_NAME
  *  global::B_NODE_NAME, B_MASTER_NODE_PREFIX and/or MY_INDEX_IN_CB_ARRAY

  *  MESH_ROOT, IS_INTERFACE and/or IS_STATION_MANUAL
  *  BOXES_COUNT

  *  PIN_COUNT
  */

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", /*read only = */false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putShort("savedSettings", preferences.getShort("savedSettings", 0) + 1);
  // preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);

  size_t _masterNodeNameRet = preferences.putShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  if (_masterNodeNameRet) {
    Serial.printf("PREFERENCES: savePreferences(): the value of ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName == %i has been saved to \"savedSettingsNS\":\"bMasterNName\"\n", ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  } else {
    Serial.printf("PREFERENCES: savePreferences(): the value of ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName == %i could not be saved to \"savedSettingsNS\":\"bMasterNName\"\n", ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  }

  preferences.end();
  Serial.print("PREFERENCES: savePreferences(): done\n");
}

void mySavedPrefs::loadPreferences() {
  Serial.print("\nSETUP: loadPreferences(): starting\n");
  // Instanciate preferences
  Preferences preferences;
  if (preferences.begin("savedSettingsNS", /*read only = */true) == true){       // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).

    // read the value of "savedSettingsNS":"savedSettings"
    unsigned int _savedSettings = preferences.getShort("savedSettings", 0);
    Serial.printf("SETUP: loadPreferences(). \"savedSettings\" = _savedSettings = %i \n", _savedSettings);

    // if the value of _savedSettings > 0, this means that some settings have been saved in the past
    if (_savedSettings > 0) {
      Serial.print("SETUP: loadPreferences(). NVS has saved settings. Loading values.\n");

      // iSlaveOnOffReaction
      // iSlaveOnOffReaction = preferences.getShort("iSlavOnOffReac", iSlaveOnOffReaction);
      // Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", iSlaveOnOffReaction);

      // bMasterBoxName
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

  // for testing purposes
  // savePreferences();

  Serial.print("SETUP: loadPreferences(): done\n");
}
