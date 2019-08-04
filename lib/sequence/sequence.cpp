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





///////////////////////////////////
// Constructors
///////////////////////////////////
sequence::sequence() {
}






///////////////////////////////////
// Initialisers
///////////////////////////////////
void sequence::_initSequence(const char cName[_char_count_in_name], const uint16_t _ui16BaseBeatInBpm, const uint16_t _ui16BaseNoteForBeat, const uint16_t _ui16BaseNotesCountPerBar, const short int __barCountInSequence, const short int __iAssociatedBars[4]){
  // Serial.println("void sequence::initSequence(). Starting.");
  strcpy(_cName, cName);
  ui16BaseBeatInBpm = _ui16BaseBeatInBpm;
  ui16BaseNoteForBeat = _ui16BaseNoteForBeat;
  ui16BaseNotesCountPerBar = _ui16BaseNotesCountPerBar;
  _barCountInSequence = __barCountInSequence;
  for (short __thisBar = 0; __thisBar < _barCountInSequence; __thisBar++) {
    _iAssociatedBars[__thisBar] = __iAssociatedBars[__thisBar];
  }
  // Serial.println("void sequence::initSequence(). Ending.");
};



void sequence::initSequences() {
  Serial.println("sequence::_initSequences(). Starting.");

  // define the bar count for this sequence
  short _barCountForThisSequence = 1;
  // define an array of references to the bars to be played in the sequence
  const short int aRelays[_barCountForThisSequence] = {0};
  // load values into sequences[0]:
  // a. the sequence's name
  // b. ui16BaseBeatInBpm: the base beat in bpm
  // c. ui16BaseNoteForBeat: the base note for each beat (1 -> full, 2 -> white,
  //                         4 -> black, etc.; the 4 in 2/4)
  // d. ui16BaseNotesCountPerBar: the number of base notes per bar (the 2 in 2/4)
  // e. the number of bars (or bars count) in the sequence
  // f. the array of references to the bars to be played in the sequence
  sequences[0]._initSequence("relays", 2, 1, 2, _barCountForThisSequence, aRelays);
  /*
    ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 2;  => partition en 2/1
  */
  // Serial.println("void sequence::_initSequences(). sequences[0].ui16BaseBeatInBpm: ");
  // Serial.println(sequences[0].ui16BaseBeatInBpm);
  // Serial.println("void sequence::_initSequences(). sequences[0]._iAssociatedBars[0][1]");
  // Serial.println(sequences[0]._iAssociatedBars[0][1]);


  _barCountForThisSequence = 1;
  const short int aTwins[_barCountForThisSequence] = {1};
  sequences[1]._initSequence("twins", 2, 1, 2, _barCountForThisSequence, aTwins);
  /*
    ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 2;  => partition en 2/1
  */

  _barCountForThisSequence = 1;
  const short int aAll[_barCountForThisSequence] = {2};
  sequences[2]._initSequence("all", 2, 1, 2, _barCountForThisSequence, aAll);
  /*
    ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 2;  => partition en 2/1
  */

  _barCountForThisSequence = 1;
  const short int aSwipeR[_barCountForThisSequence] = {3};
  sequences[3]._initSequence("swipeR", 120, 1, 4, _barCountForThisSequence, aSwipeR);
  /*
    ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 4;  => partition en 4/1
  */

  _barCountForThisSequence = 1;
  const short int aSwipeL[_barCountForThisSequence] = {4};
  sequences[4]._initSequence("swipeL", 120, 1, 4, _barCountForThisSequence, aSwipeL);
  /*
    ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 4;  => partition en 4/1
  */

  _barCountForThisSequence = 1;
  const short int aAllOff[_barCountForThisSequence] = {5};
  sequences[5]._initSequence("all of", 2, 1, 1, _barCountForThisSequence, aAllOff);
  /*
    ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 1;  => partition en 1/1
  */

  Serial.println("sequence::_initSequences(). Ending.");
}









///////////////////////////////////
// Loop Player
///////////////////////////////////
/*
    _tPlayBoxState plays once (unless restarted by tPlayBoxStates)
      - _tPlayBoxState -> onEnable: enables tPlaySequenceInLoop
      - _tPlayBoxState -> onDisable: disables tPlaySequenceInLoop
    tPlaySequenceInLoop plays forever (until interrupt by _tPlayBoxState):
      - tPlaySequenceInLoop -> onEnable: calculates the sequence duration and
            sets the interval for tPlaySequenceInLoop
      - tPlaySequenceInLoop -> main callback: starts playing the active sequence
      - tPlaySequenceInLoop -> onDisable: plays sequence 5 (all off)
*/

