/*
  laserSafetyLoop.h - Library to handle the transfer of update to the instances of the LaserPin class into HIGH/LOW states of the pins.
  Created by Cedric Lor, January 4, 2019.
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
    static void _blinkLaserIfBlinking(LaserPin &LaserPin);
    static void _blinkLaserIfTimeIsDue(LaserPin &LaserPin);

    static void _ifMasterPairedThenUpdateOnOffOfSlave(LaserPin *LaserPins, const int thisPin);
    static void _evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(LaserPin *LaserPins, const int thisPin);
    static void _updatePairedSlave(LaserPin *LaserPins, const int thisPin, const bool nextPinOnOffTarget);

    static void _executeUpdates(LaserPin &LaserPin);
};

#endif
