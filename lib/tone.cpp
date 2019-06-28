/*
  tone.cpp - notes are statuses of ALL the lasers connected to the box at one beat
  Notes do not have have a time length. Their time length is defined inside sequences.
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


const short int tone::_note_count = 16;
note tone::notes[_note_count];
const short int tone::_char_count_in_name = 15;

// constructor
tone::note() {

}


void tone::_initNote(const char cName[_char_count_in_name], const short int iLaserPinsStatus[PIN_COUNT]){
  // Serial.println("void tone::initNote(). Starting.");
  strcpy(_cName, cName);
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    _iLaserPinStatus[__thisPin] = iLaserPinsStatus[__thisPin];
  }
  // Serial.println("void tone::initNote(). Ending.");
};

void tone::initTones() {
  Serial.println("void tone::initTones(). Starting.");

  const short int aAllOff[4] = {HIGH, HIGH, HIGH, HIGH};
  notes[0]._initNote("all off", aAllOff);
  // Serial.println("void tone::initTones(). notes[0]._iLaserPinStatus[0]");
  // Serial.println(notes[0]._iLaserPinStatus[0]);

  const short int aFirstOn[4] = {LOW, HIGH, HIGH, HIGH};
  notes[1]._initNote("first on", aFirstOn);
  const short int aSecondOn[4] = {HIGH, LOW, HIGH, HIGH};
  notes[2]._initNote("second on", aSecondOn);
  const short int aThirdOn[4] = {HIGH, HIGH, LOW, HIGH};
  notes[3]._initNote("third on", aThirdOn);
  const short int aFourthOn[4] = {HIGH, HIGH, HIGH, LOW};
  notes[4]._initNote("fourth on", aFourthOn);

  const short int aFirstPairOn[4] = {LOW, LOW, HIGH, HIGH};
  notes[5]._initNote("first pair on", aFirstPairOn);
  const short int aSecondPairOn[4] = {HIGH, HIGH, LOW, LOW};
  notes[6]._initNote("second pair on", aSecondPairOn);

  const short int aOnOffOnOff[4] = {LOW, HIGH, LOW, HIGH};
  notes[7]._initNote("on off on off", aOnOffOnOff);
  const short int aOffOnOffOn[4] = {HIGH, LOW, HIGH, LOW};
  notes[8]._initNote("off on off on", aOffOnOffOn);

  const short int aExtOnIntOff[4] = {LOW, HIGH, HIGH, LOW};
  notes[9]._initNote("external on, internal off", aExtOnIntOff);
  const short int aExtOffIntOn[4] = {HIGH, LOW, LOW, HIGH};
  notes[10]._initNote("external off, internal on", aExtOffIntOn);

  const short int aLastOff[4] = {LOW, LOW, LOW, HIGH};
  notes[11]._initNote("last off", aLastOff);
  const short int aThirdOff[4] = {LOW, LOW, HIGH, LOW};
  notes[12]._initNote("third off", aThirdOff);
  const short int aSecondOff[4] = {LOW, HIGH, LOW, LOW};
  notes[13]._initNote("second off", aSecondOff);
  const short int aFirstOff[4] = {HIGH, LOW, LOW, LOW};
  notes[14]._initNote("first off", aFirstOff);

  const short int aAllOn[4] = {LOW, LOW, LOW, LOW};
  notes[15]._initNote("all on", aAllOn);

  Serial.println("void tone::initTones(). Ending.");
}



void tone::playNote(){
  // Serial.println("void tone::playNote(). Starting");
  // Serial.print("void tone::playNote(). Sequence Number: ");
  // Serial.println(noteNumber);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.println("void tone::_tcbPlayNote(). __thisPin in for loop: ");
    // Serial.println(__thisPin);
    short _physical_pin_number = relayPins[__thisPin]; // look for the physical number of the pin in the array of pin
    // Serial.println("void tone::_tcbPlayNote(). _physical_pin_number: ");
    // Serial.println(_physical_pin_number);
    // Serial.println("void tone::_tcbPlayNote(). _activeNote: ");
    // Serial.println(_activeNote);
    // Serial.println("void tone::_tcbPlayNote(). notes[_activeNote]._iLaserPinStatus[0]: ");
    // Serial.println(notes[_activeNote]._iLaserPinStatus[0]);
    const short int _target_state = _iLaserPinStatus[__thisPin]; // look for the desired status in the array of the sequence
    // Serial.println("void tone::_tcbPlayNote(). _target_state: ");
    // Serial.println(_target_state);
    digitalWrite(_physical_pin_number, _target_state); // instruct the MC to turn the desired pin to the desired status
  }
  // Serial.println("void tone::playNote(). Ending");
};
