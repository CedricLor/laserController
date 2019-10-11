/*
  laserTone.h - tone instances are statuses of ALL the lasers connected to the box at one base beat.
  Created by Cedric Lor, June 10, 2019.

*/
#include "Arduino.h"
#include <global.h>
#include <laserPin.h>

#ifndef laserTone_h
#define laserTone_h

class laserTone
{
  friend class laserTones;
  friend class test;

  private:
    int16_t const _playLaserTone(const laserPins & __laser_pins) const;
    std::array<bool, 4> _laserPinsStatesArr;  // array containing the state of each laser at each tempo

  public:
    laserTone(); // default constructor
    laserTone(std::array<bool, 4> const __laser_pins_states, const int16_t __i16_index_number=-1);
    laserTone(const laserTone & __laserTone);
    laserTone & operator=(const laserTone & __laserTone);

    int16_t i16IndexNumber;
};

#endif

#ifndef laserTones_h
#define laserTones_h
class laserTones {
  friend class note;
  friend class test;
  
  public:
    laserTones(); // default constructor
    laserTone const & at (uint16_t const __ui16IndexNumber) const;
  private:
    /** player */
    int16_t const _playLaserTone(const uint16_t __ui16LaserToneNumber) const;
    /** members */
    laserPins _laserPins;
    std::array<laserTone, 16> _laserTonesArray;
};
#endif
