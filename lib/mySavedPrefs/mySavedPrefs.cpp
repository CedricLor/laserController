/*
  mySavedPrefs.cpp - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "mySavedPrefs.h"



/**********************************************************************
***************************** MySavedPrefs ****************************
***********************************************************************/

const uint8_t mySavedPrefs::NVSVarNameSize = 12;
const uint8_t mySavedPrefs::humanReadableVarNameSize = 34;



/** Default constructor */
mySavedPrefs::mySavedPrefs(bool readOnly) : 
  debugSaveMsgStart("PREFERENCES: savePreferences(): the value of"),
  debugSaveMsgEndSuccess("has been saved to \"savedSettingsNS\":"),
  debugSaveMsgEndFail("could not be saved to \"savedSettingsNS\":"),
  debugLoadMsgStart("SETUP: loadPreferences():"),
  couldNotBeRetriedFromNVS("could not be retrieved from NVS. Using hard coded value:"),
  setFromNVS("set from NVS to:")
{
  // Instanciate preferences library
  Preferences _prefLib;

  /** Open namespace "savedSettingsNS" in read-only (readOnly = true)
   *  or read-write (readOnly = false).
   *  By default, the class constructor opens in rw. */
  _prefLib.begin("savedSettingsNS", /*read only = */readOnly);
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










// GLOBAL WRAPPERS
void mySavedPrefs::savePrefsWrapper() {
  Serial.print("PREFERENCES: savePreferences(): starting\n");

  _saveNetworkCredentials(this);
  _saveNetworkEssentialPreferences();
  _saveUselessPreferences();

  _saveBoxEssentialPreferences();
  _saveBoxBehaviorPreferences();

  _endPreferences();

  Serial.print("PREFERENCES: savePreferences(): starting\n");
}





void mySavedPrefs::loadPrefsWrapper() {
  Serial.print("\nSETUP: loadPrefsWrapper(): starting\n");

  _loadNetworkCredentials();
  _loadNetworkEssentialPreferences();
  _loadUselessPreferences();

  Serial.println(String(debugLoadMsgStart) + " --- Loading OTA Reboot Prefs ");
  _loadBoxStartupTypePreferences();
  _loadOTASuccess();
  Serial.println(String(debugLoadMsgStart) + " --- End OTA Reboot Prefs ");

  _loadBoxEssentialPreferences();
  _loadBoxBehaviorPreferences();

  _endPreferences();

  _handleOTAReboots();

  Serial.print("\nSETUP: loadPrefsWrapper(): ending\n");
}







  /** On reboot, if the number of requested OTA reboots is superior to 0,
   *  decrement it by 1 (and save it to NVS) until it reaches 0, where
   *  the ESP will reboot to its normal state */
void mySavedPrefs::_handleOTAReboots() {
  if (gi8RequestedOTAReboots) {
    _prefLib.begin("savedSettingsNS", false);
    gi8OTAReboot = 1;
    gi8RequestedOTAReboots = gi8RequestedOTAReboots - 1;
    actOnPrefsThroughCallback(_saveBoxStartupTypePreferences);
  }
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

    mySavedPrefs _myPrefsRef;
    _myPrefsRef.actOnPrefsThroughCallback(_saveNetworkCredentials);
    return;
  }


  // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 0, reboots: 2}
  if (_obj["val"] == "gi8RequestedOTAReboots") {
    Serial.println("mySavedPrefs::saveFromNetRequest: going to save gi8RequestedOTAReboots preferences.");
    gi8RequestedOTAReboots = _obj["reboots"];

    mySavedPrefs _myPrefsRef;
    _myPrefsRef.actOnPrefsThroughCallback(_saveBoxStartupTypePreferences);
    _myPrefsRef.actOnPrefsThroughCallback(_resetOTASuccess);
    return;
  }
}







