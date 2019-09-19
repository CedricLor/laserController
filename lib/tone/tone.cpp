/*
  tone.cpp - tones are statuses of ALL the lasers connected to the box at one base beat
  Tones do not have have a time length. Their time length is defined inside sequences.
  Created by Cedric Lor, June 10, 2019.

*/

#include "Arduino.h"
#include "tone.h"

//****************************************************************//
// Tone //********************************************************//
//****************************************************************//

// constructor
tone::tone():
  _laserPinsStatesArr({})
{}

tone::tone(std::array<bool, 4> const __laser_pins_states_arr, const int16_t __i16_index_number):
  _laserPinsStatesArr(__laser_pins_states_arr),
  i16IndexNumber(__i16_index_number)
{}

tone::tone(const tone & __tone):
  _laserPinsStatesArr(__tone._laserPinsStatesArr),
  i16IndexNumber(__tone.i16IndexNumber)
{}

tone& tone::operator=(const tone& __tone)
{
  if (&__tone != this) {
    _laserPinsStatesArr = __tone._laserPinsStatesArr;
    i16IndexNumber = __tone.i16IndexNumber;
  }
  return *this;
}


/** tone::_playTone()
 *   
 *  _playTone() is an instance method of a tone instance. A tone instance
 *  stores in its _laserPinsStatesArr the state of each pin for this tone.
 *  
 *  _playTone() iterates over the __laser_pins array passed as parameters 
 *  and switches each pin to the corresponding state (HIGH or LOW) stored in 
 *  _laserPinsStatesArr.
*/
int16_t const tone::_playTone(const laserPins & __laser_pins) const {
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    __laser_pins.at(__thisPin)._switchPin(_laserPinsStatesArr[__thisPin]);
  }
  return i16IndexNumber;
};




//****************************************************************//
// Tones //*******************************************************//
//****************************************************************//

// default constructor
tones::tones() :
  _laserPins(laserPins{})
{

  // Initialize the tones' _array: _tonesArray
  /** "all off" */
  _tonesArray[0] = {std::array<bool, 4>{HIGH, HIGH, HIGH, HIGH}, 0};

  /**"first on"*/
  _tonesArray[1] = {std::array<bool, 4>{LOW, HIGH, HIGH, HIGH}, 1};
  /**"second on*/
  _tonesArray[2] = {std::array<bool, 4>{HIGH, LOW, HIGH, HIGH}, 2};
  /**"third on"*/
  _tonesArray[3] = {std::array<bool, 4>{HIGH, HIGH, LOW, HIGH}, 3};
  /**"fourth on"*/
  _tonesArray[4] = {std::array<bool, 4>{HIGH, HIGH, HIGH, LOW}, 4};

  /**"first pair on"*/
  _tonesArray[5] = {std::array<bool, 4>{LOW, LOW, HIGH, HIGH}, 5};
  /**"second pair on*/
  _tonesArray[6] = {std::array<bool, 4>{HIGH, HIGH, LOW, LOW}, 6};

  /**"on off on off"*/
  _tonesArray[7] = {std::array<bool, 4>{LOW, HIGH, LOW, HIGH}, 7};
  /**"off on off on"*/
  _tonesArray[8] = {std::array<bool, 4>{HIGH, LOW, HIGH, LOW}, 8};

  /**"two external lasers on, two internal lasers off"*/
  _tonesArray[9] = {std::array<bool, 4>{LOW, HIGH, HIGH, LOW}, 9};
  /**"two external lasers off, two internal lasers on*/
  _tonesArray[10] = {std::array<bool, 4>{HIGH, LOW, LOW, HIGH}, 10};
  /**"last off"*/
  _tonesArray[11] = {std::array<bool, 4>{LOW, LOW, LOW, HIGH}, 11};
  /**"third off"*/
  _tonesArray[12] = {std::array<bool, 4>{LOW, LOW, HIGH, LOW}, 12};
  /**"second off"*/
  _tonesArray[13] = {std::array<bool, 4>{LOW, HIGH, LOW, LOW}, 13};
  /**"first off"*/
  _tonesArray[14] = {std::array<bool, 4>{HIGH, LOW, LOW, LOW}, 14};

  /**"all on"*/
  _tonesArray[15] = {std::array<bool, 4>{LOW, LOW, LOW, LOW}, 15};
}




tone const & tones::at (uint16_t const __ui16IndexNumber) const {
  return _tonesArray[__ui16IndexNumber];
}




/** tones::_playTone()
 *   
 *  _playTone() is an instance method of a tones instance.
 *  
 *  _playTone() is mainly a wrapper over the _playTone method of tone instances.
*/
int16_t const tones::_playTone(const uint16_t __ui16ToneNumber) {
  return at(__ui16ToneNumber)._playTone(_laserPins);
};