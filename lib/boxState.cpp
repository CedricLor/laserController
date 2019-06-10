/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.
*/

#include "Arduino.h"
#include "boxState.h"

short int boxState::_activeboxState = 0;
const short int boxState::BOX_STATES_COUNT = 7;
boxState boxState::boxStates[BOX_STATES_COUNT];
const short int boxState::_NAME_CHAR_COUNT = 15;

// constructor
boxState::boxState() {

}

boxState::boxState(const char cName[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
}

void boxState::initBoxState(const char cName[_NAME_CHAR_COUNT], const unsigned long ulDuration, const short int iAssociatedSequence, const short int iIRTrigger, const short int iMeshTrigger){
  strcpy(_cName, cName);
  _ulDuration = ulDuration;
  _iAssociatedSequence = iAssociatedSequence;
  _iIRTrigger = iIRTrigger;
  _iMeshTrigger = iMeshTrigger;
};

void boxState::initBoxStates() {
  Serial.println("void boxState::initboxStates(). Starting.");
  boxStates[0].initBoxState("default - manual", 0, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::initboxStates(). boxStates[0]._cName: ");
  // Serial.println(boxStates[0]._cName);
  // Serial.println("void boxState::initboxStates(). boxStates[0]._ulDuration");
  // Serial.println(boxStates[0]._ulDuration);
  boxStates[1].initBoxState("align lasers", 0, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  boxStates[2].initBoxState("pir Startup", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  boxStates[3].initBoxState("pir High", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[4].initBoxState("mesh High", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  boxStates[5].initBoxState("waiting", 0, 5, 1, 1); // sequence "all of" for indefinite time until trigger from both IR or mesh

  Serial.println("void boxState::initboxStates(). Ending.");
}

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
