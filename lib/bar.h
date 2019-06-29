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
  |  |  |  |--tone.cpp (called to play some member functions)
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

    static bar bars[];
    static void initBars(); // initializer of the array of bas

    static void setActiveBar(const short int activeBar);

    static Task tPlayBars;

  private:
    static const short int _bar_count;
    static short int _activeBar;
    static const short int _char_count_in_name;

    void _initBar(const char cName[], const unsigned long ulTempo, const short int iNumberOfBeatsInBar, const short int iLaserPinStatusAtEachBeat[]);

    char _cName[7];  // array of character to hold the name of each bars
    unsigned long  _ulTempo; // tempo at which the task executing the bar will update the state of each laser pin, in milliseconds
    short int _iNumberOfBeatsInBar; // number of tempos required to execute one full bar
    short int _iLaserPinStatusAtEachBeat[4];  // array containing the state of each laser at each tempo


    static void tPlayBars();
    static bool tPlayBars();
    static void tPlayBars();

    void _playBar();
    static Task _tPlayBar;
    static void _oetcbPlaybar();
    static void _odtcbPlaybar();
};

#endif
