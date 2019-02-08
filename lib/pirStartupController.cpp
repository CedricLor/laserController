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

short LaserPinsArray::highPinsParityDuringStartup = 0;

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
Task pirStartupController::tPirStartUpDelayBlinkLaser( L_PIR_START_UP_DELAY, SI_PIR_START_UP_DELAY_ITERATIONS, &pirStartupController::cbtPirStartUpDelayBlinkLaser, &userScheduler, false, &pirStartupController::onEnablePirStartUpDelayBlinkLaser, &pirStartupController::onDisablePirStartUpDelayBlinkLaser );

/*
   tPirStartUpDelayPrintDash prints a dash to the console
   every second, for 9 seconds, during startup.
   It is re-enabled (re-started) 6 times by cbtPirStartUpDelayBlinkLaser().
*/
Task pirStartupController::tPirStartUpDelayPrintDash( 1000UL, 9, &cbtPirStartUpDelayPrintDash, &userScheduler );
void pirStartupController::cbtPirStartUpDelayPrintDash() {
  Serial.print("-");
}


void pirStartupController::cbtPirStartUpDelayBlinkLaser() {
  Serial.print("+");

  if (!(tPirStartUpDelayBlinkLaser.isFirstIteration())) {
    LaserGroupedUnitsArray::irStartupSwitch(HIGH);
    LaserPinsArray::highPinsParityDuringStartup = (LaserPinsArray::highPinsParityDuringStartup == 0) ? 1 : 0;
  }
  LaserGroupedUnitsArray::irStartupSwitch(LOW);
  tPirStartUpDelayPrintDash.restartDelayed();
  if (!(tPirStartUpDelayBlinkLaser.isLastIteration())) {
    tLaserOff.restartDelayed(1000);
    tLaserOn.restartDelayed(2000);
  }
}

bool pirStartupController::onEnablePirStartUpDelayBlinkLaser() {
  LaserPinsArray::pinGlobalModeWitness = 0;  // 0 means "in pirStartup cycle"
  LaserGroupedUnitsArray::pairUnpairAllPins(-1); // unpairs all the pins
  return true;
}

void pirStartupController::onDisablePirStartUpDelayBlinkLaser() {
  LaserGroupedUnitsArray::pairUnpairAllPins(1); // // pairs all the pins, in cooperative pairing
  LaserGroupedUnitsArray::irStartupSwitch(HIGH);
  LaserGroupedUnitsArray::inclExclAllInPir(HIGH);              // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE (TO DO). CURRENTLY: includes all the relays in PIR mode
  pirController::tPirCntrl.enable();
}



Task pirStartupController::tLaserOff( 0, 1, &cbtLaserOff, &userScheduler );

Task pirStartupController::tLaserOn( 0, 1, &cbtLaserOn, &userScheduler );

void pirStartupController::cbtLaserOff() {
  LaserGroupedUnitsArray::irStartupSwitch(HIGH);
}

void pirStartupController::cbtLaserOn() {
  LaserGroupedUnitsArray::irStartupSwitch(LOW);
}
