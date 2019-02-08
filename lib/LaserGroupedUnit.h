/*
  LaserGroupedUnit.h - Library to handle the laser grouped units.
  Created by Cedric Lor, February 7, 2019.
*/

#ifndef LaserGroupedUnit_h
#define LaserGroupedUnit_h

#include "Arduino.h"

class LaserGroupedUnit
{
  public:
    LaserGroupedUnit();

    short index_number;             // LaserPin type index number in the LaserPinsArray
    bool on_off;                    // variable to store the state (HIGH or LOW) of the pin (LOW = the relay is closed, HIGH = the relay is open)
    unsigned long blinking_interval;// how long should this pin blink on and off // NOTE: This variable will be replaced by a "sequence" data structure
    bool pir_state;                 // shall this pin respond to a change coming from the IR sensor; HIGH or LOW: HIGH -> reacting to changes in the PIR
    short pairing_type;             // what kind of pairing type does this pin have: -1 is unpaired, 0 is twin pairing (when one is on, the other is also on), 1 is cooperative (when one is on, the other is off and each takes the relay of the other)

    void switchOnOff(const bool _bTargetOnOffState);

    void manualSwitch(const bool _bTargetOnOffState);

    void inclExclInPir(const bool _bTargetPirState);

    void pairUnpairPin(const short _sPinParityWitness, const short _sPairingType);

    void changeBlinkingInterval(const unsigned long targetBlinkingInterval);

  private:
    static bool const _default_laser_group_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_laser_group_pir_state_value;       // by default, the pin is not controlled by the PIR

    static const unsigned long _max_interval_on;
};

#endif
