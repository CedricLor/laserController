/*
  mySavedPrefs.h - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef mySavedPrefs_h
#define mySavedPrefs_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <global.h>       
#include <secret.h>       
#include "../ControlerBox.h"

class mySavedPrefs
{
  public:
    const PROGMEM char * debugSaveMsgStart;
    const PROGMEM char * debugSaveMsgEndSuccess;
    const PROGMEM char * debugSaveMsgEndFail;

    mySavedPrefs(bool readOnly=false);

    Preferences _prefLib;

    void savePrefsWrapper();
    void loadPrefsWrapper();

    static void saveFromNetRequest(JsonObject& _obj);

    static uint8_t ui8OTA1SuccessErrorCode;
    static uint8_t ui8OTA2SuccessErrorCode;

    void actOnPrefsThroughCallback(void (&callBack)(mySavedPrefs*));

  private:

    const PROGMEM char * debugLoadMsgStart;

    // Save Prefs
    void _handleOTAReboots();

    static void _saveNetworkCredentials(mySavedPrefs * _myPrefsRef);
    void _saveNetworkEssentialPreferences();
    void _saveUselessPreferences();

    static void _saveBoxStartupTypePreferences(mySavedPrefs * _myPrefsRef);
    static void _resetOTASuccess(mySavedPrefs * _myPrefsRef);
    void _saveBoxEssentialPreferences();
    void _saveBoxBehaviorPreferences();

    // Save Abstract
    void _saveUCharTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], uint8_t& ui8EnvVar);
    void _saveUi16ToUCharTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], uint16_t& ui16EnvVar);

    // Load Prefs
    const char * couldNotBeRetriedFromNVS;
    const char * setFromNVS;

    void _loadNetworkCredentials();
    void _loadNetworkEssentialPreferences();
    void _loadUselessPreferences();

    void _loadBoxStartupTypePreferences();
    void _loadOTASuccess();
    void _loadBoxEssentialPreferences();
    void _loadBoxBehaviorPreferences();

    // Load Abstract
    void _loadStringTypePrefs(const char NVSVarName[], const char humanReadableVarName[], char* strEnvVar);
    void _loadIPTypePrefs(const char NVSVarName[], const char humanReadableVarName[], IPAddress& envIP);
    void _loadUCharTypePrefs(const char NVSVarName[], const char humanReadableVarName[], uint8_t& ui8EnvVar);
    void _loadCharTypePrefs(const char NVSVarName[], const char humanReadableVarName[], int8_t& i8EnvVar);
    void _loadUCharToUi16TypePrefs(const char NVSVarName[], const char humanReadableVarName[], uint16_t& ui16EnvVar);
    void _loadI16TypePrefs(const char NVSVarName[], const char humanReadableVarName[], int16_t& i16EnvVar);
    void _loadUi16TypePrefs(const char NVSVarName[], const char humanReadableVarName[], uint16_t& ui16EnvVar);

    void _endPreferences();
};


#endif
