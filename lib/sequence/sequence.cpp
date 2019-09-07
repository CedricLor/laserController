/*
  sequence.cpp - sequences are precoded sequences of bars
  Created by Cedric Lor, June 4, 2019.

Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "sequence.h"

int16_t sequence::_i16ActiveSequence = 0;
const int16_t sequence::_i16_sequence_count = 7;
sequence sequence::sequences[_i16_sequence_count];


// pointer to functions to produce an interface for sequence
void (*sequence::sendCurrentSequence)(const int16_t __i16ActiveSequence) = nullptr;



///////////////////////////////////
// Constructors
///////////////////////////////////
// default constructor
sequence::sequence():
  _beat(beat(0, 0)),
  _ui16BaseNotesCountPerBar(0), 
  _i16BarCountInSequence(0), 
  _i16AssociatedBars(nullptr)
{ }

// parameterized constructor
sequence::sequence(
  const beat __beat,
  const uint16_t __ui16BaseNotesCountPerBar, 
  const int16_t __i16BarCountInSequence, 
  int16_t *__i16AssociatedBars
):
  _beat(__beat),
  _ui16BaseNotesCountPerBar(__ui16BaseNotesCountPerBar), 
  _i16BarCountInSequence(__i16BarCountInSequence), 
  _i16AssociatedBars(__i16AssociatedBars)
{ }

// copy constructor
sequence::sequence( const sequence& ) 
{ }

// assignement operator
sequence& sequence::operator=(const sequence& )
{
  return *this;
}










///////////////////////////////////
// Initialisers
///////////////////////////////////
void sequence::initSequences() {
  Serial.println("sequence::_initSequences(). Starting.");

  /** Signature for calls:
    a. the sequence's name
    b. a beat instance, composed of:
       1. _ui16BaseBeatInBpm: the base beat in bpm
       2. _ui16BaseNoteForBeat: the base note for each beat (1 -> full, 2 -> white,
                          4 -> black, etc.; the 4 in 2/4)
    d. _ui16BaseNotesCountPerBar: the number of base notes per bar (the 2 in 2/4)
    e. the number of bars in the sequence _i16BarCountForThisSequence
    f. the array of references to the bars to be played in the sequence (_i16AssociatedBars) */
  

  // --> Sequence 0: "Relays"
  int16_t _i16BarCountForThisSequence = 1;
  /** array of references to the bars to be played in the sequence.
   *  0 is a reference to _bars[0]
   */
  int16_t _i16Relays[_i16BarCountForThisSequence] = {0}; // _bars[0]
  /** const beat _beat_2_1(2,1): an instance of beat
   *    _ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_2_1(2,1);
  /** _ui16BaseNotesCountPerBar = 2;  => partition en 2/1 */
  sequences[0] = {_beat_2_1, 2, _i16BarCountForThisSequence, _i16Relays};
  // Serial.println("void sequence::_initSequences(). sequences[0]._ui16BaseBeatInBpm: ");
  // Serial.println(sequences[0]._ui16BaseBeatInBpm);
  // Serial.println("void sequence::_initSequences(). sequences[0]._i16AssociatedBars[0][1]");
  // Serial.println(sequences[0]._i16AssociatedBars[0][1]);


  // --> Sequence 1: "Twins"
  _i16BarCountForThisSequence = 1;
  // array of references to the bars to be played in the sequence
  int16_t _i16Twins[_i16BarCountForThisSequence] = {1};
  /**
   * _ui16BaseNotesCountPerBar = 2;  => partition en 2/1 */
  sequences[1] = {_beat_2_1, 2, _i16BarCountForThisSequence, _i16Twins};


  // --> Sequence 2: "All"
  _i16BarCountForThisSequence = 1;
  // array of references to the bars to be played in the sequence
  int16_t _i16All[_i16BarCountForThisSequence] = {2};
  /**
   * _ui16BaseNotesCountPerBar = 2;  => partition en 2/1 */
  sequences[2] = {_beat_2_1, 2, _i16BarCountForThisSequence, _i16All};


  // --> Sequence 3: "Swipe Right"
  _i16BarCountForThisSequence = 1;
  // array of references to the bars to be played in the sequence
  int16_t _i16SwipeR[_i16BarCountForThisSequence] = {3};
  /** const beat _beat_120_1(120,1): an instance of beat
   *    _ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_120_1(120,1);
  /** _ui16BaseNotesCountPerBar = 4;  => partition en 4/1 */
  sequences[3] = {_beat_120_1, 4, _i16BarCountForThisSequence, _i16SwipeR};


  // --> Sequence 4: "Swipe Left"
  _i16BarCountForThisSequence = 1;
  // array of references to the bars to be played in the sequence
  int16_t _i16SwipeL[_i16BarCountForThisSequence] = {4};
  /**
   * _ui16BaseNotesCountPerBar = 4;  => partition en 4/1 */
  sequences[4] = {_beat_120_1, 4, _i16BarCountForThisSequence, _i16SwipeL};


  // --> Sequence 5: "All Off"
  _i16BarCountForThisSequence = 1;
  // array of references to the bars to be played in the sequence
  int16_t _i16AllOff[_i16BarCountForThisSequence] = {5};
  /**
   * _ui16BaseNotesCountPerBar = 1;  => partition en 1/1*/
  sequences[5] = {_beat_2_1, 1, _i16BarCountForThisSequence, _i16AllOff};

  Serial.println("sequence::_initSequences(). Ending.");
}





