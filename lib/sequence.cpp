/*
  sequence.cpp - sequences corresponding to pre-determined blinking patterns of lasers
  Created by Cedric Lor, June 4, 2019.
*/

#include "Arduino.h"
#include "sequence.h"


short int sequence::_activeSequence = 0;
const short int sequence::SEQUENCE_COUNT = 7;
sequence sequence::sequences[SEQUENCE_COUNT];

// constructor
sequence::sequence() {

}

sequence::sequence(const char cName[7], const short int iTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[4][4]){
  strcpy(_cName, cName);
  _iTempo = iTempo;
  _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
    for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
      _iLaserPinStatusAtEachBeat[__thisBeat][__thisPin] = iLaserPinStatusAtEachBeat[__thisBeat][__thisPin];
    }
  }
}

void sequence::initSequence(const char cName[7], const short int iTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[][4]){
  strcpy(_cName, cName);
  _iTempo = iTempo;
  _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
    for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
      _iLaserPinStatusAtEachBeat[__thisBeat][__thisPin] = iLaserPinStatusAtEachBeat[__thisBeat][__thisPin];
    }
  }
};

void sequence::initSequences() {
  Serial.println("void sequence::initSequences(). Starting.");
  const short int aRelays[2][4] = {{LOW, HIGH, LOW, HIGH},{HIGH, LOW, HIGH, LOW}};
  sequences[0].initSequence("relays", 30000, 2, aRelays);
  // Serial.println("void sequence::initSequences(). sequences[0]._iTempo: ");
  // Serial.println("void sequence::initSequences(). sequences[0]._iTempo: ");
  // Serial.println(sequences[0]._iTempo);
  // Serial.println("void sequence::initSequences(). sequences[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(sequences[0]._iLaserPinStatusAtEachBeat[0][1]);
  const short int aTwins[2][4] = {{LOW, LOW, HIGH, HIGH},{HIGH, HIGH, LOW, LOW}};
  sequences[1].initSequence("twins", 30000, 2, aTwins);
  const short int aAll[2][4] = {{LOW, LOW, LOW, LOW},{HIGH, HIGH, HIGH, HIGH}};
  sequences[2].initSequence("all", 30000, 2, aAll);
  const short int aSwipeR[4][4] = {{LOW, HIGH, HIGH, HIGH},{HIGH, LOW, HIGH, HIGH},{HIGH, HIGH, LOW, HIGH},{HIGH, HIGH, HIGH, LOW}};
  sequences[3].initSequence("swipeR", 500, 4, aSwipeR);
  const short int aSwipeL[4][4] = {{HIGH, HIGH, HIGH, LOW},{HIGH, HIGH, LOW, HIGH},{HIGH, LOW, HIGH, HIGH},{LOW, HIGH, HIGH, HIGH}};
  sequences[4].initSequence("swipeL", 500, 4, aSwipeL);
  const short int aAllOff[1][4] = {{HIGH, HIGH, HIGH, HIGH}};
  sequences[5].initSequence("all of", 0, 1, aAllOff);
  Serial.println("void sequence::initSequences(). Ending.");
}

Task sequence::testPlay(0, 1, &tcbTestPlay, &userScheduler, false, NULL, &odtcbTestPlay);

void sequence::tcbTestPlay() {
  Serial.println("void sequence::tcbTestPlay(). Starting.");
  short int __activeSequence = 0;
  // Serial.println("void sequence::tcbTestPlay(). sequences[0]._iTempo: ");
  // Serial.println(sequences[0]._iTempo);
  // Serial.println("void sequence::tcbTestPlay(). sequences[0]._cName: ");
  // Serial.println(sequences[0]._cName);
  // Serial.println("void sequence::tcbTestPlay(). sequences[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(sequences[0]._iLaserPinStatusAtEachBeat[0][1]);
  setActiveSequence(__activeSequence);
  playSequence(__activeSequence);
  Serial.println("void sequence::tcbTestPlay(). Ending.");
};

void sequence::odtcbTestPlay() {
  Serial.println("void sequence::odtcbTestPlay(). Starting.");
  unsigned long duration = sequences[_activeSequence]._iTempo * sequences[_activeSequence]._iNumberOfBeatsInSequence;
  tEndSequence.enableDelayed(duration);
  Serial.println("void sequence::odtcbTestPlay(). Starting.");
};

Task sequence::tEndSequence(0, 1, &_tcbTEndSequence, &userScheduler, false);

void sequence::_tcbTEndSequence() {
  setActiveSequence(5);
  playSequence(5);
}

void sequence::playSequence(const short int sequenceNumber){
  Serial.println("void sequence::playSequence(). Starting");
  // Serial.print("void sequence::playSequence(). Sequence Number: ");
  // Serial.println(sequenceNumber);
  _tPlaySequence.setInterval(sequences[sequenceNumber]._iTempo);
  // Serial.print("void sequence::playSequence(). Tempo: ");
  // Serial.println(sequences[sequenceNumber]._iTempo);
  _tPlaySequence.setIterations(sequences[sequenceNumber]._iNumberOfBeatsInSequence);
  // Serial.print("void sequence::playSequence(). Beats: ");
  // Serial.println(sequences[sequenceNumber]._iNumberOfBeatsInSequence);
  _tPlaySequence.enable();
  Serial.println("void sequence::playSequence(). Task _tPlaySequence enabled");
  Serial.println("void sequence::playSequence(). Ending");
};

Task sequence::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false);

void sequence::_tcbPlaySequence(){
  Serial.println("void sequence::_tcbPlaySequence(). Starting.");
  short _iter = _tPlaySequence.getRunCounter();
  // Serial.println("void sequence::_tcbPlaySequence(). _iter: ");
  // Serial.println(_iter);
  // Direct access to the pins.
  // For each pin
  for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
    Serial.println("void sequence::_tcbPlaySequence(). __thisPin: ");
    Serial.println(__thisPin);
    short _physical_pin_number = relayPins[__thisPin]; // look for the physical number of the pin in the array of pin
    Serial.println("void sequence::_tcbPlaySequence(). _physical_pin_number: ");
    Serial.println(_physical_pin_number);
    Serial.println("void sequence::_tcbPlaySequence(). _activeSequence: ");
    Serial.println(_activeSequence);
    Serial.println("void sequence::_tcbPlaySequence(). sequences[_activeSequence]._iTempo: ");
    Serial.println(sequences[_activeSequence]._iTempo);
    Serial.println("void sequence::_tcbPlaySequence(). sequences[_activeSequence]._iLaserPinStatusAtEachBeat[0][0]: ");
    Serial.println(sequences[_activeSequence]._iLaserPinStatusAtEachBeat[0][0]);
    const short int _target_state = sequences[_activeSequence]._iLaserPinStatusAtEachBeat[_iter - 1][__thisPin]; // look for the desired status in the array of the sequence
    Serial.println("void sequence::_tcbPlaySequence(). _target_state: ");
    Serial.println(_target_state);
    digitalWrite(_physical_pin_number, _target_state); // instruct the MC to turn the desired pin to the desired status
  }
  Serial.println("void sequence::_tcbPlaySequence(). Ending.");
};

void sequence::setActiveSequence(const short activeSequence) {
  _activeSequence = activeSequence;
};
