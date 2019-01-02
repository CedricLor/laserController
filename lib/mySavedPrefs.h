/*
  mySavedPrefs.h - Library for actions related to Preferences lib.
  Created by Cedric Lor, January 2, 2007.
*/

#ifndef mySavedPrefs_h
#define mySavedPrefs_h

#include "Arduino.h"

class mySavedPrefs
{
  public:
    // mySavedPrefs(short iSlaveOnOffReaction, short iMasterNodeName, unsigned long pinBlinkingInterval, const short I_DEFAULT_SLAVE_ON_OFF_REACTION);
    mySavedPrefs();
    static void savePreferences();
    static void loadPreferences();
  private:
    // short _iSlaveOnOffReaction;
    // short _iMasterNodeName;
    // unsigned long _pinBlinkingInterval;
    // short _I_DEFAULT_SLAVE_ON_OFF_REACTION;
};

#endif
