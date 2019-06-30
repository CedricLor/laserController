/*
  sequence.cpp - sequences are precoded sequences of bars
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
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
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
const short int sequence::_char_count_in_name = 7;





// Constructors

sequence::sequence() {
}






// Initialisers

void sequence::_initSequence(const char cName[_char_count_in_name], const unsigned long ulTempo, const short int barCountInSequence, const short int iAssociatedBarsSequence[4]){
  // Serial.println("void sequence::initSequence(). Starting.");
  strcpy(_cName, cName);
  _ulTempo = ulTempo;
  _barCountInSequence = barCountInSequence;
  for (short __thisBar = 0; __thisBar < barCountInSequence; __thisBar++) {
    _iAssociatedBarsSequence[__thisBar] = iAssociatedBarsSequence[__thisBar];
  }
  // Serial.println("void sequence::initSequence(). Ending.");
};





void sequence::initSequences() {
  Serial.println("void sequence::_initSequences(). Starting.");

  // define the bar count for this sequence
  short _barCountForThisSequence = 1;
  // define an array containing references to the bars to be played in the sequence
  const short int aRelays[_barCountForThisSequence] = {0};
  // load values into sequences[0]:
  // a. the sequence's name
  // b. the duration of each beats (i.e. the tempo) from which will be set
  //    the interval in the task
  // c. the number of bars (or bars count) in the sequence
  // d. the array of references to the bars to be played in the sequence
  sequences[0]._initSequence("relays", 30000, _barCountForThisSequence, aRelays);
  // Serial.println("void sequence::_initSequences(). sequences[0]._ulTempo: ");
  // Serial.println(sequences[0]._ulTempo);
  // Serial.println("void sequence::_initSequences(). sequences[0]._iAssociatedBarsSequence[0][1]");
  // Serial.println(sequences[0]._iAssociatedBarsSequence[0][1]);


  _barCountForThisSequence = 1;
  // for the moment, all the arrays that should contain series of
  // bars ONLY contain one single sequence. Accordingly, don't miss
  // that the following variable is an ARRAY of int.
  const short int aTwins[_barCountForThisSequence] = {1}; // this is a ref to the
                                                          // number of the single
                                                          // bar associated with
                                                          // this sequence
  sequences[1]._initSequence("twins", 30000, _barCountForThisSequence, aTwins);

  _barCountForThisSequence = 1;
  const short int aAll[_barCountForThisSequence] = {2};  // this is a ref to the
                                                         // number of the single
                                                         // bar associated with
                                                         // this sequence
  sequences[2]._initSequence("all", 30000, _barCountForThisSequence, aAll);

  _barCountForThisSequence = 1;
  const short int aSwipeR[_barCountForThisSequence] = {3};
  sequences[3]._initSequence("swipeR", 500, _barCountForThisSequence, aSwipeR);

  _barCountForThisSequence = 1;
  const short int aSwipeL[_barCountForThisSequence] = {4};
  sequences[4]._initSequence("swipeL", 500, _barCountForThisSequence, aSwipeL);

  _barCountForThisSequence = 1;
  const short int aAllOff[_barCountForThisSequence] = {5};
  sequences[5]._initSequence("all of", 0, _barCountForThisSequence, aAllOff);

  Serial.println("void sequence::_initSequences(). Ending.");
}








// Loop Player
// tPlaySequenceInLoop plays a sequence in loop, for an unlimited number of iterations,
// until it is disabled.
// tPlaySequenceInLoop is enabled and disabled by the stateBox class.
// Upon entering a new boxState (startup, IR signal received, etc.),
// the boxState::_tPlayBoxState task sets sequence::_activeSequence to the sequence index number
// associated with this boxState. Then the tPlaySequenceInLoop is enabled, until being disabled by the boxState::_tPlayBoxState task
Task sequence::tPlaySequenceInLoop(0, TASK_FOREVER, &_tcbPlaySequenceInLoop, &userScheduler, false, &_oetcbPlaySequenceInLoop, &_odtcbPlaySequenceInLoop);

// Upon enabling the tPlaySequenceInLoop task, the _activeSequence is played a
// first time and the _duration of the sequence is calculated in order to
// set the interval between each iterations of the tPlaySequenceInLoop task
bool sequence::_oetcbPlaySequenceInLoop() {
  Serial.println("----- bool sequence::_oetcbPlaySequenceInLoop(). Starting.");
  // Serial.print("----- bool sequence::_oetcbPlaySequenceInLoop(). _activeSequence: ");Serial.println(_activeSequence);

  // Start immediately playing the sequence on enable
  sequences[_activeSequence]._playSequence(_activeSequence);

  // Calculate the interval at which each iteration occur, by multiplying the tempo of the sequence by the number of bars in the sequence
  unsigned long _duration = sequences[_activeSequence]._ulTempo
   * sequences[_activeSequence]._barCountInSequence;
  // Serial.print("----- bool sequence::_oetcbPlaySequenceInLoop(). _duration: ");Serial.println(_duration);



  // Set the interval at which each iteration occur.
  // Before doing, check if _duration == 0.
  // If _duration == 0, this actually means infinite,
  // which means that we are trying to play the sequence "all lasers off"
  // Instead of starting an infinite loop, turning all lasers off,
  // let's just disable tPlaySequenceInLoop
  if (_duration == 0) {
    Serial.println("----- bool sequence::_oetcbPlaySequenceInLoop(). Ending on _duration == 0");
    return false;
  }


  tPlaySequenceInLoop.setInterval(_duration);
  // Serial.print("----- bool sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());

  Serial.println("----- bool sequence::_oetcbPlaySequenceInLoop(). Ending.");
  return true;
}

void sequence::_tcbPlaySequenceInLoop() {
  Serial.println("----- void sequence::_tcbPlaySequenceInLoop(). Starting.");
  sequences[_activeSequence]._playSequence(_activeSequence);
  Serial.println("----- void sequence::_tcbPlaySequenceInLoop(). Ending.");
}

// on disable tPlaySequenceInLoop, turn off all the laser by playing
// sequence 5 ("all off").
void sequence::_odtcbPlaySequenceInLoop() {
  Serial.println("----- void sequence::_odtcbPlaySequenceInLoop(). Starting.");
  if (!(_activeSequence == 5)) {
    sequences[_activeSequence]._playSequence(5); // All laser off
  }
  Serial.println("----- void sequence::_odtcbPlaySequenceInLoop(). Ending.");
};






// Single sequence player
// Plays a given sequence one single time.
// It is called by the Task tPlaySequenceInLoop
// 1. sets the interval of the _tPlaySequence task from the tempo of the relevant sequence
// this tempo corresponds to the duration of each bar
// 2. sets the number of iterations of the _tPlaySequence task from the number of
// bars in the sequence
// 3. enables the _tPlaySequence task
void sequence::_playSequence(short int activeSequence){
  Serial.println("----- void sequence::_playSequence(). Starting");
  setActiveSequence(activeSequence);
  // Serial.print("----- void sequence::_playSequence(). _activeSequence: ");Serial.println(_activeSequence);
  _tPlaySequence.setInterval(sequences[_activeSequence]._ulTempo);
  // Serial.print("----- void sequence::_playSequence(). Tempo: ");Serial.println(sequences[sequenceNumber]._ulTempo);
  _tPlaySequence.setIterations(sequences[_activeSequence]._barCountInSequence);
  // Serial.print("----- void sequence::_playSequence(). Beats: ");Serial.println(sequences[sequenceNumber]._barCountInSequence);
  _tPlaySequence.enable();
  // Serial.println("----- void sequence::_playSequence(). Task _tPlaySequence enabled");

  Serial.println("----- void sequence::_playSequence(). Ending");
};

Task sequence::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false);

void sequence::_tcbPlaySequence(){
  // Serial.println("----- void sequence::_tcbPlaySequence(). Starting.");
  short _iter = _tPlaySequence.getRunCounter() - 1;
  // Serial.print("----- void sequence::_tcbPlaySequence(). _iter: ");Serial.println(_iter);

  // Look for the bar number to read at this iteration
  short int _activeBar = sequences[_activeSequence]._iAssociatedBarsSequence[_iter];

  // Play bars
  bar::bars[_activeBar].playBar(_activeBar);

  // The next iteration shall occur when..? Easy, the bar wins!!!
  // If the bar has a shorter or longer beatbase, the bar value shall win over
  // the sequence tempo.
  // Reset the interval value of this task if the sequence tempo in this sequence
  // is different than that of the bar that is going to be played, to give it time
  // to play at its own slower time or avoid blank spots if it plays at a faster rate.
  if (!(bar::bars[_activeBar].ulBaseBeatInMs == sequences[_activeSequence]._ulTempo)) {
    unsigned long __ulBarDuration = _ulBarDuration(_activeBar);
    _tPlaySequence.setInterval(__ulBarDuration);
  }

  // Serial.println("----- void sequence::_tcbPlaySequence(). Ending.");
};

// do something if bars[_activeBar]._ulBaseBeatInMs is longer than sequence.tempo[_activeSequence]
long int sequence::_ulBarDuration(const short int _activeBar) {
  unsigned long __ulDurationInMs = 0;
  for(short int __thisNote = 0; __thisNote < bar::bars[_activeBar].iNotesCountInBar; __thisNote++){
    __ulDurationInMs = __ulDurationInMs + bar::bars[_activeBar].getIntervalForEachNote(__thisNote);
  }
  return __ulDurationInMs;
}

void sequence::setActiveSequence(const short activeSequence) {
  // Serial.println("void sequence::setActiveSequence(). Starting.");
  _activeSequence = activeSequence;
  // Serial.println("void sequence::setActiveSequence(). Ending.");
};
