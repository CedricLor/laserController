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

    unsigned long last_time_on;     // last time this pin was turned on
    unsigned long last_time_off;    // last time this pin was turned off
    unsigned long last_interval_on; // last interval during which this pin was turned on

    // Initialization of the laserPin
    void physicalInitLaserPin(const short physicalPinNumber);

    // State machine setters
    void blinkLaserInBlinkingCycle();

    // State machine getters (variables calculated from LaserGroupedUnit or LaserGroupedUnitsArray values)
    bool blinking();                    // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
    unsigned long blinkingInterval();   // how long should this pin blink on and off
    short pairedWith();                 // with which other pin is this pin paired (-1 means it is not paired); the number correspond to the index_number of the paired pin
    bool pirState();                    // shall this pin respond to a change coming from the IR sensor; HIGH or LOW: HIGH -> reacting to changes in the PIR

    // Execute changes
    void executePinStateChange();
    void laserProtectionSwitch();

  private:
    static bool const _default_pin_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_pin_on_off_target_state; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    static bool const _default_pin_blinking_state;       // by default, pin starts as in a blinking-cycle TO ANALYSE

    short _physical_pin_number;      // pin physical number of the ESP32 which is controlling the relay

    static const unsigned long _max_interval_on;

    void _markTimeChanges();
    unsigned long _previousTime();    // last time this pin changed state (on or off)
};

#endif
