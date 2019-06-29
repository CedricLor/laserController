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


const short int note::_note_count = 8;
note note::notes[_note_count];
const short int note::_char_count_in_name = 15;
short int note::_activeTone = 0;
short int note::_activeNote = 0;

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

  notes[0]._initNote("1- 16th", 1);
  notes[1]._initNote("2- eighth", 2);
  notes[2]._initNote("3- eighth + 16th", 3);
  notes[3]._initNote("4- quarter", 4);
  notes[4]._initNote("6- quarter + eighth", 6);
  notes[5]._initNote("8- half", 8);
  notes[6]._initNote("12- half + quarter", 12);
  notes[7]._initNote("16- full", 16);

  Serial.println("void note::initNotes(). Ending.");
}

// playNote: non-static member function; play a given tone for a given time
// to be called this way:
// notes[3].playNote(4) to play a noire, tone number 4 (fourth laser on)
void note::playNote(const short toneIndex, const short noteIndex, short sBaseBeatInMs){
  // Serial.println("void note::playNote(). Starting");
  _activeNote = noteIndex;
  _activeTone = toneIndex;
  short int _iDurationInBaseBeats = (int)_durationInBaseBeats;
  unsigned long _ulDurationInMs = _iDurationInBaseBeats * sBaseBeatInMs;
  _tPlayNote.setInterval(_ulDurationInMs);
  _tPlayNote.enable();
  // Serial.println("void note::playNote(). Ending");
};

Task note::_tPlayNote(0, 1, NULL, &userScheduler, false, &_oetcbPlayNote, &_odtcbPlayNote);

// On enable Task _tNote, turn the lasers to a given tone
bool note::_oetcbPlayNote() {
  tone::tones[_activeTone].playTone();
  return true;
}

// On disable Task _tNote, turn off the lasers
void note::_odtcbPlayNote() {
  tone::tones[0].playTone();
}
