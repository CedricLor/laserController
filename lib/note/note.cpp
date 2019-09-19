/*
  note.cpp - notes are tones played for a given length (expressed in base beat)
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

note globalNote{}; // TODO: <-- For the moment, it is at the global scope; find a way to have it stored somewhere else

///////////////////////////////////
// Static variables
///////////////////////////////////
tones note::globalTones{}; // TODO: <-- For the moment, it is at the global scope; find a way to have it stored somewhere else




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
note::note() :
  _ui16Tone(0),
  _ui16Note(0)
{}

// parameterized
note::note(
  const uint16_t __ui16_tone,
  const uint16_t __ui16_note
) :
  _ui16Tone(__ui16_tone),
  _ui16Note(__ui16_note)
{ _ui16ValidNote(); }

// copy constructor
note::note(const note& __note) :   
  _ui16Tone(__note._ui16Tone),
  _ui16Note(__note._ui16Note)
{ _ui16ValidNote(); }

// assignement operator
note& note::operator=(const note& __note)
{
  if (&__note != this) {
    _ui16Tone = __note._ui16Tone;
    _ui16Note = __note._ui16Note;
    _ui16ValidNote();
  }
  return *this;
}

note::note(note&& __note):
  _ui16Tone(0),
  _ui16Note(0)
{
  *this = std::move(__note);
}

note & note::operator=(note&& __note) {
  if (this != &__note) {
    _ui16Tone = 0;
    _ui16Note = 0;

    _ui16Tone = __note._ui16Tone;
    _ui16Note = __note._ui16Note;

    __note._ui16Tone = 0;
    __note._ui16Note = 0;
  }
  return *this;
}





///////////////////////////////////
// Setters
///////////////////////////////////
/** uint16_t note::_setTone(const tone & _target_tone): private instance setter method
 * 
 *  sets the instance reference member _tone (tone & _tone) to the passed-in reference-to 
 *  const tone. */
