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
  _ui16BarsArray({})
{ 
}

// parameterized constructor
sequence::sequence(
  const beat & __beat,
  std::array<uint16_t, 8> & __ui16BarsArray,
  int16_t __i16IndexNumber
):
  i16IndexNumber(__i16IndexNumber),
  _beat(__beat),
  _ui16BarsArray(__ui16BarsArray)
{ }

// copy constructor
sequence::sequence(const sequence& __sequence):
  i16IndexNumber(__sequence.i16IndexNumber),
  _beat(__sequence._beat),
  _ui16BarsArray(__sequence._ui16BarsArray)
{ }

// assignement operator
sequence& sequence::operator=(const sequence& __sequence)
{
  if (&__sequence != this) {
    i16IndexNumber = __sequence.i16IndexNumber;
    _beat = __sequence._beat;
    _ui16BarsArray = __sequence._ui16BarsArray;
  }
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
 * Returns a const ref to the associated bars array
 *  */
// std::array<bar, 8> const & sequence::getBarsArray() const {
/** ** ** ** ** ** ** ** **/
//   uint16_t _i = 0;
//   std::array<uint16_t, 8> barIndexNumbersArray;
//   for (auto _bar: _barsArray) {
//     barIndexNumbersArray.at(_i) = _bar.i16IndexNumber;
//     _i++;
//   }
/** ** ** ** ** ** ** ** **/
//   return _barsArray;
// }



/**std::array<uint16_t, 8> const & sequence::getBarsIndexNumbersArray() const
 * 
 * Instance getter.
 * 
 * Returns a const ref to an array of bars index numbers
 *  */
std::array<uint16_t, 8> const & sequence::getBarsIndexNumbersArray() const
{
  return _ui16BarsArray;
}



/**std::array<uint16_t, 8> const & sequence::getBarsIndexNumbersArray() const
 * 
 * Instance getter.
 * 
 * Returns the index number of the first bar in the sequence as a const uint16_t
 *  */
int16_t const sequence::ui16GetFirstBarIndexNumber() const {
  return _ui16BarsArray.at(0);
}



/**int16_t const sequence::ui16GetBarIndexNumber(const uint16_t ui16BarIxNumbInSequence) const
 * 
 * Instance getter.
 * 
 * Returns the index number of a bar in the bars database, given such bar's index
 * number in the array of bars associated to this sequence
 *  */
int16_t const sequence::ui16GetBarIndexNumber(const uint16_t ui16BarIxNumbInSequence) const {
  return _ui16BarsArray.at(ui16BarIxNumbInSequence);
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
  sequencesArray(),
  _bars(),
  ui16IxNumbOfSequenceToPreload(0), // <-- TODO: review setters method here; maybe need to cast sequenceIndex as an int16, to initialize at -1
  nextSequence(),
  sequenceFileName("/sequences.json"),
  tPlaySequenceInLoop(),
  tPlaySequence(),
  tPreloadNextSequence(),
  _defaultSequence(),
  _activeSequence(_defaultSequence)
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
  /** std::array<uint16_t, 8> _relaysBarsArray {0};
   * 
   *  Defines the array of bars to be played in the sequence. 
   *  In the following _relaysBarsArray exemple, the array of bars only contains
   *  one single bar (0)). It could contain more.
   * */
  std::array<uint16_t, 8> _relaysBarsArray {0};
  /** const beat _beat_2_1(2,1): an instance of beat
   *    _ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
   *    _ui16BaseNoteForBeat = 1; a white 
   * */
  const beat _beat_2_1(2,1);
  // Serial.printf("\nsequence::initSequences(). Passed building _beat_2_1.\n");
  // Serial.printf("sequence::initSequences(). _beat_2_1.getBaseBeatInBpm() should be equal to 2. Is equal to [%u].\n", _beat_2_1.getBaseBeatInBpm());
  sequencesArray[0] = {_beat_2_1, _relaysBarsArray, 0};
  // {"bt":{"bpm":2,"base":1}, "brs":[0], "ix":0}
  // Serial.printf("\nsequence::initSequences(). sequences[0].getBarsArray()[0].getNotesArray().at(0).getNote() shall be equal to 1. Is equal to [%i]\n", sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  // Serial.printf("sequence::initSequences(). sequences[0].getBarsArray()[0].getNotesArray().at(0).getToneNumber() shall be equal to 7. Is equal to [%i]\n", sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
  // Serial.printf("sequence::initSequences(). sequences[0].getAssociatedBeat().getBaseBeatInBpm() should be equal to 2. Is equal to [%u]\n", sequences[0].getAssociatedBeat().getBaseBeatInBpm());


  // --> Sequence 1: "Twins"
  std::array<uint16_t, 8> _twinsBarsArray {1};
  sequencesArray[1] = {_beat_2_1, _twinsBarsArray, 1};
  // {"bt":{"bpm":2,"base":1}, "brs":[1], "ix":1}


  // --> Sequence 2: "All"
  std::array<uint16_t, 8> _allBarsArray {2};
  sequencesArray[2] = {_beat_2_1, _allBarsArray, 2};
  // {"bt":{"bpm":2,"base":1}, "brs":[2], "ix":2}


  // --> Sequence 3: "Swipe Right"
  std::array<uint16_t, 8> _swipeRBarsArray {3};
  /** const beat _beat_120_1(120,1): an instance of beat
   *    _ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
   *    _ui16BaseNoteForBeat = 1; a white 
   * */
  const beat _beat_120_1(120,1);
  sequencesArray[3] = {_beat_120_1, _swipeRBarsArray, 3};
  // {"bt":{"bpm":120,"base":1}, "brs":[3], "ix":3}


  // --> Sequence 4: "Swipe Left"
  std::array<uint16_t, 8> _swipeLBarsArray {4};
  sequencesArray[4] = {_beat_120_1, _swipeLBarsArray, 4};
  // {"bt":{"bpm":120,"base":1}, "brs":[4], "ix":4}


  // --> Sequence 5: "All Off"
  std::array<uint16_t, 8> _allOffBarsArray {5};
  sequencesArray[5] = {_beat_2_1, _allOffBarsArray, 5};
  // {"bt":{"bpm":2,"base":1}, "brs":[5], "ix":5}

  tPreloadNextSequence.set(0, 1, [&](){ return _tcbPreloadNextSequence(); }, NULL, NULL);
  tPreloadNextSequence.restart();

  Serial.println("sequences::sequences(). over.");
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
  _activeSequence = __activeSequence;
  _bars.ui16IxNumbOfBarToPreload = _activeSequence.ui16GetFirstBarIndexNumber();
  _bars.tPreloadNextBar.restart();
  return _activeSequence.i16IndexNumber;
}







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
 *  This method disableAndResetPlaySequenceTasks() is called by sequences (from
 *  sequences.setActive()).
 * 
 *  Task tPlaySequenceInLoop is enabled upon entering a new boxState.
 *  It is disabled upon exiting a boxState.
 * 
 *  Task tPlaySequence is enabled by tPlaySequenceInLoop onEnable callback.
 *  It is disabled:
 *  - alone, at the expiration of its programmed iterations; or
 *  - by the onDisable callback of tPlaySequenceInLoop.
 *  
 *  Each iteration of tPlaySequence corresponds to one bar in the sequence.
 *  Task tPlaySequence iterations parameter is set, once, in its onEnable callback.
 *  Task tPlaySequence interval is reset at each iteration in its main callback.
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







/** sequence const & sequences::getCurrentSequence() const
 * 
 * Returns the active sequence as a sequence */
sequence const & sequences::getCurrentSequence() const {
  return _activeSequence;
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
/** sequences::playSequence():
 *  
 *  play a single sequence calculating the durations on the basis of the passed-in beat.
 * 
 *  {@ params} const int16_t __i16_sequence_id: optional sequence id in the 
 *             sequence array (might be needed for debug and interface purpose)
 *  {@ params} task & __sequenceTask: either tPlaySequence or tPlaySequenceInLoop 
 * */
uint16_t const sequences::playSequence(const sequence & __target_sequence, Task & __sequenceTask) {
  // 0. Do not do anything if the beat has not been set
  if ((__target_sequence._beat.getBaseBeatInBpm() == 0) || (__target_sequence._beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // 1. set the active sequence and the active beat
  if (setActive(__target_sequence) == -2) {
    return 1;
  }

  // 2. restart the tPlaySequence or tPlaySequenceStandAlone Task
  __sequenceTask.restart();

  // 3. return 2 for success
  return 2;
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
 *  being disabled by the bxStateColl.tPlayBoxState onDisable callback
 *  (bxStateColl._odtcbPlayBoxState).
 * */



/** bool sequences::_oetcbPlaySequenceInLoop()
 * 
 *  Set the interval between each iteration of tPlaySequenceInLoop.
 *  Each iteration will restart the Task tPlaySequence. Accordingly,
 *  this interval shall be equal to the duration of the sequence.
 * 
 *  This interval will only be taken into account AFTER the MAIN 
 *  callback has been called ==> the forenext iteration. Accordingly,
 *  the first iteration of the Task tPlaySequenceInLoop will occur "immediately 
 *  after" the onEnable callback, as the default interval for this Task is set to 0.
 * 
 *  The calculation of the interval is made based on the basis of the 
 *  base beat in bpm, the base note per beat, the number of base notes 
 *  per bars and the number of bars in the sequence.
 * 
 *  _oetcbPlaySequenceInLoop() does NOT start playing the sequence (i.e. it does
 *  NOT restart Task tPlaySequence). Task tPlaySequence is restarted in the main 
 *  callback of tPlaySequenceInLoop.
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
    playSequence(sequencesArray[5], tPlaySequenceInLoop);
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



/** void sequences::_tcbPlaySequence(): Main callback for tPlaySequence
 * 
 *  Each iteration of tPlaySequence corresponds to a bar.
 * 
 *  The number of iterations for the Task have been set in its onEnable 
 *  callback and do not change on each iterations.
 * 
 *  To the difference of musical partitions (where bars in a sequence always have the 
 *  same duration), the duration of each bar is recalculated at each iteration. This
 *  allows more flexibility (bars of various durations may be inserted in a sequence).
 * 
 *  At each iteration of tPlaySequence:
 *  - recalculate the bar duration (steps 1 and 2 below);
 *  - set the Task interval before next iteration accordingly (step 4);
 *  - call _bars.playBar (step 3) to play the bar which is currently active in the sequence;
 *  - preload the next bar from SPIFFS.
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
   *    context of a sequence.
   *  */
  tPlaySequence.setInterval(__ui32ThisBarDuration);

  /**5. Preload the next bar*/
  _bars.ui16IxNumbOfBarToPreload = _activeSequence.ui16GetBarIndexNumber(_ui16Iter + 1);
  _bars.tPreloadNextBar.restart();

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






///////////////////////////////////
// Sequence Loader
///////////////////////////////////
void sequences::_tcbPreloadNextSequence() {
  Serial.printf("sequences::_tcbPreloadNextSequence(): starting\n");
  // read next step values from the file system
  _preloadNextSequence(ui16IxNumbOfSequenceToPreload);
  Serial.printf("sequences::_tcbPreloadNextSequence(): ending\n");
}




void sequences::_preloadNextSequence(uint16_t _ui16IxNumbOfSequenceToPreload){
  Serial.printf("stepCollection::_preloadNextSequence: Reading file: %s\r\n", sequenceFileName);

  mySpiffs __mySpiffs;
  if (!(__mySpiffs.readCollectionItemParamsInFile(sequenceFileName, _ui16IxNumbOfSequenceToPreload))) {
    return;
  }

  // Get a reference to the root object
  JsonObject _joSequence = __mySpiffs._jdItemParams.as<JsonObject>();

  _preloadNextSequenceFromJSON(_joSequence);
}




void sequences::_preloadNextSequenceFromJSON(const JsonObject& _joSequence) {
  // {"bt":{"bpm":2,"base":1}, "brs":[0,3,2,6], "ix":0}
  Serial.println("sequences::_preloadNextSequenceFromJSON: starting");
  // Load _joSequence["brs"] into an std::array
  std::array<uint16_t, 8> & __ui16BarsArray = _parseJsonBarsArray(_joSequence["brs"].as<JsonArray>());

  // Load the next sequence into a sequence instance
  nextSequence = {
    beat{
      _joSequence["bt"]["bpm"].as<uint16_t>(),
      _joSequence["bt"]["base"].as<uint16_t>()
    },
    __ui16BarsArray,
    _joSequence["ix"].as<int16_t>()
  };
}



std::array<uint16_t, 8> & const sequences::_parseJsonBarsArray(const JsonArray& _jaBarsArray) {
  // "brs":[0,3,2,6]
  int16_t _barIx = 0;
  std::array<uint16_t, 8> __ui16BarsArray {};
  for (JsonVariant _JsonBarIxNumber : _jaBarsArray) {
    __ui16BarsArray.at(_barIx) = _JsonBarIxNumber;
    _barIx++;
  }
  return __ui16BarsArray;
}
