/*
  note.cpp - notes are tones for a given length (in base beat)
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

tones note::globalTones{}; // TODO: <-- For the moment, it is at the global scope; find a way to have it stored somewhere else
note globalNote{}; // TODO: <-- For the moment, it is at the global scope; find a way to have it stored somewhere else

///////////////////////////////////
// Static variables
///////////////////////////////////

// pointer to functions to produce an interface for note
void (*note::sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note) = nullptr;






///////////////////////////////////
// Constructors
///////////////////////////////////
// default
note::note() :
  _ui16Tone(0),
  _ui16Note(0),
  _tone(globalTones._array.at(0))
{}

// parameterized
note::note(
  const uint16_t __ui16_tone,
  const uint16_t __ui16_note
) :
  _ui16Tone(__ui16_tone),
  _ui16Note(__ui16_note),
  _tone(globalTones._array.at(_ui16Tone))
{ _ui16ValidNote(); }

// copy constructor
note::note(const note& __note) :   
  _ui16Tone(__note._ui16Tone),
  _ui16Note(__note._ui16Note),
  _tone(globalTones._array.at(_ui16Tone))
{ _ui16ValidNote(); }

// assignement operator
note& note::operator=(const note& __note)
{
  if (&__note != this) {
    _ui16Tone = __note._ui16Tone;
    _ui16Note = __note._ui16Note;
    _tone = __note._tone;
    _ui16ValidNote();
  }
  return *this;
}






///////////////////////////////////
// Setters
///////////////////////////////////
/** tone & note::_setTone(const tone & _target_tone): private instance setter method
 * 
 *  sets the instance reference member _tone (tone & _tone) to the passed-in reference-to 
 *  const tone. */
