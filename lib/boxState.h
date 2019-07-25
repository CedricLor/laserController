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
    // boxState(const unsigned long _ulDuration, const uint16_t _ui16AssociatedSequence, const uint16_t _ui16onIRTrigger, const uint16_t _ui16onMeshTrigger, const uint16_t _ui16onExpire);

    static void initBoxStates(); // initializer of the array of boxState

    static Task tPlayBoxStates;
    static const short int BOX_STATES_COUNT;

    static boxState boxStates[];

    // static ControlerBox* thisBox;
    // static ControlerBox* masterBox;
    // static boxState* myActiveState;

    unsigned long ulDuration; // duration for which the status shall stay active before automatically returning to default
    uint16_t ui16AssociatedSequence;  // sequence associated to a given state
    int16_t i16onIRTrigger;
    int16_t i16onMeshTrigger;
    int16_t i16onExpire;

  private:

    static short int _boxTargetState;
    static bool _boxActiveStateHasBeenReset;

    static void _tcbPlayBoxStates();
    static bool _oetcbPlayBoxStates();

    static Task _tPlayBoxState;
    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

    // _tcbPlayBoxStates() sub functions
    static void _setBoxTargetStateFromSignalCatchers(ControlerBox& _masterBox, ControlerBox& _thisBox);
    static void _resetSignalCatchers(ControlerBox& _masterBox);
    static void _restart_tPlayBoxState(ControlerBox& _thisBox);

    static void _setBoxTargetState(const short int targetBoxState);

    void _initBoxState(const unsigned long _ulDuration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire);
};

#endif
