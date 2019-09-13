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
  friend class test;
  friend class tones;
  friend class note;
  friend class notes;

  private:
    int16_t const _playTone(const laserPins & __laser_pins);
    std::array<bool, 4> _laserPinsStatesArr;  // array containing the state of each laser at each tempo

  public:
    tone(); // default constructor
    tone(std::array<bool, 4> const __laser_pins_states, const int16_t _i16_index_number=-1);
    int16_t i16IndexNumber;

};

#endif

#ifndef tones_h
#define tones_h
class tones {
  friend class test;
  friend class note;
  friend class notes;
  
  public:
    tones(); // default constructor
  private:
    laserPins _laserPins;
    std::array<tone, 16> _array;
};
#endif
