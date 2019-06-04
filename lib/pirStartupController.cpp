/*
  pirStartupController.cpp - Library to store functions related to the startup of the infrared sensor.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirStartupController.h"

/*
   After starting up the ESP32, the values returned by the IR sensor shall not be read
   for the following 60 seconds, because the PIR is likely to send equivoqual values
   (the IR sensor needs to warm up for about 60 s.).

   tPirStartUpDelayBlinkLaser is the first task to be enabled upon setup of the ESP32.
   Its role is to start a non-blocking (asynchronous) 60 seconds delay of during
   which the values from the IR sensor are not read.
   In addition, we wanted to inform the user that the box was in such startup mode:
   - a dash is printed to the console every second
   - a plus sign is printed to the console every 10 seconds
   - the laser pairs blink in alternance every 10 seconds
   - upon blinking, the laser do a short blink on - blink off so that the user knows that the IR sensor is still warming up.
*/

// constructor
pirStartupController::pirStartupController()
{

}

// variables
const short pirStartupController::_SI_PIR_START_UP_DELAY_ITERATIONS = 7;  // number of times the tPirStartUpDelay Task shall repeat
const long pirStartupController::_L_PIR_START_UP_DELAY = 10000UL;         // duration of the cycles (10 seconds) of the tPirStartUpDelay Task

// Main task: make the lasers blink and write - and + to Serial for a startup delay
// of about 60 seconds
Task pirStartupController::tPirStartUpDelayBlinkLaser( _L_PIR_START_UP_DELAY, _SI_PIR_START_UP_DELAY_ITERATIONS, &_cbtPirStartUpDelayBlinkLaser, &userScheduler, false, &_onEnablePirStartUpDelayBlinkLaser, &_onDisablePirStartUpDelayBlinkLaser );

// main callback for Task pirStartupController::tPirStartUpDelayBlinkLaser
// repeated actions
void pirStartupController::_cbtPirStartUpDelayBlinkLaser() {
  Serial.print("+");

  if (!(tPirStartUpDelayBlinkLaser.isFirstIteration())) {
    LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(HIGH);         // HIGH = off
  }
  LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(LOW);
  _tPirStartUpDelayPrintDash.restartDelayed();
  if (!(tPirStartUpDelayBlinkLaser.isLastIteration())) {
    LaserGroupedUnitsArray::tLaserOff.restartDelayed(1000);
    LaserGroupedUnitsArray::tLaserOn.restartDelayed(2000);
  }
}

// onEnable callback for Task pirStartupController::tPirStartUpDelayBlinkLaser
bool pirStartupController::_onEnablePirStartUpDelayBlinkLaser() {
  LaserGroupedUnitsArray::setTargetState(0);    // 0 means "in pirStartup cycle"
  LaserGroupedUnitsArray::pairUnpairAllPins(-1 /* -1 for unpairing */);
  return true;
}

// onDisable callback for Task pirStartupController::tPirStartUpDelayBlinkLaser
void pirStartupController::_onDisablePirStartUpDelayBlinkLaser() {
  // pairs all the pins, in cooperative pairing
  // first initialization of the LaserGroupedUnitsArray (i.e. first time loading of the LAserPins into the LaserGroupedUnits)
  LaserGroupedUnitsArray::pairUnpairAllPins(1 /* 1 for cooperative pairing */);
  // makes sure that all lasers are turned off
  LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(HIGH);
  // includes all the relays in PIR mode
  LaserGroupedUnitsArray::setTargetPirState(HIGH);
  // enable the pirController
  pirController::tPirCntrl.enable();
}

/*
   _tPirStartUpDelayPrintDash prints a dash to the console
   every second, for 9 seconds, during startup.
   It is re-enabled (re-started) 6 times by _cbtPirStartUpDelayBlinkLaser().
*/
Task pirStartupController::_tPirStartUpDelayPrintDash( 1000UL, 9, &_cbtPirStartUpDelayPrintDash, &userScheduler );
void pirStartupController::_cbtPirStartUpDelayPrintDash() {
  Serial.print("-");
}
