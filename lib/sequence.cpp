/*
  sequence.cpp - sequences are precoded sequences of notes
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

sequence::sequence(const char cName[7], const unsigned long ulTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[4]){
  strcpy(_cName, cName);
  _ulTempo = ulTempo;
  _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
    _iLaserPinStatusAtEachBeat[__thisBeat] = iLaserPinStatusAtEachBeat[__thisBeat];
  }
}

void sequence::initSequence(const char cName[7], const unsigned long ulTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[4]){
  strcpy(_cName, cName);
  _ulTempo = ulTempo;
  _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
    _iLaserPinStatusAtEachBeat[__thisBeat] = iLaserPinStatusAtEachBeat[__thisBeat];
  }
};

void sequence::initSequences() {
  Serial.println("void sequence::initSequences(). Starting.");
  const short int aRelays[2] = {7,8};
  sequences[0].initSequence("relays", 30000, 2, aRelays);
  // Serial.println("void sequence::initSequences(). sequences[0]._ulTempo: ");
  // Serial.println(sequences[0]._ulTempo);
  // Serial.println("void sequence::initSequences(). sequences[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(sequences[0]._iLaserPinStatusAtEachBeat[0][1]);
  const short int aTwins[2] = {5,6};
  sequences[1].initSequence("twins", 30000, 2, aTwins);
  const short int aAll[2] = {15,0};
  sequences[2].initSequence("all", 30000, 2, aAll);
  const short int aSwipeR[4] = {1,2,3,4};
  sequences[3].initSequence("swipeR", 500, 4, aSwipeR);
  const short int aSwipeL[4] = {4,3,2,1};
  sequences[4].initSequence("swipeL", 500, 4, aSwipeL);
  const short int aAllOff[1] = {0};
  sequences[5].initSequence("all of", 0, 1, aAllOff);
  Serial.println("void sequence::initSequences(). Ending.");
}

Task sequence::testPlay(0, 1, &tcbTestPlay, &userScheduler, false, NULL, &odtcbTestPlay);

void sequence::tcbTestPlay() {
  Serial.println("void sequence::tcbTestPlay(). Starting.");
  short int __activeSequence = 0;
  // Serial.println("void sequence::tcbTestPlay(). sequences[0]._ulTempo: ");
  // Serial.println(sequences[0]._ulTempo);
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
  unsigned long duration = sequences[_activeSequence]._ulTempo * sequences[_activeSequence]._iNumberOfBeatsInSequence;
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
  _tPlaySequence.setInterval(sequences[sequenceNumber]._ulTempo);
  // Serial.print("void sequence::playSequence(). Tempo: ");
  // Serial.println(sequences[sequenceNumber]._ulTempo);
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
  short _iter = _tPlaySequence.getRunCounter() - 1;
  // Serial.println("void sequence::_tcbPlaySequence(). _iter: ");
  // Serial.println(_iter);
  // Look for the note number to read at this tempo
  short int _activeNote = sequences[_activeSequence]._iLaserPinStatusAtEachBeat[_iter];
  // Play note
  note::notes[_activeNote].playNote();
  Serial.println("void sequence::_tcbPlaySequence(). Ending.");
};

void sequence::setActiveSequence(const short activeSequence) {
  _activeSequence = activeSequence;
};