/*
    tPlaySequenceInLoop

    tPlaySequenceInLoop plays a sequence in loop, for an unlimited number
    of iterations, until it is disabled by _tPlayBoxState.

    tPlaySequenceInLoop is enabled and disabled by the onEnable and onDisable
    callbacks of _tPlayBoxState.

    Upon entering a new boxState (startup, IR signal received, etc.),
    the onEnable callback of _tPlayBoxState task sets sequence::_activeSequence
    to the sequence index number associated with this boxState.

    Then the Task tPlaySequenceInLoop is enabled, until being disabled by the
    boxState::_tPlayBoxState onDisable callback.
*/
Task sequence::tPlaySequenceInLoop(0, TASK_FOREVER, &_tcbPlaySequenceInLoop, &userScheduler, false, &_oetcbPlaySequenceInLoop, &_odtcbPlaySequenceInLoop);



/*
    _oetcbPlaySequenceInLoop()

    Upon enabling the tPlaySequenceInLoop task, _oetcbPlaySequenceInLoop():
    1. sets the interval at which the task tPlaySequenceInLoop shall iterate
    (and restart the sequence). This interval will only be taken into account
    after the main callback has been called ==> the forenext iteration).

    This calculation is made based on the base beat in bpm, the base note,
    the number of base notes per bars and the number of basrs in the sequence.

    _oetcbPlaySequenceInLoop() does not start playing the sequence. This is
    done in the main callback of tPlaySequenceInLoop. The first iteration of
    the Task occurs immediately after the onEnable callback, at the initial
    interval for this Task (before being reset in this onEnable callback) is
    equal to 0.
*/
bool sequence::_oetcbPlaySequenceInLoop() {
  Serial.println("sequence::_oetcbPlaySequenceInLoop(). Starting. *****");

  // if (MY_DG_LASER) {
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("sequence::_oetcbPlaySequenceInLoop(). ******");
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). _activeSequence: ");Serial.println(_activeSequence);
  // }


  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). about to calculate the duration of the interval for tPlaySequenceinLoop.");}
  // if (MY_DG_LASER) {Serial.printf("sequence::_oetcbPlaySequenceInLoop(). _ulSequenceDuration(_activeSequence): %u \n", _ulSequenceDuration(_activeSequence));}
  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). About to call tPlaySequenceInLoop.setInterval(_duration) ******");}

  /* Set the interval between each iteration of tPlaySequenceInLoop
      (each iteration will restart the Task _tPlaySequence, so this interval
      shall be equal to the duration of the sequence). */
  tPlaySequenceInLoop.setInterval(_ulSequenceDuration(_activeSequence));

  // if (MY_DG_LASER) {
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("sequence::_oetcbPlaySequenceInLoop(). ******");
  // }

  Serial.println("sequence::_oetcbPlaySequenceInLoop(). Ending.");
  return true;
}




void sequence::_tcbPlaySequenceInLoop() {
  Serial.println("sequence::_tcbPlaySequenceInLoop(). Starting. ******");

  // if (MY_DG_LASER) {
  //   Serial.print("sequence::_tcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
  //   Serial.print("sequence::_tcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
  //   Serial.print("sequence::_tcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
  //   Serial.print("sequence::_tcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("sequence::_tcbPlaySequenceInLoop(). ******");
  //   Serial.println("sequence::_tcbPlaySequenceInLoop(). about to enable _tPlaySequence.enable()");
  // }
  Serial.println("sequence::_tcbPlaySequenceInLoop(). about to enable _tPlaySequence.enable()");
  _tPlaySequence.enable();

  Serial.println("sequence::_tcbPlaySequenceInLoop(). Ending.");
}




// On disable tPlaySequenceInLoop, turn off all the laser by playing
// sequence 5 ("all off").
void sequence::_odtcbPlaySequenceInLoop() {
  Serial.println("sequence::_odtcbPlaySequenceInLoop(). Starting. ******");

  // if (MY_DG_LASER) {
  //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). ******");
  //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). _activeSequence == ");Serial.println(_activeSequence);
  // }
  //
  if (!(_activeSequence == 5)) {
    // if (MY_DG_LASER) {
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). _activeSequence is != 5");
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). _activeSequence is going to be set to 5");
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). about to call setActiveSequence(5)");
    //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). (before calling setActiveSequence(5)) _activeSequence: ");Serial.println(_activeSequence);
    // }
    setActiveSequence(5);
    // if (MY_DG_LASER) {
    //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). (just after calling setActiveSequence(5)) _activeSequence: ");Serial.println(_activeSequence);
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). about to call sequences[_activeSequence]._playSequence()");
    // }
  }
  _tPlaySequence.enable();
  Serial.println("sequence::_odtcbPlaySequenceInLoop(). Ending.");
};








