/*
  tone.h - tones are statuses of all the lasers connected to the box at one base beat
  Created by Cedric Lor, June 10, 2019.

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

#ifndef tone_h
#define tone_h

#include "Arduino.h"
#include <global.h>
#include <laserPin.h>

class tone
{
  public:
    tone(); // default constructor
    tone(std::array<bool , 4> __bLaserPinsStatus);

    static std::array<tone, 16> tones;
    static void initTones(); // initializer of the array of tones

    void playTone(const uint16_t _ui16_active_tone);

  private:
    static const uint16_t _ui16_tone_count;

    std::array<bool, 4> _bLaserPinStatus;  // array containing the state of each laser at each tempo
};

#endif

