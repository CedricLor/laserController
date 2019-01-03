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
    LaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */);
    short number;        // pin number to which the relays are attached
    bool on_off;       // is the pin HIGH or LOW (LOW = the relay is closed, HIGH = the relay is open)
    bool on_off_target;// a variable to store the on / off change requests by the various functions
    bool blinking;     // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
    unsigned long previous_time;
    unsigned long blinking_interval;
    bool pir_state;     // HIGH or LOW: HIGH -> controlled by the PIR
    short paired;        // a variable to store with which other pin this pin is paired (8 means it is not paired)

    static void initLaserPins(LaserPin *LaserPins);
    void initLaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */);
    void physicalInitLaserPin();

    static void directPinsSwitch(LaserPin *LaserPins, const bool targetState);
    static short highPinsParityDuringStartup;
    void switchOnOffVariables(const bool targetState);
    void switchPointerBlinkCycleState(const bool state);

  private:
    static bool const default_pin_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    static bool const default_pin_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    static bool const default_pin_blinking_state = false;       // by default, pin starts as in a blinking-cycle TO ANALYSE
    static bool const default_pin_pir_state_value = LOW;       // by default, the pin is not controlled by the PIR

};

#endif
