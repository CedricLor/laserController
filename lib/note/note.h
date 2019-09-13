/*
  note.h - notes are tones for a given length (in base beat)
  Created by Cedric Lor, June 28, 2019.

*/

#include "Arduino.h"
#include <mns.h>
#include <beat.h>
#include <tone.h>

#ifndef note_h
#define note_h

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
    note(
      const uint16_t __ui16_note,
      tone & __tone
    );
    note(const note & __note);
    note& operator=(const note& __note);

    /** getters */
    uint16_t const getTone() const;
    uint16_t const getNote() const;
    uint16_t const ui16GetNoteDurationInMs() const;

  private:
    friend class notes;
    friend class bar;

    /** setters */
    // instance setter
    tone & _setTone(const tone & _target_tone);
    uint16_t _ui16ValidNote();

    // private instance properties
    uint16_t _ui16Tone;
    uint16_t _ui16Note;
    tone & _tone;
};

#endif

#ifndef notes_h
#define notes_h

class notes
{
  public:
    /** default empty constructor */
    notes();
    // /** main parameterized constructor */
    // notes(note & __activeNote);

    /** setter */
    void setActive(const note & __activeNote);
    void resetTPlayNoteToPlayNotesInBar();
    void setTPlayNote();

    /** getter */
    const note &getCurrentNote() const;

    /** player */
    void playNoteStandAlone(const note & __note, beat const & __beat=beat::_activeBeat);
    void playNoteInBar(const note & __note);

    /** member */
    Task tPlayNote;

    /** sender to mesh */
    void (*sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note);

  private:
    friend class test;
    friend class bar;

    /** private members */
    note & _activeNote;
    tones _tones;

    /** player callbacks */
    bool _oetcbPlayNote();
    void _odtcbPlayNote();
};
#endif