void mySavedPrefs::actOnPrefsThroughCallback(void (&callBack)(mySavedPrefs*)) {
  callBack(this);
  _endPreferences();
}









///////////////////////////////////////////////////
// SAVERS
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
void mySavedPrefs::_saveNetworkCredentials(mySavedPrefs * _myPrefsRef) {
  Serial.println("Saving External Wifi Credentials");

  // save value of ssid
  _myPrefsRef->_saveStringTypePrefs("ssid", "ssid", ssid);

  // save value of pass
  _myPrefsRef->_saveStringTypePrefs("pass", "password", pass);

  // save value of ui16GatewayPort
  _myPrefsRef->_saveUi16TypePrefs("gatePort", "ui16GatewayPort", ui16GatewayPort);

  // save value of ui8WifiChannel
  _myPrefsRef->_saveUCharTypePrefs("wifiChan", "ui8WifiChannel", ui8WifiChannel);

  // save value of the gatewayIP, the fixedIP and fixed Netmask
  _myPrefsRef->_saveIPTypePrefs("gateIP", "wifi gateway IP", gatewayIP);
  _myPrefsRef->_saveIPTypePrefs("fixedIP", "IF fixed IP", fixedIP);
  _myPrefsRef->_saveIPTypePrefs("netMask", "wifi netmask", fixedNetmaskIP);

  Serial.println("End External Wifi Credentials");
}






/*
  gui16ControllerBoxPrefix
  gui16BoxesCount
*/
void mySavedPrefs::_saveNetworkEssentialPreferences() {
  // save value of gui16ControllerBoxPrefix
  // -> no reboot
  _saveUi16ToUCharTypePrefs("bContrBPref", "gui16ControllerBoxPrefix", gui16ControllerBoxPrefix);

  // save value of gui16BoxesCount
  // Not a very usefull value: the number of boxes created at startup is based on the constant UI8_BOXES_COUNT
  // Even a reboot would not suffice to have this param taken into account
  _saveUi16ToUCharTypePrefs("BoxesCount", "gui16BoxesCount", gui16BoxesCount);
}





/*
  gui16InterfaceNodeName
*/
void mySavedPrefs::_saveUselessPreferences() {
  _saveUi16ToUCharTypePrefs("sIFNodNam", "gui16InterfaceNodeName", gui16InterfaceNodeName);
}








/** gi8RequestedOTAReboots(): save the fact that the next 1 and/or 2 reboots
 *   shall be OTA reboots. 
 *
 *  dynamic reload: no need to reboo, but next (and following, as the case may 
 *  be) reboots will be OTA reboots */
void mySavedPrefs::_saveBoxStartupTypePreferences(mySavedPrefs * _myPrefsRef) {
  _myPrefsRef->_saveCharTypePrefs("OTARebReq", "gi8RequestedOTAReboots", gi8RequestedOTAReboots);
}

/** mySavedPrefs::_resetOTASuccess(): resets the values of ui8OTA1SuccessErrorCode 
*  and ui8OTA2SuccessErrorCode when an OTA reboot is requested.
*  
*  dynamic reload: no need to reboot */
void mySavedPrefs::_resetOTASuccess(mySavedPrefs * _myPrefsRef) {
  uint8_t _initialSuccessCode = 11;
  _myPrefsRef->_saveUCharTypePrefs("OTASucc1", "OTA update numb. 1 success code", _initialSuccessCode);
  _myPrefsRef->_saveUCharTypePrefs("OTASucc2", "OTA update numb. 2 success code", _initialSuccessCode);
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
  _saveUi16ToUCharTypePrefs("ui8NdeName", "gui16NodeName", gui16NodeName);

  /*
    Save value of isInterface
    -> runtime change possible; would require a restart of painlessMesh
    See below for possible implications with isRoot
  */
 _saveBoolTypePrefs("isIF", "isInterface", isInterface);

  /*
    Save value of isRoot
    -> runtime change possible, but 
      - would require a restart of painlessMesh on this node
      - would also require a symetric change to be done on another
      node (ex. if this node was root and shall no longer be,
      another node shall be assigned this role and painlessMesh shall 
      also be restarted on the other node)
  */
 _saveBoolTypePrefs("isRoot", "isRoot", isRoot);
}





