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
  *  iInterfaceNodeId (e.g. 2760139053)
  *  bInterfaceNodeName (e.g. 201)
  *  gui8ControllerBoxPrefix
  *  bInterfaceIndexInCBArray -- not saved, calculated
  *  sBoxesCount
  *  PIN_COUNT
  *  ssid
  *  pass

  *  sBoxDefaultState
  *  gui8NodeName
  *  gui8MyIndexInCBArray -- not saved, calculated
  *  ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName;
  *  iSlaveOnOffReaction
  *  isInterface
  */






// Prepare some debug constant in PROGMEM
const PROGMEM char mySavedPrefs::_debugSaveMsgStart[] = "PREFERENCES: savePreferences(): the value of";
const PROGMEM char mySavedPrefs::_debugSaveMsgEndSuccess[] = "has been saved to \"savedSettingsNS\":";
const PROGMEM char mySavedPrefs::_debugSaveMsgEndFail[] = "could not be saved to \"savedSettingsNS\":";
const PROGMEM char mySavedPrefs::_debugLoadMsgStart[] = "SETUP: loadPreferences():";




// GLOBAL WRAPPERS
void mySavedPrefs::savePrefsWrapper() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");

  // Instantiate Preferences
  Preferences _preferences;

  _startSavePreferences(_preferences);

  _saveNetworkCredentials(_preferences);
  _saveNetworkEssentialPreferences(_preferences);
  _saveUselessPreferences(_preferences);

  _saveBoxEssentialPreferences(_preferences);
  _saveBoxBehaviorPreferences(_preferences);

  _endPreferences(_preferences);

  Serial.print("PREFERENCES: savePreferences(): starting\n");
}





void mySavedPrefs::loadPrefsWrapper() {
  Serial.print("\nSETUP: loadPrefsWrapper(): starting\n");

  // Instanciate preferences
  Preferences _preferences;

  if (_preferences.begin("savedSettingsNS", /*read only = */true) == true){       // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).

    // read the value of "savedSettingsNS":"savedSettings"
    unsigned int _savedSettings =_preferences.getShort("savedSettings", 0);
    Serial.printf("%s \"savedSettings\" = _savedSettings = %i \n", _debugLoadMsgStart, _savedSettings);

    // if the value of _savedSettings > 0, this means that some settings have been saved in the past
    if (_savedSettings > 0) {
      Serial.printf("%s NVS has saved settings. Loading values.\n", _debugLoadMsgStart);

      _loadNetworkCredentials(_preferences);
      _loadNetworkEssentialPreferences(_preferences);
      _loadUselessPreferences(_preferences);

      _loadBoxEssentialPreferences(_preferences);
      _loadBoxBehaviorPreferences(_preferences);

    } else {
      Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%i].bMasterBoxName (%i) and sBoxesCount (%i) will keep their default values\n", _debugLoadMsgStart, gui8MyIndexInCBArray, ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName, sBoxesCount);
    }
  }

  _endPreferences(_preferences);
  Serial.print("\nSETUP: loadPrefsWrapper(): ending\n");
}





void mySavedPrefs::saveBoxSpecificPrefsWrapper(void (&callBack)(Preferences&)) {
  // Instantiate Preferences
  Preferences _preferences;

  _startSavePreferences(_preferences);

  callBack(_preferences);

  _endPreferences(_preferences);
}






void mySavedPrefs::loadBoxSpecificPrefsWrapper(void (&callBack)(Preferences&)) {
  // Instanciate preferences
  Preferences _preferences;

  if (_preferences.begin("savedSettingsNS", /*read only = */true) == true){       // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).

    // read the value of "savedSettingsNS":"savedSettings"
    unsigned int _savedSettings =_preferences.getShort("savedSettings", 0);
    Serial.printf("%s \"savedSettings\" = _savedSettings = %i \n", _debugLoadMsgStart, _savedSettings);

    // if the value of _savedSettings > 0, this means that some settings have been saved in the past
    if (_savedSettings > 0) {
      Serial.printf("%s NVS has saved settings. Loading values.\n", _debugLoadMsgStart);

      callBack(_preferences);

    } else {
      Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%i].bMasterBoxName (%i) and sBoxesCount (%i) will keep their default values\n", _debugLoadMsgStart, gui8MyIndexInCBArray, ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName, sBoxesCount);
    }
  }

  _endPreferences(_preferences);
}





