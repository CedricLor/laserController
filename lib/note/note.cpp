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


/*
  The active tone is initially set to 0 (all off).
  It is then set in the bar class, upon reading the bars.
*/
uint16_t note::_ui16ActiveTone = 0; //
/*
  The active note is initially set to 0 (no note).
  It is then set in the bar class, upon reading the bars.
*/
uint16_t note::_ui16ActiveNote = 0; //



// pointer to functions to produce an interface for note
void (*note::sendCurrentNote)(const uint16_t __ui16_current_tone, const uint16_t __ui16_current_note) = nullptr;



// constructor
note::note() {
}



note::note(
  const uint16_t __ui16_tone,
  const uint16_t __ui16_note
) :
  _ui16Tone(__ui16_tone),
  _ui16Note(__ui16_note)
{

}





void note::setActiveTone(const uint16_t __ui16_active_tone) {
  _ui16ActiveTone = __ui16_active_tone;
}




void note::setActiveNote(const uint16_t __ui16_active_note) {
  _ui16ActiveNote = __ui16_active_note;
}




uint16_t note::getTone() const {
  return _ui16Tone;
}



uint16_t note::getNote() const {
  return _ui16Note;
}

// Get the active note
const note &note::getCurrentNote() {
  const note _note(_ui16ActiveTone, _ui16ActiveNote);
  return _note;
}




/*
  task tPlayNote is enabled upon instanciating a note in the bar class.
  It is disabled:
  - by the expiration of the interval set in the Task declaration; or
  - by _tcbPlayBar, the main callback of tPlayBar, when the note is supposed
  to finish.
  It does not need any mainCallback, as it does not iterate.

  task tPlaynote plays a given tone (set in note::_ui16ActiveTone)
  for a given note type (--> full, half, ..., set in the bar) at a given
  beat rate.
*/
Task note::tPlayNote(30000, 1, NULL, NULL/*&mns::myScheduler*/, false, &_oetcbPlayNote, &_odtcbPlayNote);

// On enable Task _tNote, turn the lasers to a given tone
bool note::_oetcbPlayNote() {
  Serial.println("note::_oetcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Going to play tone number ");Serial.println(_ui16ActiveTone);
  }
  tone::tones[_ui16ActiveTone].playTone();
  Serial.println("note::_oetcbPlayNote(). Ending");
  return true;
}

// On disable Task _tNote, turn off all the lasers
void note::_odtcbPlayNote() {
  Serial.println("note::_odtcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Turning off all the lasers");
  }
  _ui16ActiveTone = 0; // tones[0] means turn off all the lasers
  tone::tones[_ui16ActiveTone].playTone();
  Serial.println("note::_odtcbPlayNote(). Ending");
}
