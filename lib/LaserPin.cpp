/*
  LaserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "LaserPin.h"

LaserPin::LaserPin()
{
  on_off = default_pin_on_off_state;
  on_off_target = default_pin_on_off_target_state;
  blinking = default_pin_blinking_state;
  previous_time = millis();
  blinking_interval = pinBlinkingInterval;
  pir_state = default_pin_pir_state_value;
}

LaserPin::LaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */)
{
  on_off = default_pin_on_off_state;
  on_off_target = default_pin_on_off_target_state;
  blinking = default_pin_blinking_state;
  previous_time = millis();
  blinking_interval = pinBlinkingInterval;
  pir_state = default_pin_pir_state_value;
  initLaserPin(pinNumber, thisPin);
}
/////////////////////////////////////////////////////////////////////////
// INIT
void LaserPin::initLaserPin(short pinNumber /* pin number on the ESP board */, short thisPin /* index number of this pin in the array of LaserPin */)
{
  number = pinNumber;
  short pairedPinNumber = (thisPin % 2 == 0) ? (thisPin + 1) : (thisPin - 1);
  paired = pairedPinNumber;
}

void LaserPin::physicalInitLaserPin()
{
  pinMode(number, OUTPUT);     // initialization of the pin connected to each of the relay as output
  digitalWrite(number, HIGH);  // setting default value of the pins at HIGH (relay closed)
}

void LaserPin::initLaserPins(LaserPin *LaserPins) {
  Serial.print("SETUP: initLaserPins(): starting\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Initialize Laser Pin
    LaserPins[thisPin].initLaserPin(relayPins[thisPin], thisPin);
    LaserPins[thisPin].physicalInitLaserPin();
  }
  Serial.print("SETUP: initLaserPins(): done\n");
}

//////////////////////////////////////////////////////////////////////////
// SWITCHES
// Switches relay pins on and off during PIRStartUp
void LaserPin::directPinsSwitch(LaserPin *LaserPins, const bool targetState) {              // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisPin = highPinsParityDuringStartup; thisPin < PIN_COUNT; thisPin = thisPin + 2) {        // loop around all the structs representing the pins controlling the relays
    LaserPins[thisPin].switchOnOffVariables(targetState);
  }
}

void LaserPin::switchOnOffVariables(const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: switchOnOffVariables(const short thisPin, const bool targetState): switching on/off variables for LaserPins[%u] with targetState = %s \n", thisPin, (targetState == 0 ? "on (LOW)" : "off (HIGH)"));
  switchPointerBlinkCycleState(targetState);                        // turn the blinking state of the struct representing the pin on or off
  on_off_target = targetState;                                               // turn the on_off_target state of the struct on or off
                                                                             // the actual pin will be turned on or off in the LASER SAFETY TIMER
}

void LaserPin::switchPointerBlinkCycleState(const bool state) {
  // If the state passed on to the function is LOW (i.e.
  // probably the targetState in the calling function),
  // marks that the pin is in a blinking cycle.
  // If not, marks that the blinking cycle for this pin is off.
  (state == LOW) ? blinking = true : blinking = false;
}

//////////////////////////////////////////////////////////////////////////
// MANUAL SWITCHES
// Manually switches all the lasers
void LaserPin::switchAllRelays(LaserPin *LaserPins, const bool state) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].manualSwitchOneRelay(state);
  }
}

// Manually switches a single laser
void LaserPin::manualSwitchOneRelay(const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: manualSwitchOneRelay(const short thisPin, const bool targetState): switching LaserPins[%u] to targetState %s\n", thisPin, (targetState == 0 ? ": on" : ": off"));      // MIGHT CAUSE A BUG!!!
  switchOnOffVariables(targetState);
  pir_state = LOW;
}
