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
    laserSafetyLoop(/*int pin*/);

    void laserSafetyLoop();
    void blinkLaserIfBlinking(const short thisPin);
    void blinkLaserIfTimeIsDue(const short thisPin);
    void ifMasterPairedThenUpdateOnOffOfSlave(const short thisPin);
    void evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(const short thisPin);
    void updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget);
    void executeUpdates(const short thisPin);
  private:
    // int _pin;
};

#endif
