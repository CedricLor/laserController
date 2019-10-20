/*
  sequence.h - sequences are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

*/
#include "Arduino.h"
#include <painlessMesh.h>
#include <mySpiffs.h>
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
    sequence(const beat & __beat, std::array<int16_t, 8> & __i16BarsArray, int16_t __i16barsArraySize, int16_t __i16IndexNumber=-2);
    sequence(const sequence& __sequence);
    sequence& operator=(const sequence& __sequence);

    /** getters */
    uint16_t const ui16GetBarCountInSequence() const;
    uint32_t const ui32GetSequenceDuration(beat const & __beat) const;
    beat const & getAssociatedBeat() const;
    int16_t const i16GetFirstBarIndexNumber() const;
    int16_t const i16GetBarIndexNumber(const uint16_t ui16BarIxNumbInSequence) const;

    /** public instance properties */
    int16_t i16IndexNumber;

  private:

    // instance properties
    beat _beat;
    std::array<int16_t, 8> _i16BarIxNumbsArray;
    int16_t _i16barsArraySize;
};


class sequences
{

  public:
    /** constructors */
    sequences(void (*_sendCurrentSequence)(const int16_t __i16_current_sequence_id)=nullptr);
    // sequences(const sequences& __sequences);
    // sequences & operator=(const sequences& __sequences);

    /** interface to mesh */
    void (*sendCurrentSequence)(const int16_t __i16_active_sequence_id);

    /** public properties */
    bars _bars;  // <-- TODO: make it private at some point

    /** Variables to read sequences from SPIFSS */
    uint16_t ui16IxNumbOfSequenceToPreload;
    sequence nextSequence;
    char sequenceFileName[20];
    
    /** setters */
    uint16_t setActive(const uint16_t __target_sequence_ix_numb);
    void disableAndResetTPlaySequence();
    void setStopCallbackForTPlaySequence();

    /** getters */
    int16_t const i16GetCurrentSequenceId() const; // returns the current sequence i16IndexNumber
    sequence const & getCurrentSequence() const;

    /** Task tPlaySequence - sequence players */
    uint16_t const playSequence(const uint16_t __target_sequence_ix_numb);
    Task tPlaySequence;

    /** Task tPreloadNextSequence - preload next sequence from SPIFFS */
    Task tPreloadNextSequence;

  private:
    // variables
    sequence _defaultSequence;
    sequence & _activeSequence = _defaultSequence;

    /** setters */
    uint16_t _setActive(const sequence & __active_sequence);

    /** Task tPlaySequence - callbacks and methods */
    uint16_t const _playSequence(const sequence & __target_sequence);
    bool _oetcbPlaySequence();
    void _tcbPlaySequence();
    void _odtcbPlaySequence();
    void _odtcbPlaySequenceStop();

    /** Task tPreloadNextSequence - callbacks and methods */
    void _preloadNextSequence(const uint16_t _ui16IxNumbOfSequenceToPreload);
    void _tcbPreloadNextSequence();
    void _preloadNextSequenceFromJSON(const JsonObject& _joSequence);
    std::array<int16_t, 8> _parseJsonBarsArray(const JsonArray& _jaBarsArray);

};

#endif