///////////////////////////////////
// Single Sequence Player
///////////////////////////////////
/*
  Task _tPlaySequence.
  It plays a given sequence once.
*/
Task sequence::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false, &_oetcbPlaySequence, &_odtcbPlaySequence);


// onEnable callback for _tPlaySequence
// 1. sets the interval for each iterations to duration of the bars (each iteration is a bar)
// 2. sets the number of iterations to the bar count in this sequence
bool sequence::_oetcbPlaySequence(){
  // Serial.println("----------------------------on enable _tPlaySequence ------------------------------");

  // 1. sets the interval of the _tPlaySequence task
  _tPlaySequence.setInterval(_ulBarDuration(_activeSequence));
  // Serial.printf("void sequence::_oetcbPlaySequence(). Set interval: %lu ms.\n", _ulBarDuration(_activeSequence));
  // Serial.printf("void sequence::_oetcbPlaySequence(). Get interval: %lu ms.\n", _tPlaySequence.getInterval());

  // 2. sets the number of iterations of the _tPlaySequence task from the
  //    number of bars in the sequence
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_playSequence(). _tPlaySequence.setIterations() about to be called");
  //   Serial.print("void sequence::_playSequence(). bars count in sequence: ");Serial.println(sequences[_activeSequence]._barCountInSequence);
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  // }
  _tPlaySequence.setIterations(sequences[_activeSequence]._barCountInSequence);
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  // }

  return true;
}



// Main callback for _tPlaySequence
// Each iteration of _tPlaySequence corresponds to a bar. Accordingly, each iteration
// runs for an identical a fixed time -> interval.
// The iterations and the interval of the Task have been set in its onEnable
// callback and do not change on iterations.
// At each iteration of _tPlaySequence:
// - we read in the sequence the number of the next bar <- from the iterator of the
//   Task.
// - we then apply the settings (tempo in BPM, base note for beats and the notes
//   count in each bar) of this sequence to the bar, which might have other
//   settings.
// - we then inform the bar class of the active bar number, before enabling
//   the Task tPlayBar in the bar class.
void sequence::_tcbPlaySequence(){
  Serial.println("sequence::_tcbPlaySequence(). Starting.");
  Serial.println(F("------------- DEBUG --------- SEQUENCE --------- DEBUG -------------"));

  // 1. Get the number of iterations (each iteration corresponds to one bar)
  short _iter = _tPlaySequence.getRunCounter() - 1;
  Serial.println("sequence::_tcbPlaySequence(). have set _iter: " + String(_iter));

  // 2. Select the bar number corresponding to this iteration
  short int _activeBar = sequences[_activeSequence]._iAssociatedBars[_iter];
  Serial.println("sequence::_tcbPlaySequence(). got _activeBar: sequences[" + String(_activeSequence) + "]._iAssociatedBars[" + String(_iter) + "]");
  Serial.println("sequence::_tcbPlaySequence(). have _activeBar: " + String(_activeBar));

  // 3. Configure the bar
  bar::bars[_activeBar].ui16BaseBeatInBpm = sequences[_activeSequence].ui16BaseBeatInBpm; // tempo in beats per minute
  Serial.println("sequence::_tcbPlaySequence(). got sequences[" + String(_activeSequence) + "].ui16BaseBeatInBpm = " + String(sequences[_activeSequence].ui16BaseBeatInBpm));
  Serial.println("sequence::_tcbPlaySequence(). have bar::bars[" + String(_activeBar) + "].ui16BaseBeatInBpm = " + String(bar::bars[_activeBar].ui16BaseBeatInBpm));
  bar::bars[_activeBar].ui16BaseNoteForBeat = sequences[_activeSequence].ui16BaseNoteForBeat; // the 4 in 2/4, for instance
  Serial.println("sequence::_tcbPlaySequence(). got sequences[" + String(_activeSequence) + "].ui16BaseNoteForBeat = " + String(sequences[_activeSequence].ui16BaseNoteForBeat));
  Serial.println("sequence::_tcbPlaySequence(). have bar::bars[" + String(_activeBar) + "].ui16BaseNoteForBeat = " + String(bar::bars[_activeBar].ui16BaseNoteForBeat));
  bar::bars[_activeBar].ui16BaseNotesCountInBar = sequences[_activeSequence].ui16BaseNotesCountPerBar; // the 2 in 2/4, for instance
  Serial.println("sequence::_tcbPlaySequence(). got sequences[" + String(_activeSequence) + "].ui16BaseNotesCountPerBar = " + String(sequences[_activeSequence].ui16BaseNotesCountPerBar));
  Serial.println("sequence::_tcbPlaySequence(). have bar::bars[" + String(_activeBar) + "].ui16BaseNotesCountInBar = " + String(bar::bars[_activeBar].ui16BaseNotesCountInBar));

  // 4. Play the corresponding bar
  Serial.println("sequence::_tcbPlaySequence(). calling bar::setActiveBar(" + String(_activeBar) + ")");
  bar::setActiveBar(_activeBar);
  Serial.println("sequence::_tcbPlaySequence(). enabling tPlayBar");
  bar::tPlayBar.enable();

  Serial.println("void sequence::_tcbPlaySequence(). Ending.");
};