/*
  thisBox.sBoxDefaultState
  thisBox.ui16MasterBoxName
*/
void mySavedPrefs::_saveBoxBehaviorPreferences() {
  // save value of sBoxDefaultState
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from thisBox)
  _saveI16TypePrefs("sBoxDefSta", "sBoxDefaultState", thisBox.sBoxDefaultState);

  // save value of thisBox.ui16MasterBoxName
  // Note to use Prefs without reboot (would be updated without reboot):
  // -> no reboot (this is saving the value straight from thisBox)
  _saveUi16ToUCharTypePrefs("bMasterNName", "thisBox.ui16MasterBoxName", thisBox.ui16MasterBoxName);
}






/** _saveStringTypePrefs saves the value of c-strings global variable to NVS
 * 
 *  preferences library methods signatures:
 *  - size_t putString(const char* key, const char* value); */
void mySavedPrefs::_saveStringTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], char* strEnvVar){
  size_t _ret = _prefLib.putString(NVSVarName, strEnvVar);
  Serial.printf("%s %s == %s %s\"%s\"\n", debugSaveMsgStart, humanReadableVarName, strEnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveIPTypePrefs saves the value of IPs in NVS.
 * 
 *  preferences library methods signatures:
 *  - size_t putBytes(const char* key, const void* value, size_t len); */
void mySavedPrefs::_saveIPTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], IPAddress& envIP){
  uint8_t _ui8envIP[4] = {envIP[0], envIP[1], envIP[2], envIP[3]};
  size_t _ret = _prefLib.putBytes(NVSVarName, _ui8envIP, 4);
  Serial.printf("%s %s == %s %s\"%s\"\n", debugSaveMsgStart, humanReadableVarName, envIP.toString().c_str(), (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveBoolTypePrefs stores global variables of type bool in NVS.
 * 
 *  preferences library methods signatures:
 *  - size_t putBool(const char* key, bool value); */
void mySavedPrefs::_saveBoolTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], bool& bEnvVar){
  size_t _ret = _prefLib.putBool(NVSVarName, bEnvVar);
  Serial.printf("%s %s == %u %s \"%s\"\n", debugSaveMsgStart, humanReadableVarName, bEnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveCharTypePrefs stores global variables of type Char in NVS.
 * 
 *  preferences library methods signatures:
 *  - size_t putChar(const char* key, int8_t value) */
void mySavedPrefs::_saveCharTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], int8_t& i8EnvVar){
  size_t _ret = _prefLib.putChar(NVSVarName, i8EnvVar);
  Serial.printf("%s %s == %u %s \"%s\"\n", debugSaveMsgStart, humanReadableVarName, i8EnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveUCharTypePrefs stores global variables of type UChar in NVS.
 * 
 *  preferences library methods signatures:
 *  - size_t putUChar(const char* key, uint8_t value) */
void mySavedPrefs::_saveUCharTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], uint8_t& ui8EnvVar){
  size_t _ret = _prefLib.putUChar(NVSVarName, ui8EnvVar);
  Serial.printf("%s %s == %u %s \"%s\"\n", debugSaveMsgStart, humanReadableVarName, ui8EnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveUi16ToUCharTypePrefs stores global variables of type uint16_t
 *  as UChar in NVS
 * 
 *  preferences library methods signatures:
 *  - size_t putUChar(const char* key, const uint8_t defaultValue) */
void mySavedPrefs::_saveUi16ToUCharTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], uint16_t& ui16EnvVar){
  size_t _ret = _prefLib.putUChar(NVSVarName, (uint8_t)ui16EnvVar);
  Serial.printf("%s %s == %u %s \"%s\"\n", debugSaveMsgStart, humanReadableVarName, ui16EnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveI16TypePrefs stores global variables of type i16int_t in NVS.
 * 
 *  preferences library methods signatures:
 *  - size_t putShort(const char* key, int16_t value); */
void mySavedPrefs::_saveI16TypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], int16_t& i16EnvVar){
  size_t _ret = _prefLib.putShort(NVSVarName, i16EnvVar);
  Serial.printf("%s %s == %u %s \"%s\"\n", debugSaveMsgStart, humanReadableVarName, i16EnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


/** _saveUi16TypePrefs stores global variables of type ui16int_t in NVS.
 * 
 *  preferences library methods signatures:
 *  - size_t putUShort(const char* key, uint16_t value); */
void mySavedPrefs::_saveUi16TypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], uint16_t& ui16EnvVar){
  size_t _ret = _prefLib.putUShort(NVSVarName, ui16EnvVar);
  Serial.printf("%s %s == %u %s \"%s\"\n", debugSaveMsgStart, humanReadableVarName, ui16EnvVar, (_ret)?(debugSaveMsgEndSuccess):(debugSaveMsgEndFail), NVSVarName);
}


///////////////////////////////////////////////////
// LOADERS
///////////////////////////////////////////////////

/** _loadNetworkCredentials(): 
 *   
 *  * use case: web interface served on ESP station interface.
 *  * dynamic reset: -> restart painlessmesh on the IF node 
 *  * default value location: secret library 
 * 
 *  saved settings:
 *  - ssid
 *  - pass
 *  - gatewayIP
 *  - ui16GatewayPort
 *  - ui8WifiChannel
 *  - fixedIP
 *  - fixedNetmaskIP */
void mySavedPrefs::_loadNetworkCredentials(){
  Serial.println(String(debugLoadMsgStart) + " --- Loading External Wifi Credentials");

  /** get the value of the external wifi network SSID and password */
  _loadStringTypePrefs("ssid", "external wifi network ssid", ssid);
  _loadStringTypePrefs("pass", "external wifi network password", pass);

  /** get the value of ui16GatewayPort */
  _loadUi16TypePrefs("gatePort", "ui16GatewayPort", ui16GatewayPort);

  /** get the value of ui8WifiChannel */
  _loadUCharTypePrefs("wifiChan", "wifi channel", ui8WifiChannel);

  /** get the value of gatewayIP, fixedIP and fixedNetmaskIP from NVS */
  _loadIPTypePrefs("gateIP", "gateway IP", gatewayIP);
  _loadIPTypePrefs("fixedIP", "fixedIP", fixedIP);
  _loadIPTypePrefs("netMask", "fixedNetmaskIP", fixedNetmaskIP);

  Serial.println(String(debugLoadMsgStart) + " --- End External Wifi Credentials");
}





/*
  gui16ControllerBoxPrefix
  gui16BoxesCount
*/
void mySavedPrefs::_loadNetworkEssentialPreferences(){
  // gui16ControllerBoxPrefix
  _loadUCharToUi16TypePrefs("bContrBPref", "gui16ControllerBoxPrefix", gui16ControllerBoxPrefix);

  // gui16BoxesCount
  _loadUCharToUi16TypePrefs("BoxesCount", "gui16BoxesCount", gui16BoxesCount);
}








/*
  gi8RequestedOTAReboots
*/
void mySavedPrefs::_loadBoxStartupTypePreferences() {
  _loadCharTypePrefs("OTARebReq", "gi8RequestedOTAReboots", gi8RequestedOTAReboots);
}





/**
 * uint8_t ui8OTA1SuccessErrorCode
 * uint8_t ui8OTA2SuccessErrorCode */
uint8_t mySavedPrefs::ui8OTA1SuccessErrorCode = 11;
uint8_t mySavedPrefs::ui8OTA2SuccessErrorCode = 11;

void mySavedPrefs::_loadOTASuccess() {
  // save the success code in the relevant NVS location
  _loadUCharTypePrefs("OTASucc1", "1st OTA Update Success", ui8OTA1SuccessErrorCode);
  _loadUCharTypePrefs("OTASucc2", "2nd OTA Update Success", ui8OTA2SuccessErrorCode);
}






/*
  gui16InterfaceNodeName
*/
void mySavedPrefs::_loadUselessPreferences(){
  // gui16InterfaceNodeName
  _loadUCharToUi16TypePrefs("sIFNodNam", "gui16InterfaceNodeName", gui16InterfaceNodeName);
}








/* _loadBoxEssentialPreferences()
  gui16NodeName
  isInterface
  isRoot
*/
void mySavedPrefs::_loadBoxEssentialPreferences(){
  Serial.println(String(debugLoadMsgStart) + " --- Loading Node Essential Preferences");

  // gui16NodeName
  _loadUCharToUi16TypePrefs("ui8NdeName", "gui16NodeName", gui16NodeName);

  // isInterface
  isInterface = _prefLib.getBool("isIF", isInterface);
  Serial.printf("%s isInterface set to: %i\n", debugLoadMsgStart, isInterface);

  // isRoot
  isRoot = _prefLib.getBool("isRoot", isRoot);
  Serial.printf("%s isRoot set to: %i\n", debugLoadMsgStart, isRoot);

  Serial.println(String(debugLoadMsgStart) + " --- End Node Essential Preferences");
}






/**
 * thisBox.sBoxDefaultState
 * thisBox.ui16MasterBoxName */
void mySavedPrefs::_loadBoxBehaviorPreferences(){
  // sBoxDefaultState
  _loadI16TypePrefs("sBoxDefSta", "thisBox.sBoxDefaultState", thisBox.sBoxDefaultState);

  // thisBox.ui16MasterBoxName
  _loadUCharToUi16TypePrefs("bMasterNName", "thisBox.ui16MasterBoxName", thisBox.ui16MasterBoxName);
}










/** _loadStringTypePrefs gets the value of c-strings stored in NVS and
 *  copies them to a c-string global variable
 * 
 *  preferences library methods signatures:
 *  - size_t getString(const char* key, char* value, size_t maxLen); */
void mySavedPrefs::_loadStringTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], char* strEnvVar){
  // 1. set the special debug message buffer
  char _specDebugMess[60];

  // 2. process the request
  char _strEnvVar[30];
  size_t _ret = _prefLib.getString(NVSVarName, _strEnvVar, 30);
  if (_ret) {
    snprintf(strEnvVar, 30, "%s", _strEnvVar);
    snprintf(_specDebugMess, 18, "%s", setFromNVS);
  } else {
    snprintf(_specDebugMess, 58, "%s", couldNotBeRetriedFromNVS);
  }

  Serial.printf("%s %s %s %s\n", debugLoadMsgStart, humanReadableVarName, _specDebugMess, _strEnvVar);
}


