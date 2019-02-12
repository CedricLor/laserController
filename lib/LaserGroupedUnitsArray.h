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

    // initialization and recomposition of the LaserGroupedUnitsArray
    static void pairUnpairAllPins(const short _sPairingType /*-1 unpair, 0 twin pairing, 1 cooperative pairing*/);

    // state machine variables
    // 0 pirStartUp cycle, 1 IR waiting, 2 IR cycle on, 3 slave cycle on, 4 manual
    static short int currentState;  // read access from pirController
    static short int previousState; // set access from myMeshController
    static short int targetState;
    static const char* GLOBAL_WITNESS_TEXT_DESCRIPTORS[5];

    static bool bCurrentStateOfLaserGroupUnits;  // Not in use for the moment, but will be used in the future in the website --> do not classify as private variable
    static bool bPreviousStateOfLaserGroupUnits;  // Not in use for the moment, but will be used in the future in the website --> do not classify as private variable
    static bool bTargetStateOfLaserGroupUnits;  // Not in use for the moment, but will be used in the future in the website --> do not classify as private variable

    static bool currentPirState;
    static bool previousPirState;
    static bool targetPirState;

    static unsigned long currentPinBlinkingInterval;
    static unsigned long previousPinBlinkingInterval;
    static unsigned long targetPinBlinkingInterval;

    // state machine action iterator
    static void (*_stateChangeActions[5])(const bool _bTargetState);

    // state machine setter
    static void setTargetState(const short int __sTargetState);
    static void setTargetStateOfLaserGroupUnits(const bool __bCurrentStateOfLaserGroupUnits);
    static void setTargetPirState(const short int __sTargetPirState);
    static void setTargetBlinkingInterval(const unsigned long __ulTargetBlinkingInterval);

    // extracted from pirStartupController
    static Task tLaserOff;
    static Task tLaserOn;

  private:
    static void _pairingAll();
    static void _unpairingAll();

    // state machine reader
    static long int _liActUponStateChangesInterval;
    static short int _siActUponStateChangesIterations;
    static Task _tActUponStateChanges;
    static void _tcbActUponStateChanges();
    static bool _LGUAHasChanged();

    // state machine actions // TO BE DELETED ONCE LOGIC INCORPORATED IN LaserPinsArray::loop
    static void _irStartupSwitch(const bool _bTargetState);
    static void _manualSwitchAll(const bool _bTargetState);
    static void _pirSwitchAll(const bool _bTargetState);
    static void _slaveBoxSwitchAll(const bool targetState);
    static void _inclExclAllInPir(const bool _bTargetState);
    static void _changeBlinkingIntervalAll(const unsigned long _ulTargetBlinkingInterval);

    // Slave cycle controller
    static long _ulSlaveBoxCycleInterval;
    static short _siSlaveBoxCycleIterations;                      // defines the length of the cycle during which we place the pins in automatic mode (i.e. automatic mode is with Pir deactivated)
    static bool _tcbOeSlaveBoxCycle();
    static void _tcbOdSlaveBoxCycle();
    static Task _tSlaveBoxCycle;

    // StartupPir controller
    static void _cbtLaserOff();
    static void _cbtLaserOn();
};

#endif
