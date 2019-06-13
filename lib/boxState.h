/*
  boxState.h - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.
*/

#ifndef boxState_h
#define boxState_h

#include "Arduino.h"
#include "global.h"

class boxState
{
  public:
    boxState(); // default constructor
    boxState(const char cName[], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger); // constructor and initialiser
    void initBoxState(const char cName[], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger);

    static const short int BOX_STATES_COUNT;
    static boxState boxStates[];
    static void initBoxStates(); // initializer of the array of boxState

    static void setTargetActiveBoxState(const short int activeBoxState);

    static Task tPlayBoxStates;
    static void tcbPlayBoxStates();
    static bool oetcbPlayBoxStates();

    static void playBoxState();
    static Task _tPlayBoxState;
    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

  private:
    static const short int _NAME_CHAR_COUNT;
    char _cName[15];  // array of character to hold the name of each sequences
    unsigned long _ulDuration; // duration for which the status shall active before automatically returning to default
    short int _iAssociatedSequence;  // sequence associated to a given state
    short int _iIRTrigger; // in this state, does signals from IR trigger a restart or interrupt
    short int _iMeshTrigger; // in this state, does signals from IR trigger a restart or interrupt

    static short int _activeBoxState;
    static short int _targetActiveBoxState;

    static unsigned long _ulboxStateDurationSetter();
    static unsigned long _ulboxStateDuration;
};

#endif