///////////////////////////////////
// Getters
///////////////////////////////////
/**uint16_t sequence::ui16GetBaseNotesCountPerBar()
 * 
 * Instance getter.
 *  
 * Returns the base note count per bar in a given sequence 
 * (the 2 in a 2/4 tempo). */
uint16_t sequence::ui16GetBaseNotesCountPerBar() {
  return _ui16BaseNotesCountPerBar;  
}



/**uint16_t sequence::ui16GetBaseNotesCountPerBar()
 *  
 * Instance getter.
 *  
 * Returns the bar count in a given sequence. */
int16_t sequence::i16GetBarCountInSequence() {
  return _i16BarCountInSequence;
}







///////////////////////////////////
// Loop Player
///////////////////////////////////
/*
    tPlayBoxState plays once (unless restarted by tPlayBoxStates)
      - tPlayBoxState -> onEnable: enables tPlaySequenceInLoop
      - tPlayBoxState -> onDisable: disables tPlaySequenceInLoop
    tPlaySequenceInLoop plays forever (until interrupt by tPlayBoxState):
      - tPlaySequenceInLoop -> onEnable: calculates the sequence duration and
            sets the interval for tPlaySequenceInLoop
      - tPlaySequenceInLoop -> main callback: starts playing the active sequence
      - tPlaySequenceInLoop -> onDisable: plays sequence 5 (all off)
*/

/*
    tPlaySequenceInLoop

    tPlaySequenceInLoop plays a sequence in loop, for an unlimited number
    of iterations, until it is disabled by tPlayBoxState.

    tPlaySequenceInLoop is enabled and disabled by the onEnable and onDisable
    callbacks of tPlayBoxState.

    Upon entering a new boxState (startup, IR signal received, etc.),
    the onEnable callback of tPlayBoxState task sets sequence::_i16ActiveSequence
    to the sequence index number associated with this boxState.

    Then the Task tPlaySequenceInLoop is enabled, until being disabled by the
    boxState::tPlayBoxState onDisable callback.
*/
Task sequence::tPlaySequenceInLoop(0, TASK_FOREVER, &_tcbPlaySequenceInLoop, NULL/*&mns::myScheduler*/, false, &_oetcbPlaySequenceInLoop, &_odtcbPlaySequenceInLoop);



/*
    _oetcbPlaySequenceInLoop()

    Upon enabling the tPlaySequenceInLoop task, _oetcbPlaySequenceInLoop():
    1. sets the interval at which the task tPlaySequenceInLoop shall iterate
    (and restart the sequence). This interval will only be taken into account
    after the main callback has been called ==> the forenext iteration).

    This calculation is made based on the base of the base beat in bpm, the base note,
    the number of base notes per bars and the number of bars in the sequence.

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
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). _i16ActiveSequence: ");Serial.println(_i16ActiveSequence);
  // }

  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). about to calculate the duration of the interval for tPlaySequenceinLoop.");}
  // if (MY_DG_LASER) {Serial.printf("sequence::_oetcbPlaySequenceInLoop(). _ulSequenceDuration(_i16ActiveSequence): %i \n", _ulSequenceDuration(_i16ActiveSequence));}
  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). About to call tPlaySequenceInLoop.setInterval(_duration) ******");}

  /* Set the interval between each iteration of tPlaySequenceInLoop
      (each iteration will restart the Task tPlaySequence, so this interval
      shall be equal to the duration of the sequence). */
  tPlaySequenceInLoop.setInterval(_ulSequenceDuration());

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
  // }
  Serial.println("sequence::_tcbPlaySequenceInLoop(). about to enable tPlaySequence");
  tPlaySequence.enable();

  Serial.println("sequence::_tcbPlaySequenceInLoop(). Ending.");
}




