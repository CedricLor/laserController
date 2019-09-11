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
      std::array<bar, 8> & __barsArray
    );
    // copy constructor
    sequence( const sequence& __sequence);
    // assignement operator
    sequence& operator=(const sequence& __sequence);

    /** public static array of sequences */
    static std::array<sequence, 7> sequences; // <-- TODO: make it private at some point

    /** sequences initializer */
    static void initSequences(); // initializer of the array of sequences

    /** setters */
    void setActive(const int16_t __i16_active_sequence_id=-1);

    /** getters */
    static sequence & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id);
    uint16_t const ui16GetBarCountInSequence() const;
    beat const & getAssociatedBeat() const;
    std::array<bar, 8> const & getBarsArray() const;

    /** Task - sequence players */
    void playSequenceStandAlone(beat const & __beat, const int16_t __i16_sequence_id=-1);
    void playSequenceInBoxState(const int16_t __i16_sequence_id);
    static Task tPlaySequenceInLoop;
    static Task tPlaySequence;

    /** interface to mesh */
    static void (*sendCurrentSequence)(const int16_t __i16_active_sequence_id);
    static const int16_t getCurrentSequence();



  private:
    // static private constants to set stuffs
    static const int16_t _i16_sequence_count;

    // static variable: currently played active sequence
    static sequence _emptySequence;
    static sequence & _activeSequence;
    static int16_t _i16ActiveSequenceNb;
    static std::array<bar, 8> _emptyBarsArray;

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
