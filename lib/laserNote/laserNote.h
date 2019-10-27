/*
  laserNote.h - laserNotes are laserTones played for a given length (expressed in base beat)
  Created by Cedric Lor, June 28, 2019.
*/

#ifndef laserNote_h
#define laserNote_h

#include "Arduino.h"
#include <beat.h>
#include <laserTone.h>

class laserNote
{
  friend class laserNotes;
  friend class test;
  public:

    /** default empty constructor */
    laserNote();
    /** main parameterized constructor */
    laserNote(
      const uint16_t __ui16_laser_tone,
      const uint16_t __ui16_note
    );
    laserNote(const laserNote & __laserNote);
    laserNote& operator=(const laserNote& __laserNote);
    laserNote(laserNote&& __laserNote);
    laserNote & operator=(laserNote&& __laserNote);

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



class laserNotes
{
  friend class bars;
  friend class test;
  public:
    /** default constructor */
    laserNotes(
      myMeshViews & __thisMeshViews
    );
    // laserNotes(const laserNotes & __laserNotes);
    // laserNotes & operator=(const laserNotes & __laserNotes);
    // laserNotes(laserNotes&& __laserNotes);
    // laserNotes & operator=(laserNotes&& __laserNotes);

    /** setters */
    void setActive(const laserNote & __activeLaserNote);

    /** getter */
    const laserNote &getCurrentNote() const;

    /** player */
    bool const playNote(const laserNote & __laserNote, const beat & __beat);

    /** member */
    Task tPlayNote;

  private:
    /** setters */
    void _disableAndResetTPlayNote();

    /** sender to mesh */
    myMeshViews & _thisMeshViews;

    /** private members */
    laserNote _defaultLaserNote;
    laserNote & _activeLaserNote = _defaultLaserNote;
    laserTones _laserTones;

    /** player callbacks */
    bool _oetcbPlayNote();
    void _odtcbPlayNote();
};
#endif