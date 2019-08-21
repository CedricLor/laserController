/*
  mySavedPrefs.cpp - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "mySavedPrefs.h"

mySavedPrefs _myPrefsRef;

mySavedPrefs::mySavedPrefs() : 
  setFromNVS("set from NVS to:"), 
  couldNotBeRetriedFromNVS("could not be retrieved from NVS. Using hard coded value:")
{
  // Instanciate preferences library
  Preferences _prefLib;
}

  /* variables to be saved in NVS:
  *  gui16InterfaceNodeName (e.g. 201)
  *  gui16ControllerBoxPrefix
  *  gui16BoxesCount
  *  PIN_COUNT
  *  ssid
  *  pass

  *  sBoxDefaultState
  *  gui16NodeName
  *  thisBox.ui16MasterBoxName;
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

  _startSavePreferences();

  _saveNetworkCredentials();
  _saveNetworkEssentialPreferences();
  _saveUselessPreferences();

  _saveBoxEssentialPreferences();
  _saveBoxBehaviorPreferences();

  _endPreferences();

  Serial.print("PREFERENCES: savePreferences(): starting\n");
}





void mySavedPrefs::loadPrefsWrapper() {
  Serial.print("\nSETUP: loadPrefsWrapper(): starting\n");

  // Instanciate preferences
  mySavedPrefs _myPrefsInst;
  _myPrefsRef = _myPrefsInst;

    /** Open namespace "savedSettingsNS" in read only:
   *  second parameter has to be true. */
  if (_myPrefsRef._prefLib.begin("savedSettingsNS", /*read only = */true)){

      _myPrefsRef._loadNetworkCredentials();
      _myPrefsRef._loadNetworkEssentialPreferences();
      _myPrefsRef._loadUselessPreferences();

      Serial.println(String(_debugLoadMsgStart) + " --- Loading OTA Reboot Prefs ");
      _myPrefsRef._loadBoxStartupTypePreferences();
      _myPrefsRef._loadOTASuccess();
      Serial.println(String(_debugLoadMsgStart) + " --- End OTA Reboot Prefs ");

      _myPrefsRef._loadBoxEssentialPreferences();
      _myPrefsRef._loadBoxBehaviorPreferences();

    } else {
      Serial.printf("%s \"savedSettingsNS\" does not exist. thisBox ui16MasterBoxName (%i) and gui16BoxesCount (%i) will keep their default values\n", _debugLoadMsgStart, thisBox.ui16MasterBoxName, gui16BoxesCount);
    }

  _endPreferences();

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







void mySavedPrefs::saveBoxSpecificPrefsWrapper(void (&callBack)()) {
  // Instanciate preferences
  mySavedPrefs _myPrefsInst;
  _myPrefsRef = _myPrefsInst;

  _startSavePreferences();

  callBack();

  _endPreferences();
}






void mySavedPrefs::loadBoxSpecificPrefsWrapper(void (&callBack)()) {
  // Instanciate preferences
  mySavedPrefs _myPrefsInst;

  /** Open namespace "savedSettingsNS" in read only:
   *  second parameter has to be true. */
  if (_myPrefsRef._prefLib.begin("savedSettingsNS", /*read only = */true) == true){
    callBack();
  }

  _endPreferences();
}





