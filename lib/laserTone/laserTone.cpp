/*
  laserTone.cpp - tone instances are statuses of ALL the lasers connected to the box at one base beat.
  Tone instances do not have have a time length. Their time length is defined inside sequences.
  Created by Cedric Lor, June 10, 2019.

*/

#include "Arduino.h"
#include "laserTone.h"

//****************************************************************//
// laserTone //********************************************************//
//****************************************************************//

// constructor
laserTone::laserTone():
  _laserPinsStatesArr({})
{}

laserTone::laserTone(std::array<bool, 4> const __laser_pins_states_arr, const int16_t __i16_index_number):
  _laserPinsStatesArr(__laser_pins_states_arr),
  i16IndexNumber(__i16_index_number)
{}

laserTone::laserTone(const laserTone & __laserTone):
  _laserPinsStatesArr(__laserTone._laserPinsStatesArr),
  i16IndexNumber(__laserTone.i16IndexNumber)
{}

laserTone & laserTone::operator=(const laserTone & __laserTone)
{
  if (&__laserTone != this) {
    _laserPinsStatesArr = __laserTone._laserPinsStatesArr;
    i16IndexNumber = __laserTone.i16IndexNumber;
  }
  return *this;
}


/** laserTone::_playLaserTone()
 *   
 *  _playLaserTone() is an instance method of a laserTone instance. A laserTone instance
 *  stores in its _laserPinsStatesArr the state of each pin for this laserTone.
 *  
 *  _playLaserTone() iterates over the __laser_pins array passed as parameters 
 *  and switches each pin to the corresponding state (HIGH or LOW) stored in 
 *  _laserPinsStatesArr.
*/
int16_t const laserTone::_playLaserTone(const laserPins & __laser_pins) const {
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    __laser_pins.at(__thisPin)._switchPin(_laserPinsStatesArr[__thisPin]);
  }
  return i16IndexNumber;
};




//****************************************************************//
// laserTones //*******************************************************//
//****************************************************************//

// default constructor
laserTones::laserTones(
  myMeshViews & __thisMeshViews
) :
  _thisMeshViews(__thisMeshViews),
  _laserPins(laserPins{})
{

  // Initialize the laserTones' _array: _laserTonesArray
  /** "all off" */
  _laserTonesArray[0] = {std::array<bool, 4>{HIGH, HIGH, HIGH, HIGH}, 0};

  /**"first on"*/
  _laserTonesArray[1] = {std::array<bool, 4>{LOW, HIGH, HIGH, HIGH}, 1};
  /**"second on*/
  _laserTonesArray[2] = {std::array<bool, 4>{HIGH, LOW, HIGH, HIGH}, 2};
  /**"third on"*/
  _laserTonesArray[3] = {std::array<bool, 4>{HIGH, HIGH, LOW, HIGH}, 3};
  /**"fourth on"*/
  _laserTonesArray[4] = {std::array<bool, 4>{HIGH, HIGH, HIGH, LOW}, 4};

  /**"first pair on"*/
  _laserTonesArray[5] = {std::array<bool, 4>{LOW, LOW, HIGH, HIGH}, 5};
  /**"second pair on*/
  _laserTonesArray[6] = {std::array<bool, 4>{HIGH, HIGH, LOW, LOW}, 6};

  /**"on off on off"*/
  _laserTonesArray[7] = {std::array<bool, 4>{LOW, HIGH, LOW, HIGH}, 7};
  /**"off on off on"*/
  _laserTonesArray[8] = {std::array<bool, 4>{HIGH, LOW, HIGH, LOW}, 8};

  /**"two external lasers on, two internal lasers off"*/
  _laserTonesArray[9] = {std::array<bool, 4>{LOW, HIGH, HIGH, LOW}, 9};
  /**"two external lasers off, two internal lasers on*/
  _laserTonesArray[10] = {std::array<bool, 4>{HIGH, LOW, LOW, HIGH}, 10};
  /**"last off"*/
  _laserTonesArray[11] = {std::array<bool, 4>{LOW, LOW, LOW, HIGH}, 11};
  /**"third off"*/
  _laserTonesArray[12] = {std::array<bool, 4>{LOW, LOW, HIGH, LOW}, 12};
  /**"second off"*/
  _laserTonesArray[13] = {std::array<bool, 4>{LOW, HIGH, LOW, LOW}, 13};
  /**"first off"*/
  _laserTonesArray[14] = {std::array<bool, 4>{HIGH, LOW, LOW, LOW}, 14};

  /**"all on"*/
  _laserTonesArray[15] = {std::array<bool, 4>{LOW, LOW, LOW, LOW}, 15};
}




laserTone const & laserTones::at (uint16_t const __ui16IndexNumber) const {
  return _laserTonesArray[__ui16IndexNumber];
}




/** laserTones::_playLaserTone()
 *   
 *  _playLaserTone() is an instance method of a laserTones instance.
 *  
 *  _playLaserTone() is mainly a wrapper over the _playLaserTone method of laserTone instances.
*/
int16_t const laserTones::_playLaserTone(const uint16_t __ui16LaserToneNumber) const {
  _thisMeshViews.sendTone(__ui16LaserToneNumber);
  return at(__ui16LaserToneNumber)._playLaserTone(_laserPins);
};