/*
  LaserGroupedUnitsArray.h - Library to handle the array of grouped laser units.
  Created by Cedric Lor, February 7, 2019.
*/

#ifndef LaserGroupedUnitsArray_h
#define LaserGroupedUnitsArray_h

#include "Arduino.h"

class LaserGroupedUnitsArray
{
  public:
    LaserGroupedUnitsArray();

    static short loadedLaserUnits;

    static void pairingAll();
    static void unpairingAll();
    static void setPairingTypeofAll(const short _sPairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/);

    static void irStartupSwitch(const bool _bTargetState);

    static short int highPinsParityDuringStartup;

    static void manualSwitchAll(const bool _bTargetState);

    static void pirSwitchAll(const bool _bTargetState);

    static void inclExclAllInPir(const bool _bTargetState);

    static short int pinParityWitness;

    static void changeBlinkingIntervalAll(const unsigned long _ulTargetBlinkingInterval);

    static void slaveBoxSwitchAll(const bool targetState);

    static short int globalModeWitness;
    // 0 pirStartUp cycle, 1 IR waiting, 2 IR cycle on, 3 slave cycle on, 4 manual with cycle on, 5 manual with cycle off
    // !!! TO DO -- this corresponds to the former pinGlobalModeWitness of LaserPinsArray

    static const char* GLOBAL_WITNESS_TEXT_DESCRIPTORS[6];
    // Not in use for the moment, but will be used in the future in the website --> do not classify as private variable

    static char* pinGlobalMode;

  private:

    static short _siSlaveBoxCycleIterations;                      // defines the length of the cycle during which we place the pins in automatic mode (i.e. automatic mode is with Pir deactivated)
    static bool _tcbOeSlaveBoxCycle();
    static void _tcbOdSlaveBoxCycle();
    static Task _tSlaveBoxCycle;
};

#endif
