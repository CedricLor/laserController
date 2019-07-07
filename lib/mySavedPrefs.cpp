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
  *  sBoxDefaultState;
  *  gbNodeName
  *  bMasterNodePrefix
  *  MY_INDEX_IN_CB_ARRAY

  *  ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName;
  *  iSlaveOnOffReaction

  *  iInterfaceNodeId (e.g. 2760139053)
  *  bInterfaceNodeName (e.g. 201)
  *  isMeshRoot
  *  isInterface

  *  sBoxesCount
  *  PIN_COUNT

  *  ssid
  *  pass

  */

void mySavedPrefs::savePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");

  // Instantiate Preferences
  Preferences preferences;

  // Open namespace "savedSettingsNS" in read-write
  preferences.begin("savedSettingsNS", /*read only = */false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  // Immediately add a "savedSettings" key:value pair
  // (incrementing a counter of the number of time save
  // operations have taken place)
  preferences.putShort("savedSettings", preferences.getShort("savedSettings", 0) + 1);

  // Prepare some debug constant in PROGMEM
  const PROGMEM char _debugMsgStart[] = "PREFERENCES: savePreferences(): the value of";
  const PROGMEM char _debugMsgEndSuccess[] = "has been saved to \"savedSettingsNS\":";
  const PROGMEM char _debugMsgEndFail[] = "could not be saved to \"savedSettingsNS\":";

  // save value of sBoxDefaultState
  size_t _sBoxDefaultStateRet = preferences.putShort("sBoxDefSta", sBoxDefaultState);
  Serial.printf("%s sBoxDefaultState == %i %s\"sBoxDefSta\"\n", _debugMsgStart, sBoxDefaultState, (_sBoxDefaultStateRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of gbNodeName
  size_t _gbNodeNameRet = preferences.putShort("bNodeName", (short)gbNodeName);
  Serial.printf("%s gbNodeName == %i %s\"bNodeName\"\n", _debugMsgStart, gbNodeName, (_gbNodeNameRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of bMasterBoxName
  size_t _masterNodeNameRet = preferences.putShort("bMasterNName", (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName);
  Serial.printf("%s ControlerBoxes[%i].bMasterBoxName == %i %s\"bMasterNName\"\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, (short)ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, (_masterNodeNameRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of iSlaveOnOffReaction
  // preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);

  // save value of iInterfaceNodeId
  size_t _iInterfaceNodeIdRet = preferences.putInt("iIFNodId", iInterfaceNodeId);
  Serial.printf("%s iInterfaceNodeId == %i %s\"iIFNodId\"\n", _debugMsgStart, iInterfaceNodeId, (_iInterfaceNodeIdRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of bInterfaceNodeName
  size_t _bInterfaceNodeNameRet = preferences.putShort("iIFNodId", (short)bInterfaceNodeName);
  Serial.printf("%s bInterfaceNodeName == %i %s\"iIFNodId\"\n", _debugMsgStart, bInterfaceNodeName, (_bInterfaceNodeNameRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of isMeshRoot
  size_t _isMeshRootRet = preferences.putBool("isMeshRoot", isMeshRoot);
  Serial.printf("%s isMeshRoot == %i %s\"isMeshRoot\"\n", _debugMsgStart, isMeshRoot, (_isMeshRootRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of isInterface
  size_t _isInterfaceRet = preferences.putBool("isIF", isInterface);
  Serial.printf("%s isInterface == %i %s\"isIF\"\n", _debugMsgStart, isInterface, (_isInterfaceRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of sBoxesCount
  size_t _sBoxesCountRet = preferences.putShort("sBoxesCount", sBoxesCount);
  Serial.printf("%s sBoxesCount == %i %s\"sBoxesCount\"\n", _debugMsgStart, sBoxesCount, (_sBoxesCountRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of ssid
  size_t _sssidRet = preferences.putString("ssid", ssid);
  Serial.printf("%s ssid == %s %s\"ssid\"\n", _debugMsgStart, ssid, (_sssidRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // save value of pass
  size_t _spassRet = preferences.putString("pass", pass);
  Serial.printf("%s pass == %s %s\"pass\"\n", _debugMsgStart, pass, (_spassRet)?(_debugMsgEndSuccess):(_debugMsgEndFail));

  // Tell user how many free entries remain
  size_t _freeEntries = preferences.freeEntries();
  Serial.printf("%s Save complete. Remaining free entries in NVS: %i", _debugMsgStart, _freeEntries);

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

      // gbNodeName
      gbNodeName = preferences.getShort("bNodeName", (short)gbNodeName);
      Serial.printf("%s gbNodeName set to: %i\n", _debugMsgStart, gbNodeName);

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

      // isInterface
      isInterface = preferences.getBool("isIF", isInterface);
      Serial.printf("%s isInterface set to: %i\n", _debugMsgStart, isInterface);

      // isMeshRoot
      isMeshRoot = preferences.getBool("isMeshRoot", isMeshRoot);
      Serial.printf("%s isMeshRoot set to: %i\n", _debugMsgStart, isMeshRoot);

      // sBoxesCount
      sBoxesCount = preferences.getShort("sBoxesCount", sBoxesCount);
      Serial.printf("%s sBoxesCount set to: %i\n", _debugMsgStart, sBoxesCount);

      // ssid
      char _ssid[20];
      if (preferences.getString("ssid", _ssid, 20)) {
        strcpy(ssid, (const char*)_ssid);
        Serial.printf("%s ssid set to: %s\n", _debugMsgStart, ssid);
      }

      // pass
      char _pass[30];
      if (preferences.getString("pass", _pass, 30)) {
        strcpy(pass, (const char*)_pass);
        Serial.printf("%s pass set to: %s\n", _debugMsgStart, pass);
      }
    }
  } else {
    Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%i].bMasterBoxName (%i) and sBoxesCount (%i) will keep their default values\n", _debugMsgStart, MY_INDEX_IN_CB_ARRAY, ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bMasterBoxName, sBoxesCount);
  }

  // Tell user how many free entries remain
  size_t _freeEntries = preferences.freeEntries();
  Serial.printf("%s Save complete. Remaining free entries in NVS: %i", _debugMsgStart, _freeEntries);

  preferences.end();

  // for testing purposes
  // savePreferences();

  Serial.print("%s done\n");
}
