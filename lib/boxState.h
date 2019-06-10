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
    boxState(const char cName[7], const short int iTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[]); // constructor and initialiser
    void initboxState(const char cName[7], const short int iTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[]);

    static const short int BOX_STATES_COUNT;
    static boxState boxStates[];
    static void initboxStates(); // initializer of the array of boxState

    static void setActiveboxState(const short int activeboxState);

    static Task testPlay;
    static void tcbTestPlay();
    static void odtcbTestPlay();

    static Task tEndboxState;
    static void _tcbTEndboxState();

    static void playboxState(const short int boxStateNumber);
    static Task _tPlayboxState;
    static void _tcbPlayboxState();

  private:
    char _cName[7];  // array of character to hold the name of each sequences
    short int _iTempo; // tempo at which the task executing the sequence will
                       // update the state of each laser pin, in milliseconds
    short int _iNumberOfBeatsInSequence; // number of tempos required to execute
                                          // one full sequence
    short int _iLaserPinStatusAtEachBeat[4];  // bi-dimensional array containing
                                                // the state of each laser at each tempo

    static short int _activeboxState;

    static unsigned long _ulboxStateDurationSetter();
    static unsigned long _ulboxStateDuration;
};

#endif
