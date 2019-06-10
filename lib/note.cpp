/*
  note.cpp - notes are statuses of all the lasers connected to the box at one beat
  Created by Cedric Lor, June 10, 2019.
*/

#include "Arduino.h"
#include "note.h"


short int note::_activeNote = 0;
const short int note::NOTE_COUNT = 7;
note note::notes[NOTE_COUNT];

// constructor
note::sequence() {

}

note::note(const char cName[15], const short int iTempo, const short int iLaserPinsStatus[4]){
  strcpy(_cName, cName);
  _iTempo = iTempo;
  for (short __thisPin = 0; __thisPin < PINCOUNT; __thisPin++) {
    _iLaserPinStatus[__thisPin] = iLaserPinsStatus[__thisPin];
  }
}

void note::initNote(const char cName[15], const short int iTempo, const short int iLaserPinsStatus[4]){
  strcpy(_cName, cName);
  _iTempo = iTempo;
  for (short __thisPin = 0; __thisPin < PINCOUNT; __thisPin++) {
    _iLaserPinStatus[__thisPin] = iLaserPinsStatus[__thisPin];
  }
};

void note::initNotes() {
  Serial.println("void note::initNotes(). Starting.");

  const short int aAllOff[4] = {HIGH, HIGH, HIGH, HIGH};
  notes[0].initNote("all off", 30000, aAllOff);
  // Serial.println("void note::initNotes(). notes[0]._iTempo: ");
  // Serial.println(sequences[0]._iTempo);
  // Serial.println("void note::initNotes(). notes[0]._iLaserPinStatus[0]");
  // Serial.println(notes[0]._iLaserPinStatus[0]);

  const short int aFirstOn[4] = {LOW, HIGH, HIGH, HIGH};
  notes[1].initNote("first on", 30000, aFirstOn);
  const short int aSecondOn[4] = {HIGH, LOW, HIGH, HIGH};
  notes[2].initNote("second on", 30000, aSecondOn);
  const short int aThirdOn[4] = {HIGH, HIGH, LOW, HIGH};
  notes[3].initNote("third on", 30000, aThirdOn);
  const short int aFourthOn[4] = {HIGH, HIGH, HIGH, LOW};
  notes[4].initNote("fourth on", 30000, aFourthOn);

  const short int aFirstPairOn[4] = {LOW, LOW, HIGH, HIGH};
  notes[5].initNote("first pair on", 30000, aFirstPairOn);
  const short int aSecondPairOn[4] = {HIGH, HIGH, LOW, LOW};
  notes[6].initNote("second pair on", 30000, aSecondPairOn);

  const short int aOnOffOnOff[4] = {LOW, HIGH, LOW, HIGH};
  notes[7].initNote("on off on off", 30000, aOnOffOnOff);
  const short int aOffOnOffOn[4] = {HIGH, LOW, HIGH, LOW};
  notes[8].initNote("off on off on", 30000, aOffOnOffOn);

  const short int aExtOnIntOff[4] = {LOW, HIGH, HIGH, LOW};
  notes[9].initNote("external on, internal off", 30000, aExtOnIntOff);
  const short int aExtOffIntOn[4] = {HIGH, LOW, LOW, HIGH};
  notes[10].initNote("external off, internal on", 30000, aExtOffIntOn);

  const short int aLastOff[4] = {LOW, LOW, LOW, HIGH};
  notes[11].initNote("last off", 30000, aLastOff);
  const short int aThirdOff[4] = {LOW, LOW, HIGH, LOW};
  notes[12].initNote("third off", 30000, aThirdOff);
  const short int aSecondOff[4] = {LOW, HIGH, LOW, LOW};
  notes[13].initNote("second off", 30000, aThirdOn);
  const short int aFirstOff[4] = {HIGH, LOW, LOW, LOW};
  notes[14].initNote("first off", 30000, aFourthOn);

  const short int aAllOn[4] = {LOW, LOW, LOW, LOW};
  notes[1].initNote("all on", 30000, aAllOn);

  Serial.println("void note::initNotes(). Ending.");
}

void note::testPlay() {
  Serial.println("void note::tcbTestPlay(). Starting.");
  short int __activeNote = 0;
  // Serial.println("void note::tcbTestPlay(). notes[0]._iTempo: ");
  // Serial.println(notes[0]._iTempo);
  // Serial.println("void note::tcbTestPlay(). notes[0]._cName: ");
  // Serial.println(notes[0]._cName);
  // Serial.println("void note::tcbTestPlay(). notes[0]._iLaserPinStatus[0]");
  // Serial.println(notes[0]._iLaserPinStatus[0]);
  setActiveNote(__activeNote);
  playNote();
  Serial.println("void note::tcbTestPlay(). Ending.");
};


void note::playNote(){
  Serial.println("void note::playNote(). Starting");
  // Serial.print("void note::playNote(). Sequence Number: ");
  // Serial.println(noteNumber);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    // Serial.println("void note::_tcbPlayNote(). __thisPin in for loop: ");
    // Serial.println(__thisPin);
    short _physical_pin_number = relayPins[__thisPin]; // look for the physical number of the pin in the array of pin
    // Serial.println("void note::_tcbPlayNote(). _physical_pin_number: ");
    // Serial.println(_physical_pin_number);
    // Serial.println("void note::_tcbPlayNote(). _activeNote: ");
    // Serial.println(_activeNote);
    // Serial.println("void note::_tcbPlayNote(). notes[_activeNote]._iTempo: ");
    // Serial.println(notes[_activeNote]._iTempo);
    // Serial.println("void note::_tcbPlayNote(). notes[_activeNote]._iLaserPinStatus[0]: ");
    // Serial.println(notes[_activeNote]._iLaserPinStatus[0]);
    const short int _target_state = notes[_activeNote]._iLaserPinStatus[__thisPin]; // look for the desired status in the array of the sequence
    // Serial.println("void note::_tcbPlayNote(). _target_state: ");
    // Serial.println(_target_state);
    digitalWrite(_physical_pin_number, _target_state); // instruct the MC to turn the desired pin to the desired status
  }
  Serial.println("void note::playNote(). Ending");
};


void note::setActiveNote(const short activeNote) {
  _activenote = activeNote;
};
