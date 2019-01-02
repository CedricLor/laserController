/*
  LaserPin.h - Library to handle laser controller web server.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef LaserPin_h
#define LaserPin_h

#include "Arduino.h"

class LaserPin
{
  public:
    LaserPin();
    short _number;        // pin number to which the relays are attached
    bool _on_off;       // is the pin HIGH or LOW (LOW = the relay is closed, HIGH = the relay is open)
    bool _on_off_target;// a variable to store the on / off change requests by the various functions
    bool _blinking;     // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
    unsigned long _previous_time;
    unsigned long _blinking_interval;
    bool _pir_state;     // HIGH or LOW: HIGH -> controlled by the PIR
    short _paired;        // a variable to store with which other pin this pin is paired (8 means it is not paired)
  private:
};

#endif
