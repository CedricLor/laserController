/*
  pirController.cpp - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirController.h"

short const pirController::_INPUT_PIN = 23;               // choose the input pin (for PIR sensor)

pirController::pirController()
{
}

void pirController::initPir() {
  Serial.print("SETUP: initPir(): starting\n");
  pinMode(_INPUT_PIN, INPUT);                                // declare sensor as input
  Serial.print("SETUP: initPir(): done\n");
}

void pirController::pirCntrl() {
  if (digitalRead(_INPUT_PIN) == HIGH) {
    ControlerBoxes[0].valPir = HIGH;
  }
}
