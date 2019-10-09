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
  friend class sequences;
  public:
    /** constructors */
    // default constructor
    sequence();
    // parameterized constructor
    sequence(
      const beat & __beat,
      std::array<bar, 8> & __barsArray,
      int16_t __i16IndexNumber=-2
    );
    // copy constructor
    sequence(const sequence& __sequence);
    // assignement operator
    sequence& operator=(const sequence& __sequence);

    /** public static array of sequences */
    static std::array<sequence, 7> sequences; // <-- TODO: make it private at some point
    static bars globalBars;

    /** setters */
    void setActive(const int16_t __i16_active_sequence_id=-1);

    /** getters */
    static sequence & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id);
    uint16_t const ui16GetBarCountInSequence() const;
    uint32_t const ui32GetSequenceDuration() const;
    beat const & getAssociatedBeat() const;
    std::array<bar, 8> const & getBarsArray() const;

    /** public instance properties */
    int16_t i16IndexNumber;

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

extern sequence globalSequence;

#endif

#ifndef sequences_h
#define sequences_h

class sequences
{

  public:

    /** constructors */
    sequences(
      void (*_sendCurrentSequence)(const int16_t __i16_current_sequence_id)=nullptr
    );

    /** interface to mesh */
    void (*sendCurrentSequence)(const int16_t __i16_active_sequence_id);

    /** public properties */
    std::array<sequence, 7> sequencesArray; // <-- TODO: make it private at some point
    bars _bars;  // <-- TODO: make it private at some point

    /** setters */
    uint16_t setActive(const sequence & __ActiveSequence);
    void disableAndResetPlaySequenceTasks();

    /** getters */
    int16_t const i16GetCurrentSequenceId() const; // returns the current sequence i16IndexNumber
    sequence const & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id) const;

    /** Task - sequence players */
    uint16_t const playSequenceStandAlone(const sequence & __target_sequence);
    bool playSequenceInBoxState(const sequence & __target_sequence);
    Task tPlaySequenceInLoop;
    Task tPlaySequence;
    Task & tPlayBar;
    Task & tPlayNote;

    /** interface to mesh */
    const int16_t getCurrentSequence();

    // variable: currently played active sequence
    sequence emptySequence;
    sequence & activeSequence;

  private:
    // tasks callbacks
    bool _oetcbPlaySequenceInLoop();
    void _tcbPlaySequenceInLoop();
    void _odtcbPlaySequenceInLoop();

    bool _oetcbPlaySequence();
    void _tcbPlaySequence();
    void _odtcbPlaySequence();
};

extern sequences globalSequences;

#endif
