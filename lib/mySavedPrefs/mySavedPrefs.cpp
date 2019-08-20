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
  *  gui32InterfaceNodeId (e.g. 2760139053)
  *  gui16InterfaceNodeName (e.g. 201)
  *  gui16ControllerBoxPrefix
  *  gui16InterfaceIndexInCBArray -- not saved, calculated
  *  gui16BoxesCount
  *  PIN_COUNT
  *  ssid
  *  pass

  *  sBoxDefaultState
  *  gui16NodeName
  *  gui16MyIndexInCBArray -- not saved, calculated
  *  ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName;
  *  iSlaveOnOffReaction
  *  isInterface
  */






// Prepare some debug constant in PROGMEM
const PROGMEM char mySavedPrefs::debugSaveMsgStart[] = "PREFERENCES: savePreferences(): the value of";
const PROGMEM char mySavedPrefs::debugSaveMsgEndSuccess[] = "has been saved to \"savedSettingsNS\":";
const PROGMEM char mySavedPrefs::debugSaveMsgEndFail[] = "could not be saved to \"savedSettingsNS\":";
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

  if (_preferences.begin("savedSettingsNS", /*read only = */true)){       // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).

    // read the value of "savedSettingsNS":"savedSettings"
    unsigned int _savedSettings =_preferences.getShort("savedSettings", 0);
    Serial.printf("%s \"savedSettings\" = _savedSettings = %i \n", _debugLoadMsgStart, _savedSettings);

      _loadNetworkCredentials(_preferences);
      _loadNetworkEssentialPreferences(_preferences);
      _loadUselessPreferences(_preferences);

      loadOTASuccess(_preferences);
      _loadBoxStartupTypePreferences(_preferences);
      _loadBoxEssentialPreferences(_preferences);
      _loadBoxBehaviorPreferences(_preferences);

    } else {
      Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%i].ui16MasterBoxName (%i) and gui16BoxesCount (%i) will keep their default values\n", _debugLoadMsgStart, gui16MyIndexInCBArray, ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName, gui16BoxesCount);
    }

  _endPreferences(_preferences);

  // On reboot, if the number of requested OTA reboots is superior to 0,
  // decrement it by 1 (and save it to NVS) until it reaches 0, where
  // the ESP will reboot to its normal state
  if (gi8RequestedOTAReboots) {
    gi8OTAReboot = 1;
    gi8RequestedOTAReboots = gi8RequestedOTAReboots - 1;
    saveBoxSpecificPrefsWrapper(_saveBoxStartupTypePreferences);
  }

  Serial.print("\nSETUP: loadPrefsWrapper(): ending\n");
}