///////////////////////////////////////////////////
// SAVERS
///////////////////////////////////////////////////
void mySavedPrefs::_startSavePreferences(Preferences& _preferences) {
  Serial.print("PREFERENCES: savePreferences(): starting\n");


  // Open namespace "savedSettingsNS" in read-write
  _preferences.begin("savedSettingsNS", /*read only = */false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  // // Immediately add a "savedSettings" key:value pair
  // // (incrementing a counter of the number of time save
  // // operations have taken place)
  // // size_t _sSavedSettingsRet =_preferences.putShort("savedSettings",_preferences.getShort("savedSettings", 0) + 1);
  // // Serial.printf("%s savedSettings == %u %s\"savedSettings\"\n", _debugSaveMsgStart, _preferences.getShort("savedSettings"), (_sSavedSettingsRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));
}




/*
  ssid
  pass
  gatewayIP
  ui16GatewayPort
  ui8WifiChannel
*/
void mySavedPrefs::_saveNetworkCredentials(Preferences& _preferences) {
  // save value of ssid
  // Interface only
  // -> reboot required
  size_t _sssidRet = _preferences.putString("ssid", ssid);
  Serial.printf("%s ssid == %s %s\"ssid\"\n", _debugSaveMsgStart, ssid, (_sssidRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of pass
  // Interface only
  // -> reboot required
  size_t _spassRet = _preferences.putString("pass", pass);
  Serial.printf("%s pass == %s %s\"pass\"\n", _debugSaveMsgStart, pass, (_spassRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of gatewayIP
  // Interface only
  // -> reboot required
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8GatewayIP[4] = {gatewayIP[0], gatewayIP[1], gatewayIP[2], gatewayIP[3]};
  size_t _bsGatewayIPRet = _preferences.putBytes("gateIP", _ui8GatewayIP, 4);
  Serial.printf("%s gatewayIP == %s %s\"gateIP\"\n", _debugSaveMsgStart, gatewayIP.toString().c_str(), (_bsGatewayIPRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of ui16GatewayPort
  // Interface only
  // -> reboot required
  // putShort(const char* key, int16_t value)
  size_t _ui16GatewayPortRet = _preferences.putShort("gatePort", ui16GatewayPort);
  Serial.printf("%s ui16GatewayPort == %i %s\"gatePort\"\n", _debugSaveMsgStart, ui16GatewayPort, (_ui16GatewayPortRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of ui8WifiChannel
  // Interface only
  // -> reboot required
  // putUChar(const char* key, uint8_t value)
  size_t _ui8WifiChannelRet = _preferences.putUChar("wifiChan", ui8WifiChannel);
  Serial.printf("%s ui8WifiChannel == %u %s\"wifiChan\"\n", _debugSaveMsgStart, ui8WifiChannel, (_ui8WifiChannelRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));
}






/*
  gui8ControllerBoxPrefix
  sBoxesCount
*/
void mySavedPrefs::_saveNetworkEssentialPreferences(Preferences& _preferences) {
  // save value of gui8ControllerBoxPrefix
  // -> no reboot but very messy if no reboot of the IF and the whole mesh
  // putUChar(const char* key, uint8_t value)
  size_t _ggui8ControllerBoxPrefixRet = _preferences.putUChar("bContrBPref", gui8ControllerBoxPrefix);
  Serial.printf("%s gui8ControllerBoxPrefix == %u %s\"bContrBPref\"\n", _debugSaveMsgStart, gui8ControllerBoxPrefix, (_ggui8ControllerBoxPrefixRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of sBoxesCount
  // Not a very usefull value: the number of boxes created at startup is based on the constant BOXES_COUNT
  // Even a reboot would not suffice to have this param taken into account
  size_t _sBoxesCountRet = _preferences.putShort("sBoxesCount", sBoxesCount);
  Serial.printf("%s sBoxesCount == %i %s\"sBoxesCount\"\n", _debugSaveMsgStart, sBoxesCount, (_sBoxesCountRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));
}





/*
  iInterfaceNodeId
  bInterfaceNodeName
  bInterfaceIndexInCBArray
*/
void mySavedPrefs::_saveUselessPreferences(Preferences& _preferences) {
  // USELESS PREFERENCES
  // save value of iInterfaceNodeId
  // Not a very usefull value: the interface nodeid is fixed at startup in global
  // It is never used thereafter (usually using ControlerBoxes[bInterfaceIndexInCBArray].nodeId or the senderID in the mesh)
  // Even a reboot would not suffice to have this param taken into account
  size_t _iInterfaceNodeIdRet = _preferences.putUInt("iIFNodId", iInterfaceNodeId);
  Serial.printf("%s iInterfaceNodeId == %u %s\"iIFNodId\"\n", _debugSaveMsgStart, iInterfaceNodeId, (_iInterfaceNodeIdRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of bInterfaceNodeName
  // Not a very usefull value: the interface node name is fixed at startup based on the constant B_DEFAULT_INTERFACE_NODE_NAME
  // It is never used thereafter (usually using ControlerBoxes[bInterfaceIndexInCBArray].nodeName)
  // Even a reboot would not suffice to have this param taken into account
  size_t _bInterfaceNodeNameRet = _preferences.putShort("sIFNodNam", (short)bInterfaceNodeName);
  Serial.printf("%s bInterfaceNodeName == %i %s\"sIFNodNam\"\n", _debugSaveMsgStart, bInterfaceNodeName, (_bInterfaceNodeNameRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // recalculate bInterfaceIndexInCBArray with the new values of bInterfaceNodeName and gui8ControllerBoxPrefix
  // It is never used thereafter (usually using ControlerBoxes[bInterfaceIndexInCBArray].nodeName)
  // -> no reboot but very messy if no reboot of the IF and the whole mesh
  bInterfaceIndexInCBArray = bInterfaceNodeName - gui8ControllerBoxPrefix;
  Serial.printf("%s bInterfaceIndexInCBArray recalculated to: %i (not saved)\n", _debugSaveMsgStart, bInterfaceIndexInCBArray);
}






/*
  gui8NodeName
  gui8MyIndexInCBArray
  isInterface
*/
// Need a reboot
void mySavedPrefs::_saveBoxEssentialPreferences(Preferences& _preferences) {
  // save value of gui8NodeName (global byte nodeName)
  // Note to use Prefs without reboot:
  // -> would need a reboot
  // -> fix: call ControlerBox::updateThisBoxProperties
  // this value is then used in ControlerBox::updateThisBoxProperties
  // to set ControlerBoxes[gui8MyIndexInCBArray].bNodeName
  // putUChar(const char* key, uint8_t value)
  size_t _gui8NodeNameRet = _preferences.putUChar("bNodeName", gui8NodeName);
  Serial.printf("%s gui8NodeName == %u %s\"bNodeName\"\n", _debugSaveMsgStart, gui8NodeName, (_gui8NodeNameRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // recalculate gui8MyIndexInCBArray with the new values of gui8NodeName and gui8ControllerBoxPrefix
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot required, but very messy without a reboot of the whole mesh
  // -> fix: complicated; would need each and everybox to update its ControlerBoxes array
  // this value is then used in ControlerBox::updateThisBoxProperties
  // to set ControlerBoxes[gui8MyIndexInCBArray].bNodeName
  gui8MyIndexInCBArray = gui8NodeName - gui8ControllerBoxPrefix;
  Serial.printf("%s gui8MyIndexInCBArray recalculated to: %i (not saved)\n", _debugSaveMsgStart, gui8MyIndexInCBArray);

  // save value of isInterface
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot but quite messy (this would require a mesh reconfiguration)
  // In addition, the interface is supposed to be also box 200 and the root node
  size_t _isInterfaceRet = _preferences.putBool("isIF", isInterface);
  Serial.printf("%s isInterface == %i %s\"isIF\"\n", _debugSaveMsgStart, isInterface, (_isInterfaceRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));
}





/*
  ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState
  ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName
*/
void mySavedPrefs::_saveBoxBehaviorPreferences(Preferences& _preferences) {
  // save value of sBoxDefaultState
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from ControlerBoxes[gui8MyIndexInCBArray])
  size_t _sBoxDefaultStateRet = _preferences.putShort("sBoxDefSta", ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState);
  Serial.printf("%s sBoxDefaultState == %i %s\"sBoxDefSta\"\n", _debugSaveMsgStart, ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState, (_sBoxDefaultStateRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from ControlerBoxes[gui8MyIndexInCBArray])
  size_t _masterNodeNameRet = _preferences.putShort("bMasterNName", (short)ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName);
  Serial.printf("%s ControlerBoxes[%i].bMasterBoxName == %i %s\"bMasterNName\"\n", _debugSaveMsgStart, gui8MyIndexInCBArray, (short)ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName, (_masterNodeNameRet)?(_debugSaveMsgEndSuccess):(_debugSaveMsgEndFail));

  // save value of iSlaveOnOffReaction
  // _preferences.putShort("iSlavOnOffReac", iSlaveOnOffReaction);
}







///////////////////////////////////////////////////
// LOADERS
///////////////////////////////////////////////////
/*
  ssid
  pass
*/
void mySavedPrefs::_loadNetworkCredentials(Preferences& _preferences){
  // ssid
  char _ssid[20];
  if (_preferences.getString("ssid", _ssid, 20)) {
    strcpy(ssid, (const char*)_ssid);
    Serial.printf("%s ssid set to: %s\n", _debugLoadMsgStart, ssid);
  }

  // pass
  char _pass[30];
  if (_preferences.getString("pass", _pass, 30)) {
    strcpy(pass, (const char*)_pass);
    Serial.printf("%s pass set to: %s\n", _debugLoadMsgStart, pass);
  }

  // get the value of gatewayIP
  // Interface only
  // -> reboot required
  // getBytesLength(const char* key)
  // getBytes(const char* key, void * buf, size_t maxLen)
  if (_preferences.getBytesLength("gateIP")) {
    char _gatewayIPBuffer[4];
    _preferences.getBytes("gateIP", _gatewayIPBuffer, 4);
    for (int __ipIt=0; __ipIt<4; __ipIt++) {
      gatewayIP[__ipIt] = _gatewayIPBuffer[__ipIt];
    }
    Serial.printf("%s gatewayIP set to: %s\n", _debugLoadMsgStart, gatewayIP.toString().c_str());
  } else {
    Serial.printf("%s gatewayIP could not be retrieved.\n", _debugLoadMsgStart);
  }

  // get the value of ui16GatewayPort
  // getShort(const char* key, const int16_t defaultValue)
  ui16GatewayPort = _preferences.getShort("gatePort", ui16GatewayPort);
  Serial.printf("%s ui16GatewayPort set to: %i\n", _debugLoadMsgStart, ui16GatewayPort);

  // get the value of ui8WifiChannel
  // getUChar(const char* key, const uint8_t defaultValue)
  ui8WifiChannel = _preferences.getUChar("wifiChan", ui8WifiChannel);
  Serial.printf("%s ui8WifiChannel set to: %i\n", _debugLoadMsgStart, ui8WifiChannel);
}





/*
  gui8ControllerBoxPrefix
  sBoxesCount
*/
void mySavedPrefs::_loadNetworkEssentialPreferences(Preferences& _preferences){
  // gui8ControllerBoxPrefix
  // getUChar(const char* key, const uint8_t defaultValue)
  gui8ControllerBoxPrefix = _preferences.getUChar("bContrBPref", gui8ControllerBoxPrefix);
  Serial.printf("%s gui8ControllerBoxPrefix set to: %i\n", _debugLoadMsgStart, gui8ControllerBoxPrefix);

  // sBoxesCount
  sBoxesCount =_preferences.getShort("sBoxesCount", sBoxesCount);
  Serial.printf("%s sBoxesCount set to: %i\n", _debugLoadMsgStart, sBoxesCount);
}






/*
  iInterfaceNodeId
  bInterfaceNodeName
  bInterfaceIndexInCBArray
*/
void mySavedPrefs::_loadUselessPreferences(Preferences& _preferences){
  // iInterfaceNodeId
  // Serial.printf("%s TRACING I_DEFAULT_INTERFACE_NODE_ID AND iInterfaceNodeId\n", _debugLoadMsgStart);
  // Serial.print("I_DEFAULT_INTERFACE_NODE_ID = ");Serial.println(I_DEFAULT_INTERFACE_NODE_ID);
  // Serial.print("iInterfaceNodeId = ");Serial.println(iInterfaceNodeId);
  iInterfaceNodeId =_preferences.getUInt("iIFNodId", iInterfaceNodeId);
  // Serial.print("iInterfaceNodeId = ");Serial.println(iInterfaceNodeId);
  Serial.printf("%s iInterfaceNodeId set to: %u\n", _debugLoadMsgStart, iInterfaceNodeId);

  // bInterfaceNodeName
  bInterfaceNodeName = (byte)_preferences.getShort("iIFNodName", (short)bInterfaceNodeName);
  Serial.printf("%s bInterfaceNodeName set to: %i\n", _debugLoadMsgStart, bInterfaceNodeName);

  // recalculate bInterfaceIndexInCBArray with the new values of bInterfaceNodeName and gui8ControllerBoxPrefix
  bInterfaceIndexInCBArray = bInterfaceNodeName - gui8ControllerBoxPrefix;
  Serial.printf("%s bInterfaceIndexInCBArray reset to: %i\n", _debugLoadMsgStart, bInterfaceIndexInCBArray);
}







/*
  gui8NodeName
  gui8MyIndexInCBArray
  isInterface
*/
void mySavedPrefs::_loadBoxEssentialPreferences(Preferences& _preferences){
  // gui8NodeName
  // getUChar(const char* key, const uint8_t defaultValue)
  gui8NodeName = _preferences.getUChar("bNodeName", gui8NodeName);
  Serial.printf("%s gui8NodeName set to: %u\n", _debugLoadMsgStart, gui8NodeName);

  // recalculate gui8MyIndexInCBArray with the new values of gui8NodeName and gui8ControllerBoxPrefix
  gui8MyIndexInCBArray = gui8NodeName - gui8ControllerBoxPrefix;
  Serial.printf("%s gui8MyIndexInCBArray reset to: %i\n", _debugLoadMsgStart, gui8MyIndexInCBArray);

  // isInterface
  isInterface =_preferences.getBool("isIF", isInterface);
  Serial.printf("%s isInterface set to: %i\n", _debugLoadMsgStart, isInterface);
}






/*
  ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState
  ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName
*/
void mySavedPrefs::_loadBoxBehaviorPreferences(Preferences& _preferences){
  // sBoxDefaultState
  ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState =_preferences.getShort("sBoxDefSta", ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState);
  Serial.printf("%s sBoxDefaultState set to: %i\n", _debugLoadMsgStart, ControlerBoxes[gui8MyIndexInCBArray].sBoxDefaultState);

  // ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName
  // If there is a value saved for bMasterNName, reset
  // ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName
  // which is set by default to UI8_DEFAULT_MASTER_NODE_NAME
  // in the ControlerBox constructor. Else, the value of
  // ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName
  // will stay unchanged.
  ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName = (byte)_preferences.getShort("bMasterNName", (short)ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName);
  Serial.printf("%s ControlerBoxes[%i].bMasterBoxName set to: %i\n", _debugLoadMsgStart, gui8MyIndexInCBArray, ControlerBoxes[gui8MyIndexInCBArray].bMasterBoxName);

  // iSlaveOnOffReaction
  // iSlaveOnOffReaction =_preferences.getShort("iSlavOnOffReac", iSlaveOnOffReaction);
  // Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", iSlaveOnOffReaction);
}





void mySavedPrefs::_endPreferences(Preferences& _preferences) {
  // Tell user how many free entries remain
  size_t _freeEntries = _preferences.freeEntries();
  Serial.printf("PREFERENCES: Remaining free entries in NVS: %i\n", _freeEntries);

  _preferences.end();
  Serial.print(F("PREFERENCES: done\n"));
}
