/*
  beat.h - beat handles the beat timing at which notes, bars and sequences shall be played
  Created by Cedric Lor, September 6, 2019.
*/
#ifndef beat_h
#define beat_h

#include "Arduino.h"
#include <global.h>

class beat
{
  public:
    /** sender to mesh */
    static void (*sendCurrentBeat)(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat);

    /** default empty constructor */
    beat();
    /** main parameterized constructor */
    beat(
      const uint16_t __ui16_base_beat_in_bpm, 
      const uint16_t __ui16_base_note_for_beat
    ); 
    // // copy constructor
    // beat( const beat& );
    // // assignement operator
    // beat& operator=(const beat& );

    // objects array initializer
    static void initBeats(); // initializer of the array of beats

    /** setters */
    void setActive();

    /** getters */
    static beat const & getCurrentBeat(); // return a ref to the _activeBeat instance
    uint16_t const getBaseBeatInBpm() const;
    uint16_t const getBaseNoteForBeat() const;
    uint16_t const ui16GetBaseNoteDurationInMs() const;

  private:
    friend class sequence;
    friend class bar;
    friend class note;

    // static properties
    static std::array<beat, 7> _beats;
    static beat &_activeBeat;

    // private instance properties
    uint16_t _ui16BaseBeatInBpm; // basic time unit for the smallest note to be played (_iBaseNoteForBeat)
    uint16_t _ui16BaseNoteForBeat; // base note for the beat
};

#endif
