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
    sequence(const beat & __beat, std::array<uint16_t, 8> & __ui16BarsArray, int16_t __i16IndexNumber=-2);
    sequence(const sequence& __sequence);
    sequence& operator=(const sequence& __sequence);

    /** getters */
    uint16_t const ui16GetBarCountInSequence() const;
    uint32_t const ui32GetSequenceDuration(beat const & __beat) const;
    beat const & getAssociatedBeat() const;
    std::array<bar, 8> const & getBarsArray() const;
    std::array<uint16_t, 8> const & getBarsIndexNumbersArray() const;
    int16_t const ui16GetFirstBarIndexNumber() const;
    int16_t const ui16GetBarIndexNumber(const uint16_t ui16BarIxNumbInSequence) const;

    /** public instance properties */
    int16_t i16IndexNumber;

  private:

    // instance properties
    beat _beat;
    std::array<uint16_t, 8> _ui16BarsArray;
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
    std::array<sequence, 7> sequencesArray; // <-- TODO: make it private at some point
    bars _bars;  // <-- TODO: make it private at some point

    /** Variables to read sequences from SPIFSS */
    uint16_t ui16IxNumbOfSequenceToPreload;
    sequence nextSequence;
    char sequenceFileName[20];
    
    /** setters */
    uint16_t setActive(const sequence & __ActiveSequence);
    void disableAndResetPlaySequenceTasks();

    /** getters */
    int16_t const i16GetCurrentSequenceId() const; // returns the current sequence i16IndexNumber
    sequence const & getCurrentSequence() const;
    sequence const & getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id) const;

    /** Task - sequence players */
    uint16_t const playSequence(const sequence & __target_sequence, Task & __sequenceTask);
    Task tPlaySequenceInLoop;
    Task tPlaySequence;

    /** Task - preload next sequence from SPIFFS */
    Task tPreloadNextSequence;

    void _preloadNextSequence(uint16_t _ui16IxNumbOfSequenceToPreload);
    void _tcbPreloadNextSequence();
    void _preloadNextSequenceFromJSON(const JsonObject& _joSequence);
    std::array<uint16_t, 8> const _parseJsonBarsArray(const JsonArray& _jaBarsArray);

  private:
    // variables
    sequence _defaultSequence;
    sequence & _activeSequence = _defaultSequence;

    // tasks callbacks
    bool _oetcbPlaySequenceInLoop();
    void _tcbPlaySequenceInLoop();
    void _odtcbPlaySequenceInLoop();

    bool _oetcbPlaySequence();
    void _tcbPlaySequence();
    void _odtcbPlaySequence();
};

#endif
