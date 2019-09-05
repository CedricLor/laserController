/*
  bar.h - bars are two beats to four beats, precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h



*/
#ifndef bar_h
#define bar_h

#include "Arduino.h"
#include <mns.h>
#include <note.h>

class bar
{
  public:
    bar(); // default empty constructor
    bar(
      const uint16_t __ui16_base_beat_in_bpm, 
      const uint16_t __ui16_base_note_for_beat, 
      const uint16_t __ui16_base_notes_count_in_bar,
      const uint16_t __ui16_notes_count_in_bar,
      std::array<note, 16> __notesArray
      // std::array<std::array<uint16_t, 2>, 16> __ui16NoteTone
    ); 
    // // copy constructor
    // bar( const bar& );
    // // assignement operator
    // bar& operator=(const bar& );

    // static
    static std::array<bar, 5> bars;
    static void initBars(); // initializer of the array of bars

    static void setActiveBar(const uint16_t __ui16_active_bar);

    // instance property
    uint16_t ui16BaseBeatInBpm; // basic time unit for the smallest note to be played (_iBaseNoteForBeat)
    uint16_t ui16BaseNoteForBeat; // base note for the beat
    uint16_t ui16BaseNotesCountInBar;

    static Task tPlayBar;

    /** interface to mesh */
    static void (*sendCurrentBar)(const uint16_t __ui16_active_bar);
    static int16_t getCurrentBar(); // returns _ui16ActiveBar

  private:
    static const uint16_t _ui16_bar_count; // number of bar instances to be created in the array of bars
    static uint16_t _ui16ActiveBar;

    static void _tcbPlayBar();
    static bool _oetcbPlayBar();
    static void _odtcbPlayBar();

    // instance properties
    uint16_t _ui16NotesCountInBar; // number of actual notes in the bar
    std::array<note, 16> _notesArray;
};

#endif
