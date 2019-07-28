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
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
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
  |  |  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |  |  |--global.h


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
  /*
    For the moment, all the arrays ONLY contain one single bar.
    In principle, sequence are supposed to contain several bars.
    Accordingly, the array should contain more than one single bar.
    This will be one of the artistic parts.
  */
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
  sequences[5]._initSequence("all of", 30000, _barCountForThisSequence, aAllOff);

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
  Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). Starting. *****");

  if (MY_DG_LASER) {
    Serial.print("-- bool sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
    Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
    Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
    Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
    Serial.println("-- void sequence::_oetcbPlaySequenceInLoop(). ******");
    Serial.print("-- bool sequence::_oetcbPlaySequenceInLoop(). _activeSequence: ");Serial.println(_activeSequence);
  }

  // Why do not start playing immediately? Why are these lines commented out?
  // Because we are setting the interval for tPlaySequenceInLoop in its own onEnable callback.
  // As such, the next iteration of tPlaySequenceInLoop will take place immediatelly after the onEnable callback ends.
  // And the main callback will be calling sequences[_activeSequence]._playSequence().
  //
  // Start immediately playing the sequence on enable
  // Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). about to call sequences[_activeSequence]._playSequence().");
  // sequences[_activeSequence]._playSequence();
  // Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). returning from sequences[_activeSequence]._playSequence().");

  // Calculate the interval at which each iteration occur
  if (MY_DG_LASER) {Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). about to calculate the duration of the interval for tPlaySequenceinLoop.");}
  unsigned long _duration = _ulSequenceDuration(_activeSequence);
  // sequences[_activeSequence]._ulTempo
  //  * sequences[_activeSequence]._barCountInSequence;
  if (MY_DG_LASER) {Serial.print("-- bool sequence::_oetcbPlaySequenceInLoop(). _duration: ");Serial.println(_duration);}

  // // Set the interval at which each iteration occur.
  // // Before doing, check if _duration == 0.
  // // If _duration == 0, this actually means infinite,
  // // which means that we are trying to play the sequence "all lasers off"
  // // Instead of starting an infinite loop, turning all lasers off,
  // // let's just disable tPlaySequenceInLoop
  // if (_duration == 0) {
  //   Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). _duration == 0");
  //   Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). about to return from onEnable callback with a FALSE.");
  //   return false;
  // }

  if (MY_DG_LASER) {Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). About to call tPlaySequenceInLoop.setInterval(_duration) ******");}

  tPlaySequenceInLoop.setInterval(_duration);

  if (MY_DG_LASER) {
    Serial.print("-- bool sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
    Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
    Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
    Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
    Serial.println("-- void sequence::_oetcbPlaySequenceInLoop(). ******");
  }

  Serial.println("-- bool sequence::_oetcbPlaySequenceInLoop(). Ending.");
  return true;
}


void sequence::_tcbPlaySequenceInLoop() {
  Serial.println("-- void sequence::_tcbPlaySequenceInLoop(). Starting. ******");

  if (MY_DG_LASER) {
    Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
    Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
    Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
    Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
    Serial.println("-- void sequence::_tcbPlaySequenceInLoop(). ******");
    Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). about to call sequences[_activeSequence]._playSequence() with _activeSequence = ");Serial.println(_activeSequence);
  }

  sequences[_activeSequence]._playSequence();

  Serial.println("-- void sequence::_tcbPlaySequenceInLoop(). Ending.");
}


// on disable tPlaySequenceInLoop, turn off all the laser by playing
// sequence 5 ("all off").
void sequence::_odtcbPlaySequenceInLoop() {
  Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). Starting. ******");

  if (MY_DG_LASER) {
    Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
    Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). ******");
    Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). _activeSequence == ");Serial.println(_activeSequence);
  }

  if (!(_activeSequence == 5)) {
    if (MY_DG_LASER) {
      Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). _activeSequence is != 5");
      Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). _activeSequence is going to be set to 5");
      Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). about to call setActiveSequence(5)");
      Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). (before calling setActiveSequence(5)) _activeSequence: ");Serial.println(_activeSequence);
    }
    setActiveSequence(5);
    if (MY_DG_LASER) {
      Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). (just after calling setActiveSequence(5)) _activeSequence: ");Serial.println(_activeSequence);
      Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). about to call sequences[_activeSequence]._playSequence()");
    }
    sequences[_activeSequence]._playSequence(); // All laser off
  }
  Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). Ending.");
};



