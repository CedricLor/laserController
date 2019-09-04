/*
  note.h - notes are tones for a given length (in base beat)
  Created by Cedric Lor, June 28, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
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
  |  |  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |  |  |--global.h


*/

#ifndef note_h
#define note_h

#include "Arduino.h"
#include <mns.h>
#include <tone.h>

class note
{
  public:
    note(); // default constructor
    note(
      const uint16_t __ui16_tone,
      const uint16_t __ui16_duration
    ); // default constructor


    static Task tPlayNote;

    /** interface to mesh */
    static void setActiveTone(const uint16_t _ui16_active_tone);
  private:

    static uint16_t _ui16ActiveTone;
    static uint16_t _ui16ActiveNote;

    static bool _oetcbPlayNote();
    static void _odtcbPlayNote();

    uint16_t _ui16Tone;
    uint16_t _ui16Note;
};

#endif
