/*
  laserSequence.cpp - laserSequences are precoded sequences of bars
  Created by Cedric Lor, June 4, 2019.

Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "laserSequence.h"






///////////////////////////////////
// Constructors
///////////////////////////////////
// default constructor
laserSequence::laserSequence():
  i16IndexNumber(-2),
  _beat(beat{0, 0}),
  _i16BarIxNumbsArray({}),
  _i16barsArraySize(0)
{
}

// parameterized constructor
laserSequence::laserSequence(
  const beat & __beat,
  std::array<int16_t, 8> & __i16BarIxNumbsArray,
  int16_t __i16barsArraySize,
  int16_t __i16IndexNumber
):
  i16IndexNumber(__i16IndexNumber),
  _beat(__beat),
  _i16BarIxNumbsArray(__i16BarIxNumbsArray),
  _i16barsArraySize(__i16barsArraySize)
{ }

// copy constructor
laserSequence::laserSequence(const laserSequence& __laserSequence):
  i16IndexNumber(__laserSequence.i16IndexNumber),
  _beat(__laserSequence._beat),
  _i16BarIxNumbsArray(__laserSequence._i16BarIxNumbsArray),
  _i16barsArraySize(__laserSequence._i16barsArraySize)
{ }

// assignement operator
laserSequence& laserSequence::operator=(const laserSequence& __laserSequence)
{
  if (&__laserSequence != this) {
    i16IndexNumber = __laserSequence.i16IndexNumber;
    _beat = __laserSequence._beat;
    _i16BarIxNumbsArray = __laserSequence._i16BarIxNumbsArray;
    _i16barsArraySize = __laserSequence._i16barsArraySize;
  }
  return *this;
}










///////////////////////////////////
// Getters
///////////////////////////////////
/**uint16_t const laserSequence::ui16GetBarCountInLaserSequence() const 
 *  
 * Instance getter.
 *  
 * Returns the bar count in a given laserSequence. */
uint16_t const laserSequence::ui16GetBarCountInLaserSequence() const {
  return _i16barsArraySize;
}



/**beat const & laserSequence::getAssociatedBeat() const 
 * 
 * Instance getter.
 * 
 * Returns the associated beat */
beat const & laserSequence::getAssociatedBeat() const {
  return _beat;
}



/**int16_t const laserSequence::i16GetFirstBarIndexNumber
 * 
 * Instance getter.
 * 
 * Returns the index number of the first bar in the laserSequence as a const uint16_t
 *  */
int16_t const laserSequence::i16GetFirstBarIndexNumber() const {
  return _i16BarIxNumbsArray.at(0);
}



/**int16_t const laserSequence::i16GetBarIndexNumber(const uint16_t ui16BarIxNumbInSequence) const
 * 
 * Instance getter.
 * 
 * Returns the index number of a bar in the bars database, given such bar's index
 * number in the array of bars associated to this laserSequence
 *  */
int16_t const laserSequence::i16GetBarIndexNumber(const uint16_t __ui16BarIxNumbInSequence) const {
  int16_t _ui16BarIxNumb = 5; // laserSequence 5: allOff
  try {
    _ui16BarIxNumb = _i16BarIxNumbsArray.at(__ui16BarIxNumbInSequence);
  }
  catch (const std::out_of_range& oor) {
    Serial.printf("laserSequence::i16GetBarIndexNumber() called with __ui16BarIxNumbInSequence. Out of range error: \n");
    Serial.println(oor.what());
  }
  return _ui16BarIxNumb;
}























//****************************************************************//
// laserSequences //***************************************************//
//****************************************************************//

/** laserSequences::laserSequences()
 * 
 *  default constructor */
