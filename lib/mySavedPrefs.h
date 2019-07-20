/*
  mySavedPrefs.h - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef mySavedPrefs_h
#define mySavedPrefs_h

#include "Arduino.h"

class mySavedPrefs
{
  public:
    mySavedPrefs();

    static void savePrefsWrapper();
    static void loadPrefsWrapper();

    static void saveFromNetRequest(JsonObject& _obj);
    
    static void saveBoxSpecificPrefsWrapper(void (&callBack)(Preferences&));
    static void loadBoxSpecificPrefsWrapper(void (&callBack)(Preferences&));

  private:
    static const PROGMEM char _debugSaveMsgStart[];
    static const PROGMEM char _debugSaveMsgEndSuccess[];
    static const PROGMEM char _debugSaveMsgEndFail[];
    static const PROGMEM char _debugLoadMsgStart[];

    // Save Prefs
    static void _startSavePreferences(Preferences& _preferences);

    static void _saveNetworkCredentials(Preferences& _preferences);
    static void _saveNetworkEssentialPreferences(Preferences& _preferences);
    static void _saveUselessPreferences(Preferences& _preferences);

    static void _saveBoxEssentialPreferences(Preferences& _preferences);
    static void _saveBoxBehaviorPreferences(Preferences& _preferences);

    // Load Prefs
    static void _loadNetworkCredentials(Preferences& _preferences);
    static void _loadNetworkEssentialPreferences(Preferences& _preferences);
    static void _loadUselessPreferences(Preferences& _preferences);

    static void _loadBoxEssentialPreferences(Preferences& _preferences);
    static void _loadBoxBehaviorPreferences(Preferences& _preferences);

    // Common
    static void _endPreferences(Preferences& _preferences);
};

#endif