void mySavedPrefs::saveFromNetRequest(JsonObject& _obj) {
  Serial.println("mySavedPrefs::saveFromNetRequest: starting.");
  // {"action":"changeBox","key":"save","val":"wifi","dataset":{"wssid":"LTVu_dG9ydG9y","wpass":"totototo","wgw":"192.168.43.1","wgwp":"0","wfip":"192.168.43.50","wnm":"255.255.255.0","wch":"6"},"lb":0}
  if (_obj["val"] == "wifi") {
    Serial.println("mySavedPrefs::saveFromNetRequest: going to save WIFI preferences.");
    // load data from Json to memory
    JsonObject _joDataset = _obj["dataset"];
    
    strlcpy(ssid,               _joDataset["wssid"] | ssid, 32);
    strlcpy(pass,               _joDataset["wpass"] | pass, 32);
    gatewayIP.fromString(       _joDataset["wgw"].as<const char*>());
    ui16GatewayPort           = _joDataset["wgwp"];
    ui8WifiChannel            = _joDataset["wch"];
    fixedIP.fromString(         _joDataset["wfip"].as<const char*>());
    fixedNetmaskIP.fromString(  _joDataset["wnm"].as<const char*>());

    saveBoxSpecificPrefsWrapper(_saveNetworkCredentials);
    return;
  }


  // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 0, reboots: 2}
  if (_obj["val"] == "gi8RequestedOTAReboots") {
    Serial.println("mySavedPrefs::saveFromNetRequest: going to save gi8RequestedOTAReboots preferences.");
    gi8RequestedOTAReboots = _obj["reboots"];
    saveBoxSpecificPrefsWrapper(_saveBoxStartupTypePreferences);
    saveBoxSpecificPrefsWrapper(_resetOTASuccess);
    return;
  }

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
      Serial.printf("%s \"savedSettingsNS\" does not exist. ControlerBoxes[%u].ui16MasterBoxName (%u) and gui16BoxesCount (%u) will keep their default values\n", _debugLoadMsgStart, gui16MyIndexInCBArray, ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName, gui16BoxesCount);
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
  // // Serial.printf("%s savedSettings == %u %s\"savedSettings\"\n", debugSaveMsgStart, _preferences.getShort("savedSettings"), (_sSavedSettingsRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}




/*
  ssid
  pass
  gatewayIP
  ui16GatewayPort
  ui8WifiChannel
  fixedIP
  fixedNetmaskIP
*/
void mySavedPrefs::_saveNetworkCredentials(Preferences& _preferences) {
  // save value of ssid
  // Interface only
  // -> restart the mesh
  size_t _sssidRet = _preferences.putString("ssid", ssid);
  Serial.printf("%s ssid == %s %s\"ssid\"\n", debugSaveMsgStart, ssid, (_sssidRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of pass
  // Interface only
  // -> restart the mesh
  size_t _spassRet = _preferences.putString("pass", pass);
  Serial.printf("%s pass == %s %s\"pass\"\n", debugSaveMsgStart, pass, (_spassRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of gatewayIP
  // Interface only
  // -> restart the mesh
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8GatewayIP[4] = {gatewayIP[0], gatewayIP[1], gatewayIP[2], gatewayIP[3]};
  size_t _bsGatewayIPRet = _preferences.putBytes("gateIP", _ui8GatewayIP, 4);
  Serial.printf("%s gatewayIP == %s %s\"gateIP\"\n", debugSaveMsgStart, gatewayIP.toString().c_str(), (_bsGatewayIPRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of ui16GatewayPort
  // Interface only
  // -> restart the mesh
  // putShort(const char* key, int16_t value)
  size_t _ui16GatewayPortRet = _preferences.putUShort("gatePort", ui16GatewayPort);
  Serial.printf("%s ui16GatewayPort == %u %s\"gatePort\"\n", debugSaveMsgStart, ui16GatewayPort, (_ui16GatewayPortRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of ui8WifiChannel
  // Interface only
  // -> restart the mesh
  // putUChar(const char* key, uint8_t value)
  size_t _ui8WifiChannelRet = _preferences.putUChar("wifiChan", ui8WifiChannel);
  Serial.printf("%s ui8WifiChannel == %u %s\"wifiChan\"\n", debugSaveMsgStart, ui8WifiChannel, (_ui8WifiChannelRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of fixedIP
  // Interface only
  // -> restart the mesh
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8fixedIP[4] = {fixedIP[0], fixedIP[1], fixedIP[2], fixedIP[3]};
  size_t _bsFixedIPRet = _preferences.putBytes("fixedIP", _ui8fixedIP, 4);
  Serial.printf("%s IF fixed IP == %s %s\"fixedIP\"\n", debugSaveMsgStart, fixedIP.toString().c_str(), (_bsFixedIPRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of fixedNetmaskIP
  // Interface only
  // -> restart the mesh
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8FixedNetmaskIP[4] = {fixedNetmaskIP[0], fixedNetmaskIP[1], fixedNetmaskIP[2], fixedNetmaskIP[3]};
  size_t _bsFixedNetmaskIPRet = _preferences.putBytes("netMask", _ui8FixedNetmaskIP, 4);
  Serial.printf("%s Wifi netmask == %s %s\"netMask\"\n", debugSaveMsgStart, fixedNetmaskIP.toString().c_str(), (_bsFixedNetmaskIPRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}






/*
  gui16ControllerBoxPrefix
  gui16BoxesCount
*/
void mySavedPrefs::_saveNetworkEssentialPreferences(Preferences& _preferences) {
  // save value of gui16ControllerBoxPrefix
  // -> no reboot but very messy if no reboot of the IF and the whole mesh
  // putUChar(const char* key, uint8_t value)
  size_t _ggui16ControllerBoxPrefixRet = _preferences.putUChar("bContrBPref", (uint8_t)gui16ControllerBoxPrefix);
  Serial.printf("%s gui16ControllerBoxPrefix == %i %s\"bContrBPref\"\n", debugSaveMsgStart, gui16ControllerBoxPrefix, (_ggui16ControllerBoxPrefixRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of gui16BoxesCount
  // Not a very usefull value: the number of boxes created at startup is based on the constant UI8_BOXES_COUNT
  // Even a reboot would not suffice to have this param taken into account
  size_t _gui16BoxesCountRet = _preferences.putUChar("BoxesCount", (uint8_t)gui16BoxesCount);
  Serial.printf("%s gui16BoxesCount == %u %s\"BoxesCount\"\n", debugSaveMsgStart, gui16BoxesCount, (_gui16BoxesCountRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}





/*
  gui32InterfaceNodeId
  gui16InterfaceNodeName
  gui16InterfaceIndexInCBArray
*/
void mySavedPrefs::_saveUselessPreferences(Preferences& _preferences) {
  // USELESS PREFERENCES
  // save value of gui32InterfaceNodeId
  // Not a very usefull value: the interface nodeid is fixed at startup in global
  // It is never used thereafter (usually using ControlerBoxes[gui16InterfaceIndexInCBArray].nodeId or the senderID in the mesh)
  // Even a reboot would not suffice to have this param taken into account
  size_t _gui32InterfaceNodeIdRet = _preferences.putUInt("iIFNodId", gui32InterfaceNodeId);
  Serial.printf("%s gui32InterfaceNodeId == %u %s\"iIFNodId\"\n", debugSaveMsgStart, gui32InterfaceNodeId, (_gui32InterfaceNodeIdRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of gui16InterfaceNodeName
  // Not a very usefull value: the interface node name is fixed at startup based on the constant
  // It is never used thereafter (usually using ControlerBoxes[gui16InterfaceIndexInCBArray].nodeName)
  // Even a reboot would not suffice to have this param taken into account
  size_t _gui16InterfaceNodeNameRet = _preferences.putUChar("sIFNodNam", (uint8_t)gui16InterfaceNodeName);
  Serial.printf("%s gui16InterfaceNodeName == %u %s\"sIFNodNam\"\n", debugSaveMsgStart, gui16InterfaceNodeName, (_gui16InterfaceNodeNameRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // recalculate gui16InterfaceIndexInCBArray with the new values of gui16InterfaceNodeName and gui16ControllerBoxPrefix
  // It is never used thereafter (usually using ControlerBoxes[gui16InterfaceIndexInCBArray].nodeName)
  // -> no reboot but very messy if no reboot of the IF and the whole mesh
  gui16InterfaceIndexInCBArray = gui16InterfaceNodeName - gui16ControllerBoxPrefix;
  Serial.printf("%s gui16InterfaceIndexInCBArray recalculated to: %u (not saved)\n", debugSaveMsgStart, gui16InterfaceIndexInCBArray);
}








/*
  gi8RequestedOTAReboots
*/
void mySavedPrefs::_saveBoxStartupTypePreferences(Preferences& _preferences) {
  // save value of gi8RequestedOTAReboots
  // Note to use Prefs without reboot: needs a reboot to be effective
  size_t _gi8RequestedOTARebootsRet = _preferences.putChar("OTARebReq", gi8RequestedOTAReboots);
  Serial.printf("%s gi8RequestedOTAReboots == %i %s\"OTARebReq\"\n", debugSaveMsgStart, gi8RequestedOTAReboots, (_gi8RequestedOTARebootsRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}

/*
  ui8OTA1SuccessErrorCode
  ui8OTA2SuccessErrorCode
*/
void mySavedPrefs::_resetOTASuccess(Preferences& _preferences) {
  // resets the values of ui8OTA1SuccessErrorCode and ui8OTA2SuccessErrorCode when an OTA reboot is requested
  // Note to use Prefs without reboot: no need for reboots; used to display data in console and webpage
  size_t _ui8OTA1SuccessErrorCodeRet = _preferences.putUChar("OTASucc1", 11);
  Serial.printf("%s OTA update numb. 1 success code == %u %s\"OTASucc1\"\n", debugSaveMsgStart, 11, (_ui8OTA1SuccessErrorCodeRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
  size_t _ui8OTA2SuccessErrorCodeRet = _preferences.putUChar("OTASucc2", 11);
  Serial.printf("%s OTA update numb. 2 success code == %u %s\"OTASucc2\"\n", debugSaveMsgStart, 11, (_ui8OTA2SuccessErrorCodeRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}







/*
  gui16NodeName
  gui16MyIndexInCBArray
  isInterface
  ui32RootNodeId
  isRoot
*/
// Need a reboot
void mySavedPrefs::_saveBoxEssentialPreferences(Preferences& _preferences) {
  // save value of gui16NodeName (global int8_t nodeName)
  // Note to use Prefs without reboot:
  // -> would need a reboot
  // -> fix: call ControlerBox::updateThisBoxProperties
  // this value is then used in ControlerBox::updateThisBoxProperties
  // to set ControlerBoxes[gui16MyIndexInCBArray].ui16NodeName
  // putUChar(const char* key, uint8_t value)
  size_t _gui16NodeNameRet = _preferences.putUChar("ui8NdeName", (uint8_t)(gui16NodeName));
  Serial.printf("%s gui16NodeName == %u %s\"ui8NdeName\"\n", debugSaveMsgStart, gui16NodeName, (_gui16NodeNameRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // recalculate gui16MyIndexInCBArray with the new values of gui16NodeName and gui16ControllerBoxPrefix
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot required, but very messy without a reboot of the whole mesh
  // -> fix: complicated; would need each and everybox to update its ControlerBoxes array
  // this value is then used in ControlerBox::updateThisBoxProperties
  // to set ControlerBoxes[gui16MyIndexInCBArray].ui16NodeName
  gui16MyIndexInCBArray = gui16NodeName - gui16ControllerBoxPrefix;
  Serial.printf("%s gui16MyIndexInCBArray recalculated to: %u (not saved)\n", debugSaveMsgStart, gui16MyIndexInCBArray);

  /*
    Save value of isInterface
    -> runtime change possible; would require a restart of painlessMesh
    See below for possible implications with ui32RootNodeId and isRoot
  */
  size_t _isInterfaceRet = _preferences.putBool("isIF", isInterface);
  Serial.printf("%s isInterface == %i %s\"isIF\"\n", debugSaveMsgStart, isInterface, (_isInterfaceRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // /*
  //   Save value of ui32RootNodeId
  //   -> runtime change possible, but 
  //     - would require a restart of painlessMesh on this node (and probably on all the other nodes)
  //     - would also require a symetric change to be done on another
  //     node (ex. if this node was root and shall no longer be,
  //     another node shall be assigned this role and painlessMesh shall 
  //     also be restarted on the other node, at the minimum ; ideally, the mesh shall be restarted
  //     on all the nodes, so that each get the opportunity to look for the root node).
  // */
  // // size_t putULong(const char* key, uint32_t value);
  // size_t _ui32RootNodeIdRet = _preferences.putULong("RootNId", ui32RootNodeId);
  // Serial.printf("%s ui32RootNodeId == %u %s\"ui32RootNodeId\"\n", debugSaveMsgStart, ui32RootNodeId, (_ui32RootNodeIdRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  /*
    Save value of isRoot
    -> runtime change possible, but 
      - would require a restart of painlessMesh on this node
      - would also require a symetric change to be done on another
      node (ex. if this node was root and shall no longer be,
      another node shall be assigned this role and painlessMesh shall 
      also be restarted on the other node)
    isRoot shall not be changed "directly" (i.e. by the user). isRoot shall
    be calculated from the comparison of ui32RootNodeId with the current
    node id of this ESP in the mesh.
  */
  // isRoot = (ui32RootNodeId == laserControllerMesh.getNodeId());
  size_t _isRootRet = _preferences.putBool("isRoot", isRoot);
  Serial.printf("%s isRoot == %i %s\"isRoot\"\n", debugSaveMsgStart, isRoot, (_isRootRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}





/*
  ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState
  ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName
*/
void mySavedPrefs::_saveBoxBehaviorPreferences(Preferences& _preferences) {
  // save value of sBoxDefaultState
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from ControlerBoxes[gui16MyIndexInCBArray])
  size_t _sBoxDefaultStateRet = _preferences.putShort("sBoxDefSta", ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState);
  Serial.printf("%s sBoxDefaultState == %i %s\"sBoxDefSta\"\n", debugSaveMsgStart, ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState, (_sBoxDefaultStateRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from ControlerBoxes[gui16MyIndexInCBArray])
  size_t _masterNodeNameRet = _preferences.putUChar("bMasterNName", (uint8_t)ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName);
  Serial.printf("%s ControlerBoxes[%u].ui16MasterBoxName == %u %s\"bMasterNName\"\n", debugSaveMsgStart, gui16MyIndexInCBArray, ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName, (_masterNodeNameRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}







///////////////////////////////////////////////////
// LOADERS
///////////////////////////////////////////////////
/*
  ssid
  pass
  gatewayIP
  ui16GatewayPort
  ui8WifiChannel
  fixedIP
  fixedNetmaskIP
*/
void mySavedPrefs::_loadNetworkCredentials(Preferences& _preferences){
  // ssid
  // -> restart the mesh
  char _ssid[20];
  if (_preferences.getString("ssid", _ssid, 20)) {
    strcpy(ssid, (const char*)_ssid);
    Serial.printf("%s ssid set to: %s\n", _debugLoadMsgStart, ssid);
  }

  // pass
  // -> restart the mesh
  char _pass[30];
  if (_preferences.getString("pass", _pass, 30)) {
    strcpy(pass, (const char*)_pass);
    Serial.printf("%s pass set to: %s\n", _debugLoadMsgStart, pass);
  }

  // get the value of gatewayIP
  // Interface only
  // -> restart the mesh
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
  // -> restart the mesh
  // getShort(const char* key, const int16_t defaultValue)
  ui16GatewayPort = _preferences.getUShort("gatePort", ui16GatewayPort);
  Serial.printf("%s ui16GatewayPort set to: %u\n", _debugLoadMsgStart, ui16GatewayPort);

  // get the value of ui8WifiChannel
  // -> restart the mesh
  // getUChar(const char* key, const uint8_t defaultValue)
  ui8WifiChannel = _preferences.getUChar("wifiChan", ui8WifiChannel);
  Serial.printf("%s ui8WifiChannel set to: %i\n", _debugLoadMsgStart, ui8WifiChannel);

  // get the value of fixedIP
  // Interface only
  // -> restart the mesh
  // getBytesLength(const char* key)
  // getBytes(const char* key, void * buf, size_t maxLen)
  if (_preferences.getBytesLength("fixedIP")) {
    char _fixedIPBuffer[4];
    _preferences.getBytes("fixedIP", _fixedIPBuffer, 4);
    for (int __ipIt=0; __ipIt<4; __ipIt++) {
      fixedIP[__ipIt] = _fixedIPBuffer[__ipIt];
    }
    Serial.printf("%s fixedIP set to: %s\n", _debugLoadMsgStart, fixedIP.toString().c_str());
  } else {
    Serial.printf("%s fixedIP could not be retrieved.\n", _debugLoadMsgStart);
  }

  // get the value of fixedNetmaskIP
  // Interface only
  // -> restart the mesh
  // getBytesLength(const char* key)
  // getBytes(const char* key, void * buf, size_t maxLen)
  if (_preferences.getBytesLength("netMask")) {
    char _fixedNetmaskIPBuffer[4];
    _preferences.getBytes("netMask", _fixedNetmaskIPBuffer, 4);
    for (int __ipIt=0; __ipIt<4; __ipIt++) {
      fixedNetmaskIP[__ipIt] = _fixedNetmaskIPBuffer[__ipIt];
    }
    Serial.printf("%s fixedNetmaskIP set to: %s\n", _debugLoadMsgStart, fixedNetmaskIP.toString().c_str());
  } else {
    Serial.printf("%s fixedNetmaskIP could not be retrieved.\n", _debugLoadMsgStart);
  }
}





/*
  gui16ControllerBoxPrefix
  gui16BoxesCount
*/
void mySavedPrefs::_loadNetworkEssentialPreferences(Preferences& _preferences){
  // gui16ControllerBoxPrefix
  // getUChar(const char* key, const uint8_t defaultValue)
  gui16ControllerBoxPrefix = (uint16_t)_preferences.getUChar("bContrBPref", gui16ControllerBoxPrefix);
  Serial.printf("%s gui16ControllerBoxPrefix set to: %i\n", _debugLoadMsgStart, gui16ControllerBoxPrefix);

  // gui16BoxesCount
  gui16BoxesCount = (uint16_t)_preferences.getUChar("gui16BoxesCount", gui16BoxesCount);
  Serial.printf("%s gui16BoxesCount set to: %u\n", _debugLoadMsgStart, gui16BoxesCount);
}








/*
  uint8_t ui8OTA1SuccessErrorCode
  uint8_t ui8OTA2SuccessErrorCode
  uint8_t ui8RebootsSinceLastOTAReboot = 0;
*/
uint8_t mySavedPrefs::ui8OTA1SuccessErrorCode = 11;
uint8_t mySavedPrefs::ui8OTA2SuccessErrorCode = 11;

void mySavedPrefs::loadOTASuccess(Preferences& _preferences) {
  // save the success code in the relevant NVS location
  ui8OTA1SuccessErrorCode = _preferences.getUChar("OTASucc1", ui8OTA1SuccessErrorCode);
  Serial.printf("%s ui8OTA1SuccessErrorCode set to: %u\n", _debugLoadMsgStart, ui8OTA1SuccessErrorCode);
  ui8OTA2SuccessErrorCode = _preferences.getUChar("OTASucc2", ui8OTA2SuccessErrorCode);
  Serial.printf("%s ui8OTA2SuccessErrorCode set to: %u\n", _debugLoadMsgStart, ui8OTA2SuccessErrorCode);
}






/*
  gui32InterfaceNodeId
  gui16InterfaceNodeName
  gui16InterfaceIndexInCBArray
*/
void mySavedPrefs::_loadUselessPreferences(Preferences& _preferences){
  // gui32InterfaceNodeId
  // Serial.printf("%s TRACING UI32_DEFAULT_INTERFACE_NODE_ID AND gui32InterfaceNodeId\n", _debugLoadMsgStart);
  // Serial.print("UI32_DEFAULT_INTERFACE_NODE_ID = ");Serial.println(UI32_DEFAULT_INTERFACE_NODE_ID);
  // Serial.print("gui32InterfaceNodeId = ");Serial.println(gui32InterfaceNodeId);
  gui32InterfaceNodeId =_preferences.getUInt("iIFNodId", gui32InterfaceNodeId);
  // Serial.print("gui32InterfaceNodeId = ");Serial.println(gui32InterfaceNodeId);
  Serial.printf("%s gui32InterfaceNodeId set to: %u\n", _debugLoadMsgStart, gui32InterfaceNodeId);

  // gui16InterfaceNodeName
  gui16InterfaceNodeName = (uint16_t)(_preferences.getUChar("iIFNodName", gui16InterfaceNodeName));
  Serial.printf("%s gui16InterfaceNodeName set to: %u\n", _debugLoadMsgStart, gui16InterfaceNodeName);

  // recalculate gui16InterfaceIndexInCBArray with the new values of gui16InterfaceNodeName and gui16ControllerBoxPrefix
  gui16InterfaceIndexInCBArray = gui16InterfaceNodeName - gui16ControllerBoxPrefix;
  Serial.printf("%s gui16InterfaceIndexInCBArray reset to: %u\n", _debugLoadMsgStart, gui16InterfaceIndexInCBArray);
}








/*
  gi8RequestedOTAReboots
*/
void mySavedPrefs::_loadBoxStartupTypePreferences(Preferences& _preferences) {
  gi8RequestedOTAReboots =_preferences.getChar("OTARebReq", gi8RequestedOTAReboots);
  Serial.printf("%s gi8RequestedOTAReboots set to: %i\n", _debugLoadMsgStart, gi8RequestedOTAReboots);
}





/*
  gui16NodeName
  gui16MyIndexInCBArray
  isInterface
  ui32RootNodeId
  isRoot
*/
void mySavedPrefs::_loadBoxEssentialPreferences(Preferences& _preferences){
  // gui16NodeName
  // getUChar(const char* key, const uint8_t defaultValue)
  gui16NodeName = (uint16_t)(_preferences.getUChar("ui8NdeName", gui16NodeName));
  Serial.printf("%s gui16NodeName set to: %i\n", _debugLoadMsgStart, gui16NodeName);

  // recalculate gui16MyIndexInCBArray with the new values of gui16NodeName and gui16ControllerBoxPrefix
  gui16MyIndexInCBArray = gui16NodeName - gui16ControllerBoxPrefix;
  Serial.printf("%s gui16MyIndexInCBArray reset to: %i\n", _debugLoadMsgStart, gui16MyIndexInCBArray);

  // isInterface
  isInterface = _preferences.getBool("isIF", isInterface);
  Serial.printf("%s isInterface set to: %i\n", _debugLoadMsgStart, isInterface);

  // // ui32RootNodeId
  // // uint32_t getULong(const char* key, uint32_t defaultValue = 0);
  // ui32RootNodeId = _preferences.getULong("RootNId", ui32RootNodeId);
  // Serial.printf("%s ui32RootNodeId set to: %u\n", _debugLoadMsgStart, ui32RootNodeId);

  // isRoot
  isRoot = _preferences.getBool("isRoot", isRoot);
  Serial.printf("%s isRoot set to: %i\n", _debugLoadMsgStart, isRoot);
}






/*
  ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState
  ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName
*/
void mySavedPrefs::_loadBoxBehaviorPreferences(Preferences& _preferences){
  // sBoxDefaultState
  ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState =_preferences.getShort("sBoxDefSta", ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState);
  Serial.printf("%s ControlerBoxes[%u].sBoxDefaultState set to: %i\n", _debugLoadMsgStart, gui16MyIndexInCBArray, ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState);

  // ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName
  // If there is a value saved for bMasterNName, reset
  // ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName
  // which is set by default to UI8_DEFAULT_MASTER_NODE_NAME
  // in the ControlerBox constructor. Else, the value of
  // ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName
  // will stay unchanged.
  ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName = _preferences.getUChar("bMasterNName", ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName);
  Serial.printf("%s ControlerBoxes[%u].ui16MasterBoxName set to: %u\n", _debugLoadMsgStart, gui16MyIndexInCBArray, ControlerBoxes[gui16MyIndexInCBArray].ui16MasterBoxName);

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
