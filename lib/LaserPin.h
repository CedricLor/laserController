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

    void initLaserPin(short thisPin /* index number of this pin in the array of LaserPin */);
    void physicalInitLaserPin();

    void switchOnOffVariables(const bool targetState);
    void switchPointerBlinkCycleState(const bool state);

    void manualSwitchOneRelay(const bool targetState);

    void inclExclOneRelayInPir(const bool state);

    void pairPin(LaserPin *LaserPins, const short thisPin, const bool targetPairingState/*, const short _pinParityWitness*/);

    void changeIndividualBlinkingDelay(const unsigned long blinkingDelay);
    void changeTheBlinkingInterval(const unsigned long blinkingDelay);

  private:
    static bool const _default_pin_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_pin_on_off_target_state; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    static bool const _default_pin_blinking_state;       // by default, pin starts as in a blinking-cycle TO ANALYSE
    static bool const _default_pin_pir_state_value;       // by default, the pin is not controlled by the PIR

    void _rePairPin(LaserPin *LaserPins, const short thisPin, const short thePairedPin);
};

#endif
