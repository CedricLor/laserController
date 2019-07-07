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
  *  sBoxDefaultState = S_BOX_DEFAULT_STATE;

  *  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName;
  *  iSlaveOnOffReaction

  *  iInterfaceNodeId (e.g. 2760139053)
  *  bInterfaceNodeName (e.g. 201)
  *  global::B_NODE_NAME, B_MASTER_NODE_PREFIX and/or MY_INDEX_IN_CB_ARRAY

  *  isMeshRoot
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

  // sBoxDefaultState
  size_t _sBoxDefaultStateRet = preferences.putShort("sBoxDefSta", sBoxDefaultState);
  Serial.printf("%s sBoxDefaultState == %i %s\"sBoxDefSta\"\n", _debugMsgStart, sBoxDefaultState, (_sBoxDefaultStateRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // bMasterBoxName
  size_t _masterNodeNameRet = preferences.putShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  Serial.printf("%s ControlerBoxes[%i].bMasterBoxName == %i %s\"bMasterNName\"\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, (_masterNodeNameRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // iSlaveOnOffReaction
  // preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);

  // iInterfaceNodeId
  size_t _iInterfaceNodeIdRet = preferences.putInt("iIFNodId", iInterfaceNodeId);
  Serial.printf("%s iInterfaceNodeId == %i %s\"iIFNodId\"\n", _debugMsgStart, iInterfaceNodeId, (_iInterfaceNodeIdRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // bInterfaceNodeName
  size_t _bInterfaceNodeNameRet = preferences.putShort("iIFNodId", (short)bInterfaceNodeName);
  Serial.printf("%s bInterfaceNodeName == %i %s\"iIFNodId\"\n", _debugMsgStart, bInterfaceNodeName, (_bInterfaceNodeNameRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // isMeshRoot
  size_t _isMeshRootRet = preferences.putBool("isMeshRoot", isMeshRoot);
  Serial.printf("%s isMeshRoot == %i %s\"isMeshRoot\"\n", _debugMsgStart, isMeshRoot, (_isMeshRootRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // sBoxesCount
  size_t _sBoxesCountRet = preferences.putShort("sBoxesCount", sBoxesCount);
  Serial.printf("%s sBoxesCount == %i %s\"sBoxesCount\"\n", _debugMsgStart, sBoxesCount, (_sBoxesCountRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

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

      // sBoxDefaultState
      sBoxDefaultState = preferences.getShort("sBoxDefSta", sBoxDefaultState);
      Serial.printf("%s sBoxDefaultState set to: %i\n", _debugMsgStart, sBoxDefaultState);

      // bMasterBoxName
      // If there is a value saved for bMasterNName, reset
      // ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName
      // which is set by default to B_DEFAULT_MASTER_NODE_NAME
      // in the ControlerBox constructor. Else, the value of
      // ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName
      // will stay unchanged.
      ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName = (byte)preferences.getShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
      Serial.printf("%s ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName set to: %i\n", _debugMsgStart, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);

      // iSlaveOnOffReaction
      // iSlaveOnOffReaction = preferences.getShort("iSlavOnOffReac", iSlaveOnOffReaction);
      // Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", iSlaveOnOffReaction);

      // iInterfaceNodeId
      iInterfaceNodeId = preferences.getInt("iIFNodId", iInterfaceNodeId);
      Serial.printf("%s iInterfaceNodeId set to: %i\n", _debugMsgStart, iInterfaceNodeId);

      // bInterfaceNodeName
      bInterfaceNodeName = preferences.getShort("iIFNodName", bInterfaceNodeName);
      Serial.printf("%s bInterfaceNodeName set to: %i\n", _debugMsgStart, bInterfaceNodeName);

      // isMeshRoot
      isMeshRoot = preferences.getBool("isMeshRoot", isMeshRoot);
      Serial.printf("%s isMeshRoot set to: %i\n", _debugMsgStart, isMeshRoot);

      // sBoxesCount
      sBoxesCount = preferences.getShort("sBoxesCount", sBoxesCount);
      Serial.printf("%s sBoxesCount set to: %i\n", _debugMsgStart, sBoxesCount);
    }
  } else {
    Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%i].bMasterBoxName (%i) and sBoxesCount (%i) will keep their default values\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, sBoxesCount);
  }

  preferences.end();

  // for testing purposes
  // savePreferences();

  Serial.print("%s done\n");
}
