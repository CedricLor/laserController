/*
  sequence.cpp - sequences are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

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


Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "sequence.h"

short int sequence::_activeSequence = 0;
const short int sequence::_sequence_count = 7;
sequence sequence::sequences[_sequence_count];





// Constructors
sequence::sequence() {
}






// Initialisers
void sequence::_initSequence(const char cName[_sequence_count], const unsigned long ulTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[4]){
  // Serial.println("void sequence::initSequence(). Starting.");
  strcpy(_cName, cName);
  _ulTempo = ulTempo;
  _iNumberOfBeatsInSequence = iNumberOfBeatsInSequence;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInSequence; __thisBeat++) {
    _iLaserPinStatusAtEachBeat[__thisBeat] = iLaserPinStatusAtEachBeat[__thisBeat];
  }
  // Serial.println("void sequence::initSequence(). Ending.");
};

void sequence::initSequences() {
  Serial.println("void sequence::_initSequences(). Starting.");
  // define an array containing references to the notes to be played in the sequence
  const short int aRelays[2] = {7,8};
  // load values into sequences[0]:
  // a. the sequence's name
  // b. the duration of each beats (i.e. the tempo)
  // c. the number of beats in the sequence
  // d. the array of references to the notes to be played in the sequence
  sequences[0]._initSequence("relays", 30000, 2, aRelays);
  // Serial.println("void sequence::_initSequences(). sequences[0]._ulTempo: ");
  // Serial.println(sequences[0]._ulTempo);
  // Serial.println("void sequence::_initSequences(). sequences[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(sequences[0]._iLaserPinStatusAtEachBeat[0][1]);
  const short int aTwins[2] = {5,6};
  sequences[1]._initSequence("twins", 30000, 2, aTwins);
  const short int aAll[2] = {15,0};
  sequences[2]._initSequence("all", 30000, 2, aAll);
  const short int aSwipeR[4] = {1,2,3,4};
  sequences[3]._initSequence("swipeR", 500, 4, aSwipeR);
  const short int aSwipeL[4] = {4,3,2,1};
  sequences[4]._initSequence("swipeL", 500, 4, aSwipeL);
  const short int aAllOff[1] = {0};
  sequences[5]._initSequence("all of", 0, 1, aAllOff);
  Serial.println("void sequence::_initSequences(). Ending.");
}






// Loop Player
// tPlaySequenceInLoop plays a sequence in loop, for an unlimited number of iterations,
// until it is disabled.
// tPlaySequenceInLoop is enabled and disabled by the stateBox class.
// Upon entering a new stateBox (startup, IR signal received, etc.),
// the sequence associated with this state is set as the _activeSequence
// by the stateBox tasks and the tPlaySequenceInLoop is enabled
Task sequence::tPlaySequenceInLoop(0, -1, &_tcbPlaySequenceInLoop, &userScheduler, false, &_oetcbPlaySequenceInLoop, &_odtcbPlaySequenceInLoop);

// Upon enabling the tPlaySequenceInLoop task, the _activeSequence is played a
// first time and the _duration of the sequence is calculated in order to
// set the interval between each iterations of the tPlaySequenceInLoop task
bool sequence::_oetcbPlaySequenceInLoop() {
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). Starting.");
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). _activeSequence: ");
  // Serial.println(_activeSequence);
  // Start immediately playing the sequence on enable
  _playSequence();
  // Calculate the interval at which each iteration occur, by multiplying the tempo of the sequence by the number of beats in the sequence
  unsigned long _duration = sequences[_activeSequence]._ulTempo * sequences[_activeSequence]._iNumberOfBeatsInSequence;
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). _duration: ");
  // Serial.println(_duration);
  // Set the interval at which each iteration occur
  tPlaySequenceInLoop.setInterval(_duration);
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval()");
  // Serial.println(tPlaySequenceInLoop.getInterval());
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). Ending.");
  return true;
}

void sequence::_tcbPlaySequenceInLoop() {
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). Starting.");
  _playSequence();
  // Serial.println("bool sequence::oetcbPlaySequenceInLoop(). Ending.");
}

// on disable tPlaySequenceInLoop, turn off all the laser by setting the activeSequence
// to state 5 ("all off"), then playSequence 5.
void sequence::_odtcbPlaySequenceInLoop() {
  // Serial.println("void sequence::odtcbPlaySequenceInLoop(). Starting.");
  setActiveSequence(5); // all lasers off
  _playSequence();
  // Serial.println("void sequence::odtcbPlaySequenceInLoop(). Ending.");
};




// Single sequence player
// Plays a given sequence one single time.
// It is called by the tPlaySequenceInLoop
// 1. sets the interval of the _tPlaySequence task from the tempo of the relevant sequence
// this tempo corresponds to the duration for which each note will be played
// 2. sets the number of iterations of the _tPlaySequence task from the number of
// beats (i.e. notes) in the sequence
// 3. enables the _tPlaySequence task
void sequence::_playSequence(){
  // Serial.println("void sequence::playSequence(). Starting");
  // Serial.print("void sequence::playSequence(). _activeSequence: ");
  // Serial.println(_activeSequence);
  _tPlaySequence.setInterval(sequences[_activeSequence]._ulTempo);
  // Serial.print("void sequence::playSequence(). Tempo: ");
  // Serial.println(sequences[sequenceNumber]._ulTempo);
  _tPlaySequence.setIterations(sequences[_activeSequence]._iNumberOfBeatsInSequence);
  // Serial.print("void sequence::playSequence(). Beats: ");
  // Serial.println(sequences[sequenceNumber]._iNumberOfBeatsInSequence);
  _tPlaySequence.enable();
  // Serial.println("void sequence::playSequence(). Task _tPlaySequence enabled");
  // Serial.println("void sequence::playSequence(). Ending");
};

Task sequence::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false);

void sequence::_tcbPlaySequence(){
  // Serial.println("void sequence::_tcbPlaySequence(). Starting.");
  short _iter = _tPlaySequence.getRunCounter() - 1;
  // Serial.println("void sequence::_tcbPlaySequence(). _iter: ");
  // Serial.println(_iter);
  // Look for the note number to read at this tempo
  short int _activeNote = sequences[_activeSequence]._iLaserPinStatusAtEachBeat[_iter];
  // Play note
  note::notes[_activeNote].playNote();
  // Serial.println("void sequence::_tcbPlaySequence(). Ending.");
};

void sequence::setActiveSequence(const short activeSequence) {
  // Serial.println("void sequence::setActiveSequence(). Starting.");
  _activeSequence = activeSequence;
  // Serial.println("void sequence::setActiveSequence(). Ending.");
};
