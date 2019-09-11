/*
  tone.h - tones are statuses of all the lasers connected to the box at one base beat
  Created by Cedric Lor, June 10, 2019.

*/
#include "Arduino.h"
#include <global.h>
#include <laserPin.h>

#ifndef tone_h
#define tone_h

class tone
{
  friend class tones;
  friend class note;
  public:
    tone(); // default constructor
    tone(std::array<bool, 4> const __bLaserPinsStatus);

    static void initTones(); // initializer of the array of tones

  private:
    static std::array<tone, 16> _tones;
    void _playTone(const uint16_t _ui16_active_tone);

    std::array<bool, 4> _bLaserPinStatus;  // array containing the state of each laser at each tempo
};

#endif

#ifndef tones_h
#define tones_h
class tones {
  public:
    tones(); // default constructor
  private:
    std::array<laserPin, 4> _laserPins;
    std::array<tone, 16> _array;
};
#endif
