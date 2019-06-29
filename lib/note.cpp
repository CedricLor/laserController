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
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--note.cpp (called to play some member functions)
  |  |  |  |  |--note.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h

*/

#include "Arduino.h"
#include "note.h"


const short int note::_note_count = 8;  // number of type of notes (full, half, etc.)
note note::notes[_note_count];  // creating an array of note types
const short int note::_char_count_in_name = 15;
short int note::_activeTone = 0; // the active tone is initially set at 0 (all off)
short int note::_activeNote = 0; // the active note type is initially set at 0 (16th)

// constructor
note::note() {

}


void note::_initNote(const char cName[_char_count_in_name], const byte durationInBaseBeats){
  // Serial.println("void note::initNote(). Starting.");
  strcpy(_cName, cName);
  _durationInBaseBeats = durationInBaseBeats;
  // Serial.println("void note::initNote(). Ending.");
};


void note::initNotes() {
  Serial.println("void note::initNotes(). Starting.");

  // notes[index]._initNote("cname", numbeOfBaseBeats);
  notes[0]._initNote("1- full", 16);
  notes[1]._initNote("1/2- half + quarter", 12);
  notes[2]._initNote("1/8- half", 8);
  notes[3]._initNote("1/6- quarter + eighth", 6);
  notes[4]._initNote("1/4- quarter", 4);
  notes[5]._initNote("1/6- eighth + 16th", 3);
  notes[6]._initNote("1/8- eighth", 2);
  notes[7]._initNote("1/16- 16th", 1);

  Serial.println("void note::initNotes(). Ending.");
}

/*
  playNote: non-static member function;

  Playa a given tone for a given time. to be called this way:
  notes[7].playNote(4) to play a full with tone number 4 (fourth laser on)

  playNote:
  - sets various parameters (i) in the note instance notes[index]
  from which it is accessed and (ii) in the Task _tPlayNote.
  - then enables _tPlayNote.
*/
void note::playNote(const short toneIndex, const short noteIndex, short sBaseBeatInMs){
  Serial.println("void note::playNote(). Starting");
  // set static variables note::_activeNote and note::_activeTone from values provided to the sub
  _activeNote = noteIndex;
  _activeTone = toneIndex;
  // calculate the Task interval (_durationInBaseBeats multiplied by the base beat duration in ms)
  short int _iDurationInBaseBeats = (int)_durationInBaseBeats;
  unsigned long _ulDurationInMs = _iDurationInBaseBeats * sBaseBeatInMs;
  // set the Task interval
  _tPlayNote.setInterval(_ulDurationInMs);
  /* On enabling the Task. onEnable callback will be immediately executed, startuing to play the note.
     After expiration of the the interval, the Task will automatically be disabled.
     Its onDisable callback will just turn off all the lasers. */
  _tPlayNote.enableDelayed();
  Serial.println("-------- void note::playNote(). Ending");
};

/*
  task _tPlayNote is enabled from note[index].playNote().
  It is disabled by the expiration of the interval set in note[index].playNote().
  It does not need any mainCallback, as it does not iterate.

  task _tPlaynote plays a given tone (set in note::_activeTone) for a given note type
  --> full, half, ... the actual duration is calculated by multiplying
  the note[index]._iDurationInBaseBeats by an externally provided number of millisconds (sBaseBeatInMs)
*/
Task note::_tPlayNote(0, 1, NULL, &userScheduler, false, &_oetcbPlayNote, &_odtcbPlayNote);

// On enable Task _tNote, turn the lasers to a given tone
bool note::_oetcbPlayNote() {
  Serial.println("-------- bool note::_oetcbPlayNote(). Starting");
  tone::tones[_activeTone].playTone();
  Serial.println("-------- bool note::_oetcbPlayNote(). Ending");
  return true;
}

// On disable Task _tNote, turn off all the lasers
void note::_odtcbPlayNote() {
  Serial.println("-------- void note::_odtcbPlayNote(). Starting");
  tone::tones[0].playTone();
  Serial.println("-------- void note::_odtcbPlayNote(). Ending");
}
