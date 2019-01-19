/*
  MasterSlaveBox.h - Library to handle relationship of this box with other controller box related functions.
  Created by Cedric Lor, January 3, 2019.
*/

#ifndef MasterSlaveBox_h
#define MasterSlaveBox_h

#include "Arduino.h"

class MasterSlaveBox
{
  public:
    MasterSlaveBox();
    static void changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action);

  private:
    static void changeTheMasterBoxId(const short masterBoxNumber);
    static void changeSlaveReaction(const char* action);
};

#endif