// Get the sequence duration, to set the correct interval for tPlaySequenceInLoop
long int sequence::_ulSequenceDuration(const short int __activeSequence) {
  Serial.println("----- long int sequence::_ulSequenceDuration(). Starting.");
  unsigned long __ulDurationInMs = 0;
  // get the index number of each bar in the sequence and sum up their durations
  for(short int __thisBar = 0; __thisBar < sequences[__activeSequence]._barCountInSequence; __thisBar++){
    short int __activeBarIndexNumber = sequences[__activeSequence]._iAssociatedBarsSequence[__thisBar];
    __ulDurationInMs = __ulDurationInMs + sequences[__activeSequence]._ulBarDuration(__activeBarIndexNumber);
  }
  Serial.println("----- long int sequence::_ulSequenceDuration(). Ending.");
  return __ulDurationInMs;
}







// Single sequence player
// Plays a given sequence one single time.
// It is called by the Task tPlaySequenceInLoop
// 1. sets the interval of the _tPlaySequence task from the tempo of the relevant sequence
// this tempo corresponds to the duration of each bar
// 2. sets the number of iterations of the _tPlaySequence task from the number of
// bars in the sequence
// 3. enables the _tPlaySequence task
void sequence::_playSequence(){
  Serial.println("----- void sequence::_playSequence(). Starting");

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_playSequence(). Just before enabling _tPlaySequence *!*!*!*!*!");
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
    Serial.print("----- void sequence::_playSequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
    Serial.print("----- void sequence::_playSequence(). millis() = ");Serial.println(millis());
    Serial.println("----- void sequence::_playSequence(). *!*!*!*!*!");
  }

  // _tPlaySequence.setInterval(sequences[_activeSequence]._ulTempo);
  // Serial.print("----- void sequence::_playSequence(). Tempo of the sequence: ");Serial.println(sequences[_activeSequence]._ulTempo);

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_playSequence(). _tPlaySequence.setIterations() about to be called");
    Serial.print("----- void sequence::_playSequence(). bars count in sequence: ");Serial.println(sequences[_activeSequence]._barCountInSequence);
  }

  _tPlaySequence.setIterations(sequences[_activeSequence]._barCountInSequence);

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_playSequence(). _tPlaySequence.setIterations() has been called");
    Serial.println("----- void sequence::_playSequence(). Just after _tPlaySequence.setIterations(): *!*!*!*!*!");
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
    Serial.print("----- void sequence::_playSequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
    Serial.print("----- void sequence::_playSequence(). millis() = ");Serial.println(millis());
    Serial.println("----- void sequence::_playSequence(). *!*!*!*!*!");

    Serial.println("----- void sequence::_playSequence(). Task _tPlaySequence about to be enabled");
  }

  _tPlaySequence.enable();

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_playSequence(). Task _tPlaySequence enabled *!*!*!*!*!");
    Serial.println("----- void sequence::_playSequence(). Just after enabling _tPlaySequence");
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
    Serial.print("----- void sequence::_playSequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
    Serial.print("----- void sequence::_playSequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
    Serial.print("----- void sequence::_playSequence(). millis() = ");Serial.println(millis());
    Serial.println("----- void sequence::_playSequence(). *!*!*!*!*!");
  }

  Serial.println("----- void sequence::_playSequence(). Ending");
};


Task sequence::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false, NULL, &_odtcbPlaySequence);


