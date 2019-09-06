/*
  tone.h - tones are statuses of all the lasers connected to the box at one base beat
  Created by Cedric Lor, June 10, 2019.

*/

#ifndef tone_h
#define tone_h

#include "Arduino.h"
#include <global.h>
#include <laserPin.h>

class tone
{
  friend class note;
  public:
    tone(); // default constructor
    tone(std::array<bool, 4> const __bLaserPinsStatus);

    static void initTones(); // initializer of the array of tones

  private:
    static std::array<tone, 16> _tones;
    void _playTone(const uint16_t _ui16_active_tone);

    static const uint16_t _ui16_tone_count;

    std::array<bool, 4> _bLaserPinStatus;  // array containing the state of each laser at each tempo
};

#endif

