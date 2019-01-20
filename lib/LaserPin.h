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

    short number;                   // pin number of the ESP32 which is controling the relay
    bool on_off;                    // variable to store the state (HIGH or LOW) of the pin (LOW = the relay is closed, HIGH = the relay is open)
    bool on_off_target;             // variable to store the on / off change requests by the various functions
    bool blinking;                  // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
    unsigned long previous_time;
    unsigned long blinking_interval;// variable to store for how long a pin shall blink
    bool pir_state;                 // variable to store whether the pin shall respond to a change coming from the IR sensor; HIGH or LOW: HIGH -> controlled by the PIR
    short paired;                   // variable to store with which other pin this pin is paired (8 means it is not paired)
    unsigned long last_time_on;
    unsigned long last_time_off;
    unsigned long last_interval_on;

    void physicalInitLaserPin();

    void switchOnOffVariables(const bool targetState);
    void switchPointerBlinkCycleState(const bool state);

    void manualSwitchOneRelay(const bool targetState);

    void inclExclOneRelayInPir(const bool state);

    void pairUnpairPin(const short thisPin, const bool targetPairingState, const short _pinParityWitness);
    void pairWithNextPin(const short thisPin, const short _pinParityWitness);
    void pairWithNextPinPlusOne(const short thisPin /* index number of this pin in LaserPinsArray */, const short _pinQuaternaryWitness);

    void changeIndividualBlinkingDelay(const unsigned long blinkingDelay);
    void changeTheBlinkingInterval(const unsigned long blinkingDelay);

    void executePinStateChange();
    void laserProtectionSwitch();

  private:
    static bool const _default_pin_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_pin_on_off_target_state; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    static bool const _default_pin_blinking_state;       // by default, pin starts as in a blinking-cycle TO ANALYSE
    static bool const _default_pin_pir_state_value;       // by default, the pin is not controlled by the PIR

    static const unsigned long _max_interval_on;

    void _markTimeChanges();
};

#endif
