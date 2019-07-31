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
void sequence::_initSequence(const char cName[_char_count_in_name], const uint16_t _ui16BaseBeatInBpm, const uint16_t _ui16BaseNoteForBeat, const uint16_t _ui16BaseNotesCountPerBar, const short int barCountInSequence, const short int iAssociatedBarsSequence[4]){
  // Serial.println("void sequence::initSequence(). Starting.");
  strcpy(_cName, cName);
  ui16BaseBeatInBpm = _ui16BaseBeatInBpm;
  ui16BaseNoteForBeat = _ui16BaseNoteForBeat;
  ui16BaseNotesCountPerBar = _ui16BaseNotesCountPerBar;
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
  sequences[1]._initSequence("twins", 2, 1, 2, _barCountForThisSequence, aTwins);
  /*
    ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
    ui16BaseNoteForBeat = 1; a white
    ui16BaseNotesCountPerBar = 2;  => partition en 2/1
  */

  _barCountForThisSequence = 1;
  const short int aAll[_barCountForThisSequence] = {2};  // this is a ref to the
                                                         // number of the single
                                                         // bar associated with
                                                         // this sequence
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

  Serial.println("void sequence::_initSequences(). Ending.");
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
    1. calculates the interval at which the task tPlaySequenceInLoop
    shall iterate (and restart the sequence);
    2. sets such interval (which will only be taken into account after the
    main callback has been called ==> the forenext iteration).

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


  /* 1. Calculate the interval at which each iteration shall occur (each iteration
        will restart the Task _tPlaySequence, so this interval shall be equal to the
        duration of the sequence).  */
  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). about to calculate the duration of the interval for tPlaySequenceinLoop.");}
  unsigned long _duration = _ulSequenceDuration(_activeSequence);
  // if (MY_DG_LASER) {Serial.print("sequence::_oetcbPlaySequenceInLoop(). _duration: ");Serial.println(_duration);}
  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). About to call tPlaySequenceInLoop.setInterval(_duration) ******");}

  /* 2. set the interval between each iteration of tPlaySequenceInLoop */
  tPlaySequenceInLoop.setInterval(_ulSequenceDuration(_activeSequence));

  // if (MY_DG_LASER) {
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
  //   Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
  //   Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
  //   Serial.print("-- void sequence::_oetcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("-- void sequence::_oetcbPlaySequenceInLoop(). ******");
  // }

  Serial.println("sequence::_oetcbPlaySequenceInLoop(). Ending.");
  return true;
}




void sequence::_tcbPlaySequenceInLoop() {
  Serial.println("-- void sequence::_tcbPlaySequenceInLoop(). Starting. ******");

  // if (MY_DG_LASER) {
  //   Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). tPlaySequenceInLoop.getInterval() = ");Serial.println(tPlaySequenceInLoop.getInterval());
  //   Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). tPlaySequenceInLoop.getIterations() = ");Serial.println(tPlaySequenceInLoop.getIterations());
  //   Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). userScheduler.timeUntilNextIteration(tPlaySequenceInLoop) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlaySequenceInLoop));
  //   Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("-- void sequence::_tcbPlaySequenceInLoop(). ******");
  //   Serial.print("-- void sequence::_tcbPlaySequenceInLoop(). about to call sequences[_activeSequence]._playSequence() with _activeSequence = ");Serial.println(_activeSequence);
  // }
  sequences[_activeSequence]._playSequence();

  Serial.println("-- void sequence::_tcbPlaySequenceInLoop(). Ending.");
}




// On disable tPlaySequenceInLoop, turn off all the laser by playing
// sequence 5 ("all off").
void sequence::_odtcbPlaySequenceInLoop() {
  Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). Starting. ******");

  // if (MY_DG_LASER) {
  //   Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). ******");
  //   Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). _activeSequence == ");Serial.println(_activeSequence);
  // }
  //
  if (!(_activeSequence == 5)) {
    // if (MY_DG_LASER) {
    //   Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). _activeSequence is != 5");
    //   Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). _activeSequence is going to be set to 5");
    //   Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). about to call setActiveSequence(5)");
    //   Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). (before calling setActiveSequence(5)) _activeSequence: ");Serial.println(_activeSequence);
    // }
    setActiveSequence(5);
    // if (MY_DG_LASER) {
    //   Serial.print("-- void sequence::_odtcbPlaySequenceInLoop(). (just after calling setActiveSequence(5)) _activeSequence: ");Serial.println(_activeSequence);
    //   Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). about to call sequences[_activeSequence]._playSequence()");
    // }
    sequences[_activeSequence]._playSequence(); // All laser off
  }
  Serial.println("-- void sequence::_odtcbPlaySequenceInLoop(). Ending.");
};




