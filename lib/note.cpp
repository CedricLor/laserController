/*
  note.cpp - notes are tones for a given length (in base beat)
  Created by Cedric Lor, June 28, 2019.

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
  |  |  |  |--note.cpp (called to play some member functions)
  |  |  |  |  |--note.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h

*/

#include "Arduino.h"
#include "note.h"


const short int note::_note_count = 16;
note note::notes[_note_count];
const short int note::_char_count_in_name = 15;

// constructor
note::note() {

}


void note::_initNote(const char cName[_char_count_in_name], const short int iLaserPinsStatus[PIN_COUNT]){
  // Serial.println("void note::initNote(). Starting.");
  strcpy(_cName, cName);
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    _iLaserPinStatus[__thisPin] = iLaserPinsStatus[__thisPin];
  }
  // Serial.println("void note::initNote(). Ending.");
};

void note::initNotes() {
  Serial.println("void note::initNotes(). Starting.");

  const short int aAllOff[4] = {HIGH, HIGH, HIGH, HIGH};
  notes[0]._initNote("all off", aAllOff);
  // Serial.println("void note::initNotes(). notes[0]._iLaserPinStatus[0]");
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

  Serial.println("void note::initNotes(). Ending.");
}



void note::playNote(){
  // Serial.println("void note::playNote(). Starting");
  // Serial.print("void note::playNote(). Sequence Number: ");
  // Serial.println(noteNumber);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.println("void note::playNote(). __thisPin in for loop: ");
    // Serial.println(__thisPin);
    short _physical_pin_number = relayPins[__thisPin]; // look for the physical number of the pin in the array of pin
    // Serial.println("void note::playNote(). _physical_pin_number: ");
    // Serial.println(_physical_pin_number);
    // Serial.println("void note::playNote(). _activenote: ");
    // Serial.println(_activeNote);
    // Serial.println("void note::playNote(). notes[_activeNote]._iLaserPinStatus[0]: ");
    // Serial.println(notes[_activeNote]._iLaserPinStatus[0]);
    const short int _target_state = _iLaserPinStatus[__thisPin]; // look for the desired status in the array of the sequence
    // Serial.println("void note::playNote(). _target_state: ");
    // Serial.println(_target_state);
    digitalWrite(_physical_pin_number, _target_state); // instruct the MC to turn the desired pin to the desired status
  }
  // Serial.println("void note::playNote(). Ending");
};
