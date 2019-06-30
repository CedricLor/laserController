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
#include "global.h"

class bar
{
  public:
    bar(); // default constructor

    // static
    static bar bars[];
    static void initBars(); // initializer of the array of bas

    static void setActiveBar(const short int activeBar);

    // static Task tPlayBars;

    // non-static
    void playBar(const short activeBar);
    unsigned long getIntervalForEachNote(const short int _iter);
    unsigned long  ulBaseBeatInMs; // basic time unit for the smallest note to be played (_iBaseNoteForBeat)
    short int iNotesCountInBar; // number of basic time unit for the smallest note to be played in one bar
    short int iBaseNotesCountInBar;

  private:
    static const short int _bar_count;
    static short int _activeBar;
    static const short int _char_count_in_name;

    // static void _tcbPlayBars();
    // static bool _oetcbPlayBars();
    // static void _odtcbPlayBars();

    static Task _tPlayBar;
    static void _tcbPlayBar();
    static bool _oetcbPlayBar();
    static void _odtcbPlayBar();

    // non-static
    void _initBar(const char cName[], const unsigned long ulBaseBeatInMs, const short iBaseNoteForBeat, const short int __iBaseNotesCountInBar, const short int iNotesCountInBar, const short int iNoteTone[][2]);

    char _cName[7];  // array of character to hold the name of each bars
    short int _iBaseNoteForBeat;
    short int _note[16][2];  // array containing the state of each laser at each tempo
};

#endif
