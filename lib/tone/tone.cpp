/*
  tone.cpp - tones are statuses of ALL the lasers connected to the box at one base beat
  Tones do not have have a time length. Their time length is defined inside sequences.
  Created by Cedric Lor, June 10, 2019.

*/

#include "Arduino.h"
#include "tone.h"


std::array<tone, 16> tone::_tones;
laserPins tone::_laserPins;

// constructor
tone::tone():
  _bLaserPinStatus({})
{}

tone::tone(std::array<bool, 4> const __bLaserPinsStatus):
  _bLaserPinStatus(__bLaserPinsStatus)
{}


void tone::initTones() {
  Serial.println("tone::initTones(). Starting.");

  // Initialize the tones
  /** "all off" */
  std::array<bool, 4> const _aAllOff = {HIGH, HIGH, HIGH, HIGH};
  _tones[0] = {_aAllOff};
  Serial.printf("tone::initTones(). _tones[0]._bLaserPinStatus[0] = %i\n", _tones[0]._bLaserPinStatus[0]);

  /**"first on"*/
  std::array<bool, 4> const _aFirstOn = {LOW, HIGH, HIGH, HIGH};
  _tones[1] = {_aFirstOn};
  /**"second on*/
  std::array<bool, 4> const _aSecondOn = {HIGH, LOW, HIGH, HIGH};
  _tones[2] = {_aSecondOn};
  /**"third on"*/
  std::array<bool, 4> const _aThirdOn = {HIGH, HIGH, LOW, HIGH};
  _tones[3] = {_aThirdOn};
  /**"fourth on"*/
  std::array<bool, 4> const _aFourthOn = {HIGH, HIGH, HIGH, LOW};
  _tones[4] = {_aFourthOn};

  /**"first pair on"*/
  std::array<bool, 4> const _aFirstPairOn = {LOW, LOW, HIGH, HIGH};
  _tones[5] = {_aFirstPairOn};
  /**"second pair on*/
  std::array<bool, 4> const _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  _tones[6] = {_aSecondPairOn};

  /**"on off on off"*/
  std::array<bool, 4> const _aOnOffOnOff = {LOW, HIGH, LOW, HIGH};
  _tones[7] = {_aOnOffOnOff};
  /**"off on off on"*/
  std::array<bool, 4> const _aOffOnOffOn = {HIGH, LOW, HIGH, LOW};
  _tones[8] = {_aOffOnOffOn};

  /**"external on"*/
  std::array<bool, 4> const _aExtOnIntOff = {LOW, HIGH, HIGH, LOW};
  _tones[9] = {_aExtOnIntOff};
  /**"external off*/
  std::array<bool, 4> const _aExtOffIntOn = {HIGH, LOW, LOW, HIGH};
  _tones[10] = {_aExtOffIntOn};
  /**"last off"*/
  std::array<bool, 4> const _aLastOff = {LOW, LOW, LOW, HIGH};
  _tones[11] = {_aLastOff};
  /**"third off"*/
  std::array<bool, 4> const _aThirdOff = {LOW, LOW, HIGH, LOW};
  _tones[12] = {_aThirdOff};
  /**"second off"*/
  std::array<bool, 4> const _aSecondOff = {LOW, HIGH, LOW, LOW};
  _tones[13] = {_aSecondOff};
  /**"first off"*/
  std::array<bool, 4> const _aFirstOff = {HIGH, LOW, LOW, LOW};
  _tones[14] = {_aFirstOff};

  /**"all on"*/
  std::array<bool, 4> const _aAllOn = {LOW, LOW, LOW, LOW};
  _tones[15] = {_aAllOn};

  Serial.println("tone::initTones(). Ending.");
}



/* _playTone()

  tone::_tones[_activeTone]._playTone();

  _playTone() is called as a member of an instance of _tones[index].
*/
void tone::_playTone(const uint16_t _ui16_active_tone){
  Serial.printf("tone::_playTone(). Starting. Tone about to be played: %i\n", _ui16_active_tone);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.print("tone::_playTone(). __thisPin in for loop. Iteration ");Serial.println(__thisPin);
    _laserPins._laserPins[__thisPin]._switchPin(_bLaserPinStatus[__thisPin]);
    // Serial.print("tone::_playTone(). _physical_pin_number: ");Serial.print(_physical_pin_number);Serial.print(" _target_state: ");Serial.println(_target_state);
  }
  Serial.printf("tone::_playTone(). Ending. Tone being played: %i \n", _ui16_active_tone);
};




// TONES CLASS
// default constructor
tones::tones() {
  Serial.println("tones::tones(). Starting.");

  // Initialize the tones
  /** "all off" */
  std::array<bool, 4> const _aAllOff = {HIGH, HIGH, HIGH, HIGH};
  _array[0] = {_aAllOff};
  Serial.printf("tones::tones(). _array[0]._bLaserPinStatus[0] = %i\n", _array[0]._bLaserPinStatus[0]);

  /**"first on"*/
  std::array<bool, 4> const _aFirstOn = {LOW, HIGH, HIGH, HIGH};
  _array[1] = {_aFirstOn};
  /**"second on*/
  std::array<bool, 4> const _aSecondOn = {HIGH, LOW, HIGH, HIGH};
  _array[2] = {_aSecondOn};
  /**"third on"*/
  std::array<bool, 4> const _aThirdOn = {HIGH, HIGH, LOW, HIGH};
  _array[3] = {_aThirdOn};
  /**"fourth on"*/
  std::array<bool, 4> const _aFourthOn = {HIGH, HIGH, HIGH, LOW};
  _array[4] = {_aFourthOn};

  /**"first pair on"*/
  std::array<bool, 4> const _aFirstPairOn = {LOW, LOW, HIGH, HIGH};
  _array[5] = {_aFirstPairOn};
  /**"second pair on*/
  std::array<bool, 4> const _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  _array[6] = {_aSecondPairOn};

  /**"on off on off"*/
  std::array<bool, 4> const _aOnOffOnOff = {LOW, HIGH, LOW, HIGH};
  _array[7] = {_aOnOffOnOff};
  /**"off on off on"*/
  std::array<bool, 4> const _aOffOnOffOn = {HIGH, LOW, HIGH, LOW};
  _array[8] = {_aOffOnOffOn};

  /**"external on"*/
  std::array<bool, 4> const _aExtOnIntOff = {LOW, HIGH, HIGH, LOW};
  _array[9] = {_aExtOnIntOff};
  /**"external off*/
  std::array<bool, 4> const _aExtOffIntOn = {HIGH, LOW, LOW, HIGH};
  _array[10] = {_aExtOffIntOn};
  /**"last off"*/
  std::array<bool, 4> const _aLastOff = {LOW, LOW, LOW, HIGH};
  _array[11] = {_aLastOff};
  /**"third off"*/
  std::array<bool, 4> const _aThirdOff = {LOW, LOW, HIGH, LOW};
  _array[12] = {_aThirdOff};
  /**"second off"*/
  std::array<bool, 4> const _aSecondOff = {LOW, HIGH, LOW, LOW};
  _array[13] = {_aSecondOff};
  /**"first off"*/
  std::array<bool, 4> const _aFirstOff = {HIGH, LOW, LOW, LOW};
  _array[14] = {_aFirstOff};

  /**"all on"*/
  std::array<bool, 4> const _aAllOn = {LOW, LOW, LOW, LOW};
  _array[15] = {_aAllOn};

  Serial.println("tones::tones(). Ending.");
}

