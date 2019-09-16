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
    static tones globalTones;

    /** default empty constructor */
    note();
    /** main parameterized constructor */
    note(
      const uint16_t __ui16_tone,
      const uint16_t __ui16_note
    );
    note(const note & __note);
    note& operator=(const note& __note);

    /** getters */
    uint16_t const getToneNumber() const;
    uint16_t const getNote() const;
    uint16_t const ui16GetNoteDurationInMs() const;

  private:
    friend class notes;
    friend class bar;
    friend class test;

    /** setters */
    // instance setter
    uint16_t _setTone(const tone & _target_tone);
    uint16_t _ui16ValidNote();

    /** player */
    const int16_t _playTone() const;

    // private instance properties
    uint16_t _ui16Tone;
    uint16_t _ui16Note;
};

#endif

#ifndef notes_h
#define notes_h

class notes
{
  public:
    /** default constructor */
    notes(
      void (*sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note)=nullptr
    );
    notes(const notes & __notes);
    notes & operator=(const notes & __notes);
    notes(notes&& __notes);
    notes & operator=(notes&& __notes);

    /** sender to mesh */
    void (*sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note);

    /** setters */
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
