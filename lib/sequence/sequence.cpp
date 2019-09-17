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

bars sequence::globalBars;

const int16_t sequence::_i16_sequence_count = 7;
sequence sequence::_emptySequence;
sequence &sequence::_activeSequence = _emptySequence;
int16_t sequence::_i16ActiveSequenceNb = -1;
std::array<sequence, 7> sequence::sequences;
std::array<bar, 8> sequence::_emptyBarsArray;





// pointer to functions to produce an interface for sequence
void (*sequence::sendCurrentSequence)(const int16_t __i16_active_sequence_id) = nullptr;






///////////////////////////////////
// Constructors
///////////////////////////////////
// default constructor
sequence::sequence():
  _beat(beat{0, 0}),
  _barsArray(_emptyBarsArray)
{ 
}

// parameterized constructor
sequence::sequence(
  const beat & __beat,
  std::array<bar, 8> & __barsArray
):
  _beat(__beat),
  _barsArray(__barsArray)
{ }

// copy constructor
sequence::sequence(const sequence& __sequence):
  _beat(__sequence._beat),
  _barsArray(__sequence._barsArray)
{ }

// assignement operator
sequence& sequence::operator=(const sequence& __sequence)
{
  // Serial.printf("sequence::operator=(const sequence& ): assignment operator starting\n");
  if (&__sequence != this) {
    // Serial.printf("sequence::operator=(const sequence& ): self assignmenttest passed\n");
    // Serial.printf("sequence::operator=(const sequence& ): __sequence._beat.getBaseBeatInBpm(): [%u]\n", __sequence._beat.getBaseBeatInBpm());
    _beat = __sequence._beat;
    // Serial.printf("sequence::operator=(const sequence& ): _beat.getBaseBeatInBpm(): [%u]\n", __sequence._beat.getBaseBeatInBpm());
    // Serial.printf("sequence::operator=(const sequence& ): __sequence._barsArray.at(0)._notesArray.at(0).getNote(): [%u]\n", __sequence._barsArray.at(0)._notesArray.at(0).getNote());
    _barsArray = __sequence._barsArray;
    // Serial.printf("sequence::operator=(const sequence& ): __sequence._barsArray.at(0)._notesArray.at(0).getNote(): [%u]\n", __sequence._barsArray.at(0)._notesArray.at(0).getNote());
  }
  // Serial.printf("sequence::operator=(const sequence& ): assignment operator starting. About to return this.\n");
  return *this;
}










