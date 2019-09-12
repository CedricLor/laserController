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
    note( const note& __note);
    note& operator=(const note& __note);

    /** setters */
    void setActive();

    /** getters */
    static const note &getCurrentNote();
    uint16_t const getTone() const;
    uint16_t const getNote() const;
    uint16_t const ui16GetNoteDurationInMs() const;

    /** player */
    void playNoteStandAlone(beat const & __beat=beat::_activeBeat);
    void playNoteInBar();
    static Task tPlayNote;

  private:
    friend class notes;
    friend class bar;

    // private static variables
    static note &_activeNote;
    static tones _tones;

    /** player callbacks */
    static bool _oetcbPlayNote();
    static void _odtcbPlayNote();

    /** setters */
    // instance setter
    void _setTone(const uint16_t __ui16_target_tone);
    void _validNote();

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
    /** main parameterized constructor */
    notes(note & __activeNote, tones & __tones);

    /** setter */
    void resetTPlayNoteToPlayNotesInBar();

    /** getter */
    const note &getCurrentNote();

    /** player */
    void playNoteStandAlone(beat const & __beat=beat::_activeBeat);
    void playNoteInBar();

    /** member */
    Task tPlayNote;

    /** sender to mesh */
    void (*sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note);

  private:
    friend class test;
    friend class bar;

    /** private members */
    note &_activeNote;
    tones _tones;

    /** player callbacks */
    bool _oetcbPlayNote();
    void _odtcbPlayNote();
};
#endif
