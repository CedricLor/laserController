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
    /** constructors */
    sequence(); // default constructor
    sequence(
      const beat & __beat,
      const uint16_t & __ui16BaseNotesCountPerBar, 
      const int16_t & __i16BarCountInSequence, 
      const std::array<int16_t, 8> & __i16AssociatedBars
    );
    // copy constructor
    sequence( const sequence& __sequence);
    // assignement operator
    sequence& operator=(const sequence& __sequence);

    /** public static array of sequences */
    static std::array<sequence, 7> sequences;

    /** sequences initializer */
    static void initSequences(); // initializer of the array of sequences

    /** getters */
    const uint16_t ui16GetBaseNotesCountPerBar() const;
    const int16_t i16GetBarCountInSequence() const;
    const beat & getAssociatedBeat() const;
    const std::array<int16_t, 8> getAssociatedBars() const;

    /** static setter */
    static void setActiveSequence(const int16_t __i16ActiveSequence);

    /** Task - sequence players */
    static Task tPlaySequenceInLoop;
    static Task tPlaySequence;

    /** interface to mesh */
    static void (*sendCurrentSequence)(const int16_t __i16ActiveSequence);
    static const int16_t getCurrentSequence();



  private:
    // static private constants to set stuffs
    static const int16_t _i16_sequence_count;

    // static variable: currently played active sequence
    static int16_t _i16ActiveSequence;

    // helper functions
    static long unsigned int _ulSequenceDuration();
    static long unsigned int _ulBarDuration();

    // instance properties
    beat _beat;
    uint16_t _ui16BaseNotesCountPerBar; // the 2 in 2/4, for instance
    int16_t _i16BarCountInSequence; // number of tempos required to execute one full sequence
    std::array<int16_t, 8> _i16AssociatedBars;  // array containing the state of each laser at each tempo


    static bool _oetcbPlaySequenceInLoop();
    static void _tcbPlaySequenceInLoop();
    static void _odtcbPlaySequenceInLoop();

    static bool _oetcbPlaySequence();
    static void _tcbPlaySequence();
    static void _odtcbPlaySequence();
};

#endif
