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
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--tone.cpp (called to play some member functions)
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h


*/
#ifndef sequence_h
#define sequence_h

#include "Arduino.h"
#include "global.h"

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

    void _initSequence(const char cName[], const unsigned long ulTempo, const short int iNumberOfBeatsInSequence, const short int iAssociatedBarsSequence[]);

    char _cName[7];  // array of character to hold the name of each sequences
    unsigned long _ulTempo; // tempo at which the task executing the sequence will change bar, in milliseconds
    short int _iNumberOfBarsInSequence; // number of tempos required to execute one full sequence
    short int _iAssociatedBarsSequence[4];  // array containing the state of each laser at each tempo


    static void _tcbPlaySequenceInLoop();
    static bool _oetcbPlaySequenceInLoop();
    static void _odtcbPlaySequenceInLoop();

    static void _playSequence();
    static Task _tPlaySequence;
    static void _tcbPlaySequence();
};

#endif