// On disable tPlaySequenceInLoop, turn off all the laser by playing
// sequence 5 ("all off").
void sequence::_odtcbPlaySequenceInLoop() {
  Serial.println("sequence::_odtcbPlaySequenceInLoop(). Starting. ******");

  // if (MY_DG_LASER) {
  //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). millis() = ");Serial.println(millis());
  //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). ******");
  //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). _i16ActiveSequence == ");Serial.println(_i16ActiveSequence);
  // }
  //
  if (!(_i16ActiveSequence == 5)) {
    // if (MY_DG_LASER) {
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). _i16ActiveSequence is != 5");
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). _i16ActiveSequence is going to be set to 5");
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). about to call setActiveSequence(5)");
    //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). (before calling setActiveSequence(5)) _i16ActiveSequence: ");Serial.println(_i16ActiveSequence);
    // }
    setActiveSequence(5);
    // if (MY_DG_LASER) {
    //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). (just after calling setActiveSequence(5)) _i16ActiveSequence: ");Serial.println(_i16ActiveSequence);
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). about to call sequences[_i16ActiveSequence]._playSequence()");
    // }
  }
  tPlaySequence.enable();
  Serial.println("sequence::_odtcbPlaySequenceInLoop(). Ending.");
};








///////////////////////////////////
// Single Sequence Player
///////////////////////////////////
/*
  Task tPlaySequence.
  It plays a given sequence once.
*/
Task sequence::tPlaySequence(0, 0, &_tcbPlaySequence, NULL/*&mns::myScheduler*/, false, &_oetcbPlaySequence, &_odtcbPlaySequence);


// onEnable callback for tPlaySequence
// 1. sets the interval for each iterations to duration of the bars (each iteration is a bar)
// 2. sets the number of iterations to the bar count in this sequence
bool sequence::_oetcbPlaySequence(){
  // Serial.println("----------------------------on enable tPlaySequence ------------------------------");

  // 1. sets the interval of the tPlaySequence task
  tPlaySequence.setInterval(_ulBarDuration());
  // Serial.printf("void sequence::_oetcbPlaySequence(). Set interval: %lu ms.\n", _ulBarDuration(_i16ActiveSequence));
  // Serial.printf("void sequence::_oetcbPlaySequence(). Get interval: %lu ms.\n", tPlaySequence.getInterval());

  // 2. sets the number of iterations of the tPlaySequence task from the
  //    number of bars in the sequence
  // if (MY_DG_LASER) {
  //   Serial.println("void sequence::_playSequence(). tPlaySequence.setIterations() about to be called");
  //   Serial.print("void sequence::_playSequence(). _bars count in sequence: ");Serial.println(sequences[_i16ActiveSequence]._i16BarCountInSequence);
  //   Serial.print("void sequence::_playSequence(). tPlaySequence.getIterations() = ");Serial.println(tPlaySequence.getIterations());
  // }
  tPlaySequence.setIterations(sequences[_i16ActiveSequence]._i16BarCountInSequence);
  // if (MY_DG_LASER) {
  //   Serial.print("void sequence::_playSequence(). tPlaySequence.getIterations() = ");Serial.println(tPlaySequence.getIterations());
  // }

  // 3. Signal the change of state to the mesh
  if (sendCurrentSequence != nullptr) {
    sendCurrentSequence(_i16ActiveSequence);
  }


  return true;
}



// Main callback for tPlaySequence
// Each iteration of tPlaySequence corresponds to a bar. Accordingly, each iteration
// runs for an identical a fixed time -> interval.
// The iterations and the interval of the Task have been set in its onEnable
// callback and do not change on iterations.
// At each iteration of tPlaySequence:
// - we read in the sequence the number of the next bar <- from the iterator of the
//   Task.
// - we then apply the settings (tempo in BPM, base note for beats and the notes
//   count in each bar) of this sequence to the bar, which might have other
//   settings.
// - we then inform the bar class of the active bar number, before enabling
//   the Task tPlayBar in the bar class.
void sequence::_tcbPlaySequence(){
  Serial.println("sequence::_tcbPlaySequence(). Starting.");
  // Serial.println(F("------------- DEBUG --------- SEQUENCE --------- DEBUG -------------"));

  // 1. Get the number of iterations (each iteration corresponds to one bar)
  int16_t _i16Iter = tPlaySequence.getRunCounter() - 1;
  // Serial.println("sequence::_tcbPlaySequence(). have set _iter: " + String(_iter));

  // 2. Select the bar number corresponding to this iteration
  int16_t _i16ActiveBar = sequences[_i16ActiveSequence]._i16AssociatedBars[_i16Iter];
  // Serial.println("sequence::_tcbPlaySequence(). got _i16ActiveBar: sequences[" + String(_i16ActiveSequence) + "]._i16AssociatedBars[" + String(_iter) + "]");
  // Serial.println("sequence::_tcbPlaySequence(). have _i16ActiveBar: " + String(_i16ActiveBar));

  // 3. Configure the bar
  bar::_bars[_i16ActiveBar]._ui16BaseNotesCountInBar = sequences[_i16ActiveSequence]._ui16BaseNotesCountPerBar; // the 2 in 2/4, for instance
  // Serial.println("sequence::_tcbPlaySequence(). got sequences[" + String(_i16ActiveSequence) + "]._ui16BaseNotesCountPerBar = " + String(sequences[_i16ActiveSequence]._ui16BaseNotesCountPerBar));
  // Serial.println("sequence::_tcbPlaySequence(). have bar::_bars[" + String(_i16ActiveBar) + "]._ui16BaseNotesCountInBar = " + String(bar::_bars[_i16ActiveBar]._ui16BaseNotesCountInBar));

  // 4. Play the corresponding bar
  // Serial.println("sequence::_tcbPlaySequence(). calling bar::setActiveBar(" + String(_i16ActiveBar) + ")");
  bar::setActiveBar(_i16ActiveBar);
  // Serial.println("sequence::_tcbPlaySequence(). enabling tPlayBar");
  bar::tPlayBar.enable();

  Serial.println("void sequence::_tcbPlaySequence(). Ending.");
};



