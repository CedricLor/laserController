/*
  pirStartupController.cpp - Library to store functions related to the startup of the infrared sensor.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirStartupController.h"

pirStartupController::pirStartupController()
{
}

// after being started, the Pir values shall not be read for the next 60 seconds, as the PIR is likely to send equivoqual values
const short pirStartupController::SI_PIR_START_UP_DELAY_ITERATIONS = 7;  // This const stores the number of times the tPirStartUpDelay Task shall repeat and inform the user that the total delay for the PIR to startup has not expired
const long pirStartupController::L_PIR_START_UP_DELAY = 10000UL;         // This const stores the duration of the cycles (10 seconds) of the tPirStartUpDelay Task

short int pirStartupController::_highPinsParityDuringStartup = 0;

/*
   tPirStartUpDelayBlinkLaser is the first task to be enabled upon setup of the ESP32
   Its role is to start a non-blocking (asynchronous) delay of 60 seconds to let the IR sensor warm up.
   Upon startup, the IR sensor needs more or less 60 seconds to warm up.
   During this delay, the IR sensors sends equivocal signals to the pin to which it is connected.
   Accordingly, we added this task to deactivate the IR sensors.
   During this 60 second period of time, we wanted some signal to be sent to the user of the boxes/lasers:
   - a dash is printed to the console every second
   - a plus sign is printed to the console every 10 seconds
   - the laser pairs blink in alternance every 10 seconds
   - upon blinking, the laser do a short blink on - blink off so that the user knows that the IR sensor is still warming up.
   The whole sequence would probably need to be redrafted to relay on the blinking interval defined at the LaserPin level (but for the moment, this has been done here).
*/
Task pirStartupController::tPirStartUpDelayBlinkLaser( L_PIR_START_UP_DELAY, SI_PIR_START_UP_DELAY_ITERATIONS, &_cbtPirStartUpDelayBlinkLaser, &userScheduler, false, &_onEnablePirStartUpDelayBlinkLaser, &_onDisablePirStartUpDelayBlinkLaser );

/*
   _tPirStartUpDelayPrintDash prints a dash to the console
   every second, for 9 seconds, during startup.
   It is re-enabled (re-started) 6 times by _cbtPirStartUpDelayBlinkLaser().
*/
Task pirStartupController::_tPirStartUpDelayPrintDash( 1000UL, 9, &_cbtPirStartUpDelayPrintDash, &userScheduler );
void pirStartupController::_cbtPirStartUpDelayPrintDash() {
  Serial.print("-");
}


void pirStartupController::_cbtPirStartUpDelayBlinkLaser() {
  Serial.print("+");

  if (!(tPirStartUpDelayBlinkLaser.isFirstIteration())) {
    LaserGroupedUnitsArray::irStartupSwitch(HIGH);
    _highPinsParityDuringStartup = (_highPinsParityDuringStartup == 0) ? 1 : 0;
  }
  LaserGroupedUnitsArray::irStartupSwitch(LOW);
  _tPirStartUpDelayPrintDash.restartDelayed();
  if (!(tPirStartUpDelayBlinkLaser.isLastIteration())) {
    _tLaserOff.restartDelayed(1000);
    _tLaserOn.restartDelayed(2000);
  }
}

bool pirStartupController::_onEnablePirStartUpDelayBlinkLaser() {
  LaserGroupedUnitsArray::globalModeWitness = 0;                // 0 means "in pirStartup cycle"
  LaserGroupedUnitsArray::pairUnpairAllPins(-1);                // unpairs all the pins
  return true;
}

void pirStartupController::_onDisablePirStartUpDelayBlinkLaser() {
  LaserGroupedUnitsArray::pairUnpairAllPins(1);                // pairs all the pins, in cooperative pairing
  LaserGroupedUnitsArray::irStartupSwitch(HIGH);               // makes sure that all lasers are turned off
  LaserGroupedUnitsArray::inclExclAllInPir(HIGH);              // includes all the relays in PIR mode
  pirController::tPirCntrl.enable();
}



Task pirStartupController::_tLaserOff( 0, 1, &_cbtLaserOff, &userScheduler );

Task pirStartupController::_tLaserOn( 0, 1, &_cbtLaserOn, &userScheduler );

void pirStartupController::_cbtLaserOff() {
  LaserGroupedUnitsArray::irStartupSwitch(HIGH);
}

void pirStartupController::_cbtLaserOn() {
  LaserGroupedUnitsArray::irStartupSwitch(LOW);
}
