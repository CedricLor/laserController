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
  friend class test;

  private:
    int16_t const _playTone(const laserPins & __laser_pins) const;
    std::array<bool, 4> _laserPinsStatesArr;  // array containing the state of each laser at each tempo

  public:
    tone(); // default constructor
    tone(std::array<bool, 4> const __laser_pins_states, const int16_t __i16_index_number=-1);
    tone(const tone & __tone);
    tone& operator=(const tone& __tone);

    int16_t i16IndexNumber;
};

#endif

#ifndef tones_h
#define tones_h
class tones {
  friend class note;
  friend class test;
  
  public:
    tones(); // default constructor
    tone const & at (uint16_t const __ui16IndexNumber) const;
  private:
    /** player */
    int16_t const _playTone(const uint16_t __ui16ToneNumber) const;
    /** members */
    laserPins _laserPins;
    std::array<tone, 16> _tonesArray;
};
#endif
