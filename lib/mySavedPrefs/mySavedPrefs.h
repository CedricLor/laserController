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

    static void savePrefsWrapper();
    static void loadPrefsWrapper();

    static void saveFromNetRequest(JsonObject& _obj);

    static uint8_t ui8OTA1SuccessErrorCode;
    static uint8_t ui8OTA2SuccessErrorCode;

    static void saveBoxSpecificPrefsWrapper(void (&callBack)(Preferences&));
    static void loadBoxSpecificPrefsWrapper(void (&callBack)(Preferences&));

    static const PROGMEM char debugSaveMsgStart[];
    static const PROGMEM char debugSaveMsgEndSuccess[];
    static const PROGMEM char debugSaveMsgEndFail[];

  private:

    static const PROGMEM char _debugLoadMsgStart[];

    // Save Prefs
    static void _startSavePreferences(Preferences& _preferences);

    static void _saveNetworkCredentials(Preferences& _preferences);
    static void _saveNetworkEssentialPreferences(Preferences& _preferences);
    static void _saveUselessPreferences(Preferences& _preferences);

    static void _saveBoxStartupTypePreferences(Preferences& _preferences);
    static void _resetOTASuccess(Preferences& _preferences);
    static void _saveBoxEssentialPreferences(Preferences& _preferences);
    static void _saveBoxBehaviorPreferences(Preferences& _preferences);

    // Load Prefs
    static const char * setFromNVS;
    static const char * couldNotBeRetriedFromNVS;

    static void _loadNetworkCredentials(Preferences& _preferences);
    static void _loadNetworkEssentialPreferences(Preferences& _preferences);
    static void _loadUselessPreferences(Preferences& _preferences);

    static void _loadOTASuccess(Preferences& _preferences);
    static void _loadBoxStartupTypePreferences(Preferences& _preferences);
    static void _loadBoxEssentialPreferences(Preferences& _preferences);
    static void _loadBoxBehaviorPreferences(Preferences& _preferences);

    // Common
    static void _loadIPTypePrefs(Preferences& _preferences, const char NVSVarName[], const char humanReadableVarName[], IPAddress& envIP);
    static void _loadUCharTypePrefs(Preferences& _preferences, const char NVSVarName[], const char humanReadableVarName[], uint8_t& ui8EnvVar);
    static void _loadUCharToU16TypePrefs(Preferences& _preferences, const char NVSVarName[], const char humanReadableVarName[], uint16_t& ui16EnvVar);

    static void _endPreferences(Preferences& _preferences);
};

#endif
