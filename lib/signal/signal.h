/*
  signal.h - Library to handle the signals received from whichever source
  Created by Cedric Lor, August 28, 2019.
*/

#ifndef signal_h
#define signal_h

#include "Arduino.h"


class signal
{
  public:
    signal();

    uint16_t state;
    uint16_t type;
    uint16_t source;
    uint32_t startTime;
};

#endif