/** _loadIPTypePrefs gets the value of IPs stored in NVS.
 * 
 *  preferences library methods signatures:
 *  - getBytesLength(const char* key)
 *  - getBytes(const char* key, void * buf, size_t maxLen)*/
void mySavedPrefs::_loadIPTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], IPAddress& envIP){
  // 1. set the special debug message buffer
  char _specDebugMess[60];

  // 2. process the request
  if (_prefLib.getBytesLength(NVSVarName)) {
    char _IPBuffer[4];
    _prefLib.getBytes(NVSVarName, _IPBuffer, 4);
    for (int __ipIt=0; __ipIt<4; __ipIt++) {
      envIP[__ipIt] = _IPBuffer[__ipIt];
    }
    snprintf(_specDebugMess, 18, "%s", setFromNVS);
  } else {
    snprintf(_specDebugMess, 58, "%s", couldNotBeRetriedFromNVS);
  }
  Serial.printf("%s %s %s %s\n", debugLoadMsgStart, humanReadableVarName, _specDebugMess, envIP.toString().c_str());
}


/** _loadCharTypePrefs gets values of type Char stored in NVS.
 * 
 *  preferences library methods signatures:
 *  - getChar(const char* key, const int8_t defaultValue) */
void mySavedPrefs::_loadCharTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], int8_t& i8EnvVar){
  int8_t _i8EnvVar = _prefLib.getChar(NVSVarName);
  if (_i8EnvVar) i8EnvVar = _i8EnvVar;
  Serial.printf("%s %s %s %u\n", debugLoadMsgStart, humanReadableVarName, (_i8EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), i8EnvVar);
}


