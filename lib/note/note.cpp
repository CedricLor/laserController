/*
  note.cpp - notes are tones for a given length (in base beat)
  Created by Cedric Lor, June 28, 2019.

*/

#include "Arduino.h"
#include "note.h"

///////////////////////////////////
// Static variables
///////////////////////////////////
note _note;
note &note::_activeNote = _note;



// pointer to functions to produce an interface for note
void (*note::sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note) = nullptr;






///////////////////////////////////
// Constructors
///////////////////////////////////
// default
note::note() :
  _ui16Tone(0),
  _ui16Note(0),
  _tone(tone::_tones[0])
{}

// parameterized
note::note(
  const uint16_t __ui16_tone,
  const uint16_t __ui16_note
) :
  _ui16Tone(__ui16_tone),
  _ui16Note(__ui16_note),
  _tone(tone::_tones[_ui16Tone])
{
}

// copy constructor
note::note( const note& ) 
: _tone(tone::_tones[_ui16Tone])
{
}

// assignement operator
note& note::operator=(const note& )
{
  return *this;
}






///////////////////////////////////
// Setters
///////////////////////////////////
/** note::setActiveNoteFromNote(): public static setter method
 * 
 *  sets the parameters of the static variable &note::_activeNote 
 *  from a passed in note reference. */
void note::setActiveNoteFromNote(const note & __target_note) {
  _activeNote = __target_note;
}


/** note::_setTone(): private instance setter method
 * 
 *  sets the instance reference to the tone associated with
 *  the note from a passed in tone number. */
void note::_setTone(const uint16_t __ui16_target_tone) {
  _ui16Note = __ui16_target_tone;
  _tone = tone::_tones[_ui16Tone];
}


/** note::_setTPlayNote(): public static setter method
 * 
 *  sets the parameters of the Task tPlayNote. */
void note::_setTPlayNote(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, uint16_t const __ui16_iterations) {
  beat::setActiveBeat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm);
  tPlayNote.setInterval(_activeNote.ui16GetNoteDurationInMs());
  tPlayNote.setIterations(__ui16_iterations);
  tPlayNote.setOnDisable([](){
    beat::setActiveBeat(0, 0);
    tPlayNote.setOnDisable(NULL);
  });
}








///////////////////////////////////
// Getters
///////////////////////////////////
/** note &note::getCurrentNote(): public static getter method
 *  
 *  returns a reference to the note instance that is currently active. */
const note &note::getCurrentNote() {
  return _activeNote;
}


/** uint16_t note::getTone(): public instance getter method
 * 
 *  Returns the tone of a note instance. */
uint16_t const note::getTone() const {
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
  Serial.println("note::ui16GetNoteDurationInMs(). Starting.");
  Serial.println(F("------------- DEBUG --------- note --------- DEBUG -------------"));
  Serial.printf("note::ui16GetNoteDurationInMs(). _ui16Note == %u\n", _ui16Note);
  Serial.printf("note::ui16GetNoteDurationInMs(). beat::getCurrentBeat().getBaseNoteForBeat() == %u\n", beat::getCurrentBeat().getBaseNoteForBeat());

  if ((_ui16Note == 0) || (beat::getCurrentBeat().getBaseNoteForBeat() == 0)) {
    return 0;
  }
  Serial.printf("note::ui16GetNoteDurationInMs(). Passed the 0 tests.\n");

  // see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
  uint64_t __ulDurationInMs =
  ((beat::getCurrentBeat().getBaseNoteForBeat() * beat::getCurrentBeat().ui16GetBaseNoteDurationInMs()) + _ui16Note - 1)
  / _ui16Note; 

  // Serial.print("note::ulGetNoteDurationInMs(). __ulDurationInMs = ");Serial.println(__ulDurationInMs);
  if (__ulDurationInMs > 30000) {
    return 30000;
  }
  Serial.println("note::ui16GetNoteDurationInMs(). Ending.");
}






///////////////////////////////////
// Task - Player
///////////////////////////////////
/**note::playNoteStandAlone:
 *  
 *  play a single note for a given duration.
 * 
 *  {@ params} uint16_t const __ui16_base_note_for_beat: pass the base note 
 *             for a given beat
 *  {@ params} uint16_t const __ui16_base_beat_in_bpm: pass the base beat 
 *             in bpm
 *  {@ params} note const & __target_note: pass a target note ref to const
 *             ex. note(8,1): tone 8 for the duration of a white (1)
*/
void note::playNoteStandAlone(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, note const & __target_note) {
  tPlayNote.disable();
  setActiveNoteFromNote(__target_note);
  _setTPlayNote(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm, 1);
  tPlayNote.restartDelayed();
}

/** task tPlayNote:
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
Task note::tPlayNote(30000, 1, NULL, NULL/*&mns::myScheduler*/, false, &_oetcbPlayNote, &_odtcbPlayNote);

/** note::_oetcbPlayNote()
 *  On enable Task _tNote, turn the lasers to a given tone */
bool note::_oetcbPlayNote() {
  Serial.println("note::_oetcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.printf("note::_oetcbPlayNote(). Going to play tone number %u\n", _activeNote._ui16Tone);
  }
  _activeNote._tone._playTone(_activeNote._ui16Tone);
  Serial.println("note::_oetcbPlayNote(). Ending");
  return true;
}

/** note::_oetcbPlayNote()
 *  On disable Task _tNote, turn off all the lasers */
void note::_odtcbPlayNote() {
  Serial.println("note::_odtcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Turning off all the lasers");
  }
  _activeNote._setTone(0); // tones[0] means turn off all the lasers
  _activeNote._tone._playTone(_activeNote._ui16Tone);
  Serial.println("note::_odtcbPlayNote(). Ending");
}