///////////////////////////////////////////////////
// SAVERS
///////////////////////////////////////////////////
void mySavedPrefs::_startSavePreferences() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");


  /** Open namespace "savedSettingsNS" in read-write:
   *  second parameter has to be false. */
  _myPrefsRef._prefLib.begin("savedSettingsNS", /*read only = */false);
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
void mySavedPrefs::_saveNetworkCredentials() {
  Serial.println("Saving External Wifi Credentials");

  // save value of ssid
  // Interface only
  // -> restart the mesh
  size_t _sssidRet = _myPrefsRef._prefLib.putString("ssid", ssid);
  Serial.printf("%s ssid == %s %s\"ssid\"\n", debugSaveMsgStart, ssid, (_sssidRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of pass
  // Interface only
  // -> restart the mesh
  size_t _spassRet = _myPrefsRef._prefLib.putString("pass", pass);
  Serial.printf("%s pass == %s %s\"pass\"\n", debugSaveMsgStart, pass, (_spassRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of gatewayIP
  // Interface only
  // -> restart the mesh
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8GatewayIP[4] = {gatewayIP[0], gatewayIP[1], gatewayIP[2], gatewayIP[3]};
  size_t _bsGatewayIPRet = _myPrefsRef._prefLib.putBytes("gateIP", _ui8GatewayIP, 4);
  Serial.printf("%s gatewayIP == %s %s\"gateIP\"\n", debugSaveMsgStart, gatewayIP.toString().c_str(), (_bsGatewayIPRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of ui16GatewayPort
  // Interface only
  // -> restart the mesh
  // putShort(const char* key, int16_t value)
  size_t _ui16GatewayPortRet = _myPrefsRef._prefLib.putUShort("gatePort", ui16GatewayPort);
  Serial.printf("%s ui16GatewayPort == %u %s\"gatePort\"\n", debugSaveMsgStart, ui16GatewayPort, (_ui16GatewayPortRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of ui8WifiChannel
  // Interface only
  // -> restart the mesh
  // putUChar(const char* key, uint8_t value)
  size_t _ui8WifiChannelRet = _myPrefsRef._prefLib.putUChar("wifiChan", ui8WifiChannel);
  Serial.printf("%s ui8WifiChannel == %u %s\"wifiChan\"\n", debugSaveMsgStart, ui8WifiChannel, (_ui8WifiChannelRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of fixedIP
  // Interface only
  // -> restart the mesh
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8fixedIP[4] = {fixedIP[0], fixedIP[1], fixedIP[2], fixedIP[3]};
  size_t _bsFixedIPRet = _myPrefsRef._prefLib.putBytes("fixedIP", _ui8fixedIP, 4);
  Serial.printf("%s IF fixed IP == %s %s\"fixedIP\"\n", debugSaveMsgStart, fixedIP.toString().c_str(), (_bsFixedIPRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of fixedNetmaskIP
  // Interface only
  // -> restart the mesh
  // putBytes(const char* key, const void* value, size_t len)
  uint8_t _ui8FixedNetmaskIP[4] = {fixedNetmaskIP[0], fixedNetmaskIP[1], fixedNetmaskIP[2], fixedNetmaskIP[3]};
  size_t _bsFixedNetmaskIPRet = _myPrefsRef._prefLib.putBytes("netMask", _ui8FixedNetmaskIP, 4);
  Serial.printf("%s Wifi netmask == %s %s\"netMask\"\n", debugSaveMsgStart, fixedNetmaskIP.toString().c_str(), (_bsFixedNetmaskIPRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  Serial.println("End External Wifi Credentials");
}






/*
  gui16ControllerBoxPrefix
  gui16BoxesCount
*/
void mySavedPrefs::_saveNetworkEssentialPreferences() {
  // save value of gui16ControllerBoxPrefix
  // -> no reboot but very messy if no reboot of the IF and the whole mesh
  // putUChar(const char* key, uint8_t value)
  size_t _gui16ControllerBoxPrefixRet = _myPrefsRef._prefLib.putUChar("bContrBPref", (uint8_t)gui16ControllerBoxPrefix);
  Serial.printf("%s gui16ControllerBoxPrefix == %i %s\"bContrBPref\"\n", debugSaveMsgStart, gui16ControllerBoxPrefix, (_gui16ControllerBoxPrefixRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of gui16BoxesCount
  // Not a very usefull value: the number of boxes created at startup is based on the constant UI8_BOXES_COUNT
  // Even a reboot would not suffice to have this param taken into account
  size_t _gui16BoxesCountRet = _myPrefsRef._prefLib.putUChar("BoxesCount", (uint8_t)gui16BoxesCount);
  Serial.printf("%s gui16BoxesCount == %u %s\"BoxesCount\"\n", debugSaveMsgStart, gui16BoxesCount, (_gui16BoxesCountRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}





/*
  gui16InterfaceNodeName
*/
void mySavedPrefs::_saveUselessPreferences() {
  // USELESS PREFERENCES
  // save value of gui16InterfaceNodeName
  size_t _gui16InterfaceNodeNameRet = _myPrefsRef._prefLib.putUChar("sIFNodNam", (uint8_t)gui16InterfaceNodeName);
  Serial.printf("%s gui16InterfaceNodeName == %u %s\"sIFNodNam\"\n", debugSaveMsgStart, gui16InterfaceNodeName, (_gui16InterfaceNodeNameRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}








/*
  gi8RequestedOTAReboots
*/
void mySavedPrefs::_saveBoxStartupTypePreferences() {
  // save value of gi8RequestedOTAReboots
  // Note to use Prefs without reboot: needs a reboot to be effective
  size_t _gi8RequestedOTARebootsRet = _myPrefsRef._prefLib.putChar("OTARebReq", gi8RequestedOTAReboots);
  Serial.printf("%s gi8RequestedOTAReboots == %i %s\"OTARebReq\"\n", debugSaveMsgStart, gi8RequestedOTAReboots, (_gi8RequestedOTARebootsRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}

/*
  ui8OTA1SuccessErrorCode
  ui8OTA2SuccessErrorCode
*/
void mySavedPrefs::_resetOTASuccess() {
  // resets the values of ui8OTA1SuccessErrorCode and ui8OTA2SuccessErrorCode when an OTA reboot is requested
  // Note to use Prefs without reboot: no need for reboots; used to display data in console and webpage
  size_t _ui8OTA1SuccessErrorCodeRet = _myPrefsRef._prefLib.putUChar("OTASucc1", 11);
  Serial.printf("%s OTA update numb. 1 success code == %u %s\"OTASucc1\"\n", debugSaveMsgStart, 11, (_ui8OTA1SuccessErrorCodeRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
  size_t _ui8OTA2SuccessErrorCodeRet = _myPrefsRef._prefLib.putUChar("OTASucc2", 11);
  Serial.printf("%s OTA update numb. 2 success code == %u %s\"OTASucc2\"\n", debugSaveMsgStart, 11, (_ui8OTA2SuccessErrorCodeRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}







/*
  gui16NodeName
  isInterface
  isRoot
*/
// Need a reboot
void mySavedPrefs::_saveBoxEssentialPreferences() {
  // save value of gui16NodeName (global int8_t nodeName)
  // Note to use Prefs without reboot:
  // -> would need a reboot
  // -> fix: call ControlerBox::updateThisBoxProperties
  // this value is then used in ControlerBox::updateThisBoxProperties
  // to set thisBox.ui16NodeName
  // putUChar(const char* key, uint8_t value)
  size_t _gui16NodeNameRet = _myPrefsRef._prefLib.putUChar("ui8NdeName", (uint8_t)(gui16NodeName));
  Serial.printf("%s gui16NodeName == %u %s\"ui8NdeName\"\n", debugSaveMsgStart, gui16NodeName, (_gui16NodeNameRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  /*
    Save value of isInterface
    -> runtime change possible; would require a restart of painlessMesh
    See below for possible implications with isRoot
  */
  size_t _isInterfaceRet = _myPrefsRef._prefLib.putBool("isIF", isInterface);
  Serial.printf("%s isInterface == %i %s\"isIF\"\n", debugSaveMsgStart, isInterface, (_isInterfaceRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  /*
    Save value of isRoot
    -> runtime change possible, but 
      - would require a restart of painlessMesh on this node
      - would also require a symetric change to be done on another
      node (ex. if this node was root and shall no longer be,
      another node shall be assigned this role and painlessMesh shall 
      also be restarted on the other node)
  */
  size_t _isRootRet = _myPrefsRef._prefLib.putBool("isRoot", isRoot);
  Serial.printf("%s isRoot == %i %s\"isRoot\"\n", debugSaveMsgStart, isRoot, (_isRootRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
}





/*
  thisBox.sBoxDefaultState
  thisBox.ui16MasterBoxName
*/
void mySavedPrefs::_saveBoxBehaviorPreferences() {
  // save value of sBoxDefaultState
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from thisBox)
  size_t _sBoxDefaultStateRet = _myPrefsRef._prefLib.putShort("sBoxDefSta", thisBox.sBoxDefaultState);
  Serial.printf("%s sBoxDefaultState == %i %s\"sBoxDefSta\"\n", debugSaveMsgStart, thisBox.sBoxDefaultState, (_sBoxDefaultStateRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));

  // save value of thisBox.ui16MasterBoxName
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from thisBox)
  size_t _masterNodeNameRet = _myPrefsRef._prefLib.putUChar("bMasterNName", (uint8_t)thisBox.ui16MasterBoxName);
  Serial.printf("%s thisBox.ui16MasterBoxName == %u %s\"bMasterNName\"\n", debugSaveMsgStart, thisBox.ui16MasterBoxName, (_masterNodeNameRet)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail));
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
void mySavedPrefs::_loadNetworkCredentials(){
  Serial.println(String(_debugLoadMsgStart) + " --- Loading External Wifi Credentials");

  // ssid
  // -> restart the mesh
  char _ssid[20];
  if (_myPrefsRef._prefLib.getString("ssid", _ssid, 20)) {
    strcpy(ssid, (const char*)_ssid);
    Serial.printf("%s ssid set to: %s\n", _debugLoadMsgStart, ssid);
  }

  // pass
  // -> restart the mesh
  char _pass[30];
  if (_myPrefsRef._prefLib.getString("pass", _pass, 30)) {
    strcpy(pass, (const char*)_pass);
    Serial.printf("%s pass set to: %s\n", _debugLoadMsgStart, pass);
  }

  // get the value of ui16GatewayPort
  // -> restart the mesh
  _myPrefsRef._loadUi16TypePrefs("gatePort", "ui16GatewayPort", ui16GatewayPort);

  // get the value of ui8WifiChannel
  // -> restart the mesh
  _myPrefsRef._loadUCharTypePrefs("wifiChan", "wifi channel", ui8WifiChannel);

  /** get the value of gatewayIP, fixedIP and fixedNetmaskIP from NVS
   *  
   *  * use case: web interface served on ESP station interface.
   *  * dynamic reset: -> restart the mesh
   *  * default value location: secret library */
  _myPrefsRef._loadIPTypePrefs("gateIP", "gateway IP", gatewayIP);
  _myPrefsRef._loadIPTypePrefs("fixedIP", "fixedIP", fixedIP);
  _myPrefsRef._loadIPTypePrefs("netMask", "fixedNetmaskIP", fixedNetmaskIP);

  Serial.println(String(_debugLoadMsgStart) + " --- End External Wifi Credentials");
}





/*
  gui16ControllerBoxPrefix
  gui16BoxesCount
*/
void mySavedPrefs::_loadNetworkEssentialPreferences(){
  // gui16ControllerBoxPrefix
  _myPrefsRef._loadUCharToUi16TypePrefs("bContrBPref", "gui16ControllerBoxPrefix", gui16ControllerBoxPrefix);

  // gui16BoxesCount
  _myPrefsRef._loadUCharToUi16TypePrefs("BoxesCount", "gui16BoxesCount", gui16BoxesCount);
}








/*
  gi8RequestedOTAReboots
*/
void mySavedPrefs::_loadBoxStartupTypePreferences() {
  _myPrefsRef._loadCharTypePrefs("OTARebReq", "gi8RequestedOTAReboots", gi8RequestedOTAReboots);
}





/**
 * uint8_t ui8OTA1SuccessErrorCode
 * uint8_t ui8OTA2SuccessErrorCode */
uint8_t mySavedPrefs::ui8OTA1SuccessErrorCode = 11;
uint8_t mySavedPrefs::ui8OTA2SuccessErrorCode = 11;

void mySavedPrefs::_loadOTASuccess() {
  // save the success code in the relevant NVS location
  _myPrefsRef._loadUCharTypePrefs("OTASucc1", "1st OTA Update Success", ui8OTA1SuccessErrorCode);
  _myPrefsRef._loadUCharTypePrefs("OTASucc2", "2nd OTA Update Success", ui8OTA2SuccessErrorCode);
}






/*
  gui16InterfaceNodeName
*/
void mySavedPrefs::_loadUselessPreferences(){
  // gui16InterfaceNodeName
  _myPrefsRef._loadUCharToUi16TypePrefs("sIFNodNam", "gui16InterfaceNodeName", gui16InterfaceNodeName);
}








/*
  gui16NodeName
  isInterface
  isRoot
*/
void mySavedPrefs::_loadBoxEssentialPreferences(){
  Serial.println(String(_debugLoadMsgStart) + " --- Loading Node Essential Preferences");

  // gui16NodeName
  // getUChar(const char* key, const uint8_t defaultValue)
  _myPrefsRef._loadUCharToUi16TypePrefs("ui8NdeName", "gui16NodeName", gui16NodeName);

  // isInterface
  isInterface = _myPrefsRef._prefLib.getBool("isIF", isInterface);
  Serial.printf("%s isInterface set to: %i\n", _debugLoadMsgStart, isInterface);

  // isRoot
  isRoot = _myPrefsRef._prefLib.getBool("isRoot", isRoot);
  Serial.printf("%s isRoot set to: %i\n", _debugLoadMsgStart, isRoot);

  Serial.println(String(_debugLoadMsgStart) + " --- End Node Essential Preferences");
}






/**
 * thisBox.sBoxDefaultState
 * thisBox.ui16MasterBoxName */
void mySavedPrefs::_loadBoxBehaviorPreferences(){
  // sBoxDefaultState
  _myPrefsRef._loadI16TypePrefs("sBoxDefSta", "thisBox.sBoxDefaultState", thisBox.sBoxDefaultState);

  // thisBox.ui16MasterBoxName
  _myPrefsRef._loadUCharToUi16TypePrefs("bMasterNName", "thisBox.ui16MasterBoxName", thisBox.ui16MasterBoxName);
}










/** _loadStringTypePrefs gets the value of c-strings stored in NVS and
 *  copies them to a c-string global variable
 * 
 *  preferences library methods signatures:
 *  - size_t getString(const char* key, char* value, size_t maxLen); */
void mySavedPrefs::_loadStringTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], char* strEnvVar){
  // 1. set the special debug message buffer
  char _specDebugMess[60];

  // 2. process the request
  char _strEnvVar[30];
  size_t _ret = _myPrefsRef._prefLib.getString(NVSVarName, _strEnvVar, 30);
  if (_ret) {
    snprintf(strEnvVar, 30, "%s", _strEnvVar);
    snprintf(_specDebugMess, 18, "%s", setFromNVS);
  } else {
    snprintf(_specDebugMess, 58, "%s", couldNotBeRetriedFromNVS);
  }

  Serial.printf("%s %s %s %s\n", _debugLoadMsgStart, humanReadableVarName, _specDebugMess, _strEnvVar);
}


/** _loadIPTypePrefs gets the value of IPs stored in NVS.
 * 
 *  preferences library methods signatures:
 *  - getBytesLength(const char* key)
 *  - getBytes(const char* key, void * buf, size_t maxLen)*/
void mySavedPrefs::_loadIPTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], IPAddress& envIP){
  // 1. set the special debug message buffer
  char _specDebugMess[60];

  // 2. process the request
  if (_myPrefsRef._prefLib.getBytesLength(NVSVarName)) {
    char _IPBuffer[4];
    _myPrefsRef._prefLib.getBytes(NVSVarName, _IPBuffer, 4);
    for (int __ipIt=0; __ipIt<4; __ipIt++) {
      envIP[__ipIt] = _IPBuffer[__ipIt];
    }
    snprintf(_specDebugMess, 18, "%s", setFromNVS);
  } else {
    snprintf(_specDebugMess, 58, "%s", couldNotBeRetriedFromNVS);
  }
  Serial.printf("%s %s %s %s\n", _debugLoadMsgStart, humanReadableVarName, _specDebugMess, envIP.toString().c_str());
}


/** _loadCharTypePrefs gets values of type Char stored in NVS.
 * 
 *  preferences library methods signatures:
 *  - getChar(const char* key, const int8_t defaultValue) */
void mySavedPrefs::_loadCharTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], int8_t& i8EnvVar){
  int8_t _i8EnvVar = _myPrefsRef._prefLib.getChar(NVSVarName);
  if (_i8EnvVar) i8EnvVar = _i8EnvVar;
  Serial.printf("%s %s %s %u\n", _debugLoadMsgStart, humanReadableVarName, (_i8EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), i8EnvVar);
}


/** _loadUCharTypePrefs gets values of type UChar stored in NVS.
 * 
 *  preferences library methods signatures:
 *  - getUChar(const char* key, const uint8_t defaultValue) */
void mySavedPrefs::_loadUCharTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], uint8_t& ui8EnvVar){
  uint8_t _ui8EnvVar = _myPrefsRef._prefLib.getUChar(NVSVarName);
  if (_ui8EnvVar) ui8EnvVar = _ui8EnvVar;
  Serial.printf("%s %s %s %u\n", _debugLoadMsgStart, humanReadableVarName, (_ui8EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), ui8EnvVar);
}


/** _loadUCharToUi16TypePrefs gets values of type UChar stored in NVS
 *  to load it to an uint16_t global variable.
 * 
 *  preferences library methods signatures:
 *  - getUChar(const char* key, const uint8_t defaultValue) */
void mySavedPrefs::_loadUCharToUi16TypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], uint16_t& ui16EnvVar){
  uint8_t _ui8EnvVar = _myPrefsRef._prefLib.getUChar(NVSVarName);
  if (_ui8EnvVar) ui16EnvVar = (uint16_t)_ui8EnvVar;
  Serial.printf("%s %s %s %u\n", _debugLoadMsgStart, humanReadableVarName, (_ui8EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), ui16EnvVar);
}


/** _loadI16TypePrefs gets values of type I16 stored in NVS
 *  to load it to an int16_t global variable.
 * 
 *  preferences library methods signatures:
 *  - getShort(const char* key, const int16_t defaultValue) */
void mySavedPrefs::_loadI16TypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], int16_t& i16EnvVar){
  int16_t _i16EnvVar = _myPrefsRef._prefLib.getShort(NVSVarName);
  if (_i16EnvVar) i16EnvVar = _i16EnvVar;
  Serial.printf("%s %s %s %u\n", _debugLoadMsgStart, humanReadableVarName, (_i16EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), i16EnvVar);
}


/** _loadUi16TypePrefs gets values of type UI16 stored in NVS
 *  to load it to an uint16_t global variable.
 * 
 *  preferences library methods signatures:
 *  - getUShort(const char* key, const uint16_t defaultValue) */
void mySavedPrefs::_loadUi16TypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], uint16_t& ui16EnvVar){
  uint16_t _ui16EnvVar = _myPrefsRef._prefLib.getUShort(NVSVarName);
  if (_ui16EnvVar) ui16EnvVar = _ui16EnvVar;
  Serial.printf("%s %s %s %u\n", _debugLoadMsgStart, humanReadableVarName, (_ui16EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), ui16EnvVar);
}





void mySavedPrefs::_endPreferences() {
  // Tell user how many free entries remain
  size_t _freeEntries = _myPrefsRef._prefLib.freeEntries();
  Serial.printf("PREFERENCES: Remaining free entries in NVS: %i\n", _freeEntries);

  _myPrefsRef._prefLib.end();

  Serial.print(F("PREFERENCES: done\n"));
}