tone & note::_setTone(const tone & _target_tone) {
  _tone = _target_tone;
  _ui16Tone = _tone.i16IndexNumber;
  return _tone;
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
/** uint16_t note::getToneNumber(): public instance getter method
 * 
 *  Returns the tone number of a note instance. */
uint16_t const note::getToneNumber() const {
  return _ui16Tone;
}


/** uint16_t note::getNote(): public instance getter method
 *  
 *  Returns the note of a note instance. */
uint16_t const note::getNote() const {
  return _ui16Note;
}


/** ul note::ui16GetNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration of a note instance in ms. */
uint16_t const note::ui16GetNoteDurationInMs() const {
  // Serial.println("note::ui16GetNoteDurationInMs(). Starting.");
  // Serial.println(F("------------- DEBUG --------- note --------- DEBUG -------------"));
  // Serial.printf("note::ui16GetNoteDurationInMs(). _ui16Note == %u\n", _ui16Note);
  // Serial.printf("note::ui16GetNoteDurationInMs(). beat::getCurrentBeat().getBaseNoteForBeat() == %u\n", beat::getCurrentBeat().getBaseNoteForBeat());

  if ((_ui16Note == 0) || (beat::getCurrentBeat().getBaseNoteForBeat() == 0)) {
    return 0;
  }
  // Serial.printf("note::ui16GetNoteDurationInMs(). Passed the 0 tests.\n");

  // const uint16_t __ui16baseNoteDuration = beat::getCurrentBeat().ui16GetBaseNoteDurationInMs();
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ui16baseNoteDuration == %u\n", __ui16baseNoteDuration);
  // const uint16_t __ui16BaseNoteForBeat = beat::getCurrentBeat().getBaseNoteForBeat();
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ui16BaseNoteForBeat == %u\n", __ui16BaseNoteForBeat);
  // const uint16_t __num = __ui16baseNoteDuration * __ui16BaseNoteForBeat;
  // Serial.printf("note::ui16GetNoteDurationInMs(). __num == %u\n", __num);
  // see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
  uint16_t __ui16DurationInMs =
  (uint16_t)ceil(((beat::getCurrentBeat().getBaseNoteForBeat() * beat::getCurrentBeat().ui16GetBaseNoteDurationInMs()))
  / _ui16Note);

  // Serial.printf("note::ui16GetNoteDurationInMs(). __ulDurationInM == %u\n", __ui16DurationInMs);
  if (__ui16DurationInMs > 30000) {
    return 30000;
  }
  // Serial.println("note::ui16GetNoteDurationInMs(). Ending.");
  return __ui16DurationInMs;
}









//****************************************************************//
// Notes //*******************************************************//
//****************************************************************//

notes::notes():
  _activeNote(globalNote),
  _tones(tones{})
{}

// notes::notes(note & __activeNote):
//   _activeNote(__activeNote),
//   _tones(tones{})
// {}



///////////////////////////////////
// Getters
///////////////////////////////////
/** note &note::getCurrentNote(): public static getter method
 *  
 *  returns a reference to the note instance that is currently active. */
const note &notes::getCurrentNote() const {
  return _activeNote;
}


///////////////////////////////////
// Setters
///////////////////////////////////
/** notes::setActive(const note & __activeNote): public instance setter method
 * 
 *  sets the instance variable _activeNote 
 *  from a passed in note reference. */
void notes::setActive(const note & __activeNote) {
  this->tPlayNote.disable();
  _activeNote = __activeNote;
}


/** notes::resetTPlayNoteToPlayNotesInBar(): public static setter method
 * 
 *  resets the parameters of the static Task tPlayNote to  
 *  play notes read from a bar. */
void notes::resetTPlayNoteToPlayNotesInBar() {
  this->tPlayNote.disable();
  this->tPlayNote.setInterval(30000);
  this->tPlayNote.setOnDisable([&](){
    this->_odtcbPlayNote();
  });
}

// TODO: simplify/unify/reuse code re. setTPlayNote() and resetTPlayNoteToPlayNotesInBar()
/** setTPlayNote:
 * 
 *   The Task is enabled upon instanciating a note in the bar class.
 *   It is disabled:
 *   - by the expiration of the interval set in the Task declaration; or
 *   - by _tcbPlayBar, the main callback of tPlayBar, when the note is supposed
 *     to finish.
 *   It does not need any mainCallback, as it does not iterate.
 * 
 *   task tPlaynote plays a given tone (set in note::_ui16ActiveTone)
 *   for a given note type (--> full, half, ..., set in the bar) at a given
 *   beat rate. */
void notes::setTPlayNote() {
  this->tPlayNote.setInterval(30000);
  this->tPlayNote.setOnEnable([&](){
    return this->_oetcbPlayNote();
  });
  this->tPlayNote.setOnDisable([&](){this->_odtcbPlayNote();});
}


///////////////////////////////////
// Task - Player
///////////////////////////////////
/**notes::playNoteStandAlone:
 *  
 *  play a single note for a given duration (calculated using the passed-in beat).
 * 
 *  {@ params} beat const & __beat: pass a beat to be taken into account
 *             to calculate the notes duration */
void notes::playNoteStandAlone(const note & __note, beat const & __beat) {
  setActive(__note);
  beat(__beat).setActive();
  this->tPlayNote.setInterval(__note.ui16GetNoteDurationInMs());
  this->tPlayNote.setOnDisable([&](){
    beat(0, 0).setActive();
    this->_odtcbPlayNote();
    this->tPlayNote.setOnDisable([&](){this->_odtcbPlayNote();});
  });
  this->tPlayNote.restartDelayed();
}

/**notes::playNoteInBar:
 *  
 *  play a single note for its maximum duration.
 *  _tcbPlayBar manages the real duration (and the beat). 
*/
void notes::playNoteInBar(const note & __note) {
  setActive(__note);
  this->tPlayNote.restartDelayed();
}


/** notes::_oetcbPlayNote()
 *  On enable Task _tNote, turn the lasers to a given tone */
bool notes::_oetcbPlayNote() {
  Serial.println("note::_oetcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.printf("note::_oetcbPlayNote(). Going to play tone number %i\n", _activeNote._tone.i16IndexNumber);
  }
  _activeNote._tone._playTone(_tones._laserPins);
  Serial.println("note::_oetcbPlayNote(). Ending");
  return true;
}


/** notes::_odtcbPlayNote()
 *  On disable Task _tNote, turn off all the lasers */
void notes::_odtcbPlayNote() {
  Serial.println("note::_odtcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Turning off all the lasers");
  }
  _activeNote._setTone(_tones._array.at(0)); // tones[0] means turn off all the lasers
  _activeNote._tone._playTone(_tones._laserPins);
  Serial.println("note::_odtcbPlayNote(). Ending");
}
