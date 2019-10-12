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






///////////////////////////////////
// Constructors
///////////////////////////////////
// default constructor
sequence::sequence():
  i16IndexNumber(-2),
  _beat(beat{0, 0}),
  _barsArray({})
{ 
}

// parameterized constructor
sequence::sequence(
  const beat & __beat,
  std::array<bar, 8> & __barsArray,
  int16_t __i16IndexNumber
):
  i16IndexNumber(__i16IndexNumber),
  _beat(__beat),
  _barsArray(__barsArray)
{ }

// copy constructor
sequence::sequence(const sequence& __sequence):
  i16IndexNumber(__sequence.i16IndexNumber),
  _beat(__sequence._beat),
  _barsArray(__sequence._barsArray)
{ }

// assignement operator
sequence& sequence::operator=(const sequence& __sequence)
{
  // Serial.printf("sequence::operator=(const sequence& ): assignment operator starting\n");
  if (&__sequence != this) {
    // Serial.printf("sequence::operator=(const sequence& ): self assignmenttest passed\n");
    i16IndexNumber = __sequence.i16IndexNumber;
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
// Getters
///////////////////////////////////
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
uint32_t const sequence::ui32GetSequenceDuration(beat const & __beat) const {
  uint32_t __ui32SequenceDuration = 0;
  uint16_t __ui16BarCountInSequence = ui16GetBarCountInSequence();
  for (uint16_t __ui = 0; __ui < __ui16BarCountInSequence; __ui++) {
    __ui32SequenceDuration += getBarsArray().at(__ui).ui32GetBarDuration(__beat);
  }

  return __ui32SequenceDuration;
}



/** sequence const & sequences::getCurrentSequence() const
 * 
 * Returns the active sequence as a sequence */
sequence const & sequences::getCurrentSequence() const {
  return _activeSequence;
}













//****************************************************************//
// Sequences //***************************************************//
//****************************************************************//

/** sequences::sequences()
 * 
 *  default constructor */
sequences::sequences(
  void (*_sendCurrentSequence)(const int16_t __i16_active_sequence_id)
): 
  sendCurrentSequence(_sendCurrentSequence),
  sequencesArray({}),
  tPlayBar(_bars.tPlayBar),
  tPlayNote(_bars.tPlayNote)
{
  // 1. Disable and reset the Task tPlaySequence and tPlaySequenceInLoop
  disableAndResetPlaySequenceTasks();

  // 2. Define sequencesArray, an array containing a series of hard coded sequences

  /** Signature of a sequence:
   *  a. a beat instance, composed of:
   *    1. _ui16BaseBeatInBpm: the base beat in bpm (e.g. 120 bpm, 2 bpm, etc.);
   *    2. _ui16BaseNoteForBeat: the base note for each beat (e.g. 4 -> black, etc.; the 4 in 2/4);
   *  b. the array of bars to be played in the sequence (_barsArray);
   *  c. the sequence's index number in the sequencesArray.
   * 
   *  There used to be an additional param/member:
   *  - _ui16BaseNotesCountPerBar: the number of base notes per bar (the 2 in 2/4).
   *  This param/member has been removed. This comment is kept for reference purposes,
   *  The purpose was to match traditional musical notation (where all the keys of the bars 
   *  match the key given at the beginning of the sequence).
   * 
   *  As a consequence of the removal of this param, bars sized to any number of base notes
   *  can be inserted in sequence, and the sequence will just adapt its tPlaySequence and 
   *  tPlaySequenceInLoop interval to the various bars durations.
   * */
  
  // --> Sequence 0: "Relays"
  /** array of bars to be played in the sequence. */
  // Serial.printf("\nsequence::initSequences(). Before building _relaysBarsArray.\n");
  // Serial.printf("sequence::initSequences(). bar::_barsArray[0].getNotesArray().at(0).getNote() should be equal to 1. Is equal to [%u].\n", bar::_barsArray[0].getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). bar::_barsArray[0].getNotesArray().at(0).getToneNumber() should be equal to 7. Is equal to [%u].\n", bar::_barsArray[0].getNotesArray().at(0).getToneNumber());
  std::array<bar, 8> _relaysBarsArray {_bars._barsArray[0]};
  // Serial.printf("\nsequence::initSequences(). Passed building _relaysBarsArray.\n");
  // Serial.printf("sequence::initSequences(). _relaysBarsArray.at(0).getNotesArray().at(0).getNote() should be equal to 1. Is equal to [%u].\n", _relaysBarsArray.at(0).getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). _relaysBarsArray.at(0).getNotesArray().at(0).getToneNumber() should be equal to 7. Is equal to [%u].\n", _relaysBarsArray.at(0).getNotesArray().at(0).getToneNumber());
  /** const beat _beat_2_1(2,1): an instance of beat
   *    _ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_2_1(2,1);
  // Serial.printf("\nsequence::initSequences(). Passed building _beat_2_1.\n");
  // Serial.printf("sequence::initSequences(). _beat_2_1.getBaseBeatInBpm() should be equal to 2. Is equal to [%u].\n", _beat_2_1.getBaseBeatInBpm());
  sequencesArray[0] = {_beat_2_1, _relaysBarsArray, 0};
  // Serial.printf("\nsequence::initSequences(). sequences[0].getBarsArray()[0].getNotesArray().at(0).getNote() shall be equal to 1. Is equal to [%i]\n", sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). sequences[0].getBarsArray()[0].getNotesArray().at(0).getToneNumber() shall be equal to 7. Is equal to [%i]\n", sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
  // Serial.printf("sequence::initSequences(). sequences[0].getAssociatedBeat().getBaseBeatInBpm() should be equal to 2. Is equal to [%u]\n", sequences[0].getAssociatedBeat().getBaseBeatInBpm());


  // --> Sequence 1: "Twins"
  std::array<bar, 8> _twinsBarsArray {_bars._barsArray[1]};
  sequencesArray[1] = {_beat_2_1, _twinsBarsArray, 1};


  // --> Sequence 2: "All"
  std::array<bar, 8> _allBarsArray {_bars._barsArray[2]};
  sequencesArray[2] = {_beat_2_1, _allBarsArray, 2};


  // --> Sequence 3: "Swipe Right"
  std::array<bar, 8> _swipeRBarsArray {_bars._barsArray[3]};
  /** const beat _beat_120_1(120,1): an instance of beat
   *    _ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
   *    _ui16BaseNoteForBeat = 1; a white */
  const beat _beat_120_1(120,1);
  sequencesArray[3] = {_beat_120_1, _swipeRBarsArray, 3};


  // --> Sequence 4: "Swipe Left"
  std::array<bar, 8> _swipeLBarsArray {_bars._barsArray[4]};
  sequencesArray[4] = {_beat_120_1, _swipeLBarsArray, 4};


  // --> Sequence 5: "All Off"
  std::array<bar, 8> _allOffBarsArray {_bars._barsArray[5]};
  sequencesArray[5] = {_beat_2_1, _allOffBarsArray, 5};

  Serial.println("sequences::sequences(). over.");
}


// copy constructor
sequences::sequences(const sequences & __sequences):
  sendCurrentSequence(__sequences.sendCurrentSequence),
  sequencesArray(__sequences.sequencesArray),
  tPlayBar(__sequences.tPlayBar),
  tPlayNote(__sequences.tPlayNote),
  _activeSequence(__sequences._activeSequence)
{ }



// assignement operator
sequences & sequences::operator=(const sequences & __sequences)
{
  // Serial.printf("sequences::operator=(const sequences& ): assignment operator starting\n");
  if (&__sequences != this) {
    // Serial.printf("sequence::operator=(const sequences& ): self assignmenttest passed\n");
    sendCurrentSequence = __sequences.sendCurrentSequence;
    sequencesArray = __sequences.sequencesArray;
    _activeSequence = __sequences._activeSequence;
  }
  return *this;
}







///////////////////////////////////
// Setters
///////////////////////////////////
/** sequences::setActive()
 * 
 *  sets the class instance variable _activeSequence 
 *  from a passed in lvalue const sequence reference.
 * 
 *  public instance setter 
 * */
uint16_t sequences::setActive(const sequence & __activeSequence) {
  disableAndResetPlaySequenceTasks();
  // laserInterface::activeBeat = __activeSequence._beat;
  _activeSequence = __activeSequence;
  return __activeSequence.i16IndexNumber;
}







/** TODO: - review calls to disableAndResetPlaySequenceTasks,
 *          disableAndResetTPlayBar and _disableAndResetTPlayNote;
 *        - check whether they are rather called upon terminating or upon starting a player;
 *        - try to systematically use them as a replacement for calls to task disable.
 * 
 *        - try to rationalize, comparing what is happening in each of the stacks. I have got the sentiment that it is a bit fuzzy at the moment.
 * */

/** sequences::disableAndResetPlaySequenceTasks(): public setter method
 * 
 *  Resets the parameters of the Tasks [tPlaySequence/tPlaySequenceInLoop] 
 *  to their default parameters, to play a sequence, as instructed from a boxState
 *  or stand alone. 
 * 
 *  Task tPlaySequenceInLoop default parameters:
 *  - interval: 0 second;
 *  - iteration forever;
 *  - main callback: &_tcbPlaySequenceInLoop;
 *  - onEnable callback: &_oetcbPlaySequenceInLoop;
 *  - onDisable callback: &_odtcbPlaySequenceInLoop;
 *  - added to myScheduler in setup(); disabled by default.
 * 
 *  Task tPlaySequence default parameters:
 *  - interval: 0 second;
 *  - 0 iteration;
 *  - main callback: &_oetcbPlaySequence;
 *  - onEnable callback:  &_oetcbPlaySequence;
 *  - onDisable callback: &_odtcbPlaySequence)
 *  - added to myScheduler in setup(); disabled by default.
 * 
 *  This method disableAndResetPlaySequenceTasks() is called by [boxState] 
 *  (from [---]) and by sequence ([from the 
 *  standalone sequence player]).
 * 
 *  Task tPlaySequenceInLoop is enabled upon entering a new boxState.
 *  It is disabled upon exiting a boxState.
 * 
 *  Task tPlaySequence is enabled by tPlaySequenceInLoop onEnable callback.
 *  It is disabled:
 *  - at the expiration of its programmed iterations;
 *  - by the onDisable callback of tPlaySequenceInLoop.
 *  TODO: mention the laserInterface interrupts, here and in other parts.
 *  
 *  Each iteration of tPlaySequence correspond to one bar in the sequence.
 *  Task tPlaySequence iterations is set in its onEnable callback.
 *  Task tPlaySequence interval is reset in its main callback, at each iteration.
 * 
 *  public instance setter 
 * */
void sequences::disableAndResetPlaySequenceTasks() {
  _bars.disableAndResetTPlayBar();
  tPlaySequence.disable();
  tPlaySequence.set(0, 1, [&](){_tcbPlaySequence();}, [&](){return _oetcbPlaySequence();}, [&](){return _odtcbPlaySequence();});
  tPlaySequenceInLoop.disable();
  tPlaySequenceInLoop.set(0, -1, [&](){_tcbPlaySequenceInLoop();}, [&](){return _oetcbPlaySequenceInLoop();}, [&](){return _odtcbPlaySequenceInLoop();});
}







/** int16_t const bars::i16GetCurrentSequenceId() const
 * 
 * Returns the active sequence index number 
 * */
int16_t const sequences::i16GetCurrentSequenceId() const {
  return _activeSequence.i16IndexNumber;
}







/** sequence & sequences::getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id)
 * 
 * Instance getter.
 * 
 * Returns a const ref to one of the hard coded sequences given its index number */
sequence const & sequences::getSequenceFromSequenceArray(const uint16_t __ui16_sequence_id) const {
  return sequencesArray.at(__ui16_sequence_id);
}










///////////////////////////////////
// Sequence Players
///////////////////////////////////
/** sequences::playSequenceStandAlone():
 *  
 *  play a single sequence calculating the durations
 *  on the basis of the passed-in beat.
 * 
 *  {@ params} const int16_t __i16_sequence_id: optional sequence id in the 
 *             sequence array (might be needed for debug and interface purpose)
 * */
uint16_t const sequences::playSequenceStandAlone(const sequence & __target_sequence) {
  // 0. Do not do anything if the beat has not been set
  if ((__target_sequence._beat.getBaseBeatInBpm() == 0) || (__target_sequence._beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // 1. set the active sequence and the active beat
  if (setActive(__target_sequence) == -2) {
    return 1;
  }

  /**2. set the onDisable callback of tPlaySequence to reset the active beat to (0,0) once
   *    the stand alone sequence has been read, so that any other object that may 
   *    thereafter depend on beat finds a clean beat to start with, as required. */
  tPlaySequence.setOnDisable([&](){
    // _activeBeat = beat(0, 0);
    tPlaySequence.setOnDisable([&](){
      _odtcbPlaySequence();
    });
  });

  // 3. restart the tPlaySequence Task
  tPlaySequence.restart();

  // 4. return 2 for success
  return 2;
}


bool sequences::playSequenceInBoxState(const sequence & __target_sequence) {
  // Serial.printf("sequences::playSequenceInBoxState(const sequence & __target_sequence): starting with __target_sequence.i16IndexNumber = [%i]\n", __target_sequence.i16IndexNumber);
  if ((__target_sequence._beat.getBaseNoteForBeat() == 0) || (__target_sequence._beat.getBaseBeatInBpm() == 0)) {
    // Serial.printf("sequences::playSequenceInBoxState(const sequence & __target_sequence): __target_sequence._beat.getBaseNoteForBeat() == [%u]\n", __target_sequence._beat.getBaseNoteForBeat());
    // Serial.printf("sequences::playSequenceInBoxState(const sequence & __target_sequence): __target_sequence._beat.getBaseNoteForBeat() == [%u]\n", __target_sequence._beat.getBaseBeatInBpm());
    // Serial.println("sequences::playSequenceInBoxState(const sequence & __target_sequence): success");
    return false;
  }
  setActive(__target_sequence);
  tPlaySequenceInLoop.restart();
  // Serial.println("sequences::playSequenceInBoxState(const sequence & __target_sequence): success");
  return true;
}










///////////////////////////////////
// Loop Player
///////////////////////////////////
/**
 *  tPlayBoxState plays once (unless restarted by tPlayBoxStates)
 *    - tPlayBoxState -> onEnable: enables tPlaySequenceInLoop by calling
 *      playSequenceInBoxState();
 *    - tPlayBoxState -> onDisable: disables tPlaySequenceInLoop
 *  tPlaySequenceInLoop plays forever (until interrupt by tPlayBoxState):
 *    - tPlaySequenceInLoop -> onEnable: calculates the sequence duration and
 *      sets the interval for tPlaySequenceInLoop
 *    - tPlaySequenceInLoop -> main callback: starts tPlaySequence
 *    - tPlaySequenceInLoop -> onDisable: plays sequence 5 (all off) */

/** tPlaySequenceInLoop
 * 
 *  tPlaySequenceInLoop plays a sequence in loop, for an unlimited number
 *  of iterations, until it is disabled by tPlayBoxState.
 * 
 *  tPlaySequenceInLoop is enabled and disabled by the onEnable and
 *  onDisable callbacks of tPlayBoxState.
 * 
 *  Upon entering a new boxState (startup, IR signal received, etc.), the onEnable 
 *  callback of tPlayBoxState task calls the playSequenceInBoxState() method of
 *  sequence::sequences[_currentBoxState.ui16AssociatedSequence].
 * 
 *  _currentBoxState.ui16AssociatedSequence is the index number, in the 
 *  sequences array, of the sequence associated with the calling boxState.
 * 
 *  This Task tPlaySequenceInLoop is enabled by playSequenceInBoxState, until 
 *  being disabled by the boxState::tPlayBoxState onDisable callback.
 * */



/** bool sequences::_oetcbPlaySequenceInLoop()
 * 
 *  Set the interval between each iteration of tPlaySequenceInLoop
 *  (each iteration will restart the Task tPlaySequence, so this
 *  interval shall be equal to the duration of the sequence).
 * 
 *  This interval will only be taken into account after the main 
 *  callback has been called ==> the forenext iteration.
 * 
 *  This calculation is made based on the base of the base beat in bpm,
 *  the base note, the number of base notes per bars and the number 
 *  of bars in the sequence.
 * 
 *  _oetcbPlaySequenceInLoop() does not start playing the sequence. This is
 *  done in the main callback of tPlaySequenceInLoop. The first iteration of
 *  the Task occurs immediately after the onEnable callback, at the initial
 *  interval for this Task (before being reset in this onEnable callback) is
 *  equal to 0. 
 * */
bool sequences::_oetcbPlaySequenceInLoop() {
  Serial.println("sequences::_oetcbPlaySequenceInLoop(). starting. *****");

  tPlaySequenceInLoop.setInterval(_activeSequence.ui32GetSequenceDuration(_activeSequence._beat));

  Serial.println("sequences::_oetcbPlaySequenceInLoop(). over.");
  return true;
}




void sequences::_tcbPlaySequenceInLoop() {
  Serial.println("sequences::_tcbPlaySequenceInLoop(). starting. ******");

  Serial.println("sequences::_tcbPlaySequenceInLoop(). about to enable tPlaySequence");
  tPlaySequence.restart(); // <-- TODO: replace by a call to playSequenceInBoxState or something similar

  Serial.println("sequences::_tcbPlaySequenceInLoop(). over.");
}




/** On disable tPlaySequenceInLoop, turn off all the laser by playing
 *  sequence 5 ("all off"). 
 * */
void sequences::_odtcbPlaySequenceInLoop() {
  Serial.println("sequences::_odtcbPlaySequenceInLoop(). starting. ******");
  if (_activeSequence.i16IndexNumber != 5) {
    playSequenceInBoxState(sequencesArray[5]);
  }
  Serial.println("sequences::_odtcbPlaySequenceInLoop(). over.");
};







///////////////////////////////////
// Single Sequence Player
///////////////////////////////////
/** Task tPlaySequence.
 *  
 * It plays a given sequence once.
 * */



/** bool sequences::_oetcbPlaySequence(): onEnable callback for tPlaySequence
 *  
 *  sets the number of iterations of tPlaySequence to the number of bars in this sequence.
 * */
bool sequences::_oetcbPlaySequence() {
  /** 1. Set the number of iterations of the tPlaySequence task from the number of bars in the sequence. */
  tPlaySequence.setIterations(_activeSequence.ui16GetBarCountInSequence());

  /** 2. Signal the change of sequence to the mesh. */
  if (sendCurrentSequence != nullptr) {
    sendCurrentSequence(_activeSequence.i16IndexNumber);
  }

  return true;
}



/** void sequences::_tcbPlaySequence(): 
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
void sequences::_tcbPlaySequence() {
  Serial.println("sequences::_tcbPlaySequence(). starting.");
  Serial.println(F("------------- DEBUG --------- SEQUENCE --------- DEBUG -------------"));

  // 1. Get the number of iterations (each iteration corresponds to one bar)
  uint16_t _ui16Iter = tPlaySequence.getRunCounter();
  _ui16Iter = ((0 == _ui16Iter) ? 0 : (_ui16Iter - 1));

  // 2. Calculate the bar duration
  uint32_t __ui32ThisBarDuration = _activeSequence.getBarsArray().at(_ui16Iter).ui32GetBarDuration(_activeSequence._beat);

  // 3. Play the corresponding bar
  _bars.playBar(_activeSequence.getBarsArray().at(_ui16Iter), _activeSequence._beat);

  /**4. Set the interval for next iteration of tPlaySequence
   * 
   *    At each pass, reset the interval before the next iteration of this 
   *    Task sequences::tPlaySequence. This marks the duration of each bar played in the
   *    context of a sequence. */
  tPlaySequence.setInterval(__ui32ThisBarDuration);

  Serial.println("sequences::_tcbPlaySequence(). over.");
};



/** tPlaySequence disable callback */
void sequences::_odtcbPlaySequence() {
  // if (MY_DG_LASER) {
  //   Serial.println("----------------------------on disable tPlaySequence ------------------------------");
  //   Serial.print("sequences::_odtcbPlaySequence(). millis() = ");Serial.println(millis());
  //   Serial.println("sequences::_odtcbPlaySequence(). tPlaySequence BYE BYE!");
  // }
}