///////////////////////////////////
// Initialisers
///////////////////////////////////
void sequence::initSequences() {
  Serial.println("sequence::_initSequences(). Starting.");

  /** Signature for calls:
    a. a beat instance, composed of:
       1. _ui16BaseBeatInBpm: the base beat in bpm
       2. _ui16BaseNoteForBeat: the base note for each beat (1 -> full, 2 -> white,
                          4 -> black, etc.; the 4 in 2/4)
    b. the array of bars to be played in the sequence (_barsArray).
    
    There used to be an additional param/member:
    - _ui16BaseNotesCountPerBar: the number of base notes per bar (the 2 in 2/4)
    But this param/member has been removed. This comment is kept to make the link with traditional
    musical notation.
    As a consequence of the removal of this param, bars sized to any number of base notes
    can be inserted in sequence, and the sequence will just adapt its tPlaySequence and
    tPlaySequenceInLoop interval to the various bars durations.
    */
  

  // --> Sequence 0: "Relays"
  /** array of bars to be played in the sequence. */
  // Serial.printf("\nsequence::initSequences(). Before building _relaysBarsArray.\n");
  // Serial.printf("sequence::initSequences(). bar::_barsArray[0].getNotesArray().at(0).getNote() should be equal to 1. Is equal to [%u].\n", bar::_barsArray[0].getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). bar::_barsArray[0].getNotesArray().at(0).getToneNumber() should be equal to 7. Is equal to [%u].\n", bar::_barsArray[0].getNotesArray().at(0).getToneNumber());
  // TODO: refactor the bars/sequences factory
  //       1. one single stack bar/sequence
  //       2. pass straight values into the objects 
  //         (ex. _relaysBarsArray could probably be replaced by a direct
  //         call to sequence::globalBars...)
  std::array<bar, 8> _relaysBarsArray {sequence::globalBars._barsArray[0]};
  // Serial.printf("\nsequence::initSequences(). Passed building _relaysBarsArray.\n");
  // Serial.printf("sequence::initSequences(). _relaysBarsArray.at(0).getNotesArray().at(0).getNote() should be equal to 1. Is equal to [%u].\n", _relaysBarsArray.at(0).getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). _relaysBarsArray.at(0).getNotesArray().at(0).getToneNumber() should be equal to 7. Is equal to [%u].\n", _relaysBarsArray.at(0).getNotesArray().at(0).getToneNumber());
  /** const beat _beat_2_1(2,1): an instance of beat
   *    _ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_2_1(2,1);
  // Serial.printf("\nsequence::initSequences(). Passed building _beat_2_1.\n");
  // Serial.printf("sequence::initSequences(). _beat_2_1.getBaseBeatInBpm() should be equal to 2. Is equal to [%u].\n", _beat_2_1.getBaseBeatInBpm());
  sequences[0] = {_beat_2_1, _relaysBarsArray};
  // Serial.printf("\nsequence::initSequences(). sequences[0].getBarsArray()[0].getNotesArray().at(0).getNote() shall be equal to 1. Is equal to [%i]\n", sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). sequences[0].getBarsArray()[0].getNotesArray().at(0).getToneNumber() shall be equal to 7. Is equal to [%i]\n", sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
  // Serial.printf("sequence::initSequences(). sequences[0].getAssociatedBeat().getBaseBeatInBpm() should be equal to 2. Is equal to [%u]\n", sequences[0].getAssociatedBeat().getBaseBeatInBpm());


  // --> Sequence 1: "Twins"
  std::array<bar, 8> _twinsBarsArray {sequence::globalBars._barsArray[1]};
  sequences[1] = {_beat_2_1, _twinsBarsArray};


  // --> Sequence 2: "All"
  std::array<bar, 8> _allBarsArray {sequence::globalBars._barsArray[2]};
  sequences[2] = {_beat_2_1, _allBarsArray};


  // --> Sequence 3: "Swipe Right"
  std::array<bar, 8> _swipeRBarsArray {sequence::globalBars._barsArray[3]};
  /** const beat _beat_120_1(120,1): an instance of beat
   *    _ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_120_1(120,1);
  sequences[3] = {_beat_120_1, _swipeRBarsArray};


  // --> Sequence 4: "Swipe Left"
  std::array<bar, 8> _swipeLBarsArray {sequence::globalBars._barsArray[4]};
  sequences[4] = {_beat_120_1, _swipeLBarsArray};


  // --> Sequence 5: "All Off"
  std::array<bar, 8> _allOffBarsArray {sequence::globalBars._barsArray[5]};
  sequences[5] = {_beat_2_1, _allOffBarsArray};

  Serial.println("sequence::_initSequences(). Ending.");
}







///////////////////////////////////
// Setters
///////////////////////////////////
/** sequence::setActive()
 * 
 *  public instance setter 
 * 
 *  sets the caller sequence as the static variable &sequence::_sequence.
 * */
void sequence::setActive(const int16_t __i16_active_sequence_id) {
  _activeSequence = *this;
  _i16ActiveSequenceNb = __i16_active_sequence_id;
}










///////////////////////////////////
// Getters
///////////////////////////////////
/** sequence & sequence::getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id) 
 * 
 * Returns a const ref to one of the hard coded bars given its index number */
sequence & sequence::getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id) {
  return sequences.at(__ui16_sequence_id);
}



/**uint16_t const sequence::ui16GetBarCountInSequence() const 
 *  
 * Instance getter.
 *  
 * Returns the bar count in a given sequence. */
uint16_t const sequence::ui16GetBarCountInSequence() const {
  uint16_t __ui16BarCountInSequence = 0;
  while (getBarsArray().at(__ui16BarCountInSequence).ui16GetNotesCountInBar() > 0) {
    __ui16BarCountInSequence++;
  }
  return __ui16BarCountInSequence;
}



/**beat const & sequence::getAssociatedBeat() const 
 * 
 * Instance getter.
 * 
 * Returns the associated beat */
beat const & sequence::getAssociatedBeat() const {
  return _beat;
}



/**std::array<bar, 8> const & sequence::getBarsArray() const
 * 
 * Instance getter.
 * 
 * Returns a pointer to the associated bars array */
std::array<bar, 8> const & sequence::getBarsArray() const {
  return _barsArray;
}



/** uint32_t const sequence::ui32GetSequenceDuration()
 * 
 * Instance getter.
 * 
 * Returns the sequence duration. 
 * Used to set the interval for tPlaySequenceInLoop. */
