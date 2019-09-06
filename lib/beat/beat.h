/*
  beat.h - beat handles the beat timing at which notes, bars and sequences shall be played
  Created by Cedric Lor, September 6, 2019.
*/
#ifndef bar_h
#define bar_h

#include "Arduino.h"
#include <mns.h>
#include <note.h>

class bar
{
  public:
    /** sender to mesh */
    static void (*sendCurrentBar)(const uint16_t __ui16_active_bar);

    /** default empty constructor */
    bar();
    /** main parameterized constructor */
    bar(
      const uint16_t __ui16_base_beat_in_bpm, 
      const uint16_t __ui16_base_note_for_beat, 
      const uint16_t __ui16_base_notes_count_in_bar,
      const uint16_t __ui16_notes_count_in_bar,
      std::array<note, 16> __notesArray
    ); 
    // // copy constructor
    // bar( const bar& );
    // // assignement operator
    // bar& operator=(const bar& );

    // objects array initializer
    static void initBars(); // initializer of the array of bars

    /** setters */
    static void setActiveBar(const uint16_t __ui16_active_bar);

    /** getters */
    static int16_t getCurrentBar(); // returns _ui16ActiveBar

    /** player */
    static Task tPlayBar;

  private:
    friend class sequence;

    // static properties
    static std::array<bar, 7> _bars;
    static uint16_t _ui16ActiveBar;

    /** player callbackss */
    static void _tcbPlayBar();
    static bool _oetcbPlayBar();
    static void _odtcbPlayBar();

    // private instance properties
    uint16_t _ui16BaseBeatInBpm; // basic time unit for the smallest note to be played (_iBaseNoteForBeat)
    uint16_t _ui16BaseNoteForBeat; // base note for the beat
    uint16_t _ui16BaseNotesCountInBar;
    uint16_t _ui16NotesCountInBar; // number of actual notes in the bar
    std::array<note, 16> _notesArray;
};

#endif