/** _loadUCharTypePrefs gets values of type UChar stored in NVS.
 * 
 *  preferences library methods signatures:
 *  - getUChar(const char* key, const uint8_t defaultValue) */
void mySavedPrefs::_loadUCharTypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], uint8_t& ui8EnvVar){
  uint8_t _ui8EnvVar = _prefLib.getUChar(NVSVarName);
  if (_ui8EnvVar) ui8EnvVar = _ui8EnvVar;
  Serial.printf("%s %s %s %u\n", debugLoadMsgStart, humanReadableVarName, (_ui8EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), ui8EnvVar);
}


/** _loadUCharToUi16TypePrefs gets values of type UChar stored in NVS
 *  to load it to an uint16_t global variable.
 * 
 *  preferences library methods signatures:
 *  - getUChar(const char* key, const uint8_t defaultValue) */
void mySavedPrefs::_loadUCharToUi16TypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], uint16_t& ui16EnvVar){
  uint8_t _ui8EnvVar = _prefLib.getUChar(NVSVarName);
  if (_ui8EnvVar) ui16EnvVar = (uint16_t)_ui8EnvVar;
  Serial.printf("%s %s %s %u\n", debugLoadMsgStart, humanReadableVarName, (_ui8EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), ui16EnvVar);
}