// tPlaySequence disable callback
void sequence::_odtcbPlaySequence(){
  // if (MY_DG_LASER) {
  //   // Serial.println("----------------------------on disable _tPlaySequence ------------------------------");
  //   // Serial.print("sequence::_odtcbPlaySequence(). millis() = ");Serial.println(millis());
  //   // Serial.println("sequence::_odtcbPlaySequence(). tPlaySequence BYE BYE!");
  // }
}







///////////////////////////////////
// Helpers
///////////////////////////////////


// Helper function to _oetcbPlaySequenceInLoop
// Get the sequence duration, to set the correct interval for tPlaySequenceInLoop
long unsigned int sequence::_ulSequenceDuration(const short int __activeSequence) {
  Serial.println("long int sequence::_ulSequenceDuration(). Starting.");
  unsigned long __ulDurationInMs = sequences[__activeSequence]._barCountInSequence * _ulBarDuration(__activeSequence);
  // iterate over each bar pertaining to this sequence and add up their durations
  // for(short int __thisBar = 0; __thisBar < sequences[__activeSequence]._barCountInSequence; __thisBar++){
  //   short int __activeBarIndexNumber = sequences[__activeSequence]._iAssociatedBars[__thisBar];
  //   __ulDurationInMs = __ulDurationInMs + sequences[__activeSequence]._ulBarDuration(__activeBarIndexNumber);
  // }
  Serial.println("long int sequence::_ulSequenceDuration(). Ending.");
  return __ulDurationInMs;
}



// Helper function to _tPlaySequence
// returns the current bar effective duration
long unsigned int sequence::_ulBarDuration(const short int __activeSequence) {
  Serial.println("void sequence::_ulBarDuration(). Starting.");

  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_ulBarDuration(). _activeBar = ");Serial.println(_activeBar);
  // }
  unsigned long __ulDurationInMs = sequences[__activeSequence].ui16BaseNotesCountPerBar *
                              (60 / sequences[__activeSequence].ui16BaseBeatInBpm * 1000);

  // iterate over each note in the sequence to get their interval in ms based on their tempo
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_ulBarDuration(). bar::bars[_activeBar].iNotesCountInBar = ");Serial.println(bar::bars[_activeBar].iNotesCountInBar);
  // }
  // for(short int __thisNote = 0; __thisNote < bar::bars[_activeBar].ui16NotesCountInBar; __thisNote++){
  //   __ulDurationInMs = __ulDurationInMs + bar::bars[_activeBar].getSingleNoteInterval(__thisNote);
  // }

  Serial.println("void sequence::_ulBarDuration(). Ending.");
  return __ulDurationInMs;
}







// Set the active sequence
void sequence::setActiveSequence(const short activeSequence) {
  Serial.println("-void sequence::setActiveSequence(). Starting.");
  Serial.print("-void sequence::setActiveSequence(). (before setting) _activeSequence = ");Serial.println(_activeSequence);
  _activeSequence = activeSequence;
  Serial.print("-void sequence::setActiveSequence(). (after setting) _activeSequence = ");Serial.println(_activeSequence);
  Serial.println("-void sequence::setActiveSequence(). Ending.");
};