// Helper function to _oetcbPlaySequenceInLoop
// Get the sequence duration, to set the correct interval for tPlaySequenceInLoop
long int sequence::_ulSequenceDuration(const short int __activeSequence) {
  Serial.println("long int sequence::_ulSequenceDuration(). Starting.");
  unsigned long __ulDurationInMs = sequences[__activeSequence]._barCountInSequence *
                     sequences[__activeSequence].ui16BaseNotesCountPerBar *
                     sequences[__activeSequence].ui16BaseNoteForBeat *
                     (60 / sequences[__activeSequence].ui16BaseBeatInBpm * 1000);
  // iterate over each bar pertaining to this sequence and add up their durations
  // for(short int __thisBar = 0; __thisBar < sequences[__activeSequence]._barCountInSequence; __thisBar++){
  //   short int __activeBarIndexNumber = sequences[__activeSequence]._iAssociatedBarsSequence[__thisBar];
  //   __ulDurationInMs = __ulDurationInMs + sequences[__activeSequence]._ulBarDuration(__activeBarIndexNumber);
  // }
  Serial.println("long int sequence::_ulSequenceDuration(). Ending.");
  return __ulDurationInMs;
}









///////////////////////////////////
// Single Sequence Player
///////////////////////////////////
/*
  _playSequence() is the single sequence starter
  It is called by:
  - the main callback of tPlaySequenceInLoop;
  - the onDisable callback of tPlaySequenceInLoop (to turn the lasers off).
  Its roles are to:
  1.  set the interval of the _tPlaySequence task from the tempo of
      the relevant sequence;
  2.  set the number of iterations of the _tPlaySequence task from the
      number of bars in the sequence;
  3.  enable the _tPlaySequence task.
*/
void sequence::_playSequence(){
  Serial.println("void sequence::_playSequence(). Starting");

  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_playSequence(). Just before enabling _tPlaySequence *!*!*!*!*!");
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
  //   Serial.print("void sequence::_playSequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
  //   Serial.print("void sequence::_playSequence(). millis() = ");Serial.println(millis());
  //   Serial.println("void sequence::_playSequence(). *!*!*!*!*!");
  // }

  /*
      1. sets the interval of the _tPlaySequence task
  */
  // _tPlaySequence.setInterval(sequences[_activeSequence].ui16BaseBeatInBpm);
  // Serial.print("void sequence::_playSequence(). Tempo of the sequence: ");Serial.println(sequences[_activeSequence].ui16BaseBeatInBpm);


  /*
      2. sets the number of iterations of the _tPlaySequence task from the
        number of bars in the sequence
  */
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_playSequence(). _tPlaySequence.setIterations() about to be called");
  //   Serial.print("void sequence::_playSequence(). bars count in sequence: ");Serial.println(sequences[_activeSequence]._barCountInSequence);
  // }
  _tPlaySequence.setIterations(sequences[_activeSequence]._barCountInSequence);
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_playSequence(). _tPlaySequence.setIterations() has been called");
  //   Serial.println("void sequence::_playSequence(). Just after _tPlaySequence.setIterations(): *!*!*!*!*!");
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
  //   Serial.print("void sequence::_playSequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
  //   Serial.print("void sequence::_playSequence(). millis() = ");Serial.println(millis());
  //   Serial.println("void sequence::_playSequence(). *!*!*!*!*!");
  //
  //   Serial.println("void sequence::_playSequence(). Task _tPlaySequence about to be enabled");
  // }


  /*
    3. enables the _tPlaySequence task
  */
  _tPlaySequence.enable();
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_playSequence(). Task _tPlaySequence enabled *!*!*!*!*!");
  //   Serial.println("void sequence::_playSequence(). Just after enabling _tPlaySequence");
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  //   Serial.print("void sequence::_playSequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
  //   Serial.print("void sequence::_playSequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
  //   Serial.print("void sequence::_playSequence(). millis() = ");Serial.println(millis());
  //   Serial.println("void sequence::_playSequence(). *!*!*!*!*!");
  // }

  Serial.println("void sequence::_playSequence(). Ending");
};