uint16_t note::_setTone(const tone & _target_tone) {
  _ui16Tone = _target_tone.i16IndexNumber;
  return _ui16Tone;
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


/** uint16_t note::ui16GetNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration of a note instance in ms. */
uint16_t const note::ui16GetNoteDurationInMs() const {
  // Serial.println("note::ui16GetNoteDurationInMs(). starting.");
  // Serial.println(F("------------- DEBUG --------- note --------- DEBUG -------------"));
  // Serial.printf("note::ui16GetNoteDurationInMs(). _ui16Note == %u\n", _ui16Note);
  // Serial.printf("note::ui16GetNoteDurationInMs(). activeBeat.getBaseNoteForBeat() == %u\n", activeBeat.getBaseNoteForBeat());

  if ((_ui16Note == 0) || (activeBeat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // const uint16_t __ui16baseNoteDuration = activeBeat.ui16GetBaseNoteDurationInMs();
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ui16baseNoteDuration == %u\n", __ui16baseNoteDuration);

  // const uint16_t __ui16BaseNoteForBeat = activeBeat.getBaseNoteForBeat();
  // Serial.printf("note::ui16GetNoteDurationInMs(). __ui16BaseNoteForBeat == %u\n", __ui16BaseNoteForBeat);

  // const uint16_t __num = __ui16baseNoteDuration * __ui16BaseNoteForBeat;
  // Serial.printf("note::ui16GetNoteDurationInMs(). __num == %u\n", __num);

  // see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
  uint16_t __ui16DurationInMs =
    (uint16_t)ceil(
      ((activeBeat.getBaseNoteForBeat() * activeBeat.ui16GetBaseNoteDurationInMs()))
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
const int16_t note::_playTone() const {
  return globalTones._playTone(getToneNumber());
}





//****************************************************************//
// Notes //*******************************************************//
//****************************************************************//

notes::notes(
  void (*_sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note)
):
  sendCurrentNote(_sendCurrentNote),
  _activeNote(globalNote),
  _tones({})
{
  disableAndResetTPlayNote();
}
// copy constructor
notes::notes(const notes & __notes):
  sendCurrentNote(__notes.sendCurrentNote),
  _activeNote(__notes._activeNote),
  _tones(__notes._tones)
{
  disableAndResetTPlayNote();
}
// copy assignment operator
notes & notes::operator=(const notes & __notes) {
  if (&__notes != this) {
    sendCurrentNote = __notes.sendCurrentNote;
    _activeNote = __notes._activeNote;
    _tones = __notes._tones;
    disableAndResetTPlayNote();
  }
  return *this;
}
// move constructor
notes::notes(notes&& __notes):
  sendCurrentNote(nullptr),
  _activeNote(globalNote),
  _tones({})
{
  *this = std::move(__notes);
}
// move assignment op
notes & notes::operator=(notes&& __notes) {
  if (this != &__notes) {
    sendCurrentNote = nullptr;
    _activeNote = globalNote;
    _tones = {};

    sendCurrentNote = __notes.sendCurrentNote;
    _activeNote = __notes._activeNote;
    _tones = __notes._tones;

    __notes.sendCurrentNote = nullptr;
    __notes._activeNote = globalNote;
    __notes._tones = {};

    disableAndResetTPlayNote();
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
 *  (i)  in this class, to start the tone player for the currently active note,
 *       from the tPlayNote Task (onEnable and onDiable) callbacks;
 *  (ii) in laserInterface, to retrieve the params (_ui16Note and _ui16Tone) of
 *       the currently active note and send them to the mesh. */
void notes::setActive(const note & __activeNote) {
  // Serial.println("notes::setActive: starting");
  tPlayNote.disable();
  // Serial.println("notes::setActive: tPlayNote disabled");
  // Serial.printf("notes::setActive: __activeNote._ui16Tone: %u\n", __activeNote._ui16Tone);
  // Serial.printf("notes::setActive: __activeNote._ui16Note: %u\n", __activeNote._ui16Note);
  // Serial.printf("notes::setActive: _activeNote._ui16Tone: %u\n", _activeNote._ui16Tone);
  // Serial.printf("notes::setActive: _activeNote._ui16Note: %u\n", _activeNote._ui16Note);
  _activeNote = __activeNote;
  // Serial.println("notes::setActive: over");
}


/** notes::disableAndResetTPlayNote(): public setter method
 * 
 *  resets the parameters of the Task tPlayNote to  
 *  play notes read from a bar. */
void notes::disableAndResetTPlayNote() {
  tPlayNote.disable();
  resetTPlayNote();
}

/** void notes::resetTPlayNote(): public setter method
 * 
 *   notes::resetTPlayNote sets Task notes::tPlayNote to its default 
 *   parameters:
 *   - an interval of 30.000 seconds;
 *   - no main callback;
 *   - 
 * 
 *   It is called by sequence (from the players (standalone and in boxState)) and
 *   bar (from the standalone bar player).
 * 
 *   The Task is enabled upon instanciating a note in the bar class.
 *   It is disabled:
 *   - at the expiration of the interval set herein (30000 ms); or
 *   - by _tcbPlayBar, the main callback of tPlayBar, when the note is supposed
 *     to finish.
 *   It does not need any mainCallback, as it does not iterate.
 * 
 *   task tPlaynote plays a given tone (set in note::_ui16ActiveTone)
 *   for a given note type (--> full, half, ..., set in the bar) at a given
 *   beat rate. 
 * 
 *   */
void notes::resetTPlayNote() {
  this->tPlayNote.setIterations(1);
  this->tPlayNote.setInterval(30000);
  this->tPlayNote.setCallback(NULL);
  this->tPlayNote.setOnEnable([&](){return this->_oetcbPlayNote();});
  this->tPlayNote.setOnDisable([&](){this->_odtcbPlayNote();});
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
/** void notes::playNoteStandAlone(const note & __note, beat const & __beat):
 *  
 *  play a single note for a given duration (calculated using the passed-in beat).
 * 
 *  {@ params} beat const & __beat: pass a beat to be taken into account
 *             to calculate the notes duration */
void notes::playNoteStandAlone(const note & __note, beat const & __beat) {
  setActive(__note);
  activeBeat = __beat;
  this->tPlayNote.setInterval(__note.ui16GetNoteDurationInMs());
  this->tPlayNote.setOnDisable([&](){
    activeBeat = beat(0, 0);
    this->_odtcbPlayNote();
    this->tPlayNote.setOnDisable([&](){this->_odtcbPlayNote();});
  });
  this->tPlayNote.restartDelayed();
}

/** void notes::playNoteInBar(const note & __note):
 *  
 *  play a single note for its maximum duration.
 *  _tcbPlayBar manages the real duration (and the beat). 
*/
void notes::playNoteInBar(const note & __note) {
  Serial.println("notes::playNoteInBar: starting");
  setActive(__note);
  tPlayNote.restartDelayed();
  Serial.println("notes::playNoteInBar: over");
}


/** notes::_oetcbPlayNote()
 *  On enable Task _tNote, turn the lasers to a given tone */
bool notes::_oetcbPlayNote() {
  Serial.println("note::_oetcbPlayNote(). starting");
  if (MY_DG_LASER) {
    Serial.printf("note::_oetcbPlayNote(). Going to play tone number (_activeNote.getToneNumber()) %u\n", _activeNote.getToneNumber());
    Serial.printf("note::_oetcbPlayNote(). Going to play tone number (_tones._tonesArray.at(_activeNote.getToneNumber()).i16IndexNumber) %i\n", _tones._tonesArray.at(_activeNote.getToneNumber()).i16IndexNumber);
    Serial.printf("note::_oetcbPlayNote(). TEST: (_activeNote.getToneNumber()) == (_tones._tonesArray.at(_activeNote.getToneNumber()).i16IndexNumber): [%i]\n", (_activeNote.getToneNumber()) == (_tones._tonesArray.at(_activeNote.getToneNumber()).i16IndexNumber));
  }
  bool _return = false;
  if (_activeNote._playTone() >= 0) {
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
  _activeNote._setTone(_tones._tonesArray.at(0)); // tones[0] means turn off all the lasers
  _activeNote._playTone();
  Serial.println("note::_odtcbPlayNote(). over");
}
