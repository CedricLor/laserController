/*
  boxState.h - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--note.cpp (called to play some member functions)
  |  |  |  |  |--note.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h

*/

#ifndef boxState_h
#define boxState_h

#include "Arduino.h"
#include "global.h"

class boxState
{
  public:
    boxState(); // default constructor

    static void initBoxStates(); // initializer of the array of boxState

    static Task tPlayBoxStates;
    static const short int BOX_STATES_COUNT;

    static boxState boxStates[];

    char cName[15];  // array of character to hold the name of each sequences
    char cHtmlTag[4];  // array of character to hold the name of each sequences

  private:

    static const short int _NAME_CHAR_COUNT;
    static const short int _HTML_TAG_CHAR_COUNT;
    static short int _boxTargetState;
    static bool _boxActiveStateHasBeenReset;
    static short int _boxDefaultState;

    static void _tcbPlayBoxStates();
    static bool _oetcbPlayBoxStates();

    static void _setBoxTargetState(const short int targetBoxState);

    static Task _tPlayBoxState;
    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

    unsigned long _ulDuration; // duration for which the status shall active before automatically returning to default
    short int _iAssociatedSequence;  // sequence associated to a given state
    short int _iIRTrigger; // in this state, does signals from IR trigger a restart or interrupt
    short int _iMeshTrigger; // in this state, does signals from IR trigger a restart or interrupt

    void _initBoxState(const char cpName[], const char cpHtmlTag[], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger);
};

#endif
