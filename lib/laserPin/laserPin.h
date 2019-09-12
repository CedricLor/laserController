/*
  laserPin.h - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, July 19, 2019.
*/

#include "Arduino.h"
#include <global.h>

#ifndef laserPin_h
#define laserPin_h

class laserPin
{
  friend class test;
  friend class tone;
  friend class tones;
  public:
    laserPin();
    laserPin(const short __s_index_number, const short __s_physical_pin_number);
    ~laserPin();

  private:
    bool const _switchPin(const bool __b_target_state) const;

    short _sIndexNumber;          // index number in the array of _laserPins
    short _sPhysicalPinNumber;   // physical number of the pin controlling the relay
};

#endif

#ifndef laserPins_h
#define laserPins_h

constexpr short    PIN_COUNT                           = 4;

class laserPins
{
  friend class test;
  friend class tone;
  friend class tones;
  public:
    laserPins();
    ~laserPins();
  private:
    std::array<laserPin, PIN_COUNT> _laserPins;
};
#endif
