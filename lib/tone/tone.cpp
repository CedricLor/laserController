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
tone tone::tones[_tone_count];

// constructor
tone::tone() {}

tone::tone(const bool *__bLaserPinsStatus)
{
    for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    _bLaserPinStatus[__thisPin] = __bLaserPinsStatus[__thisPin];
  }
}


void tone::initTones() {
  Serial.println("tone::initTones(). Starting.");

  // Initialize the pins
  laserPin::initLaserPins();

  // Initialize the tones
  /** "all off" */
  const bool _aAllOff[4] = {HIGH, HIGH, HIGH, HIGH};
  tones[0] = {_aAllOff};
  Serial.printf("tone::initTones(). tones[0]._bLaserPinStatus[0] = %i\n", tones[0]._bLaserPinStatus[0]);

  /**"first on"*/
  const bool _aFirstOn[4] = {LOW, HIGH, HIGH, HIGH};
  tones[1] = {_aFirstOn};
  /**"second on*/
  const bool _aSecondOn[4] = {HIGH, LOW, HIGH, HIGH};
  tones[2] = {_aSecondOn};
  /**"third on"*/
  const bool _aThirdOn[4] = {HIGH, HIGH, LOW, HIGH};
  tones[3] = {_aThirdOn};
  /**"fourth on"*/
  const bool _aFourthOn[4] = {HIGH, HIGH, HIGH, LOW};
  tones[4] = {_aFourthOn};

  /**"first pair on"*/
  const bool _aFirstPairOn[4] = {LOW, LOW, HIGH, HIGH};
  tones[5] = {_aFirstPairOn};
  /**"second pair on*/
  const bool _aSecondPairOn[4] = {HIGH, HIGH, LOW, LOW};
  tones[6] = {_aSecondPairOn};

  /**"on off on off"*/
  const bool _aOnOffOnOff[4] = {LOW, HIGH, LOW, HIGH};
  tones[7] = {_aOnOffOnOff};
  /**"off on off on"*/
  const bool _aOffOnOffOn[4] = {HIGH, LOW, HIGH, LOW};
  tones[8] = {_aOffOnOffOn};

  /**"external on"*/
  const bool _aExtOnIntOff[4] = {LOW, HIGH, HIGH, LOW};
  tones[9] = {_aExtOnIntOff};
  /**"external off*/
  const bool _aExtOffIntOn[4] = {HIGH, LOW, LOW, HIGH};
  tones[10] = {_aExtOffIntOn};
  /**"last off"*/
  const bool _aLastOff[4] = {LOW, LOW, LOW, HIGH};
  tones[11] = {_aLastOff};
  /**"third off"*/
  const bool _aThirdOff[4] = {LOW, LOW, HIGH, LOW};
  tones[12] = {_aThirdOff};
  /**"second off"*/
  const bool _aSecondOff[4] = {LOW, HIGH, LOW, LOW};
  tones[13] = {_aSecondOff};
  /**"first off"*/
  const bool _aFirstOff[4] = {HIGH, LOW, LOW, LOW};
  tones[14] = {_aFirstOff};

  /**"all on"*/
  const bool _aAllOn[4] = {LOW, LOW, LOW, LOW};
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
    short _physical_pin_number = relayPins[__thisPin]; // look for the physical number of the pin in the array of pin
    const bool _target_state = _bLaserPinStatus[__thisPin]; // look for the desired status in the array of the sequence
    // Serial.print("void tone::playTone(). _physical_pin_number: ");Serial.print(_physical_pin_number);Serial.print(" _target_state: ");Serial.println(_target_state);
    digitalWrite(_physical_pin_number, _target_state); // instruct the MC to turn the desired pin to the desired status
  }
  Serial.printf("void tone::playTone(). Ending. Tone being played: %i \n", _ui16_active_tone);
};



