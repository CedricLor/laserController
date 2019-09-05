/*
  note.cpp - notes are tones for a given length (in base beat)
  Created by Cedric Lor, June 28, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |  |  |--global.h


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
void note::setActiveNote(const uint16_t __ui16_active_tone, const uint16_t __ui16_active_note) {
  _activeNote._setTone(__ui16_active_tone);
  _activeNote._ui16Note = __ui16_active_note;
}





void note::setActiveNoteFromNote(const note & _target_note) {
  _activeNote = _target_note;
}





///////////////////////////////////
// Getters
///////////////////////////////////
/** note &note::getCurrentNote(): instance getter
 * 
 *  Returns the tone of a note instance. */
uint16_t note::getTone() const {
  return _ui16Tone;
}


/** note &note::getCurrentNote(): instance getter
 *  
 *  Returns the note of a note instance. */
uint16_t note::getNote() const {
  return _ui16Note;
}


/** note &note::getCurrentNote(): static getter
 *  
 *  returns the note that is currently active. */
const note &note::getCurrentNote() {
  return _activeNote;
}




///////////////////////////////////
// Task - Player
///////////////////////////////////
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





void note::_setTone(const uint16_t __ui16_target_tone) {
  _ui16Note = __ui16_target_tone;
  _tone = tone::_tones[_ui16Tone];
}



unsigned long note::ulGetNoteDuration(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm) const {
  // Serial.println("bar::getNoteDuration(). Starting.");
  // Serial.print("bar::getNoteDuration(). _ui16ActiveBar = ");Serial.println(_ui16ActiveBar);
  // Serial.print("bar::getNoteDuration(). __ui16BaseNoteForBeat = ");Serial.println(__ui16BaseNoteForBeat);
  // Serial.print("bar::getNoteDuration(). _ui16_iter = ");Serial.println(_ui16_iter);
  // Serial.println(F("------------- DEBUG --------- BAR --------- DEBUG -------------"));
  // Serial.println("bar::getNoteDuration(). bars[" + String(_ui16ActiveBar) + "]._note[" + String(_ui16_iter) + "][0] = "+ String(bars[_ui16ActiveBar]._note[_ui16_iter][0]));
  // Serial.println("bar::getNoteDuration(). bars["+ String(_ui16ActiveBar) + "].ui16BaseBeatInBpm = "+ String(bars[_ui16ActiveBar].ui16BaseBeatInBpm));

  // unsigned long __ulDurationInMs = (__ui16BaseNoteForBeat / bars[_ui16ActiveBar]._ui16NoteTone[_ui16_iter][0])
  unsigned long __ulDurationInMs = (__ui16_base_note_for_beat / _ui16Note)
                                  *(60 / _ui16_base_beat_in_bpm * 1000);
  if (__ulDurationInMs > 30000) {
    __ulDurationInMs = 30000;
  }
  // Serial.print("unsigned long bar::getNoteDuration(). __ulDurationInMs = ");Serial.println(__ulDurationInMs);
  // Serial.println("unsigned long bar::getNoteDuration(). Ending.");
  return __ulDurationInMs;
}
