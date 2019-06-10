/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.
*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_activeboxState = 0;
const short int boxState::BOX_STATES_COUNT = 7;
boxState boxState::boxStates[BOX_STATES_COUNT];

// constructor
boxState::boxState() {

}

boxState::boxState(const char cName[7], const short int ulDuration, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[4]){
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
    _iLaserPinStatusAtEachBeat[__thisBeat] = iLaserPinStatusAtEachBeat[__thisBeat];
  }
}

// void boxState::initSequence(const char cName[7], const short int iTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[4]){
//   strcpy(_cName, cName);
//   _iTempo = iTempo;
//   _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
//   for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
//     _iLaserPinStatusAtEachBeat[__thisBeat] = iLaserPinStatusAtEachBeat[__thisBeat];
//   }
// };
//
// void boxState::initSequences() {
//   Serial.println("void boxState::initSequences(). Starting.");
//   const short int aRelays[2] = {7,8};
//   sequences[0].initSequence("relays", 30000, 2, aRelays);
//   // Serial.println("void boxState::initSequences(). sequences[0]._iTempo: ");
//   // Serial.println("void boxState::initSequences(). sequences[0]._iTempo: ");
//   // Serial.println(sequences[0]._iTempo);
//   // Serial.println("void boxState::initSequences(). sequences[0]._iLaserPinStatusAtEachBeat[0][1]");
//   // Serial.println(sequences[0]._iLaserPinStatusAtEachBeat[0][1]);
//   const short int aTwins[2] = {5,6};
//   sequences[1].initSequence("twins", 30000, 2, aTwins);
//   const short int aAll[2] = {15,0};
//   sequences[2].initSequence("all", 30000, 2, aAll);
//   const short int aSwipeR[4] = {1,2,3,4};
//   sequences[3].initSequence("swipeR", 500, 4, aSwipeR);
//   const short int aSwipeL[4] = {4,3,2,1};
//   sequences[4].initSequence("swipeL", 500, 4, aSwipeL);
//   const short int aAllOff[1] = {0};
//   sequences[5].initSequence("all of", 0, 1, aAllOff);
//   Serial.println("void boxState::initSequences(). Ending.");
// }
//
// Task boxState::testPlay(0, 1, &tcbTestPlay, &userScheduler, false, NULL, &odtcbTestPlay);
//
// void boxState::tcbTestPlay() {
//   Serial.println("void boxState::tcbTestPlay(). Starting.");
//   short int __activeSequence = 0;
//   // Serial.println("void boxState::tcbTestPlay(). sequences[0]._iTempo: ");
//   // Serial.println(sequences[0]._iTempo);
//   // Serial.println("void boxState::tcbTestPlay(). sequences[0]._cName: ");
//   // Serial.println(sequences[0]._cName);
//   // Serial.println("void boxState::tcbTestPlay(). sequences[0]._iLaserPinStatusAtEachBeat[0][1]");
//   // Serial.println(sequences[0]._iLaserPinStatusAtEachBeat[0][1]);
//   setActiveSequence(__activeSequence);
//   playSequence(__activeSequence);
//   Serial.println("void boxState::tcbTestPlay(). Ending.");
// };
//
// void boxState::odtcbTestPlay() {
//   Serial.println("void boxState::odtcbTestPlay(). Starting.");
//   unsigned long duration = sequences[_activeSequence]._iTempo * sequences[_activeSequence]._iNumberOfBeatsInSequence;
//   tEndSequence.enableDelayed(duration);
//   Serial.println("void boxState::odtcbTestPlay(). Starting.");
// };
//
// Task boxState::tEndSequence(0, 1, &_tcbTEndSequence, &userScheduler, false);
//
// void boxState::_tcbTEndSequence() {
//   setActiveSequence(5);
//   playSequence(5);
// }
//
// void boxState::playSequence(const short int sequenceNumber){
//   Serial.println("void boxState::playSequence(). Starting");
//   // Serial.print("void boxState::playSequence(). Sequence Number: ");
//   // Serial.println(sequenceNumber);
//   _tPlaySequence.setInterval(sequences[sequenceNumber]._iTempo);
//   // Serial.print("void boxState::playSequence(). Tempo: ");
//   // Serial.println(sequences[sequenceNumber]._iTempo);
//   _tPlaySequence.setIterations(sequences[sequenceNumber]._iNumberOfBeatsInSequence);
//   // Serial.print("void boxState::playSequence(). Beats: ");
//   // Serial.println(sequences[sequenceNumber]._iNumberOfBeatsInSequence);
//   _tPlaySequence.enable();
//   Serial.println("void boxState::playSequence(). Task _tPlaySequence enabled");
//   Serial.println("void boxState::playSequence(). Ending");
// };
//
// Task boxState::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false);
//
// void boxState::_tcbPlaySequence(){
//   Serial.println("void boxState::_tcbPlaySequence(). Starting.");
//   short _iter = _tPlaySequence.getRunCounter() - 1;
//   // Serial.println("void boxState::_tcbPlaySequence(). _iter: ");
//   // Serial.println(_iter);
//   // Look for the note number to read at this tempo
//   short int _activeNote = sequences[_activeSequence]._iLaserPinStatusAtEachBeat[_iter];
//   // Play note
//   note::notes[_activeNote].playNote();
//   Serial.println("void boxState::_tcbPlaySequence(). Ending.");
// };
//
// void boxState::setActiveSequence(const short activeSequence) {
//   _activeSequence = activeSequence;
// };
