/*
  laserPin.h - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, July 19, 2019.
*/

#include "Arduino.h"
#include <global.h>

#ifndef laserPin_h
#define laserPin_h

constexpr short    PIN_COUNT                           = 4;

class laserPin
{
  friend class tone;
  friend class tones;
  public:
    laserPin();
    laserPin(const short __s_index_number, const short __s_physical_pin_number);
    ~laserPin();

  private:
    static void _initLaserPins();

    static laserPin _laserPins[];

    void _switchPin(const bool __b_target_state);

    short _sIndexNumber;          // index number in the array of _laserPins
    short _sPhysicalPinNumber;   // physical number of the pin controlling the relay
};

#endif

#ifndef laserPins_h
#define laserPins_h
class laserPins
{
  friend class tone;
  friend class tones;
  public:
    laserPins();
  private:
    std::array<laserPin, PIN_COUNT> _laserPins;
};
#endif
