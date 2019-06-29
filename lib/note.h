/*
  note.h - notes are tones for a given length (in base beat)
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
  |  |  |  |--tone.cpp (called to play some member functions)
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h

*/

#ifndef note_h
#define note_h

#include "Arduino.h"

class note
{
  public:
    note(); // default constructor

    static note notes[];
    static void initNotes(); // initializer of the array of tones

    void playNote(const short toneIndex, const short noteIndex, short sBaseBeatInMs);

  private:
    static short int _activeTone;
    static short int _activeNote;
    static const short int _note_count;
    static const short int _char_count_in_name;

    static Task _tPlayNote;
    static bool _oetcbPlayNote();
    static void _odtcbPlayNote();

    void _initNote(const char cName[], const byte durationInBaseBeats);

    char _cName[15];  // array of character to hold the name of each sequences
    byte _durationInBaseBeats;
};

#endif