laserSequences::laserSequences(
  void (*_sendCurrentSequence)(const int16_t __i16_active_laser_sequence_id)
): 
  sendCurrentSequence(_sendCurrentSequence),
  _bars(),
  ui16IxNumbOfSequenceToPreload(0), // <-- TODO: review setters method here; maybe need to cast ui16IxNumbOfSequenceToPreload as an int16, to initialize at -1
  nextSequence(),
  sequenceFileName("/laserSequences.json"),
  tPlaySequence(),
  tPreloadNextSequence(),
  _defaultLaserSequence(),
  _activeLaserSequence(_defaultLaserSequence)
{
  // 1. Disable and reset the Task tPlaySequence
  disableAndResetTPlaySequence();

  // 2. Define sequencesArray, an array containing a series of hard coded laserSequences

  /** Signature of a laserSequence:
   *  a. a beat instance, composed of:
   *    1. _ui16BaseBeatInBpm: the base beat in bpm (e.g. 120 bpm, 2 bpm, etc.);
   *    2. _ui16BaseNoteForBeat: the base laserNote for each beat (e.g. 4 -> black, etc.; the 4 in 2/4);
   *  b. the array of bars to be played in the laserSequence (_barsArray);
   *  c. the laserSequence's index number in the sequencesArray.
   * 
   *  There used to be an additional param/member:
   *  - _ui16BaseNotesCountPerBar: the number of base laserNotes per bar (the 2 in 2/4).
   *  This param/member has been removed. This comment is kept for reference purposes,
   *  The purpose was to match traditional musical notation (where all the keys of the bars 
   *  match the key given at the beginning of the laserSequence).
   * 
   *  As a consequence of the removal of this param, bars sized to any number of base laserNotes
   *  can be inserted in laserSequence, and laserSequences will just adapt its tPlaySequence
   *  interval to the various bars durations.
   * */
  

  // --> laserSequence 0: "Relays"
  /** std::array<uint16_t, 8> _relaysBarsArray {0};
   * 
   *  Defines the array of bars to be played in the laserSequence. 
   *  In the following _relaysBarsArray exemple, the array of bars only contains
   *  one single bar (0)). It could contain more.
   * */
  // std::array<int16_t, 8> _relaysBarsArray {0};
  /** const beat _beat_2_1(2,1): an instance of beat
   *    _ui16BaseBeatInBpm = 2 for 2 bpm -> a beat every 30 seconds
   *    _ui16BaseNoteForBeat = 1; a white 
   * */
  // const beat _beat_2_1(2,1);
  // Serial.printf("\nsequence::initSequences(). Passed building _beat_2_1.\n");
  // Serial.printf("laserSequence::initSequences(). _beat_2_1.getBaseBeatInBpm() should be equal to 2. Is equal to [%u].\n", _beat_2_1.getBaseBeatInBpm());
  // sequencesArray[0] = {_beat_2_1, _relaysBarsArray, 1, 0};
  // {"bt":{"bpm":2,"base":1}, "brs":[0], "ix":0}
  // Serial.printf("\nsequence::initSequences(). laserSequences[0].getBarsArray()[0].getNotesArray().at(0).getNote() shall be equal to 1. Is equal to [%i]\n", laserSequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  // Serial.printf("laserSequence::initSequences(). laserSequences[0].getBarsArray()[0].getNotesArray().at(0).getToneNumber() shall be equal to 7. Is equal to [%i]\n", laserSequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
  // Serial.printf("laserSequence::initSequences(). laserSequences[0].getAssociatedBeat().getBaseBeatInBpm() should be equal to 2. Is equal to [%u]\n", laserSequences[0].getAssociatedBeat().getBaseBeatInBpm());


  // --> laserSequence 1: "Twins"
  // std::array<int16_t, 8> _twinsBarsArray {1};
  // sequencesArray[1] = {_beat_2_1, _twinsBarsArray, 1, 1};
  // {"bt":{"bpm":2,"base":1}, "brs":[1], "ix":1}


  // --> laserSequence 2: "All"
  // std::array<int16_t, 8> _allBarsArray {2};
  // sequencesArray[2] = {_beat_2_1, _allBarsArray, 1, 2};
  // {"bt":{"bpm":2,"base":1}, "brs":[2], "ix":2}


  // --> laserSequence 3: "Swipe Right"
  // std::array<int16_t, 8> _swipeRBarsArray {3};
  /** const beat _beat_120_1(120,1): an instance of beat
   *    _ui16BaseBeatInBpm = 120 for 120 bpm -> a beat every 500 milliseconds
   *    _ui16BaseNoteForBeat = 1; a white 
   * */
  // const beat _beat_120_1(120,1);
  // sequencesArray[3] = {_beat_120_1, _swipeRBarsArray, 1, 3};
  // {"bt":{"bpm":120,"base":1}, "brs":[3], "ix":3}


  // --> laserSequence 4: "Swipe Left"
  // std::array<int16_t, 8> _swipeLBarsArray {4};
  // sequencesArray[4] = {_beat_120_1, _swipeLBarsArray, 1, 4};
  // {"bt":{"bpm":120,"base":1}, "brs":[4], "ix":4}


  // --> laserSequence 5: "All Off"
  // std::array<int16_t, 8> _allOffBarsArray {5};
  // sequencesArray[5] = {_beat_2_1, _allOffBarsArray, 1, 5};
  // {"bt":{"bpm":2,"base":1}, "brs":[5], "ix":5}

  tPreloadNextSequence.set(0, 1, [&](){ return _tcbPreloadNextSequence(); }, NULL, NULL);

  Serial.println("laserSequences::laserSequences(). over.");
}








