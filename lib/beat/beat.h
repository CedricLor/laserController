/*
  beat.h - beat handles the beat timing at which notes, bars and sequences shall be played
  Created by Cedric Lor, September 6, 2019.
*/

#include "Arduino.h"
#include <global.h>

#ifndef beat_h
#define beat_h

class beat
{
  friend class sequence;
  friend class bar;
  friend class bars;
  friend class notes;
  friend class note;
  friend class test;

  public:
    /** default empty constructor */
    beat();
    /** main parameterized constructor */
    beat(
      const uint16_t __ui16_base_beat_in_bpm, 
      const uint16_t __ui16_base_note_for_beat,
      void (*_sendCurrentBeat)(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat)=nullptr
    ); 
    // copy constructor
    beat(const beat & __beat);
    // assignement operator
    beat& operator=(const beat & __beat);
    // move constructor
    beat(beat&& __beat);
    // move assignement operator
    beat & operator=(beat&& __beat);

    // objects array initializer
    // static void initBeats(); // initializer of the array of beats
    /** sender to mesh */
    void (*sendCurrentBeat)(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat);

    /** getters */
    uint16_t const getBaseBeatInBpm() const;
    uint16_t const getBaseNoteForBeat() const;
    uint16_t const ui16GetBaseNoteDurationInMs() const;

  private:

    // static properties
    // static std::array<beat, 7> _beats;

    // private instance properties
    uint16_t _ui16BaseBeatInBpm; // basic time unit for the smallest note to be played (_iBaseNoteForBeat)
    uint16_t _ui16BaseNoteForBeat; // base note for the beat
};

extern beat activeBeat;

#endif
