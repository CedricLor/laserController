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
    mySavedPrefs();

    Preferences _prefLib;

    static void savePrefsWrapper();
    static void loadPrefsWrapper();

    static void saveFromNetRequest(JsonObject& _obj);

    static uint8_t ui8OTA1SuccessErrorCode;
    static uint8_t ui8OTA2SuccessErrorCode;

    static void saveBoxSpecificPrefsWrapper(void (&callBack)());
    static void loadBoxSpecificPrefsWrapper(void (&callBack)());

    static const PROGMEM char debugSaveMsgStart[];
    static const PROGMEM char debugSaveMsgEndSuccess[];
    static const PROGMEM char debugSaveMsgEndFail[];

  private:

    static const PROGMEM char _debugLoadMsgStart[];

    // Save Prefs
    static void _startSavePreferences();

    static void _saveNetworkCredentials();
    static void _saveNetworkEssentialPreferences();
    static void _saveUselessPreferences();

    static void _saveBoxStartupTypePreferences();
    static void _resetOTASuccess();
    static void _saveBoxEssentialPreferences();
    static void _saveBoxBehaviorPreferences();

    // Load Prefs
    static const char * setFromNVS;
    static const char * couldNotBeRetriedFromNVS;

    static void _loadNetworkCredentials();
    static void _loadNetworkEssentialPreferences();
    static void _loadUselessPreferences();

    static void _loadBoxStartupTypePreferences();
    static void _loadOTASuccess();
    static void _loadBoxEssentialPreferences();
    static void _loadBoxBehaviorPreferences();

    // Common
    void _loadStringTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], char* strEnvVar);
    void _loadIPTypePrefs(const char NVSVarName[], const char humanReadableVarName[], IPAddress& envIP);
    void _loadUCharTypePrefs(const char NVSVarName[], const char humanReadableVarName[], uint8_t& ui8EnvVar);
    void _loadCharTypePrefs(const char NVSVarName[13], const char humanReadableVarName[30], int8_t& i8EnvVar);
    void _loadUCharToUi16TypePrefs(const char NVSVarName[], const char humanReadableVarName[], uint16_t& ui16EnvVar);
    void _loadI16TypePrefs(const char NVSVarName[], const char humanReadableVarName[], int16_t& i16EnvVar);
    void _loadUi16TypePrefs(const char NVSVarName[], const char humanReadableVarName[], uint16_t& ui16EnvVar);

    static void _endPreferences();
};

extern mySavedPrefs _myPrefsRef;

#endif
