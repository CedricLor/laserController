/*
  pirController.cpp - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirController.h"

pirController::pirController()
{
}

short const pirController::INPUT_PIN = 23;               // choose the input pin (for PIR sensor)
bool pirController::valPir = LOW;                        // we start assuming no motion detected // variable to store the pin status

void pirController::setPirValue() {
  pirController::valPir = digitalRead(pirController::INPUT_PIN); // read input value from the pin connected to the IR. If IR has detected motion, digitalRead(INPUT_PIN) will be HIGH.
  // Serial.printf(pirController::valPir\n);
}

// loop over each of the structs representing pins to turn them on or off (if they are controlled by the PIR)
void pirController::switchPirRelays(LaserPin *LaserPins, const bool state) {
  Serial.print("PIR: switchPirRelays(const bool state): starting -------\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    if (LaserPins[thisPin].pir_state == HIGH) {
      LaserPins[thisPin].switchOnOffVariables(state);
    }
  }
  Serial.print("PIR: switchPirRelays(const bool state): leaving -------\n");
}
