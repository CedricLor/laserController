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

tone::tone(std::array<bool, 4> const __laser_pins_states_arr, const int16_t _i16_index_number):
  _laserPinsStatesArr(__laser_pins_states_arr),
  i16IndexNumber(_i16_index_number)
{}



/* _playTone()

  tone::_tones[_activeTone]._playTone();

  _playTone() is called as a member of an instance of _tones[index].
*/
uint16_t const tone::_playTone(const uint16_t _ui16_active_tone, const laserPins & __laser_pins){
  Serial.printf("tone::_playTone(). Starting. Tone about to be played: %i\n", _ui16_active_tone);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.print("tone::_playTone(). __thisPin in for loop. Iteration ");Serial.println(__thisPin);
    __laser_pins._array.at(__thisPin)._switchPin(_laserPinsStatesArr[__thisPin]);
    // Serial.print("tone::_playTone(). _physical_pin_number: ");Serial.print(_physical_pin_number);Serial.print(" _target_state: ");Serial.println(_target_state);
  }
  Serial.printf("tone::_playTone(). Ending. Tone being played: %i \n", _ui16_active_tone);
  return _ui16_active_tone;
};




//****************************************************************//
// Tones //*******************************************************//
//****************************************************************//

// default constructor
tones::tones() :
  _laserPins(laserPins{})
{

  // Initialize the tones' _array
  /** "all off" */
  std::array<bool, 4> const _aAllOff = {HIGH, HIGH, HIGH, HIGH};
  _array[0] = {_aAllOff, 0};

  /**"first on"*/
  std::array<bool, 4> const _aFirstOn = {LOW, HIGH, HIGH, HIGH};
  _array[1] = {_aFirstOn, 1};
  /**"second on*/
  std::array<bool, 4> const _aSecondOn = {HIGH, LOW, HIGH, HIGH};
  _array[2] = {_aSecondOn, 2};
  /**"third on"*/
  std::array<bool, 4> const _aThirdOn = {HIGH, HIGH, LOW, HIGH};
  _array[3] = {_aThirdOn, 3};
  /**"fourth on"*/
  std::array<bool, 4> const _aFourthOn = {HIGH, HIGH, HIGH, LOW};
  _array[4] = {_aFourthOn, 4};

  /**"first pair on"*/
  std::array<bool, 4> const _aFirstPairOn = {LOW, LOW, HIGH, HIGH};
  _array[5] = {_aFirstPairOn, 5};
  /**"second pair on*/
  std::array<bool, 4> const _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  _array[6] = {_aSecondPairOn, 6};

  /**"on off on off"*/
  std::array<bool, 4> const _aOnOffOnOff = {LOW, HIGH, LOW, HIGH};
  _array[7] = {_aOnOffOnOff, 7};
  /**"off on off on"*/
  std::array<bool, 4> const _aOffOnOffOn = {HIGH, LOW, HIGH, LOW};
  _array[8] = {_aOffOnOffOn, 8};

  /**"external on"*/
  std::array<bool, 4> const _aExtOnIntOff = {LOW, HIGH, HIGH, LOW};
  _array[9] = {_aExtOnIntOff, 9};
  /**"external off*/
  std::array<bool, 4> const _aExtOffIntOn = {HIGH, LOW, LOW, HIGH};
  _array[10] = {_aExtOffIntOn, 10};
  /**"last off"*/
  std::array<bool, 4> const _aLastOff = {LOW, LOW, LOW, HIGH};
  _array[11] = {_aLastOff, 11};
  /**"third off"*/
  std::array<bool, 4> const _aThirdOff = {LOW, LOW, HIGH, LOW};
  _array[12] = {_aThirdOff, 12};
  /**"second off"*/
  std::array<bool, 4> const _aSecondOff = {LOW, HIGH, LOW, LOW};
  _array[13] = {_aSecondOff, 13};
  /**"first off"*/
  std::array<bool, 4> const _aFirstOff = {HIGH, LOW, LOW, LOW};
  _array[14] = {_aFirstOff, 14};

  /**"all on"*/
  std::array<bool, 4> const _aAllOn = {LOW, LOW, LOW, LOW};
  _array[15] = {_aAllOn, 15};
}

