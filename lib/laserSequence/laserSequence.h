/*
  laserSequence.h - laserSequences are precoded sequences of laserNotes
  Created by Cedric Lor, June 4, 2019.

*/
#include "Arduino.h"
#include <painlessMesh.h>
#include <bar.h>

#ifndef laserSequence_h
#define laserSequence_h

class laserSequence
{
  friend class laserSequences;
  friend class test;
  public:
    /** constructors */
    laserSequence();
    laserSequence(const beat & __beat, std::array<int16_t, 8> & __i16BarsArray, int16_t __i16barsArraySize, int16_t __i16IndexNumber=-2);
    laserSequence(const laserSequence& __laserSequence);
    laserSequence& operator=(const laserSequence& __laserSequence);

    /** getters */
    uint16_t const ui16GetBarCountInSequence() const;
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


class laserSequences
{
  friend class test;
  public:
    /** constructors */
    laserSequences(void (*_sendCurrentSequence)(const int16_t __i16_current_laser_sequence_id)=nullptr);

    /** interface to mesh */
    void (*sendCurrentSequence)(const int16_t __i16_active_laser_sequence_id);

    /** public properties */
    bars _bars;  // <-- TODO: make it private at some point

    /** Variables to read laserSequences from SPIFSS */
    uint16_t ui16IxNumbOfSequenceToPreload;
    laserSequence nextSequence;
    char sequenceFileName[25];
    
    /** setters */
    uint16_t setActive(const uint16_t __target_laser_sequence_ix_numb);
    void disableAndResetTPlaySequence();
    void setStopCallbackForTPlaySequence();

    /** getters */
    laserSequence const & getActiveSequence() const;

    /** Task tPlaySequence - laserSequence players */
    uint16_t const playSequence(const uint16_t __target_laser_sequence_ix_numb);
    Task tPlaySequence;

    /** Task tPreloadNextSequence - preload next laserSequence from SPIFFS */
    Task tPreloadNextSequence;

  private:
    // variables
    laserSequence _defaultSequence;
    laserSequence & _activeSequence = _defaultSequence;

    /** setters */
    uint16_t _setActive(const laserSequence & __active_laser_sequence);

    /** Task tPlaySequence - callbacks and methods */
    uint16_t const _playSequence(const laserSequence & __target_laser_sequence);
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
