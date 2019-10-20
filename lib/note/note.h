/*
  note.h - laserNotes are laserTones played for a given length (expressed in base beat)
  Created by Cedric Lor, June 28, 2019.
*/

#include "Arduino.h"
#include <beat.h>
#include <laserTone.h>

#ifndef note_h
#define note_h

class note
{
  friend class laserNotes;
  friend class test;
  public:

    /** default empty constructor */
    note();
    /** main parameterized constructor */
    note(
      const uint16_t __ui16_laser_tone,
      const uint16_t __ui16_note
    );
    note(const note & __note);
    note& operator=(const note& __note);
    note(note&& __note);
    note & operator=(note&& __note);

    /** getters */
    uint16_t const getLaserToneNumber() const;
    uint16_t const getNote() const;
    uint16_t const ui16GetNoteDurationInMs(const beat & _beat) const;

  private:
    /** setters */
    // instance setter
    uint16_t _setLaserTone(const laserTone & _target_laser_tone);
    uint16_t _ui16ValidNote();

    /** player */
    const int16_t _playLaserTone(laserTones const & __laser_tones) const;

    // private instance properties
    uint16_t _ui16LaserTone;
    uint16_t _ui16Note;
};


#endif

#ifndef laserNotes_h
#define laserNotes_h

class laserNotes
{
  friend class bars;
  friend class test;
  public:
    /** default constructor */
    laserNotes(
      void (*sendCurrentNote)(const uint16_t __ui16_current_laser_tone, const uint16_t __ui16_current_note)=nullptr
    );
    laserNotes(const laserNotes & __laserNotes);
    laserNotes & operator=(const laserNotes & __laserNotes);
    laserNotes(laserNotes&& __laserNotes);
    laserNotes & operator=(laserNotes&& __laserNotes);

    /** sender to mesh */
    void (*sendCurrentNote)(const uint16_t __ui16_current_laser_tone, const uint16_t __ui16_current_note);

    /** setters */
    void setActive(const note & __activeNote);

    /** getter */
    const note &getCurrentNote() const;

    /** player */
    bool const playNote(const note & __note, const beat & __beat);

    /** member */
    Task tPlayNote;

  private:
    /** setters */
    void _disableAndResetTPlayNote();

    /** private members */
    note _defaultNote;
    note & _activeNote = _defaultNote;
    laserTones _laserTones;

    /** player callbacks */
    bool _oetcbPlayNote();
    void _odtcbPlayNote();
};
#endif