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
    uint16_t const ui16GetBarCountInLaserSequence() const;
    beat const & getAssociatedBeat() const;
    int16_t const i16GetFirstBarIndexNumber() const;
    int16_t const i16GetBarIndexNumber(const uint16_t ui16BarIxNumbInLaserSequence) const;

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
    laserSequences(void (*_sendCurrentLaserSequence)(const int16_t __i16_current_laser_sequence_id)=nullptr);

    /** interface to mesh */
    void (*sendCurrentLaserSequence)(const int16_t __i16_active_laser_sequence_id);

    /** public properties */
    bars _bars;  // <-- TODO: make it private at some point

    /** Variables to read laserSequences from SPIFSS */
    uint16_t ui16IxNumbOfSequenceToPreload;
    laserSequence nextLaserSequence;
    char sequenceFileName[25];
    
    /** setters */
    uint16_t setActive(const uint16_t __target_laser_sequence_ix_numb);
    void disableAndResetTPlayLaserSequence();
    void setStopCallbackForTPlayLaserSequence();

    /** getters */
    laserSequence const & getActiveLaserSequence() const;

    /** Task tPlayLaserSequence - laserSequence players */
    uint16_t const playSequence(const uint16_t __target_laser_sequence_ix_numb);
    Task tPlayLaserSequence;

    /** Task tPreloadNextLaserSequence - preload next laserSequence from SPIFFS */
    Task tPreloadNextLaserSequence;

  private:
    // variables
    laserSequence _defaultLaserSequence;
    laserSequence & _activeLaserSequence = _defaultLaserSequence;

    /** setters */
    uint16_t _setActive(const laserSequence & __active_laser_sequence);

    /** Task tPlayLaserSequence - callbacks and methods */
    uint16_t const _playLaserSequence(const laserSequence & __target_laser_sequence);
    bool _oetcbPlayLaserSequence();
    void _tcbPlayLaserSequence();
    void _odtcbPlayLaserSequence();
    void _odtcbPlayLaserSequenceStop();

    /** Task tPreloadNextLaserSequence - callbacks and methods */
    void _preloadNextLaserSequence(const uint16_t _ui16IxNumbOfSequenceToPreload);
    void _tcbPreloadNextLaserSequence();
    void _preloadNextLaserSequenceFromJSON(const JsonObject& _joSequence);
    std::array<int16_t, 8> _parseJsonBarsArray(const JsonArray& _jaBarsArray);
};

#endif