// tPlaySequence disable callback
void sequence::_odtcbPlaySequence(){
  // if (MY_DG_LASER) {
  //   // Serial.println("----------------------------on disable tPlaySequence ------------------------------");
  //   // Serial.print("sequence::_odtcbPlaySequence(). millis() = ");Serial.println(millis());
  //   // Serial.println("sequence::_odtcbPlaySequence(). tPlaySequence BYE BYE!");
  // }
}







///////////////////////////////////
// Helpers
///////////////////////////////////


// Helper function to _oetcbPlaySequenceInLoop
// Get the sequence duration, to set the correct interval for tPlaySequenceInLoop
long unsigned int sequence::_ulSequenceDuration() {
  Serial.println(F("sequence::_ulSequenceDuration(). Starting."));
  // Serial.printf("void sequence::_ulBarDuration(). __i16activeSequence = %u", __i16activeSequence);
  unsigned long __ulDurationInMs = sequences[_i16ActiveSequence]._i16BarCountInSequence * _ulBarDuration();
  Serial.println(F("sequence::_ulSequenceDuration(). Ending."));
  return __ulDurationInMs;
}



// Helper function to tPlaySequence
// returns the current bar effective duration
long unsigned int sequence::_ulBarDuration() {
  Serial.println(F("void sequence::_ulBarDuration(). Starting."));
  // Serial.printf("void sequence::_ulBarDuration(). __i16ActiveSequence = %i\n", __i16activeSequence);
  // Serial.printf("void sequence::_ulBarDuration(). sequences[%i]._ui16BaseNotesCountPerBar = %u\n", __i16activeSequence, sequences[_i16ActiveSequence]._ui16BaseNotesCountPerBar);
  // Serial.printf("void sequence::_ulBarDuration(). sequences[%i]._beat.ui16GetBaseNoteDurationInMs() = %u\n", __i16activeSequence, sequences[_i16ActiveSequence]._beat.ui16GetBaseNoteDurationInMs());
  // Serial.printf("void sequence::_ulBarDuration(). about to return %u\n": ,sequences[_i16ActiveSequence]._ui16BaseNotesCountPerBar * sequences[_i16ActiveSequence]._beat.ui16GetBaseNoteDurationInMs());
  return (sequences[_i16ActiveSequence]._ui16BaseNotesCountPerBar * beat::getCurrentBeat().ui16GetBaseNoteDurationInMs());
}







// Set the active sequence
void sequence::setActiveSequence(const int16_t __i16ActiveSequence) {
  Serial.println(F("-void sequence::setActiveSequence(). Starting."));
  // Serial.print(F("-void sequence::setActiveSequence(). (before setting) _i16ActiveSequence = "));Serial.println(_i16ActiveSequence);
  sequences[_i16ActiveSequence]._beat.setActiveBeatFromBeat(sequences[_i16ActiveSequence]._beat);
  _i16ActiveSequence = __i16ActiveSequence;
  // Serial.print(F("-void sequence::setActiveSequence(). (after setting) _i16ActiveSequence = "));Serial.println(_i16ActiveSequence);
  Serial.println(F("-void sequence::setActiveSequence(). Ending."));
};



// Get the active sequence
int16_t sequence::getCurrentSequence() {
  return _i16ActiveSequence;
}