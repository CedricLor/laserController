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

    void initLaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */);
    void physicalInitLaserPin();

    static void directPinsSwitch(LaserPin *LaserPins, const bool targetState);

    /* variable to store which of the odd or even pins controlling the lasers are high during the pirStartUp delay.
                0 = even pins are [high] and odds are [low];
                1 = odd pins are [low] and evens are [high];
    */
    static short highPinsParityDuringStartup;

    void switchOnOffVariables(const bool targetState);
    void switchPointerBlinkCycleState(const bool state);

    static void switchAllRelays(LaserPin *LaserPins, const bool state);
    void manualSwitchOneRelay(const bool targetState);

    static void inclExclAllRelaysInPir(LaserPin *LaserPins, const bool state);
    void inclExclOneRelayInPir(const bool state);

    static short pinParityWitness;
    static void pairAllPins(LaserPin *LaserPins, const bool targetPairingState);
    void pairPin(LaserPin *LaserPins, const short thisPin, const bool targetPairingState);

    static void changeGlobalBlinkingDelay(LaserPin *LaserPins, const unsigned long blinkingDelay);
    void changeIndividualBlinkingDelay(const unsigned long blinkingDelay);
    void changeTheBlinkingInterval(const unsigned long blinkingDelay);

    static Task tAutoSwitchAllRelays;

  private:
    static bool const _default_pin_on_off_state;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const _default_pin_on_off_target_state; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    static bool const _default_pin_blinking_state;       // by default, pin starts as in a blinking-cycle TO ANALYSE
    static bool const _default_pin_pir_state_value;       // by default, the pin is not controlled by the PIR

    void _rePairPin(LaserPin *LaserPins, const short thisPin, const short thePairedPin);


    static short _siAutoSwitchInterval;                      // defines the length of the cycle during which we place the pins in automatic mode (i.e. automatic mode is with Pir deactivated)
    static bool _tcbOaAutoSwitchAllRelays();
    static void _tcbOdAutoSwitchAllRelays();
};

#endif
