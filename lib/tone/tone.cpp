/*
  tone.cpp - tones are statuses of ALL the lasers connected to the box at one base beat
  Tones do not have have a time length. Their time length is defined inside sequences.
  Created by Cedric Lor, June 10, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |  |  |--global.h


*/

#include "Arduino.h"
#include "tone.h"


const uint16_t tone::_ui16_tone_count = 16;
std::array<tone, 16> tone::tones;

// constructor
tone::tone() {}

tone::tone(std::array<bool, 4> __bLaserPinsStatus):
  _bLaserPinStatus(__bLaserPinsStatus)
{
}


void tone::initTones() {
  Serial.println("tone::initTones(). Starting.");

  // Initialize the pins
  laserPin::_initLaserPins();

  // Initialize the tones
  /** "all off" */
  std::array<bool, 4> _aAllOff = {HIGH, HIGH, HIGH, HIGH};
  tones[0] = {_aAllOff};
  Serial.printf("tone::initTones(). tones[0]._bLaserPinStatus[0] = %i\n", tones[0]._bLaserPinStatus[0]);

  /**"first on"*/
  std::array<bool, 4> _aFirstOn = {LOW, HIGH, HIGH, HIGH};
  tones[1] = {_aFirstOn};
  /**"second on*/
  std::array<bool, 4> _aSecondOn = {HIGH, LOW, HIGH, HIGH};
  tones[2] = {_aSecondOn};
  /**"third on"*/
  std::array<bool, 4> _aThirdOn = {HIGH, HIGH, LOW, HIGH};
  tones[3] = {_aThirdOn};
  /**"fourth on"*/
  std::array<bool, 4> _aFourthOn = {HIGH, HIGH, HIGH, LOW};
  tones[4] = {_aFourthOn};

  /**"first pair on"*/
  std::array<bool, 4> _aFirstPairOn = {LOW, LOW, HIGH, HIGH};
  tones[5] = {_aFirstPairOn};
  /**"second pair on*/
  std::array<bool, 4> _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  tones[6] = {_aSecondPairOn};

  /**"on off on off"*/
  std::array<bool, 4> _aOnOffOnOff = {LOW, HIGH, LOW, HIGH};
  tones[7] = {_aOnOffOnOff};
  /**"off on off on"*/
  std::array<bool, 4> _aOffOnOffOn = {HIGH, LOW, HIGH, LOW};
  tones[8] = {_aOffOnOffOn};

  /**"external on"*/
  std::array<bool, 4> _aExtOnIntOff = {LOW, HIGH, HIGH, LOW};
  tones[9] = {_aExtOnIntOff};
  /**"external off*/
  std::array<bool, 4> _aExtOffIntOn = {HIGH, LOW, LOW, HIGH};
  tones[10] = {_aExtOffIntOn};
  /**"last off"*/
  std::array<bool, 4> _aLastOff = {LOW, LOW, LOW, HIGH};
  tones[11] = {_aLastOff};
  /**"third off"*/
  std::array<bool, 4> _aThirdOff = {LOW, LOW, HIGH, LOW};
  tones[12] = {_aThirdOff};
  /**"second off"*/
  std::array<bool, 4> _aSecondOff = {LOW, HIGH, LOW, LOW};
  tones[13] = {_aSecondOff};
  /**"first off"*/
  std::array<bool, 4> _aFirstOff = {HIGH, LOW, LOW, LOW};
  tones[14] = {_aFirstOff};

  /**"all on"*/
  std::array<bool, 4> _aAllOn = {LOW, LOW, LOW, LOW};
  tones[15] = {_aAllOn};

  Serial.println("tone::initTones(). Ending.");
}



/* playTone()

  tone::tones[_activeTone].playTone();

  playTone() is called as a member of an instance of tones[index].
*/
void tone::playTone(const uint16_t _ui16_active_tone){
  Serial.printf("void tone::playTone(). Starting. Tone about to be played: %i\n", _ui16_active_tone);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.print("void tone::playTone(). __thisPin in for loop. Iteration ");Serial.println(__thisPin);
    laserPin::_laserPins[__thisPin]._switchPin(_bLaserPinStatus[__thisPin]);
    // Serial.print("void tone::playTone(). _physical_pin_number: ");Serial.print(_physical_pin_number);Serial.print(" _target_state: ");Serial.println(_target_state);
  }
  Serial.printf("void tone::playTone(). Ending. Tone being played: %i \n", _ui16_active_tone);
};



