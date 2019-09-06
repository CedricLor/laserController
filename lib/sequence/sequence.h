/*
  sequence.h - sequences are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

*/
#ifndef sequence_h
#define sequence_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <mns.h>
#include <ControlerBox.h>
#include <bar.h>

class sequence
{
  public:
    sequence(); // default constructor
    sequence(
      const uint16_t __ui16BaseBeatInBpm, 
      const uint16_t __ui16BaseNoteForBeat, 
      const uint16_t __ui16BaseNotesCountPerBar, 
      const int16_t __i16BarCountInSequence, 
      int16_t *__i16AssociatedBars
    );

    static sequence sequences[];
    static void initSequences(); // initializer of the array of sequences

    static void setActiveSequence(const int16_t __i16ActiveSequence);

    static Task tPlaySequenceInLoop;
    static Task tPlaySequence;

    /** interface to mesh */
    static void (*sendCurrentSequence)(const int16_t __i16ActiveSequence);
    static int16_t getCurrentSequence();

  private:
    // static private constants to set stuffs
    static const int16_t _i16_sequence_count;

    // static variable: currently played active sequence
    static int16_t _i16ActiveSequence;

    // helper functions
    static long unsigned int _ulSequenceDuration();
    static long unsigned int _ulBarDuration();

    // instance properties
    uint16_t _ui16BaseBeatInBpm; // tempo in beats per minute
    uint16_t _ui16BaseNoteForBeat; // the 4 in 2/4, for instance
    uint16_t _ui16BaseNotesCountPerBar; // the 2 in 2/4, for instance
    int16_t _i16BarCountInSequence; // number of tempos required to execute one full sequence
    int16_t *_i16AssociatedBars;  // array containing the state of each laser at each tempo


    static bool _oetcbPlaySequenceInLoop();
    static void _tcbPlaySequenceInLoop();
    static void _odtcbPlaySequenceInLoop();

    static bool _oetcbPlaySequence();
    static void _tcbPlaySequence();
    static void _odtcbPlaySequence();
};

#endif
