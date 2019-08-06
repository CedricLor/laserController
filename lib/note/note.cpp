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
  The active tone is initially set at 0 (all off).
  It is then set in the bar class, upon reading the bars.
*/
short int note::activeTone = 0; //

// constructor
note::note() {
}

/*
  task tPlayNote is enabled upon instanciating a note in the bar class.
  It is disabled:
  - by the expiration of the interval set in the Task declaration; or
  - by _tcbPlayBar, the main callback of tPlayBar, when the note is supposed
  to finish.
  It does not need any mainCallback, as it does not iterate.

  task tPlaynote plays a given tone (set in note::activeTone)
  for a given note type (--> full, half, ..., set in the bar) at a given
  beat rate.
*/
Task note::tPlayNote(30000, 1, NULL, &myTaskScheduler, false, &_oetcbPlayNote, &_odtcbPlayNote);

// On enable Task _tNote, turn the lasers to a given tone
bool note::_oetcbPlayNote() {
  Serial.println("note::_oetcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Going to play tone number ");Serial.println(activeTone);
  }
  tone::tones[activeTone].playTone();
  Serial.println("note::_oetcbPlayNote(). Ending");
  return true;
}

// On disable Task _tNote, turn off all the lasers
void note::_odtcbPlayNote() {
  Serial.println("note::_odtcbPlayNote(). Starting");
  if (MY_DG_LASER) {
    Serial.print("note::_oetcbPlayNote(). Turning off all the lasers");
  }
  activeTone = 0; // tones[0] means turn off all the lasers
  tone::tones[activeTone].playTone();
  Serial.println("note::_odtcbPlayNote(). Ending");
}
