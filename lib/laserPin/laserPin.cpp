/*
  laserPin.cpp - Library to handle the pins connected to the relays controlling the lasers.
  Created by Cedric Lor, July 19, 2019.
*/

#include "Arduino.h"
#include "laserPin.h"


//****************************************************************//
// LaserPin //****************************************************//
//****************************************************************//

///////////////////////////////////
// Constructors
///////////////////////////////////
laserPin::laserPin() { }

laserPin::laserPin(const short __s_index_number, const short __s_physical_pin_number):
  _sIndexNumber{__s_index_number},
  _sPhysicalPinNumber{__s_physical_pin_number}
{
  pinMode(_sPhysicalPinNumber, OUTPUT);  // initialization of the pin connected to each of the relay as output
  _switchPin(HIGH);                       // setting default value of the pins at HIGH (relay closed)
}

///////////////////////////////////
// Destructor
///////////////////////////////////
laserPin::~laserPin()
{
  pinMode(_sPhysicalPinNumber, INPUT);  // resetting the pin as INPUT
}







bool const laserPin::_switchPin(const bool __b_target_state) const {
  digitalWrite(_sPhysicalPinNumber, __b_target_state);
  return __b_target_state;
}



//****************************************************************//
// LaserPins //***************************************************//
//****************************************************************//

///////////////////////////////////
// Constructor
///////////////////////////////////
laserPins::laserPins() 
{ 
  for (short __i = 0; __i < PIN_COUNT; __i++) {
    _laserPins[__i] = {__i, relayPins[__i]}; // relayPins = { 5, 17, 16, 4 }
  }
}

laserPins::~laserPins() {
  for (short __i = 0; __i < PIN_COUNT; __i++) {
    _laserPins[__i].~laserPin(); // relayPins = { 5, 17, 16, 4 }
  }
}

// ESP32 Pin Layout
// LEFT COLUMN
//                    3,3 V           <-- not fit
//                    EN              <-- not fit
// 36 = A0 =          SP;             --> touch
// 39 = A3 =          SM;             --> touch
// 34 = A6 =          G34;            --> touch
// 35 = A7 =          G35;            --> touch
// 32 = A4  = T9 =    G32;            --> touch
// 33 = A5  = T8 =    G33;            --> touch
// 25 = A18 = DAC1 =  G25;            --> touch
// 26 = A19 = DAC2 =  G26;            --> touch
// 27 = A17 = T7 =    G27;            --> touch
// 14 = A16 = T6 =    G14;            --> touch
// 12 = A15 = T5 =    G12;            --> touch
//                    GND;            <-- not fit
// 13 = A14 = T4 =    G13;            --> touch
// = =                SD2;            <-- not found
// = =                SD3;            <-- not found

// RIGHT COLUMN
// fit: --> at least, touch
// = = CLK
// = = SD0
// = = SD1
// 15 = A13 = T3 =    G15;             --> touch
// [2 = A12 = T2 =]   G2;             [<-- no apparent reaction to touch]
// [0 = A11 =]        G0;             [<-- always high]
// 4 = A10 = T0 =     G4;              <-- Test irrelevant; laser output
// 16 = =             G16;             <-- untested; laser output
// 17 = =             G17;             <-- untested; laser output
// 5 = SS =           G5;              <-- untested; laser output
// 18 = SCK =         G18;             <-- untested; laser output
// 19 = MISO =        G19;             --> touch; not very reliable because laser output
//                    GND;             <-- not fit
// 21 = SDA =         G21;             <-- untested; laser output
// 1 = TX =           RXD;             <-- not fit; always high
// 3 = RX =           TXD;             <-- not fit; always high
// 22 = SDL =         G22;             <-- untested; laser output
// 23 = MOSI =        G23              --> touch
//                    GND;             <-- not fit
// 