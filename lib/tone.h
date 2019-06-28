/*
  tone.h - notes are statuses of all the lasers connected to the box at one beat
  Created by Cedric Lor, June 10, 2019.

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

#ifndef tone_h
#define tone_h

#include "Arduino.h"

class note
{
  public:
    note(); // default constructor

    static note notes[];
    static void initTones(); // initializer of the array of notes

    void playNote();

  private:
    static const short int _note_count;
    static const short int _char_count_in_name;

    void _initNote(const char cName[], const short int iLaserPinsStatus[PIN_COUNT]);

    char _cName[15];  // array of character to hold the name of each sequences
    short int _iLaserPinStatus[4];  // array containing the state of each laser at each tempo
};

#endif
