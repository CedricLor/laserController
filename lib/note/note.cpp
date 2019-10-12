/*
  note.cpp - notes are laserTones played for a given length (expressed in base beat)
  Created by Cedric Lor, June 28, 2019.

  * 1. ronde                   : 16 : 16 / 1
  * 2. blanche                 : 8  : 16 / 2
  * 3. noire + croche          : 6  : 16 / 4 + (1/2)*(16 / 4) : 16 modulo 3 = 1
  * 4. noire                   : 4  : 16 / 4
  * 6. croche + double croche  : 3  : 16 / 8 + (1/2)*(16 / 8) : 16 modulo 6 = 4
  * 8. croche                  : 2  : 16 / 8
  * 16. double croche          : 1  : 16 / 16

*/

#include "Arduino.h"
#include "note.h"



//****************************************************************//
// Note //********************************************************//
//****************************************************************//

///////////////////////////////////
// Constructors
///////////////////////////////////
// default
note::note() :
  _ui16LaserTone(0),
  _ui16Note(0)
{}

// parameterized
note::note(
  const uint16_t __ui16_laser_tone,
  const uint16_t __ui16_note
) :
  _ui16LaserTone(__ui16_laser_tone),
  _ui16Note(__ui16_note)
{ _ui16ValidNote(); }

// copy constructor
note::note(const note& __note) :   
  _ui16LaserTone(__note._ui16LaserTone),
  _ui16Note(__note._ui16Note)
{ _ui16ValidNote(); }

// assignement operator
note& note::operator=(const note& __note)
{
  if (&__note != this) {
    _ui16LaserTone = __note._ui16LaserTone;
    _ui16Note = __note._ui16Note;
    _ui16ValidNote();
  }
  return *this;
}

note::note(note&& __note):
  _ui16LaserTone(0),
  _ui16Note(0)
{
  *this = std::move(__note);
}

note & note::operator=(note&& __note) {
  if (this != &__note) {
    _ui16LaserTone = 0;
    _ui16Note = 0;

    _ui16LaserTone = __note._ui16LaserTone;
    _ui16Note = __note._ui16Note;

    __note._ui16LaserTone = 0;
    __note._ui16Note = 0;
  }
  return *this;
}





///////////////////////////////////
// Setters
///////////////////////////////////
/** uint16_t note::_setTone(const laserTone & _target_laser_tone): private instance setter method
 * 
 *  sets the instance reference member _laserTone (laserTone & _laserTone) to the passed-in reference-to 
 *  const laserTone. */
uint16_t note::_setLaserTone(const laserTone & _target_laser_tone) {
  _ui16LaserTone = _target_laser_tone.i16IndexNumber;
  return _ui16LaserTone;
}



/** note::_validNote(): private instance setter method
 * 
 *  changes passed-in notes to the valid notes. */
uint16_t note::_ui16ValidNote() {
  if (_ui16Note == 5) {
    _ui16Note = 6;
  }
  if ((_ui16Note > 6) && (_ui16Note < 11)) {
    _ui16Note = 8;
  }
  if (_ui16Note > 10) {
    _ui16Note = 16;
  }
  return _ui16Note;
}









///////////////////////////////////
// Getters
///////////////////////////////////
/** uint16_t note::getLaserToneNumber(): public instance getter method
 * 
 *  Returns the laserTone number of a note instance. */
uint16_t const note::getLaserToneNumber() const {
  return _ui16LaserTone;
}


/** uint16_t note::getNote(): public instance getter method
 *  
 *  Returns the note of a note instance. */
uint16_t const note::getNote() const {
  return _ui16Note;
}


