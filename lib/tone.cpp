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
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--tone.cpp (called to play some member functions)
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h

*/

#include "Arduino.h"
#include "tone.h"


const short int tone::_tone_count = 16;
tone tone::tones[_tone_count];
const short int tone::_char_count_in_name = 15;

// constructor
tone::tone() {

}


void tone::_initTone(const char cName[_char_count_in_name], const short int iLaserPinsStatus[PIN_COUNT]){
  // Serial.println("void tone::initTone(). Starting.");
  strcpy(_cName, cName);
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    _iLaserPinStatus[__thisPin] = iLaserPinsStatus[__thisPin];
  }
  // Serial.println("void tone::initTone(). Ending.");
};

void tone::initTones() {
  Serial.println("void tone::initTones(). Starting.");

  const short int aAllOff[4] = {HIGH, HIGH, HIGH, HIGH};
  tones[0]._initTone("all off", aAllOff);
  // Serial.println("void tone::initTones(). tones[0]._iLaserPinStatus[0]");
  // Serial.println(tones[0]._iLaserPinStatus[0]);

  const short int aFirstOn[4] = {LOW, HIGH, HIGH, HIGH};
  tones[1]._initTone("first on", aFirstOn);
  const short int aSecondOn[4] = {HIGH, LOW, HIGH, HIGH};
  tones[2]._initTone("second on", aSecondOn);
  const short int aThirdOn[4] = {HIGH, HIGH, LOW, HIGH};
  tones[3]._initTone("third on", aThirdOn);
  const short int aFourthOn[4] = {HIGH, HIGH, HIGH, LOW};
  tones[4]._initTone("fourth on", aFourthOn);

  const short int aFirstPairOn[4] = {LOW, LOW, HIGH, HIGH};
  tones[5]._initTone("first pair on", aFirstPairOn);
  const short int aSecondPairOn[4] = {HIGH, HIGH, LOW, LOW};
  tones[6]._initTone("second pair on", aSecondPairOn);

  const short int aOnOffOnOff[4] = {LOW, HIGH, LOW, HIGH};
  tones[7]._initTone("on off on off", aOnOffOnOff);
  const short int aOffOnOffOn[4] = {HIGH, LOW, HIGH, LOW};
  tones[8]._initTone("off on off on", aOffOnOffOn);

  const short int aExtOnIntOff[4] = {LOW, HIGH, HIGH, LOW};
  tones[9]._initTone("external on, internal off", aExtOnIntOff);
  const short int aExtOffIntOn[4] = {HIGH, LOW, LOW, HIGH};
  tones[10]._initTone("external off, internal on", aExtOffIntOn);

  const short int aLastOff[4] = {LOW, LOW, LOW, HIGH};
  tones[11]._initTone("last off", aLastOff);
  const short int aThirdOff[4] = {LOW, LOW, HIGH, LOW};
  tones[12]._initTone("third off", aThirdOff);
  const short int aSecondOff[4] = {LOW, HIGH, LOW, LOW};
  tones[13]._initTone("second off", aSecondOff);
  const short int aFirstOff[4] = {HIGH, LOW, LOW, LOW};
  tones[14]._initTone("first off", aFirstOff);

  const short int aAllOn[4] = {LOW, LOW, LOW, LOW};
  tones[15]._initTone("all on", aAllOn);

  Serial.println("void tone::initTones(). Ending.");
}



void tone::playTone(){
  // Serial.println("void tone::playTone(). Starting");
  // Serial.print("void tone::playTone(). Sequence Number: ");
  // Serial.println(toneNumber);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.println("void tone::playTone(). __thisPin in for loop: ");
    // Serial.println(__thisPin);
    short _physical_pin_number = relayPins[__thisPin]; // look for the physical number of the pin in the array of pin
    // Serial.println("void tone::playTone(). _physical_pin_number: ");
    // Serial.println(_physical_pin_number);
    // Serial.println("void tone::playTone(). _activeTone: ");
    // Serial.println(_activeTone);
    // Serial.println("void tone::playTone(). tones[_activeTone]._iLaserPinStatus[0]: ");
    // Serial.println(tones[_activeTone]._iLaserPinStatus[0]);
    const short int _target_state = _iLaserPinStatus[__thisPin]; // look for the desired status in the array of the sequence
    // Serial.println("void tone::playTone(). _target_state: ");
    // Serial.println(_target_state);
    digitalWrite(_physical_pin_number, _target_state); // instruct the MC to turn the desired pin to the desired status
  }
  // Serial.println("void tone::playTone(). Ending");
};
