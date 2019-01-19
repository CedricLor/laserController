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
    static void savePreferences();
    static void loadPreferences();
  private:
};

#endif
