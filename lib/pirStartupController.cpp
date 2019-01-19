/*
  pirStartupController.cpp - Library to store functions related to the startup of the infrared sensor.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirStartupController.h"

pirStartupController::pirStartupController()
{
}

void pirStartupController::cbtPirStartUpDelayPrintDash() {
  Serial.print("-");
}

Task pirStartupController::tLaserOff( 0, 1, &cbtLaserOff, &userScheduler );

Task pirStartupController::tLaserOn( 0, 1, &cbtLaserOn, &userScheduler );

void pirStartupController::cbtLaserOff() {
  LaserPin::directPinsSwitch(LaserPins, HIGH);
}

void pirStartupController::cbtLaserOn() {
  LaserPin::directPinsSwitch(LaserPins, LOW);
}