/** uint16_t note::ui16GetNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration of a note instance in ms. */
uint16_t const note::ui16GetNoteDurationInMs(const beat & _beat) const {
  // Serial.println("note::ui16GetNoteDurationInMs(). starting.");
  // Serial.println(F("------------- DEBUG --------- note --------- DEBUG -------------"));
  // Serial.printf("note::ui16GetNoteDurationInMs(). _ui16Note == %u\n", _ui16Note);
  // Serial.printf("note::ui16GetNoteDurationInMs(). _beat.getBaseNoteForBeat() == %u\n", _beat.getBaseNoteForBeat());

  if ((_ui16Note == 0) || (_beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // const uint16_t __ui16baseNoteDuration = _beat.ui16GetBaseNoteDurationInMs();
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ui16baseNoteDuration == %u\n", __ui16baseNoteDuration);

  // const uint16_t __ui16BaseNoteForBeat = _beat.getBaseNoteForBeat();
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ui16BaseNoteForBeat == %u\n", __ui16BaseNoteForBeat);

  // const uint16_t __num = __ui16baseNoteDuration * __ui16BaseNoteForBeat;
  // Serial.printf("note::ui16GetNoteDurationInMs(). __num == %u\n", __num);

  // see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
  uint16_t __ui16DurationInMs =
    (uint16_t)ceil(
      ((_beat.getBaseNoteForBeat() * _beat.ui16GetBaseNoteDurationInMs()))
      / _ui16Note
    );
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ulDurationInM == %u\n", __ui16DurationInMs);

  if (__ui16DurationInMs > 30000) {
    return 30000;
  }
  // Serial.println("note::ui16GetNoteDurationInMs(). over.");
  return __ui16DurationInMs;
}









///////////////////////////////////
// Players
///////////////////////////////////
/** const int16_t note::_playTone() const */
const int16_t note::_playLaserTone(laserTones const & __laser_tones) const {
  return __laser_tones._playLaserTone(getLaserToneNumber());
}





//****************************************************************//
// Notes //*******************************************************//
//****************************************************************//

notes::notes(
  void (*_sendCurrentNote)(const uint16_t __ui16_current_laser_tone, const uint16_t __ui16_current_note)
):
  sendCurrentNote(_sendCurrentNote),
  _laserTones({})
{
  _disableAndResetTPlayNote();
}
// copy constructor
notes::notes(const notes & __notes):
  sendCurrentNote(__notes.sendCurrentNote),
  _activeNote(__notes._activeNote),
  _laserTones(__notes._laserTones)
{
  _disableAndResetTPlayNote();
}
// copy assignment operator
notes & notes::operator=(const notes & __notes) {
  if (&__notes != this) {
    sendCurrentNote = __notes.sendCurrentNote;
    _activeNote = __notes._activeNote;
    _laserTones = __notes._laserTones;
    _disableAndResetTPlayNote();
  }
  return *this;
}
// move constructor
notes::notes(notes&& __notes):
  sendCurrentNote(nullptr),
  _laserTones({})
{
  *this = std::move(__notes);
}
// move assignment op
notes & notes::operator=(notes&& __notes) {
  if (this != &__notes) {
    sendCurrentNote = nullptr;
    _activeNote = note{};
    _laserTones = {};

    sendCurrentNote = __notes.sendCurrentNote;
    _activeNote = __notes._activeNote;
    _laserTones = __notes._laserTones;

    __notes.sendCurrentNote = nullptr;
    __notes._activeNote = note{};
    __notes._laserTones = {};

    _disableAndResetTPlayNote();
  }
  return *this;
}



///////////////////////////////////
// Setters
///////////////////////////////////
/** notes::setActive(const note & __activeNote): public instance setter method
 * 
 *  sets the instance variable _activeNote from a passed in note reference.
 *  
 *  The _activeNote is then used: 
 *  (i)  in this class (notes), to provide 
 *       (a) the interval param to Task tPlayNote (ie. the note duration);
 *       (b) to call _playLaserTone (ie. the note laserTone). <-- TODO: correct erroneous comments
 *  (ii) in laserInterface, to retrieve the params (_ui16Note and _ui16Tone) of
 *       the currently active note and send them to the mesh. */
void notes::setActive(const note & __activeNote) {
  // Serial.println("notes::setActive: starting");
  _disableAndResetTPlayNote();
  // Serial.println("notes::setActive: tPlayNote disabled");
  // Serial.printf("notes::setActive: __activeNote._ui16Tone: %u\n", __activeNote._ui16Tone);
  // Serial.printf("notes::setActive: __activeNote._ui16Note: %u\n", __activeNote._ui16Note);
  // Serial.printf("notes::setActive: _activeNote._ui16Tone: %u\n", _activeNote._ui16Tone);
  // Serial.printf("notes::setActive: _activeNote._ui16Note: %u\n", _activeNote._ui16Note);
  _activeNote = __activeNote;
  // Serial.println("notes::setActive: over");
}


/** notes::_disableAndResetTPlayNote(): public setter method
 * 
 *  disables Task tPlayNote and resets its default parameters, so as
 *  it be ready to play any note.
 * 
 *  Default parameters:
 *  - an interval of 30.000 seconds;
 *  - 1 iteration;
 *  - no main callback;
 *  - an onEnable callback, that turns the lasers on in a given laserTone;
 *  - an onDisable callback, that turn the lasers off.
 * 
 * 
 *   The Task is enabled upon instanciating a note in the bar class.
 *   It is disabled:
 *   - at the expiration of the interval set herein (30000 ms); or
 *   - by _tcbPlayBar, the main callback of tPlayBar, when the note is supposed
 *     to finish.
 *   It does not need any mainCallback, as it does not iterate.
 * 
 *   task tPlaynote plays a given laserTone (set in note::_ui16ActiveTone)
 *   for a given note type (--> full, half, ..., set in the bar) at a given
 *   beat rate. 
 *  */
void notes::_disableAndResetTPlayNote() {
  tPlayNote.disable();
  tPlayNote.set(30000, 1, NULL, [&](){return this->_oetcbPlayNote();}, [&](){this->_odtcbPlayNote();});
}




///////////////////////////////////
// Getters
///////////////////////////////////
/** const note &notes::getCurrentNote() const: public static getter method
 *  
 *  returns a reference to the note instance that is currently active. */
const note &notes::getCurrentNote() const {
  return _activeNote;
}


///////////////////////////////////
// Task - Player
///////////////////////////////////
/** bool const notes::playNote(const note & __note, const beat & __beat):
 *  
 *  Play a single note for a given duration (calculated using the passed-in beat)
 *  or the maximum duration (30.000 ms) if the calculated duration exceeds 30000 ms.
 *  by enabling Task tPlayNote.
 *  
 *  Task tPlayNote may then be disabled by calls to _disableAndResetTPlayNote().
 * 
 *  {@ params} const note & __note: pass a constant reference to a note used
 *             to calculate the notes duration
 *  {@ params} const beat & beat: pass a constant reference to a beat used
 *             to calculate the notes duration.
 * */
bool const notes::playNote(const note & __note, const beat & __beat) {
  Serial.println("notes::playNote: starting");
  if ((__beat.getBaseNoteForBeat() == 0) || (__beat.getBaseBeatInBpm() == 0)) {
    return false;
  }
  setActive(__note);
  tPlayNote.setInterval(__note.ui16GetNoteDurationInMs(__beat));
  tPlayNote.restartDelayed();
  Serial.println("notes::playNote: over");
  return true;
}




/** notes::_oetcbPlayNote()
 *  On enable Task _tNote, turn the lasers to a given laserTone */
bool notes::_oetcbPlayNote() {
  Serial.println("note::_oetcbPlayNote(). starting");
  if (MY_DG_LASER) {
    Serial.printf("note::_oetcbPlayNote(). Going to play laserTone number (_activeNote.getLaserToneNumber()) %u\n", _activeNote.getLaserToneNumber());
    Serial.printf("note::_oetcbPlayNote(). Going to play laserTone number (_laser_tones.at(_activeNote.getLaserToneNumber()).i16IndexNumber) %i\n", _laserTones.at(_activeNote.getLaserToneNumber()).i16IndexNumber);
    Serial.printf("note::_oetcbPlayNote(). TEST: (_activeNote.getLaserToneNumber()) == (_laser_tones.at(_activeNote.getLaserToneNumber()).i16IndexNumber): [%i]\n", (_activeNote.getLaserToneNumber()) == (_laserTones.at(_activeNote.getLaserToneNumber()).i16IndexNumber));
  }
  bool _return = false;
  if (_activeNote._playLaserTone(_laserTones) >= 0) {
    _return = true;
  }
  Serial.printf("note::_oetcbPlayNote(). returning [%s]\n", (_return == true) ? "true" : "false");
  return _return;
}


/** notes::_odtcbPlayNote()
 * 
 *  On disable Task _tNote, turn off all the lasers */
void notes::_odtcbPlayNote() {
  Serial.println("note::_odtcbPlayNote(). starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Turning off all the lasers");
  }
  _activeNote._setLaserTone(_laserTones.at(0)); // _laserTones.at(0) means turn off all the lasers
  _activeNote._playLaserTone(_laserTones);
  Serial.println("note::_odtcbPlayNote(). over");
}
