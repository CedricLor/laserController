/*
  pirController.cpp - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirController.h"

short const pirController::INPUT_PIN = 23;               // choose the input pin (for PIR sensor)
bool pirController::valPir = LOW;                        // we start assuming no motion detected // variable to store the pin status

pirController::pirController()
{
}

void pirController::initPir() {
  Serial.print("SETUP: initPir(): starting\n");
  pinMode(INPUT_PIN, INPUT);                                // declare sensor as input
  tPirCntrl.enableDelayed(60000);
  Serial.print("SETUP: initPir(): done\n");
}

Task pirController::tPirCntrl ( TASK_SECOND * 4, TASK_FOREVER, &tcbPirCntrl, &userScheduler, false);
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE functions

////////////////////////////////////////////////////////////////////////////////


// Tasks callback for Task tPirCntrl
/* tcbPirCntrl():
    1. reads the status of the pin connected to the IR sensor;
    2. starts (or extends) a PIR cycle, if such pin (connected to the IR sensor) is HIGH,
*/
void pirController::tcbPirCntrl() {
  if (digitalRead(INPUT_PIN) == HIGH) {                                                                              // if the PIR sensor has sensed a motion,
    boxState::setTargetActiveBoxState(3);
  }
}