/*
  Task _tPlaySequence.
  It plays a given sequence once.
  It is enabled
*/
Task sequence::_tPlaySequence(0, 0, &_tcbPlaySequence, &userScheduler, false, NULL, &_odtcbPlaySequence);


void sequence::_tcbPlaySequence(){
  Serial.println("void sequence::_tcbPlaySequence(). Starting.");

  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_tcbPlaySequence(). _tPlaySequence execution parameters: *!*!*!*!*!");
  //   Serial.print("void sequence::_tcbPlaySequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
  //   Serial.print("void sequence::_tcbPlaySequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  //   Serial.print("void sequence::_tcbPlaySequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
  //   Serial.print("void sequence::_tcbPlaySequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
  //   Serial.print("void sequence::_tcbPlaySequence(). millis() = ");Serial.println(millis());
  //   Serial.println("void sequence::_tcbPlaySequence(). *!*!*!*!*!");
  // }

  short _iter = _tPlaySequence.getRunCounter() - 1;
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_tcbPlaySequence(). _iter: ");Serial.println(_iter);
  // }

  // Look for the bar number to read at this iteration
  short int _activeBar = sequences[_activeSequence]._iAssociatedBarsSequence[_iter];
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_tcbPlaySequence(). after setting _activeBar = ");Serial.println(_activeBar);
  // }

  // Play bars
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_tcbPlaySequence(). about to call bar::bars[_activeBar].playBar(_activeBar)");
  // }
  bar::bars[_activeBar].playBar(_activeBar);
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_tcbPlaySequence(). coming back from bar::bars[_activeBar].playBar(_activeBar)");
  // }

  // The next iteration shall occur when..? Easy, the bar wins!!!
  // If the bar has a shorter or longer beatbase, the bar value shall win over
  // the sequence tempo.
  // Reset the interval value of this task to give the bar that is going to be played time
  // to play at its own slower time or avoid blank spots if it plays at a faster rate.

  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_tcbPlaySequence(). About to calculate the _activeBar duration in ms");
  // }
  unsigned long __ulBarDuration = _ulBarDuration(_activeBar);
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_tcbPlaySequence(). __ulBarDuration = ");Serial.println(__ulBarDuration);
  // }

  _tPlaySequence.setInterval(__ulBarDuration);

  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_tcbPlaySequence(). _tPlaySequence just after calling _tPlaySequence.setInterval *!*!*!*!*!");
  //   Serial.println("void sequence::_tcbPlaySequence(). _tPlaySequence execution parameters:");
  //   Serial.print("void sequence::_tcbPlaySequence(). _tPlaySequence.isEnabled() = ");Serial.println(_tPlaySequence.isEnabled());
  //   Serial.print("void sequence::_tcbPlaySequence(). _tPlaySequence.getIterations() = ");Serial.println(_tPlaySequence.getIterations());
  //   Serial.print("void sequence::_tcbPlaySequence(). _tPlaySequence.getInterval() = ");Serial.println(_tPlaySequence.getInterval());
  //   Serial.print("void sequence::_tcbPlaySequence(). userScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(_tPlaySequence));
  //   Serial.print("void sequence::_tcbPlaySequence(). millis() = ");Serial.println(millis());
  //   Serial.println("void sequence::_tcbPlaySequence(). *!*!*!*!*!");
  // }

  Serial.println("void sequence::_tcbPlaySequence(). Ending.");
};


// tPlaySequence disable callback
void sequence::_odtcbPlaySequence(){
  if (MY_DG_LASER) {
    Serial.print("void sequence::_odtcbPlaySequence(). millis() = ");Serial.println(millis());
    Serial.println("void sequence::_odtcbPlaySequence(). tPlaySequence BYE BYE!");
  }
}



// returns the current bar effective duration
long int sequence::_ulBarDuration(const short int _activeBar) {
  Serial.println("void sequence::_ulBarDuration(). Starting.");

  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_ulBarDuration(). _activeBar = ");Serial.println(_activeBar);
  // }
  unsigned long __ulDurationInMs = 0;

  // iterate over each note in the sequence to get their interval in ms based on their tempo
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_ulBarDuration(). bar::bars[_activeBar].iNotesCountInBar = ");Serial.println(bar::bars[_activeBar].iNotesCountInBar);
  // }
  for(short int __thisNote = 0; __thisNote < bar::bars[_activeBar].ui16NotesCountInBar; __thisNote++){
    __ulDurationInMs = __ulDurationInMs + bar::bars[_activeBar].getSingleNoteInterval(__thisNote);
  }

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
