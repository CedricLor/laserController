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

// Broadcasts PIR status over mesh
void pirController::broadcastPirStatus(const char* state) {     // state is "on" or "off". When pirController::valPir is HIGH (the IR has detected a move),
                                                 // the Pir block calls this function with the "on" parameter. Alternatively,
                                                 //  when the the pir cycle stops, it calls this function with the "off" parameter.
  Serial.printf("PIR - broadcastPirStatus(): broadcasting status over Mesh via call to broadcastStatusOverMesh(state) with state = %s\n", state);
  myMesh::broadcastStatusOverMesh(state);

  Serial.print("PIR - broadcastPirStatus(): ending.\n");
}


void pirController::startOrRestartPirCycleIfPirValueIsHigh() {
  if (pirController::valPir == HIGH) {                                                                              // if the PIR sensor has sensed a motion,
    if (!(pirController::tPirCycle.isEnabled())) {
      pirController::tPirCycle.setIterations(pirController::SI_PIR_ITERATIONS);
      pirController::tPirCycle.restart();
    } else {
      pirController::tPirCycle.setIterations(pirController::SI_PIR_ITERATIONS);
    }
  }
  pirController::valPir = LOW;                                                                                      // Reset the ValPir to low (no motion detected)
}


// Stop the pirCycle
void pirController::stopPirCycle() {
  Serial.print("PIR: stopPirCycle(): stopping PIR cycle.\n");
  pirController::switchPirRelays(LaserPins, HIGH);                                  // turn all the PIR controlled relays off
  pirController::broadcastPirStatus("off");                              // broadcast current pir status
}

/////////////////////////////////
// PIR CYCLE
const int pirController::I_PIR_INTERVAL = 1000;      // interval in the PIR cycle task (runs every second)
const short pirController::SI_PIR_ITERATIONS = 60;   // iteration of the PIR cycle

//////////////////////
// CALLBACKS FOR PIR CONTROL Task tPirCntrl (the Task that places the lasers under control of the PIR, looping and waiting for a movement to be detected)
// tcbPirCntrl() reads the status of the pin connected to the PIR controller; if HIGH, it enables tPirCycle.
void pirController::tcbPirCntrl() {
  pirController::setPirValue();
  pirController::startOrRestartPirCycleIfPirValueIsHigh();
}

// Tasks related to the PIR cycle (on/off of the lasers upon detecting a motion)
Task pirController::tPirCycle ( pirController::I_PIR_INTERVAL, pirController::SI_PIR_ITERATIONS, NULL, &userScheduler, false, &pirController::tcbOnEnablePirCycle, &pirController::tcbOnDisablePirCycle);

//////////////////////
// CALLBACKS FOR TASK Task tPirCycle (the Task that controls the switching on and off of the laser when the PIR has detected some movement)
bool pirController::tcbOnEnablePirCycle() {
  Serial.print("PIR: tcbStartPirCycle(): Motion detected!!!\n");
  pirController::switchPirRelays(LaserPins, LOW);
  pirController::broadcastPirStatus("on");                                                                        // broadcast startup of a new pir Cycle
  Serial.print("PIR: tcbStartPirCycle(): broadcastPirStatus(\"on\")");
  return true;
}

void pirController::tcbOnDisablePirCycle() {
  Serial.print("PIR: pirController::tcbStopPirCycle(): PIR time is due. Ending PIR Cycle -------\n");
  pirController::stopPirCycle();
}
