/*
  note.h - notes are tones for a given length (in base beat)
  Created by Cedric Lor, June 28, 2019.

*/

#ifndef note_h
#define note_h

#include "Arduino.h"
#include <mns.h>
#include <beat.h>
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
    void setActive();

    /** getters */
    static const note &getCurrentNote();
    uint16_t const getTone() const;
    uint16_t const getNote() const;
    uint16_t const ui16GetNoteDurationInMs() const;

    /** player */
    void playNoteStandAlone(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm);
    static Task tPlayNote;

  private:
    friend class bar;

    // private static variables
    static note &_activeNote;

    /** player callbacks */
    static bool _oetcbPlayNote();
    static void _odtcbPlayNote();

    /** setters */
    // instance setter
    void _setTone(const uint16_t __ui16_target_tone);

    // static setter
    static void _setTPlayNote(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, uint16_t const __ui16_iterations);

    // private instance properties
    uint16_t _ui16Tone;
    uint16_t _ui16Note;
    tone & _tone;
};

#endif