///////////////////////////////////
// Setters
///////////////////////////////////
/** laserSequences::_setActive(const laserSequence & __activeLaserSequence)
 * 
 *  sets the class instance variable _activeLaserSequence 
 *  from a passed in lvalue const laserSequence reference.
 * 
 *  private instance setter 
 * */
uint16_t laserSequences::_setActive(const laserSequence & __activeLaserSequence) {
  disableAndResetTPlaySequence();
  _activeLaserSequence = __activeLaserSequence;
  _bars.preloadNextBarThroughTask(_activeLaserSequence.i16GetFirstBarIndexNumber());
  return _activeLaserSequence.i16IndexNumber;
}







/** laserSequences::setActive(const laserSequence & __activeLaserSequence)
 * 
 *  sets the class instance variable _activeLaserSequence 
 *  from a passed in index number.
 * 
 *  public instance setter 
 * */
uint16_t laserSequences::setActive(const uint16_t __target_laser_sequence_ix_numb) {
  _preloadNextSequence(__target_laser_sequence_ix_numb);
  return _setActive(nextSequence);
}







/** laserSequences::disableAndResetTPlaySequence(): public setter method
 * 
 *  Resets the parameters of the Tasks tPlaySequence to its default parameters, 
 *  to play a laserSequence, as instructed from a boxState or stand alone. 
 * 
 *  Task tPlaySequence default parameters:
 *  - interval: 0 second;
 *  - 0 iteration;
 *  - main callback: &_oetcbPlaySequence;
 *  - onEnable callback:  &_oetcbPlaySequence;
 *  - onDisable callback: &_odtcbPlaySequence)
 *  - added to myScheduler in setup(); disabled by default.
 * 
 *  This method disableAndResetPlaySequenceTasks() is called by laserSequences (from
 *  laserSequences.setActive()).
 * 
 *  Task tPlaySequence is enabled upon entering a new boxState.
 *  It is disabled:
 *  - alone, at the expiration of its programmed iterations; or
 *  - by the onDisable callback of tPlayBoxState.
 *  
 *  Each iteration of tPlaySequence corresponds to one bar in the laserSequence.
 *  Task tPlaySequence iterations parameter is set, once, in its onEnable callback.
 *  Task tPlaySequence interval is reset at each iteration in its main callback.
 * 
 *  public instance setter 
 * */
void laserSequences::disableAndResetTPlaySequence() {
  _bars.disableAndResetTPlayBar();
  tPlaySequence.disable();
  tPlaySequence.set(0, 1, [&](){_tcbPlaySequence();}, [&](){return _oetcbPlaySequence();}, [&](){return _odtcbPlaySequence();});
}







