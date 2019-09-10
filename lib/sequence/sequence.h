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
      const std::array<bar, 8> & __barsArray
    );
    // copy constructor
    sequence( const sequence& __sequence);
    // assignement operator
    sequence& operator=(const sequence& __sequence);

    /** public static array of sequences */
    static std::array<sequence, 7> sequences;

    /** sequences initializer */
    static void initSequences(); // initializer of the array of sequences

    /** static setter */
    static void setActiveSequence(const int16_t __i16ActiveSequence);

    /** getters */
    const uint16_t ui16GetBarCountInSequence() const;
    const beat & getAssociatedBeat() const;
    const std::array<bar, 8> & getBarsArray() const;

    /** Task - sequence players */
    void playSequenceStandAlone(beat const & __beat, const uint16_t __ui16_associated_sequence_idx_number);
    void playSequenceInBoxState(const uint16_t __ui16_associated_sequence_idx_number);
    static Task tPlaySequenceInLoop;
    static Task tPlaySequence;

    /** interface to mesh */
    static void (*sendCurrentSequence)(const int16_t __i16ActiveSequence);
    static const int16_t getCurrentSequence();



  private:
    // static private constants to set stuffs
    static const int16_t _i16_sequence_count;
    static std::array<bar, 8> _emptyBarsArray;

    // static variable: currently played active sequence
    static int16_t _i16ActiveSequence;

    // instance getters
    uint32_t const _ui32GetSequenceDuration() const;

    // instance properties
    beat _beat;
    std::array<bar, 8> _barsArray;

    // tasks callbacks
    static bool _oetcbPlaySequenceInLoop();
    static void _tcbPlaySequenceInLoop();
    static void _odtcbPlaySequenceInLoop();

    static bool _oetcbPlaySequence();
    static void _tcbPlaySequence();
    static void _odtcbPlaySequence();
};

#endif
