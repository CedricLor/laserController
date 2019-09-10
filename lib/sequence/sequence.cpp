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
std::array<sequence, 7> sequence::sequences;
std::array<bar, 8> sequence::_emptyBarsArray;


// pointer to functions to produce an interface for sequence
void (*sequence::sendCurrentSequence)(const int16_t __i16ActiveSequence) = nullptr;



///////////////////////////////////
// Constructors
///////////////////////////////////
// default constructor
sequence::sequence():
  _beat(beat(0, 0)),
  _barsArray(_emptyBarsArray),
  _i16AssociatedBars(std::array<int16_t, 8> {})
{ 
  _i16AssociatedBars.fill(-1);
}

// parameterized constructor
sequence::sequence(
  const beat & __beat,
  const std::array<bar, 8> & __barsArray,
  const std::array<int16_t, 8> & __i16AssociatedBars
):
  _beat(__beat),
  _barsArray(__barsArray),
  _i16AssociatedBars(__i16AssociatedBars)
{ }

// copy constructor
sequence::sequence(const sequence& __sequence):
  _beat(__sequence._beat),
  _barsArray(__sequence._barsArray),
  _i16AssociatedBars(__sequence._i16AssociatedBars)
{ }

// assignement operator
sequence& sequence::operator=(const sequence& __sequence)
{
  _beat = __sequence._beat;
  _barsArray = __sequence._barsArray;
  _i16AssociatedBars = _i16AssociatedBars;
  // Serial.printf("sequence::operator=(const sequence& ): assignment operator starting\n");
  // Serial.printf("sequence::operator=(const sequence& ): __sequence.ui16GetBaseNotesCountPerBar(): %u\n", __sequence.ui16GetBaseNotesCountPerBar());
  // Serial.printf("sequence::operator=(const sequence& ): __sequence.i16GetBarCountInSequence(): %i\n", __sequence.i16GetBarCountInSequence());
  // Serial.printf("sequence::operator=(const sequence& ): __sequence.getAssociatedBars()[0]: %i\n", __sequence.getAssociatedBars()[0]);
  // Serial.printf("sequence::operator=(const sequence& ): ui16GetBaseNotesCountPerBar(): %u\n", ui16GetBaseNotesCountPerBar());
  // Serial.printf("sequence::operator=(const sequence& ): i16GetBarCountInSequence(): %i\n", i16GetBarCountInSequence());
  // Serial.printf("sequence::operator=(const sequence& ): getAssociatedBeat().getBaseBeatInBpm(): %u\n", getAssociatedBeat().getBaseBeatInBpm());
  // Serial.printf("sequence::operator=(const sequence& ): getAssociatedBeat().getBaseBeatInBpm(): %u\n", getAssociatedBeat().getBaseNoteForBeat());
  // Serial.printf("sequence::operator=(const sequence& ): getAssociatedBars()[0]: %i\n", getAssociatedBars()[0]);
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
  /** array of references to the bars to be played in the sequence.
   *  0 is a reference to _bars[0]
   */
  const std::array<int16_t, 8> & _i16Relays {0}; // _bars[0]
  const std::array<bar, 8> & _relaysBarsArray {bar::_bars[0]};
  /** const beat _beat_2_1(2,1): an instance of beat
   *    _ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_2_1(2,1);
  sequences[0] = {_beat_2_1, _relaysBarsArray, _i16Relays};
  // Serial.println("void sequence::_initSequences(). sequences[0]._ui16BaseBeatInBpm: ");
  // Serial.println(sequences[0]._ui16BaseBeatInBpm);
  // Serial.println("void sequence::_initSequences(). sequences[0]._i16AssociatedBars[0][1]");
  // Serial.println(sequences[0]._i16AssociatedBars[0][1]);


  // --> Sequence 1: "Twins"
  // array of references to the bars to be played in the sequence
  std::array<int16_t, 8> _i16Twins {1}; // _bars[1]
  const std::array<bar, 8> & _twinsBarsArray {bar::_bars[1]};
  sequences[1] = {_beat_2_1, _twinsBarsArray, _i16Twins};
  Serial.printf("sequence::initSequences(). sequences[1].getAssociatedBeat().getBaseNoteForBeat() = %u\n", sequences[1].getAssociatedBeat().getBaseNoteForBeat());
  Serial.printf("sequence::initSequences(). sequences[1].getAssociatedBeat().getBaseBeatInBpm() = %i\n", sequences[1].getAssociatedBeat().getBaseBeatInBpm());
  Serial.printf("sequence::initSequences(). getAssociatedBars()[0]: %i\n", sequences[1].getAssociatedBars()[0]);


  // --> Sequence 2: "All"
  // array of references to the bars to be played in the sequence
  std::array<int16_t, 8> _i16All {2}; // _bars[2]
  const std::array<bar, 8> & _allBarsArray {bar::_bars[1]};
  sequences[2] = {_beat_2_1, _allBarsArray, _i16All};


  // --> Sequence 3: "Swipe Right"
  // array of references to the bars to be played in the sequence
  std::array<int16_t, 8> _i16SwipeR {3}; // _bars[3]
  const std::array<bar, 8> & _swipeRBarsArray {bar::_bars[3]};
  /** const beat _beat_120_1(120,1): an instance of beat
   *    _ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_120_1(120,1);
  sequences[3] = {_beat_120_1, _swipeRBarsArray, _i16SwipeR};


  // --> Sequence 4: "Swipe Left"
  // array of references to the bars to be played in the sequence
  std::array<int16_t, 8> _i16SwipeL {4}; // _bars[4]
  const std::array<bar, 8> & _swipeLBarsArray {bar::_bars[4]};
  sequences[4] = {_beat_120_1, _swipeLBarsArray, _i16SwipeL};


  // --> Sequence 5: "All Off"
  // array of references to the bars to be played in the sequence
  std::array<int16_t, 8> _i16AllOff {5}; // _bars[5]
  const std::array<bar, 8> & _allOffBarsArray {bar::_bars[5]};
  sequences[5] = {_beat_2_1, _allOffBarsArray, _i16AllOff};

  Serial.println("sequence::_initSequences(). Ending.");
}





///////////////////////////////////
// Setters
///////////////////////////////////
/** sequence::setActiveSequence(const int16_t __i16ActiveSequence)
 * 
 *  public static setter 
 * 
 *  Sets _i16ActiveSequence, the static index number of the currently 
 *  active sequence.
 * */
void sequence::setActiveSequence(const int16_t __i16ActiveSequence) {
  Serial.println(F("void sequence::setActiveSequence(). Starting."));
  // Serial.print("void sequence::setActiveSequence(). (before setting) _i16ActiveSequence = %i", _i16ActiveSequence);
  _i16ActiveSequence = __i16ActiveSequence;
  sequences[_i16ActiveSequence]._beat.setActive();
  // Serial.println(F("void sequence::--------------------- checking beat equality ---------------"));
  // const beat & _sequenceBeat = sequences[_i16ActiveSequence]._beat;
  // const beat & _globalBeat = beat::getCurrentBeat();
  // Serial.printf("void sequence::setActiveSequence(). &_sequenceBeat == &_globalBeat ? %i\n", &_sequenceBeat == &_globalBeat);
  // Serial.println(F("void sequence::--------------------- checking beat identity ---------------"));
  // Serial.printf("void sequence::setActiveSequence(). _sequenceBeat.getBaseBeatInBpm() == _globalBeat.getBaseBeatInBpm() ? %i\n", _sequenceBeat.getBaseBeatInBpm() == _globalBeat.getBaseBeatInBpm());
  // Serial.printf("void sequence::setActiveSequence(). _sequenceBeat.getBaseNoteForBeat() == _globalBeat.getBaseNoteForBeat() ? %i\n", _sequenceBeat.getBaseNoteForBeat() == _globalBeat.getBaseNoteForBeat());
  // Serial.print("void sequence::setActiveSequence(). (after setting) _i16ActiveSequence = %i", _i16ActiveSequence);
  Serial.println(F("void sequence::setActiveSequence(). Ending."));
};





///////////////////////////////////
// Getters
///////////////////////////////////
/**uint16_t sequence::i16GetBarCountInSequence()
 *  
 * Instance getter.
 *  
 * Returns the bar count in a given sequence. */
const uint16_t sequence::ui16GetBarCountInSequence() const {
  uint16_t __ui16BarCountInSequence = 0;
  while (sequences[_i16ActiveSequence].getAssociatedBars()[__ui16BarCountInSequence] != -1) {
    __ui16BarCountInSequence++;
  }
  return __ui16BarCountInSequence + 1;
}



/**const beat & getAssociatedBeat() const
 * 
 * Instance getter.
 * 
 * Returns the associated beat */
const beat & sequence::getAssociatedBeat() const {
  return _beat;
}



/**const beat & getAssociatedBars() const
 * 
 * Instance getter.
 * 
 * Returns a pointer to the associated bars array */
const std::array<int16_t, 8> sequence::getAssociatedBars() const {
  return _i16AssociatedBars;
}



/** uint32_t const sequence::_ui32GetSequenceDuration()
 * 
 * Instance getter.
 * 
 * Returns the sequence duration. 
 * Used to set the interval for tPlaySequenceInLoop. */
uint32_t const sequence::_ui32GetSequenceDuration() const {
  Serial.println(F("sequence::_ui32GetSequenceDuration(). Starting."));
  Serial.printf("sequence::_ui32GetSequenceDuration(). __i16activeSequence = %i\n", _i16ActiveSequence);
  uint16_t __ui = 0;
  uint32_t __ui32SequenceDuration = 0;
  while (sequences[_i16ActiveSequence].getAssociatedBars()[__ui] != -1) {
    int16_t __barIndexNumber = sequences[_i16ActiveSequence].getAssociatedBars()[__ui];
    __ui32SequenceDuration += bar::_bars[__barIndexNumber].ui32BarDuration();
    __ui++;
  }
  return __ui32SequenceDuration;
}



/** int16_t const sequence::getCurrentSequence()
 * 
 *  Static getter
 * 
 *  Returns the active sequence index number. */
int16_t const sequence::getCurrentSequence() {
  return _i16ActiveSequence;
}










///////////////////////////////////
// Sequence Players
///////////////////////////////////
/** bar::playSequenceStandAlone():
 *  
 *  play a single sequence calculating the durations
 *  on the basis of the passed-in beat.
 * 
 *  {@ params} beat const & __beat: pass a beat to be taken into account
 *             to calculate the notes duration */
void sequence::playSequenceStandAlone(beat const & __beat, const uint16_t __ui16_associated_sequence_idx_number) {
  tPlaySequenceInLoop.disable();
  tPlaySequence.disable();
  bar::tPlayBar.disable();
  bar::tPlayBar.setOnDisable(NULL);
  note::tPlayNote.disable();

  // 1. reset tPlayNote to play notes read from a bar
  note::resetTPlayNoteToPlayNotesInBar();

  /**2. set the tPlayBar Task to its default when reading bars from a sequence.
   *    tPlayBar will be managed from tPlaySequence => tPlayBar will not need to reset
   *    the beat upon being disabled (tPlaySequence will do it).
   *    
   *    tPlaySequence and tPlayBar will rely on the beat set in sequence
   *    to make the duration calculations.
   *  */
  bar::tPlayBar.setOnDisable(NULL);
  // TODO: drafting

  /**3. set the onDisable callback of tPlaySequence to reset the active beat to (0,0) once
   *    the stand alone sequence has been read, so that any other object that may 
   *    thereafter depend on beat finds a clean beat to start with, as required. */
  tPlaySequence.setOnDisable([](){
    beat(0, 0).setActive();
    tPlaySequence.setOnDisable(_odtcbPlaySequence);
  });

  // 4. set the active sequence
  // TODO: Draft a flexible way to set the active sequence
  setActiveSequence(__ui16_associated_sequence_idx_number);
 
  // 5. set the beat
  // sequences[_i16ActiveSequence]._beat.setActive(); <-- this shall not be called.
  beat(__beat).setActive();

  tPlaySequence.enable();
}

void sequence::playSequenceInBoxState(const uint16_t __ui16_associated_sequence_idx_number) {
  tPlaySequenceInLoop.disable();
  tPlaySequence.disable();
  bar::tPlayBar.disable();
  note::tPlayNote.disable();

  // TODO: the two following lines shall be defined at the beginning of entering 
  //       an automated boxState or even the stepControllerMode
  note::resetTPlayNoteToPlayNotesInBar();
  bar::tPlayBar.setOnDisable(NULL);

  tPlaySequence.setOnDisable(_odtcbPlaySequence);
  
  // TODO: the following line shall be refactored, and get rid of _i16ActiveSequence;
  // to be replaced by a reference _activeSequence
  _i16ActiveSequence = __ui16_associated_sequence_idx_number;
  sequences[_i16ActiveSequence]._beat.setActive();

  tPlaySequenceInLoop.enable();
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
  // if (MY_DG_LASER) {Serial.printf("sequence::_oetcbPlaySequenceInLoop(). sequences[_i16ActiveSequence]._ui32GetSequenceDuration(): %u \n", sequences[_i16ActiveSequence]._ui32GetSequenceDuration());}
  // if (MY_DG_LASER) {Serial.println("sequence::_oetcbPlaySequenceInLoop(). About to call tPlaySequenceInLoop.setInterval(_duration) ******");}

  /* Set the interval between each iteration of tPlaySequenceInLoop
      (each iteration will restart the Task tPlaySequence, so this interval
      shall be equal to the duration of the sequence). */
  tPlaySequenceInLoop.setInterval(sequences[_i16ActiveSequence]._ui32GetSequenceDuration());

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


/** onEnable callback for tPlaySequence
 *  sets the number of iterations to the bar count in this sequence */
bool sequence::_oetcbPlaySequence(){
  // Serial.println("----------------------------on enable tPlaySequence ------------------------------");

  // 1. Set the number of iterations of the tPlaySequence task from the
  //    number of bars in the sequence
  // if (MY_DG_LASER) {
  //   Serial.printf("void sequence::_playSequence(). About to call tPlaySequence.setIterations(%u).\n", sequences[_i16ActiveSequence].ui16GetBarCountInSequence());
  //   Serial.printf("void sequence::_playSequence(). tPlaySequence.getIterations() = %lu", tPlaySequence.getIterations());
  // }
  tPlaySequence.setIterations(sequences[_i16ActiveSequence].ui16GetBarCountInSequence());
  // if (MY_DG_LASER) {
  //   Serial.printf("void sequence::_playSequence(). tPlaySequence.getIterations() = %lu", tPlaySequence.getIterations());
  // }

  // 2. Signal the change of sequence to the mesh
  if (sendCurrentSequence != nullptr) {
    sendCurrentSequence(_i16ActiveSequence);
  }


  return true;
}



/** void sequence::_tcbPlaySequence(): 
 * 
 *  Main callback for tPlaySequence
 *  Each iteration of tPlaySequence corresponds to a bar. 
 *  Accordingly, each iteration runs for an identical a fixed time -> interval.
 * 
 *  The iterations and the interval of the Task have been set in its onEnable 
 *  callback and do not change on iterations.
 * 
 *  At each iteration of tPlaySequence:
 *  - using the Task iterator (step 1. below), we get, from this sequence's 
 *    array of associatedBars, the index number of the bar to be played (step 2 below).
 *  - we then set the active bar in the bar class from the array of hard coded bars 
 *    provided by the class bar, before enabling the Task tPlayBar in the bar class.
 * */
void sequence::_tcbPlaySequence(){
  Serial.println("sequence::_tcbPlaySequence(). Starting.");
  Serial.println(F("------------- DEBUG --------- SEQUENCE --------- DEBUG -------------"));

  // 1. Get the number of iterations (each iteration corresponds to one bar)
  int16_t _i16Iter = tPlaySequence.getRunCounter() - 1;
  Serial.printf("sequence::_tcbPlaySequence(). have set _i16Iter: %i \n", _i16Iter);

  // 2. Select the bar number corresponding to this iteration
  Serial.printf("sequence::_tcbPlaySequence(). _i16ActiveSequence: %i \n", _i16ActiveSequence);
  int16_t _i16ActiveBarId = sequences[_i16ActiveSequence].getAssociatedBars()[_i16Iter];
  Serial.printf("sequence::_tcbPlaySequence(). got _i16ActiveBarId [%i] from sequences[%i]._i16AssociatedBars[%i]\n", _i16ActiveBarId, _i16ActiveSequence, _i16Iter);

  // 3. Calculate the bar duration
  Serial.printf("sequence::_tcbPlaySequence(). about to call bar::_bars[%i].ui32BarDuration().\n", _i16ActiveBarId);
  uint32_t __ui32ThisBarDuration = bar::_bars[_i16ActiveBarId].ui32BarDuration();
  Serial.printf("sequence::_tcbPlaySequence(). got __ui32ThisBarDuration [%u] from bar::_bars[%i].ui32BarDuration()\n", _i16ActiveBarId, __ui32ThisBarDuration);

  if (_i16ActiveBarId != -1) {
    // 4. Play the corresponding bar
    /**TODO: The call to playBarInSequence() here inserted does not take into account 
     *       any differences between bar length requirement at sequence level (3/4: 3 blacks per bar)
     *       and the effective bar length (ex. a bar that would have two whites, for instance.)
     * 
     *       1. Find a way to adjust bar of a given length to sequence of a different length.
     *       2. Create a mode, in sequence, that plays the bars according to their own 
     *          length, at the tempo (beat), defined by the sequence. */
    Serial.printf("sequence::_tcbPlaySequence(). about to call bar::_bars[%i].playBarInSequence()\n", _i16ActiveBarId);
    bar::_bars[_i16ActiveBarId].playBarInSequence();
  
    /**5. Set the interval for next iteration of tPlaySequence
     * 
     *    At each pass, reset the interval before the next iteration of this 
     *    Task sequence::tPlaySequence. This marks the duration of each bar played in the
     *    context of a sequence. */
    Serial.printf("sequence::_tcbPlaySequence(). about to set the interval of tPlaySequence to __ui32ThisBarDuration = [%u]\n", __ui32ThisBarDuration);
    tPlaySequence.setInterval(__ui32ThisBarDuration);
    // Serial.printf("sequence::_tcbPlaySequence(). Set interval: %u ms.\n", __ui32ThisBarDuration);
    // Serial.printf("sequence::_tcbPlaySequence(). Get interval: %lu ms.\n", tPlaySequence.getInterval());
  }

  Serial.println("sequence::_tcbPlaySequence(). Ending.");
};



// tPlaySequence disable callback
void sequence::_odtcbPlaySequence(){
  // if (MY_DG_LASER) {
  //   // Serial.println("----------------------------on disable tPlaySequence ------------------------------");
  //   // Serial.print("sequence::_odtcbPlaySequence(). millis() = ");Serial.println(millis());
  //   // Serial.println("sequence::_odtcbPlaySequence(). tPlaySequence BYE BYE!");
  // }
}