void sequence::_tcbPlaySequence(){
  Serial.println("----- void sequence::_tcbPlaySequence(). Starting.");

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_tcbPlaySequence(). _tPlaySequence execution parameters: *!*!*!*!*!");
    Serial.print("----- void sequence::_tcbPlaySequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
    Serial.print("----- void sequence::_tcbPlaySequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
    Serial.print("----- void sequence::_tcbPlaySequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
    Serial.print("----- void sequence::_tcbPlaySequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
    Serial.print("----- void sequence::_tcbPlaySequence(). millis() = ");Serial.println(millis());
    Serial.println("----- void sequence::_tcbPlaySequence(). *!*!*!*!*!");
  }

  short _iter = _tPlaySequence.getRunCounter() - 1;
  if (MY_DG_LASER) {
    Serial.print("----- void sequence::_tcbPlaySequence(). _iter: ");Serial.println(_iter);
  }

  // Look for the bar number to read at this iteration
  short int _activeBar = sequences[_activeSequence]._iAssociatedBarsSequence[_iter];
  if (MY_DG_LASER) {
    Serial.print("----- void sequence::_tcbPlaySequence(). after setting _activeBar = ");Serial.println(_activeBar);
  }

  // Play bars
  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_tcbPlaySequence(). about to call bar::bars[_activeBar].playBar(_activeBar)");
  }
  bar::bars[_activeBar].playBar(_activeBar);
  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_tcbPlaySequence(). coming back from bar::bars[_activeBar].playBar(_activeBar)");
  }

  // The next iteration shall occur when..? Easy, the bar wins!!!
  // If the bar has a shorter or longer beatbase, the bar value shall win over
  // the sequence tempo.
  // Reset the interval value of this task to give the bar that is going to be played time
  // to play at its own slower time or avoid blank spots if it plays at a faster rate.

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_tcbPlaySequence(). About to calculate the _activeBar duration in ms");
  }
  unsigned long __ulBarDuration = _ulBarDuration(_activeBar);
  if (MY_DG_LASER) {
    Serial.print("----- void sequence::_tcbPlaySequence(). __ulBarDuration = ");Serial.println(__ulBarDuration);
  }

  _tPlaySequence.setInterval(__ulBarDuration);

  if (MY_DG_LASER) {
    Serial.println("----- void sequence::_tcbPlaySequence(). _tPlaySequence just after calling _tPlaySequence.setInterval *!*!*!*!*!");
    Serial.println("----- void sequence::_tcbPlaySequence(). _tPlaySequence execution parameters:");
    Serial.print("----- void sequence::_tcbPlaySequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
    Serial.print("----- void sequence::_tcbPlaySequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
    Serial.print("----- void sequence::_tcbPlaySequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
    Serial.print("----- void sequence::_tcbPlaySequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
    Serial.print("----- void sequence::_tcbPlaySequence(). millis() = ");Serial.println(millis());
    Serial.println("----- void sequence::_tcbPlaySequence(). *!*!*!*!*!");
  }

  Serial.println("----- void sequence::_tcbPlaySequence(). Ending.");
};


// tPlaySequence disable callback
void sequence::_odtcbPlaySequence(){
  if (MY_DG_LASER) {
    Serial.print("----- void sequence::_odtcbPlaySequence(). millis() = ");Serial.println(millis());
    Serial.println("----- void sequence::_odtcbPlaySequence(). tPlaySequence BYE BYE!");
  }
}


// returns the current bar effective duration
long int sequence::_ulBarDuration(const short int _activeBar) {
  Serial.println("----- void sequence::_ulBarDuration(). Starting.");

  if (MY_DG_LASER) {
    Serial.print("----- void sequence::_ulBarDuration(). _activeBar = ");Serial.println(_activeBar);
  }

  unsigned long __ulDurationInMs = 0;

  if (MY_DG_LASER) {
    Serial.print("----- void sequence::_ulBarDuration(). bar::bars[_activeBar].iNotesCountInBar = ");Serial.println(bar::bars[_activeBar].iNotesCountInBar);
  }

  // iterate over each note in the sequence to get their interval in ms based on their tempo
  for(short int __thisNote = 0; __thisNote < bar::bars[_activeBar].iNotesCountInBar; __thisNote++){
    __ulDurationInMs = __ulDurationInMs + bar::bars[_activeBar].getSingleNoteInterval(__thisNote);
  }

  Serial.println("----- void sequence::_ulBarDuration(). Ending.");
  return __ulDurationInMs;
}


// Set the active sequence
void sequence::setActiveSequence(const short activeSequence) {
  Serial.println("------ void sequence::setActiveSequence(). Starting.");
  Serial.print("------ void sequence::setActiveSequence(). (before setting) _activeSequence = ");Serial.println(_activeSequence);
  _activeSequence = activeSequence;
  Serial.print("------ void sequence::setActiveSequence(). (after setting) _activeSequence = ");Serial.println(_activeSequence);
  Serial.println("------ void sequence::setActiveSequence(). Ending.");
};