uint32_t const sequence::ui32GetSequenceDuration() const {

  uint32_t __ui32SequenceDuration = 0;
  uint16_t __ui16BarCountInSequence = ui16GetBarCountInSequence();
  for (uint16_t __ui = 0; __ui < __ui16BarCountInSequence; __ui++) {
    __ui32SequenceDuration += getBarsArray().at(__ui).ui32GetBarDuration();
  }

  return __ui32SequenceDuration;
}



/** int16_t const sequence::getCurrentSequence()
 * 
 *  Static getter
 * 
 *  Returns the active sequence index number. */
int16_t const sequence::getCurrentSequence() {
  return _i16ActiveSequenceNb;
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
 *             to calculate the notes duration 
 *  {@ params} const int16_t __i16_sequence_id: optional sequence id in the 
 *             sequence array (might be needed for debug and interface purpose) 
 * */
void sequence::playSequenceStandAlone(beat const & __beat, const int16_t __i16_sequence_id) {
  /** 1. disable all previously eventually enabled laser Tasks,
   *     in order to start with a clean Tasks state when playing a sequence stand alone
   */
  tPlaySequenceInLoop.disable();
  tPlaySequence.disable();
  bar::tPlayBar.disable();
  bar::_notes.tPlayNote.disable();

  // 1. reset tPlayNote to play notes read from a bar
  notes{}.disableAndResetTPlayNote();

  /**2. set the tPlayBar Task to its default when reading bars from a sequence.
   *    tPlayBar will be managed from tPlaySequence => tPlayBar will not need to reset
   *    the beat upon being disabled (tPlaySequence will do it).
   *    
   *    tPlaySequence and tPlayBar will rely on the beat set in sequence
   *    to make the duration calculations.
   *  */
  bar::tPlayBar.setOnDisable(NULL);

  /**3. set the onDisable callback of tPlaySequence to reset the active beat to (0,0) once
   *    the stand alone sequence has been read, so that any other object that may 
   *    thereafter depend on beat finds a clean beat to start with, as required. */
  tPlaySequence.setOnDisable([](){
    beat(0, 0).setActive();
    tPlaySequence.setOnDisable(_odtcbPlaySequence);
  });

  // 4. set the active sequence and associated beat
  setActive(__i16_sequence_id);
 
  beat(__beat).setActive();

  tPlaySequence.enable();
}


void sequence::playSequenceInBoxState(const int16_t __i16_sequence_id) {
  // 1. disable all previously eventually enabled laser Task
  //    TODO: check whether this shall be done here or elsewhere (in boxState)
  tPlaySequenceInLoop.disable();
  tPlaySequence.disable();
  bar::tPlayBar.disable();
  bar::_notes.tPlayNote.disable();

  // 2. set the underlying laser Tasks for a sequence -> bar -> note -> tone chain
  //    TODO: the two following lines shall be defined at the beginning of entering 
  //          an automated boxState or even the stepControllerMode
  notes{}.disableAndResetTPlayNote();
  bar::tPlayBar.setOnDisable(NULL);

  // 3. set this Task for a regular step -> boxState -> sequence -> bar -> note -> tone chain 
  //    (as opposed to a standAlone play of the sequence)
  tPlaySequence.setOnDisable(_odtcbPlaySequence);
  
  // 4. set the active sequence
  setActive(__i16_sequence_id);

  // 5. set the beat
  _beat.setActive();

  tPlaySequenceInLoop.enable();
}





///////////////////////////////////
// Loop Player
///////////////////////////////////
/*
    tPlayBoxState plays once (unless restarted by tPlayBoxStates)
      - tPlayBoxState -> onEnable: enables tPlaySequenceInLoop by calling
        playSequenceInBoxState();
      - tPlayBoxState -> onDisable: disables tPlaySequenceInLoop
    tPlaySequenceInLoop plays forever (until interrupt by tPlayBoxState):
      - tPlaySequenceInLoop -> onEnable: calculates the sequence duration and
            sets the interval for tPlaySequenceInLoop
      - tPlaySequenceInLoop -> main callback: starts tPlaySequence
      - tPlaySequenceInLoop -> onDisable: plays sequence 5 (all off)
*/

/*
    tPlaySequenceInLoop

    tPlaySequenceInLoop plays a sequence in loop, for an unlimited number
    of iterations, until it is disabled by tPlayBoxState.

    tPlaySequenceInLoop is enabled and disabled by the onEnable and onDisable
    callbacks of tPlayBoxState.

    Upon entering a new boxState (startup, IR signal received, etc.), the onEnable 
    callback of tPlayBoxState task calls the playSequenceInBoxState() method of
    sequence::sequences[_currentBoxState.ui16AssociatedSequence].

    _currentBoxState.ui16AssociatedSequence is the index number, in the 
    sequences array, of the sequence associated with the calling boxState.

    This Task tPlaySequenceInLoop is enabled by playSequenceInBoxState, until 
    being disabled by the boxState::tPlayBoxState onDisable callback.
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
  //   Serial.print("sequence::_oetcbPlaySequenceInLoop(). _i16ActiveSequenceNb: [%i] \n",_i16ActiveSequenceNb);
  // }

  // if (MY_DG_LASER) {
  //   Serial.printf("sequence::_oetcbPlaySequenceInLoop(). About to call tPlaySequenceInLoop.setInterval(%u)", _activeSequence.ui32GetSequenceDuration());
  // }

  /* Set the interval between each iteration of tPlaySequenceInLoop
      (each iteration will restart the Task tPlaySequence, so this interval
      shall be equal to the duration of the sequence). */
  tPlaySequenceInLoop.setInterval(_activeSequence.ui32GetSequenceDuration());

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
  //   Serial.printf("sequence::_odtcbPlaySequenceInLoop(). _i16ActiveSequenceNb == [%i]\n", _i16ActiveSequenceNb);
  // }
  //
  if (_i16ActiveSequenceNb != 5) {
    // if (MY_DG_LASER) {
    //   Serial.printf("sequence::_odtcbPlaySequenceInLoop(). _i16ActiveSequenceNb == [%i]\n", _i16ActiveSequenceNb);
    //   Serial.println("sequence::_odtcbPlaySequenceInLoop(). About to call sequences[5].playSequenceInBoxState(5).");
    // }
    sequences[5].playSequenceInBoxState(5);
    // if (MY_DG_LASER) {
    //   Serial.print("sequence::_odtcbPlaySequenceInLoop(). Just after calling sequences[5].playSequenceInBoxState(5).\n");
    // }
  }
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
  //   Serial.printf("void sequence::_playSequence(). About to call tPlaySequence.setIterations(%u).\n", _activeSequence.ui16GetBarCountInSequence());
  //   Serial.printf("void sequence::_playSequence(). tPlaySequence.getIterations() = %lu", tPlaySequence.getIterations());
  // }
  tPlaySequence.setIterations(_activeSequence.ui16GetBarCountInSequence());
  // if (MY_DG_LASER) {
  //   Serial.printf("void sequence::_playSequence(). tPlaySequence.getIterations() = %lu", tPlaySequence.getIterations());
  // }

  // 2. Signal the change of sequence to the mesh
  if (sendCurrentSequence != nullptr) {
    sendCurrentSequence(_i16ActiveSequenceNb);
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

  // 2. Calculate the bar duration
  uint32_t __ui32ThisBarDuration = _activeSequence.getBarsArray().at(_i16Iter).ui32GetBarDuration();
  Serial.printf("sequence::_tcbPlaySequence(). got __ui32ThisBarDuration [%u] from sequences[%i].getBarsArray().at(%i).ui32BarDuration()\n", __ui32ThisBarDuration, _i16ActiveSequenceNb, _i16Iter);

  // 3. Play the corresponding bar
  Serial.printf("sequence::_tcbPlaySequence(). about to call sequences[%i].getBarsArray()[%i].playBarInSequence(%i)\n", _i16ActiveSequenceNb, _i16Iter, _i16Iter);
  sequence::globalBars.playBarInSequence(_activeSequence.getBarsArray().at(_i16Iter));

  /**5. Set the interval for next iteration of tPlaySequence
   * 
   *    At each pass, reset the interval before the next iteration of this 
   *    Task sequence::tPlaySequence. This marks the duration of each bar played in the
   *    context of a sequence. */
  Serial.printf("sequence::_tcbPlaySequence(). about to set the interval of tPlaySequence to __ui32ThisBarDuration = [%u]\n", __ui32ThisBarDuration);
  tPlaySequence.setInterval(__ui32ThisBarDuration);
  // Serial.printf("sequence::_tcbPlaySequence(). Set interval: %u ms.\n", __ui32ThisBarDuration);
  // Serial.printf("sequence::_tcbPlaySequence(). Get interval: %lu ms.\n", tPlaySequence.getInterval());

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