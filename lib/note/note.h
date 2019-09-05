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
    /** sender to mesh */
    static void (*sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note);

    /** default empty constructor */
    note();
    /** main parameterized constructor */
    note(
      const uint16_t __ui16_tone,
      const uint16_t __ui16_note
    );
    // copy constructor
    note( const note& );
    // assignement operator
    note& operator=(const note& );

    /** setters */
    static void setActiveNoteFromNote(const note & _target_note);

    /** getters */
    static const note &getCurrentNote();
    uint16_t getTone() const;
    uint16_t getNote() const;
    unsigned long ulGetNoteDurationInMs() const;

    /** player */
    static void playNoteStandAlone(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, note const & __target_note);
    static Task tPlayNote;

  private:
    friend class bar;

    // private static variables
    static note &_activeNote;
    static uint16_t _ui16BaseNoteForBeat; 
    static uint16_t _ui16BaseBeatInBpm;

    /** player accessories */
    static bool _oetcbPlayNote();
    static void _odtcbPlayNote();

    /** setters */
    // instance setter
    void _setTone(const uint16_t __ui16_target_tone);

    // static setter
    static void _setTPlayNote(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, uint16_t const __ui16_iterations);
    static void _setTimeParams(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm);

    // private instance properties
    uint16_t _ui16Tone;
    uint16_t _ui16Note;
    tone & _tone;
};

#endif