/** _loadI16TypePrefs gets values of type I16 stored in NVS
 *  to load it to an int16_t global variable.
 * 
 *  preferences library methods signatures:
 *  - getShort(const char* key, const int16_t defaultValue) */
void mySavedPrefs::_loadI16TypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], int16_t& i16EnvVar){
  int16_t _i16EnvVar = _prefLib.getShort(NVSVarName);
  if (_i16EnvVar) i16EnvVar = _i16EnvVar;
  Serial.printf("%s %s %s %u\n", debugLoadMsgStart, humanReadableVarName, (_i16EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), i16EnvVar);
}


/** _loadUi16TypePrefs gets values of type UI16 stored in NVS
 *  to load it to an uint16_t global variable.
 * 
 *  preferences library methods signatures:
 *  - getUShort(const char* key, const uint16_t defaultValue) */
void mySavedPrefs::_loadUi16TypePrefs(const char NVSVarName[NVSVarNameSize], const char humanReadableVarName[humanReadableVarNameSize], uint16_t& ui16EnvVar){
  uint16_t _ui16EnvVar = _prefLib.getUShort(NVSVarName);
  if (_ui16EnvVar) ui16EnvVar = _ui16EnvVar;
  Serial.printf("%s %s %s %u\n", debugLoadMsgStart, humanReadableVarName, (_ui16EnvVar ? setFromNVS : couldNotBeRetriedFromNVS), ui16EnvVar);
}












void mySavedPrefs::_endPreferences() {
  // Tell user how many free entries remain
  size_t _freeEntries = _prefLib.freeEntries();
  Serial.printf("PREFERENCES: Remaining free entries in NVS: %i\n", _freeEntries);

  _prefLib.end();

  Serial.print(F("PREFERENCES: done\n"));
}
