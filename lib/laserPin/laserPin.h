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

  public:
    laserPin();
    laserPin(const uint16_t __ui16_physical_pin_number, const int16_t __i16_index_number=-1);
    laserPin(const laserPin& __laserPin);
    laserPin& operator=(const laserPin& __laserPin);
    ~laserPin();

  private:
    bool const _switchPin(const bool __b_target_state) const;

    int16_t _i16IndexNumber;          // index number in the array of _laserPins
    uint16_t _ui16PhysicalPinNumber;   // physical number of the pin controlling the relay
};

#endif

#ifndef laserPins_h
#define laserPins_h

constexpr uint16_t    PIN_COUNT                           = 4;

class laserPins
{
  friend class test;

  public:
    laserPins();
    ~laserPins();
    laserPin const & at (uint16_t const __ui16IndexNumber) const;
  private:
    std::array<laserPin, PIN_COUNT> _array;
};
#endif
