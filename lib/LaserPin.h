/*
  LaserPin.h - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef LaserPin_h
#define LaserPin_h

#include "Arduino.h"

class LaserPin
{
  public:
    LaserPin();

    // Index and cross-references
    short laserGroupedUnitId;       // Index number of the laserGroupedUnitId to which this pin pertains
    short index_number;             // LaserPin type index number in the LaserPinsArray
    // On or off state (i.e. electrical circuit is closed or open)
    bool on_off;                    // variable to store the state (HIGH or LOW) of the pin (LOW = the relay is closed, HIGH = the relay is open)
    bool on_off_target;             // variable to store the on / off change requests by the various functions
    // Blinking (the pin is considered to be active, either on or off, but if off, only waiting to cool off)
    bool blinking;                  // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
    unsigned long previous_time;    // last time this pin changed state (on or off) // NOTE: redundant with last_time_on and last_time_off below
    unsigned long last_time_on;     // last time this pin was turned on
    unsigned long last_time_off;    // last time this pin was turned off
    unsigned long last_interval_on; // last interval during which this pin was turned on
    unsigned long blinking_interval;// how long should this pin blink on and off // NOTE: This variable will be replaced by a "sequence" data structure
    short paired_with;              // with which other pin is this pin paired (-1 means it is not paired); the number correspond to the index_number of the paired pin

    bool pir_state;                 // shall this pin respond to a change coming from the IR sensor; HIGH or LOW: HIGH -> reacting to changes in the PIR

    void physicalInitLaserPin(const short physicalPinNumber);

    void switchOnOffVariables(const bool _bTargetOnOffState);

    void updateLaserPinValuesFromLaserGroupedUnitOwner();

    void blinkLaserInBlinkingCycle();

    void executePinStateChange();
    void laserProtectionSwitch();

  private:
    static bool const _default_pin_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_pin_on_off_target_state; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    static bool const _default_pin_blinking_state;       // by default, pin starts as in a blinking-cycle TO ANALYSE
    static bool const _default_pin_pir_state_value;       // by default, the pin is not controlled by the PIR

    short _physical_pin_number;      // pin physical number of the ESP32 which is controling the relay
    short _pairing_type;            // what kind of pairing type does this pin have: -1 is unpaired, 0 is twin pairing (when one is on, the other is also on), 1 is cooperative (when one is on, the other is off and each takes the relay of the other)

    static const unsigned long _max_interval_on;

    void _switchPointerBlinkCycleState(const bool _bTargetBlinkingState);

    void _markTimeChanges();
};

#endif
