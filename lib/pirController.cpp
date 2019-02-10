/*
  pirController.cpp - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirController.h"

pirController::pirController()
{
}

void pirController::initPir() {
  Serial.print("SETUP: initPir(): starting\n");
  pinMode(INPUT_PIN, INPUT);                                // declare sensor as input
  Serial.print("SETUP: initPir(): done\n");
}

/* When Task tPirCntrl is enabled, the lasers are placed under the control of the IR sensor.
   This means that when the IR will detect a movement, it will start a PIR cycle (see below).
   For the moment, tPirCntrl is disabled upon startup of the box for 60 (for the duration of the pirStartupController tasks).

 - Task tPirCntrl is enabled at the end of the pirStartup cycle in the onDisablePirStartUpDelayBlinkLaser callback.
   related to the PIR controller (placing the lasers under control of the PIR and waiting for a motion to be detected to turn the lasers on/off)

 - Task tPirCntrl is here defined to run every 4 seconds (TASK_SECOND * 4) and indefinitely (TASK_FOREVER).
   Upon each iteration, it runs its main callback tcbPirCntrl().
   Its main callback:
   a. read the state of pin connected to the IR sensor;
   b. decides to start (or extend) a PIR cycle depending on the state of the pin connected to the IR sensor.
   It is added to the userScheduler upon its creation, waiting to be enabled.
   It is created without being enabled (false) and has no onEnable and onDisable callbacks (NULL, NULL).

   TO DO:
   1. add an onEnable and an onDisable callback to cleanup the LaserPins status
   2. Task tPirCntrl shall be disabled, if the box is in slave mode and when, in slave mode, it receive an order to enter into automatic mode
 */
Task pirController::tPirCntrl ( TASK_SECOND * 4, TASK_FOREVER, &tcbPirCntrl, &userScheduler, false, &tcbOnEnablePirCntrl, NULL);
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE functions

////////////////////////////////////////////////////////////////////////////////
// PIR Controller
short const pirController::INPUT_PIN = 23;               // choose the input pin (for PIR sensor)
bool pirController::valPir = LOW;                        // we start assuming no motion detected // variable to store the pin status


// Tasks callback for Task tPirCntrl
/* tcbPirCntrl():
    1. reads the status of the pin connected to the IR sensor;
    2. starts (or extends) a PIR cycle, if such pin (connected to the IR sensor) is HIGH,
*/
void pirController::tcbPirCntrl() {
  setPirValue();
  startOrRestartPirCycleIfPirValueIsHigh();
}

bool pirController::tcbOnEnablePirCntrl() {
  LaserGroupedUnitsArray::setTargetState(1);          // 1 means "IR waiting"
  return true;
}

// read input value from the pin connected to the IR.
void pirController::setPirValue() {
  valPir = digitalRead(INPUT_PIN);                       // If IR has detected motion, digitalRead(INPUT_PIN) will be HIGH.
  // Serial.printf(pirController::valPir\n);
}

// Starts (or extends) the PIR Cycle
void pirController::startOrRestartPirCycleIfPirValueIsHigh() {
  if (valPir == HIGH) {                                                                              // if the PIR sensor has sensed a motion,
    if (!(tPirCycle.isEnabled())) {
      tPirCycle.setIterations(SI_PIR_ITERATIONS);
      tPirCycle.restart();
    } else {
      tPirCycle.setIterations(SI_PIR_ITERATIONS);
    }
  }
  pirController::valPir = LOW;                                                                                      // Reset the ValPir to low (no motion detected)
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// PIR CYCLE
const int pirController::I_PIR_INTERVAL = 1000;      // interval in the PIR cycle task (runs every second)
const short pirController::SI_PIR_ITERATIONS = 60;   // iteration of the PIR cycle

/* Task tPirCycle mainly acts as a timer.
   It is enabled for 60 seconds and iterates every second.
   It is enabled when the IR sensor has detected a movement.

   Upon being enabled and upon being disabled:
   1. it switches the state of the property .pir_state of each of the LaserGroupedUnit;
   2. it sends a message via the mesh to the other box to inform them that it has started a pirCycle.

   Note: the blinking delay of each laser or each pair of laser is not defined in the pirController but at the level of the LaserPins.
 */
Task pirController::tPirCycle ( I_PIR_INTERVAL, SI_PIR_ITERATIONS, NULL, &userScheduler, false, &tcbOnEnablePirCycle, &tcbOnDisablePirCycle);

// CALLBACKS FOR TASK Task tPirCycle (the Task that controls the switching on and off of the laser when the PIR has detected some movement)
bool pirController::tcbOnEnablePirCycle() {
  // Checks that the currentState of the LaserGroupedUnitsArray reflects the status of the laser controller and update it accordingly if necessary
  if (LaserGroupedUnitsArray::currentState == 1) {LaserGroupedUnitsArray::setTargetState(2);}  // 1 means "IR cycle on"
  Serial.print("PIR: tcbOnEnablePirCycle(): Motion detected!!!\n");
  // Place all the LAserGroupedUnits under the controle of the IR
  LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(LOW);
  // Send message to the mesh
  myMeshViews::statusMsg("on");
  Serial.print("PIR: tcbOnEnablePirCycle(): broadcastPirStatus(\"on\")");
  return true;
}

void pirController::tcbOnDisablePirCycle() {
  if (LaserGroupedUnitsArray::currentState == 2) {LaserGroupedUnitsArray::setTargetState(1);}  // 1 means "IR cycle on"
  Serial.print("PIR: pirController::tcbOnDisablePirCycle(): PIR time is due. Ending PIR Cycle -------\n");
  stopPirCycle();
}

// Stops the PIR Cycle
void pirController::stopPirCycle() {
  Serial.print("PIR: stopPirCycle(): stopping PIR cycle.\n");
  LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(HIGH);
  myMeshViews::statusMsg("off");
}
