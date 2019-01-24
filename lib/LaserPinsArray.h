/*
  LaserPinsArray.h - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, January 19, 2019.
*/

#ifndef LaserPinsArray_h
#define LaserPinsArray_h

#include "Arduino.h"

class LaserPinsArray
{
  public:
    LaserPinsArray();

    static void initLaserPins();

    static void directPinsSwitch(const bool targetState);

    static short int highPinsParityDuringStartup;

    static void manualSwitchAllRelays(const bool targetState);

    static void inclExclAllRelaysInPir(const bool targetPirState);

    static short int pinParityWitness;
    static void pairUnpairAllPins(const short pairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/);

    static void changeGlobalBlinkingInterval(const unsigned long targetBlinkingInterval);

    static void slaveBoxSwitchAllRelays(const bool targetState);

    static short int pinGlobalModeWitness;   // 0 pirStartUp cycle, 1 IR waiting, 2 IR cycle on, 3 slave cycle on, 4 manual with cycle on, 5 manual with cycle off
    static const char* PIN_GLOBAL_WITNESS_TEXT_DESCRIPTORS[6];
    static char* pinGlobalMode;

  private:

    static short _siSlaveBoxCycleIterations;                      // defines the length of the cycle during which we place the pins in automatic mode (i.e. automatic mode is with Pir deactivated)
    static bool _tcbOeSlaveBoxCycle();
    static void _tcbOdSlaveBoxCycle();
    static Task _tSlaveBoxCycle;
};

#endif
