/*
  bar.h - bars are two beats to four beats, precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

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
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h



*/
#ifndef bar_h
#define bar_h

#include "Arduino.h"
#include <global.h>
#include "../note/note.h"
#include "../note/note.cpp"

class bar
{
  public:
    bar(); // default constructor

    // static
    static bar bars[];
    static void initBars(); // initializer of the array of bas

    static void setActiveBar(const short int activeBar);

    // non-static
    unsigned long getNoteDuration(const short int _iter);
    uint16_t ui16BaseBeatInBpm; // basic time unit for the smallest note to be played (_iBaseNoteForBeat)
    uint16_t ui16BaseNoteForBeat; // base note for the beat
    uint16_t ui16BaseNotesCountInBar;
    uint16_t ui16NotesCountInBar; // number of actual notes in the bar

    static Task tPlayBar;

  private:
    static const short int _bar_count;
    static short int _activeBar;
    static const short int _char_count_in_name;

    static void _tcbPlayBar();
    static bool _oetcbPlayBar();
    static void _odtcbPlayBar();

    // non-static
    void _initBar(const char __cName[], const uint16_t __ui16BaseBeatInBpm, const uint16_t _ui16BaseNoteForBeat, const uint16_t _ui16BaseNotesCountInBar, const uint16_t __ui16NotesCountInBar, const short int iNoteTone[][2]);

    char _cName[7];  // array of character to hold the name of each bars
    short int _note[16][2];  // array containing the state of each laser at each tempo
};

#endif
