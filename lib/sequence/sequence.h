/*
  sequence.h - sequences are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
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
  |  |  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |  |  |--global.h


*/
#ifndef sequence_h
#define sequence_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <ControlerBox.h>
#include <bar.h>

class sequence
{
  public:
    sequence(); // default constructor

    static sequence sequences[];
    static void initSequences(); // initializer of the array of sequences

    static void setActiveSequence(const short int activeSequence);

    static Task tPlaySequenceInLoop;

  private:
    static const short int _sequence_count;
    static short int _activeSequence;
    static const short int _char_count_in_name;

    static long unsigned int _ulSequenceDuration(const int16_t __i16activeSequence);
    static long unsigned int _ulBarDuration(const int16_t __i16activeSequence);

    void _initSequence(const char cName[], const uint16_t _ui16BaseBeatInBpm, const uint16_t _ui16BaseNoteForBeat, const uint16_t _ui16BaseNotesCountPerBar, const short int iNumberOfBeatsInSequence, const short int _iAssociatedBars[]);

    char _cName[7];  // array of character to hold the name of each sequences
    uint16_t ui16BaseBeatInBpm; // tempo in beats per minute
    uint16_t ui16BaseNoteForBeat; // the 4 in 2/4, for instance
    uint16_t ui16BaseNotesCountPerBar; // the 2 in 2/4, for instance
    short int _barCountInSequence; // number of tempos required to execute one full sequence
    short int _iAssociatedBars[4];  // array containing the state of each laser at each tempo


    static void _tcbPlaySequenceInLoop();
    static bool _oetcbPlaySequenceInLoop();
    static void _odtcbPlaySequenceInLoop();

    static Task _tPlaySequence;
    static bool _oetcbPlaySequence();
    static void _tcbPlaySequence();
    static void _odtcbPlaySequence();
};

#endif
