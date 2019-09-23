/*
  sequence.h - sequences are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

*/
#include "Arduino.h"
#include <painlessMesh.h>
#include <mns.h>
#include <ControlerBox.h>
#include <bar.h>

#ifndef sequence_h
#define sequence_h

class sequence
{
  public:
    /** constructors */
    // default constructor
    sequence();
    // parameterized constructor
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
    static bars globalBars;

    /** sequences initializer */
    static void initSequences(); // initializer of the array of sequences

    /** setters */
    void setActive(const int16_t __i16_active_sequence_id=-1);

    /** getters */
    static sequence & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id);
    uint16_t const ui16GetBarCountInSequence() const;
    uint32_t const ui32GetSequenceDuration() const;
    beat const & getAssociatedBeat() const;
    std::array<bar, 8> const & getBarsArray() const;

    /** Task - sequence players */
    void playSequenceStandAlone(const int16_t __i16_sequence_id, beat const & __beat);
    void playSequenceInBoxState(const int16_t __i16_sequence_id);
    static Task tPlaySequenceInLoop;
    static Task tPlaySequence;

    /** interface to mesh */
    static void (*sendCurrentSequence)(const int16_t __i16_active_sequence_id);
    static const int16_t getCurrentSequence();

    // static variable: currently played active sequence
    static sequence emptySequence;
    static sequence & activeSequence;

  private:
    // static private constants to set stuffs
    static const int16_t _i16_sequence_count;

    // static variable: currently played active sequence
    static int16_t _i16ActiveSequenceNb;

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

#ifndef sequences_h
#define sequences_h

class sequences
{

  public:

    /** constructors */
    sequences(); // default constructor
    // sequences(
    // );
    // copy constructor
    sequences(const sequences & __sequences);
    // assignement operator
    sequences& operator=(const sequences & __sequences);

    /** public properties */
    std::array<sequence, 7> sequencesArray; // <-- TODO: make it private at some point
    bars globalBars;

    /** setters */
    void setActive(sequence & __sequence);

    /** getters */
    sequence & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id);

    /** Task - sequence players */
    void playSequenceStandAlone(const int16_t __i16_sequence_id=-1, beat const & __beat=activeBeat);
    void playSequenceInBoxState(const int16_t __i16_sequence_id);
    Task tPlaySequenceInLoop;
    Task tPlaySequence;
    Task & tPlayBar;
    Task & tPlayNote;

    /** interface to mesh */
    void (*sendCurrentSequence)(const int16_t __i16_active_sequence_id);
    const int16_t getCurrentSequence();

    // static variable: currently played active sequence
    static sequence emptySequence;
    static sequence & activeSequence;

  private:
    // private constants to set stuffs
    const int16_t _i16_sequence_count;

    // variable: currently played active sequence
    int16_t _i16ActiveSequenceNb;

    // tasks callbacks
    bool _oetcbPlaySequenceInLoop();
    void _tcbPlaySequenceInLoop();
    void _odtcbPlaySequenceInLoop();

    bool _oetcbPlaySequence();
    void _tcbPlaySequence();
    void _odtcbPlaySequence();
};

#endif
