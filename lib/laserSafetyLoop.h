/*
  laserSafetyLoop.h - Library to handle the transfer of update to the instances of the LaserPin class into HIGH/LOW states of the pins.
  Created by Cedric Lor, January 4, 2007.
*/

#ifndef laserSafetyLoop_h
#define laserSafetyLoop_h

#include "Arduino.h"

class laserSafetyLoop
{
  public:
    laserSafetyLoop();

    static void loop(LaserPin *LaserPins);
  private:
    static void blinkLaserIfBlinking(LaserPin &LaserPin);
    static void blinkLaserIfTimeIsDue(LaserPin &LaserPin);
    static void ifMasterPairedThenUpdateOnOffOfSlave(LaserPin *LaserPins, const int thisPin);
    static void evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(LaserPin *LaserPins, const int thisPin);
    static void updatePairedSlave(LaserPin *LaserPins, const int thisPin, const bool nextPinOnOffTarget);
    static void executeUpdates(LaserPin &LaserPin);
};

#endif
