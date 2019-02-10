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

    // state machine variables
    bool currentOnOffState;      // variable to store the state (HIGH or LOW) of the pin (LOW = the relay is closed, HIGH = the relay is open)
    bool previousOnOffState;
    bool targetOnOffState;

    bool currentPirState;
    bool previousPirState;
    bool targetPirState;

    unsigned long currentBlinkingInterval;  // how long should the underlying pins blink on and off
    unsigned long previousBlinkingInterval;
    unsigned long targetBlinkingInterval;

    short pairing_type;             // what kind of pairing type does this pin have: -1 is unpaired, 0 is twin pairing (when one is on, the other is also on), 1 is cooperative (when one is on, the other is off and each takes the relay of the other)

    // State machine setters
    void setOnOffTargetState(const bool _bTargetOnOffState);
    void setTargetPirState(const short int __sTargetPirState);
    void setTargetBlinkingInterval(const unsigned long _ulTargetBlinkingInterval);


  private:
    static bool const _default_laser_group_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_laser_group_pir_state_value;       // by default, the pin is not controlled by the PIR

    static const unsigned long _max_interval_on;

    // Former switches
    void switchOnOff(const bool _bTargetOnOffState);
    void manualSwitch(const bool _bTargetOnOffState);
    void inclExclInPir(const bool _bTargetPirState);
    void changeBlinkingInterval(const unsigned long targetBlinkingInterval);

};

#endif
