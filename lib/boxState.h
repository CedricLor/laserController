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
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h


reverse dependency graph
  |--boxState.h
  |  |--boxState.cpp
  |  |  |--main.cpp

*/

#ifndef boxState_h
#define boxState_h

#include "Arduino.h"
#include "./sequence/sequence.h"
#include "./sequence/sequence.cpp"

class boxState
{
  public:
    boxState(); // default constructor

    static void initBoxStates(); // initializer of the array of boxState

    static Task tPlayBoxStates;
    static const short int BOX_STATES_COUNT;

    static boxState boxStates[];

    char cName[25];  // array of character to hold the name of each boxState
    char cHtmlTag[4];  // array of character to hold the html tag of each boxState

  private:

    static const short int _NAME_CHAR_COUNT;
    static const short int _HTML_TAG_CHAR_COUNT;
    static short int _boxTargetState;
    static bool _boxActiveStateHasBeenReset;

    static void _tcbPlayBoxStates();
    static bool _oetcbPlayBoxStates();

    static Task _tPlayBoxState;
    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

    // _tcbPlayBoxStates() sub functions
    static void _setBoxTargetStateFromSignalCatchers();
    static void _resetSignalCatchers();
    static void _restart_tPlayBoxState();

    static void _setBoxTargetState(const short int targetBoxState);

    unsigned long _ulDuration; // duration for which the status shall stay active before automatically returning to default
    byte _bAssociatedSequence;  // sequence associated to a given state
    byte _bIRTrigger; // in this state, does signals from IR trigger a restart or interrupt
    byte _bMeshTrigger; // in this state, does signals from IR trigger a restart or interrupt

    void _initBoxState(const char cpName[], const char cpHtmlTag[], const unsigned long ulDuration, const byte bAssociatedSequence, const byte bIRTrigger, const byte bMeshTrigger);
};

#endif
