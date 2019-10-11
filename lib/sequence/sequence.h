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
    sequence();
    sequence(const beat & __beat, std::array<bar, 8> & __barsArray, int16_t __i16IndexNumber=-2);
    sequence(const sequence& __sequence);
    sequence& operator=(const sequence& __sequence);

    /** getters */
    uint16_t const ui16GetBarCountInSequence() const;
    uint32_t const ui32GetSequenceDuration() const;
    beat const & getAssociatedBeat() const;
    std::array<bar, 8> const & getBarsArray() const;

    /** public instance properties */
    int16_t i16IndexNumber;

  private:

    // instance properties
    beat _beat;
    std::array<bar, 8> _barsArray;
};

#endif

#ifndef sequences_h
#define sequences_h

class sequences
{

  public:

    /** constructors */
    sequences(sequence & __activeSequence, void (*_sendCurrentSequence)(const int16_t __i16_current_sequence_id)=nullptr);
    sequences(const sequences& __sequences);
    sequences & operator=(const sequences& __sequences);

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
    sequence const & getCurrentSequence() const;
    sequence const & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id) const;

    /** Task - sequence players */
    uint16_t const playSequenceStandAlone(const sequence & __target_sequence);
    bool playSequenceInBoxState(const sequence & __target_sequence);
    Task tPlaySequenceInLoop;
    Task tPlaySequence;
    Task & tPlayBar;
    Task & tPlayNote;

  private:
    // variables
    sequence & _activeSequence;

    // tasks callbacks
    bool _oetcbPlaySequenceInLoop();
    void _tcbPlaySequenceInLoop();
    void _odtcbPlaySequenceInLoop();

    bool _oetcbPlaySequence();
    void _tcbPlaySequence();
    void _odtcbPlaySequence();
};

#endif
