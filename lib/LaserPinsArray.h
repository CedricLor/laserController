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

    static void initLaserPins(LaserPin *LaserPins);

    static void directPinsSwitch(LaserPin *LaserPins, const bool targetState);

    /* variable to store which of the odd or even pins controlling the lasers are high during the pirStartUp delay.
                0 = even pins are [high] and odds are [low];
                1 = odd pins are [low] and evens are [high];
    */
    static short highPinsParityDuringStartup;

    static void manualSwitchAllRelays(LaserPin *LaserPins, const bool targetState);

    static void inclExclAllRelaysInPir(LaserPin *LaserPins, const bool targetPirState);

    static short pinParityWitness;
    static void pairUnpairAllPins(LaserPin *LaserPins, const short pairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/);

    static void changeGlobalBlinkingInterval(LaserPin *LaserPins, const unsigned long targetBlinkingInterval);

    static void slaveBoxSwitchAllRelays(const bool targetState);

  private:

    static short _siAutoSwitchInterval;                      // defines the length of the cycle during which we place the pins in automatic mode (i.e. automatic mode is with Pir deactivated)
    static bool _tcbOaslaveBoxSwitchAllRelays();
    static void _tcbOdslaveBoxSwitchAllRelays();
    static Task _tslaveBoxSwitchAllRelays;
};

#endif