void laserSequences::setStopCallbackForTPlaySequence() {
  tPlaySequence.setOnDisable([&](){return _odtcbPlaySequenceStop();});
}







/** laserSequence const & laserSequences::getActiveLaserSequence() const
 * 
 * Returns the active laserSequence as a laserSequence */
laserSequence const & laserSequences::getActiveLaserSequence() const {
  return _activeLaserSequence;
}







///////////////////////////////////
// laserSequence Players
///////////////////////////////////
/** laserSequences::playSequence(const laserSequence & __target_laser_sequence):
 *  
 *  play a single laserSequence calculating the durations on the basis of the passed-in beat.
 * 
 *  {@ params} const int16_t __i16_laser_sequence_id: optional laserSequence id in the 
 *             laserSequence array (might be needed for debug and interface purpose)
 * */
uint16_t const laserSequences::_playSequence(const laserSequence & __target_laser_sequence) {
  // 0. Do not do anything if the beat has not been set
  if ((__target_laser_sequence._beat.getBaseBeatInBpm() == 0) || (__target_laser_sequence._beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // 1. set the active laserSequence and the active beat
  if (_setActive(__target_laser_sequence) == -2) {
    return 1;
  }

  // 2. restart the tPlaySequence or tPlaySequenceStandAlone Task
  tPlaySequence.restart();

  // 3. return 2 for success
  return 2;
}






/** laserSequences::playSequence(const uint16_t __target_laser_sequence_ix_numb):
 *  
 *  play a single laserSequence calculating the durations on the basis of the passed-in beat.
 * 
 *  {@ params} const int16_t __i16_laser_sequence_id: optional laserSequence id in the 
 *             laserSequence array (might be needed for debug and interface purpose)
 * */
uint16_t const laserSequences::playSequence(const uint16_t __target_laser_sequence_ix_numb) {
  // 1. Load the laserSequence into memory
  _preloadNextSequence(__target_laser_sequence_ix_numb);

  // 2. Call playSequence(const laserSequence & __target_laser_sequence)
  return _playSequence(nextSequence);
}






///////////////////////////////////
// laserSequence Player
///////////////////////////////////
/** Task tPlaySequence.
 *  
 * It plays a given laserSequence once.
 * */



/** bool laserSequences::_oetcbPlaySequence(): onEnable callback for tPlaySequence
 *  
 *  sets the number of iterations of tPlaySequence to the number of bars in this laserSequence.
 * */
bool laserSequences::_oetcbPlaySequence() {
  /** 1. Set the number of iterations of the tPlaySequence task from the number of bars in the laserSequence. */
  tPlaySequence.setIterations(_activeLaserSequence.ui16GetBarCountInLaserSequence());

  /** 2. Signal the change of laserSequence to the mesh. */
  if (sendCurrentSequence != nullptr) {
    sendCurrentSequence(_activeLaserSequence.i16IndexNumber);
  }

  return true;
}



/** void laserSequences::_tcbPlaySequence(): Main callback for tPlaySequence
 * 
 *  Each iteration of tPlaySequence corresponds to a bar.
 * 
 *  The number of iterations for the Task have been set in its onEnable 
 *  callback and do not change on each iterations.
 * 
 *  To the difference of musical partitions (where bars in a laserSequence always have the 
 *  same duration), the duration of each bar is recalculated at each iteration. This
 *  allows more flexibility (bars of various durations may be inserted in a laserSequence).
 * 
 *  At each iteration of tPlaySequence:
 *  - recalculate the bar duration (steps 1 and 2 below);
 *  - set the Task interval before next iteration accordingly (step 4);
 *  - call _bars.playBar (step 3) to play the bar which is currently active in the laserSequence;
 *  - preload the next bar from SPIFFS.
 * */
void laserSequences::_tcbPlaySequence() {
  Serial.println("laserSequences::_tcbPlaySequence(). starting.");
  Serial.println(F("------------- DEBUG --------- SEQUENCE --------- DEBUG -------------"));

  /**1. Play the active bar*/
  _bars.playBar(_activeLaserSequence._beat);

  /**2. Set the interval for next iteration of tPlaySequence
   * 
   *    At each pass, reset the interval before the next iteration of this Task.
   *    This marks the duration of each bar played in the context of a laserSequence.
   *  */
  tPlaySequence.setInterval(_bars.nextBar.ui32GetBarDuration(_activeLaserSequence._beat));

  /**3. Preload the next bar*/
  _bars.preloadNextBarThroughTask(_activeLaserSequence.i16GetBarIndexNumber(tPlaySequence.getRunCounter()));

  Serial.println("laserSequences::_tcbPlaySequence(). over.");
};



/** laserSequences::_odtcbPlaySequence()
 * 
 *  tPlaySequence disable loop (default) callback 
 * */
void laserSequences::_odtcbPlaySequence() {
  Serial.println("laserSequences::_odtcbPlaySequence(). Will restart playing the active laserSequence once its last bar has been played.");
  _bars.preloadNextBarThroughTask(_activeLaserSequence.i16GetFirstBarIndexNumber());
  tPlaySequence.restartDelayed(_bars._activeBar.ui32GetBarDuration(_activeLaserSequence._beat));
}



/** tPlaySequence disable stop (non-default) callback
 * 
 *  Turns off all the laser by playing laserSequence 5 ("all off"). 
 * */
void laserSequences::_odtcbPlaySequenceStop() {
  Serial.println("laserSequences::_odtcbPlaySequenceStop(). Will start to play laserSequence 5 (allOff).");
  if (_activeLaserSequence.i16IndexNumber != 5) {
    playSequence(5);
}
};










///////////////////////////////////
// laserSequence Loader
///////////////////////////////////
void laserSequences::_tcbPreloadNextSequence() {
  Serial.printf("laserSequences::_tcbPreloadNextSequence(): starting\n");
  // read next step values from the file system
  _preloadNextSequence(ui16IxNumbOfSequenceToPreload);
  Serial.printf("laserSequences::_tcbPreloadNextSequence(): ending\n");
}




void laserSequences::_preloadNextSequence(const uint16_t _ui16IxNumbOfSequenceToPreload){
  Serial.printf("stepCollection::_preloadNextSequence: Reading file: %s\r\n", sequenceFileName);

  mySpiffs __mySpiffs;
  if (!(__mySpiffs.readCollectionItemParamsInFile(sequenceFileName, _ui16IxNumbOfSequenceToPreload))) {
    return;
  }

  // Get a reference to the root object
  JsonObject _joSequence = __mySpiffs._jdItemParams.as<JsonObject>();

  _preloadNextSequenceFromJSON(_joSequence);
}




void laserSequences::_preloadNextSequenceFromJSON(const JsonObject& _joSequence) {
  // {"bt":{"bpm":2,"base":1}, "brs":[0,3,2,6], "ix":0}
  Serial.println("laserSequences::_preloadNextSequenceFromJSON: starting");
  // Load _joSequence["brs"] into an std::array
  std::array<int16_t, 8> __i16BarsIxNumbsArray = _parseJsonBarsArray(_joSequence["brs"].as<JsonArray>());

  // Load the next laserSequence into a laserSequence instance
  nextSequence = {
    beat{
      _joSequence["bt"]["bpm"].as<uint16_t>(),
      _joSequence["bt"]["base"].as<uint16_t>()
    },
    __i16BarsIxNumbsArray,
    _joSequence["ix"].as<int16_t>()
  };
}



std::array<int16_t, 8> laserSequences::_parseJsonBarsArray(const JsonArray& _jaBarsArray) {
  // "brs":[0,3,2,6]
  int16_t _barIx = 0;
  std::array<int16_t, 8> __i16BarsIxNumbsArray {};
  for (JsonVariant _JsonBarIxNumber : _jaBarsArray) {
    __i16BarsIxNumbsArray.at(_barIx) = _JsonBarIxNumber;
    _barIx++;
  }
  return __i16BarsIxNumbsArray;
}
