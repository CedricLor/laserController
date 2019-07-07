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

  *  iInterfaceNodeId
  *  global::bInterfaceNodeName or B_DEFAULT_INTERFACE_NODE_NAME
  *  global::B_NODE_NAME, B_MASTER_NODE_PREFIX and/or MY_INDEX_IN_CB_ARRAY

  *  MESH_ROOT
  *  IS_INTERFACE
  *  sBoxesCount

  *  PIN_COUNT
  */

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", /*read only = */false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putShort("savedSettings", preferences.getShort("savedSettings", 0) + 1);

  const PROGMEM char _debugMsgStart[] = "PREFERENCES: savePreferences(): the value of";
  const PROGMEM char _debugMsgEndSuccess[] = "has been saved to \"savedSettingsNS\":";
  const PROGMEM char _debugMsgEndFail[] = "could not be saved to \"savedSettingsNS\":";

  // iSlaveOnOffReaction
  // preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);

  // bMasterBoxName
  size_t _masterNodeNameRet = preferences.putShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  if (_masterNodeNameRet) {
    Serial.printf("%s ControlerBoxes[%i].bMasterBoxName == %i %s\"bMasterNName\"\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, _debugMsgEndSuccess);
  } else {
    Serial.printf("%s ControlerBoxes[%i].bMasterBoxName == %i %s\"bMasterNName\"\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, _debugMsgEndFail);
  }

  // sBoxesCount
  size_t _sBoxesCountRet = preferences.putShort("sBoxesCount", sBoxesCount);
  if (_sBoxesCountRet) {
    Serial.printf("%s sBoxesCount == %i %s\"sBoxesCount\"\n", _debugMsgStart, sBoxesCount, _debugMsgEndSuccess);
  } else {
    Serial.printf("%s sBoxesCount == %i %s\"sBoxesCount\"\n", _debugMsgStart, sBoxesCount, _debugMsgEndFail);
  }

  // iInterfaceNodeId
  size_t _iInterfaceNodeId = preferences.putShort("iIFNodId", iInterfaceNodeId);
  if (_iInterfaceNodeId) {
    Serial.printf("%s iInterfaceNodeId == %i %s\"iIFNodId\"\n", _debugMsgStart, iInterfaceNodeId, _debugMsgEndSuccess);
  } else {
    Serial.printf("%s iInterfaceNodeId == %i %s\"bMasterNName\"\n", _debugMsgStart, iInterfaceNodeId, _debugMsgEndFail);
  }

  preferences.end();
  Serial.print(F("PREFERENCES: savePreferences(): done\n"));
}




void mySavedPrefs::loadPreferences() {
  Serial.print("\nSETUP: loadPreferences(): starting\n");
  // Instanciate preferences
  Preferences preferences;

  const PROGMEM char _debugMsgStart[] = "SETUP: loadPreferences():";

  if (preferences.begin("savedSettingsNS", /*read only = */true) == true){       // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).

    // read the value of "savedSettingsNS":"savedSettings"
    unsigned int _savedSettings = preferences.getShort("savedSettings", 0);
    Serial.printf("%s \"savedSettings\" = _savedSettings = %i \n", _debugMsgStart, _savedSettings);

    // if the value of _savedSettings > 0, this means that some settings have been saved in the past
    if (_savedSettings > 0) {
      Serial.printf("%s NVS has saved settings. Loading values.\n", _debugMsgStart);

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
      Serial.printf("%s ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName set to: %i\n", _debugMsgStart, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);

      // sBoxesCount
      sBoxesCount = preferences.getShort("sBoxesCount", sBoxesCount);
      Serial.printf("%s sBoxesCount set to: %i\n", _debugMsgStart, sBoxesCount);

      // iInterfaceNodeId
      sBoxesCount = preferences.getInt("iIFNodId", iInterfaceNodeId);
      Serial.printf("%s sBoxesCount set to: %i\n", _debugMsgStart, iInterfaceNodeId);
    }
  } else {
    Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%i].bMasterBoxName (%i) and sBoxesCount (%i) will keep their default values\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, sBoxesCount);
  }

  preferences.end();

  // for testing purposes
  // savePreferences();

  Serial.print("%s done\n");
}
