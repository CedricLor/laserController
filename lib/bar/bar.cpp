/*
  bar.cpp - bars are precoded sequences of laserNotes
  Created by Cedric Lor, June 4, 2019.


Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "bar.h"


//****************************************************************//
// Bar //*********************************************************//
//****************************************************************//






///////////////////////////////////
// Constructors
///////////////////////////////////
// default
bar::bar() :
  i16IndexNumber(-2), /** default initialization at -2 */
  _laserNotesArray{{}} /** TODO: This initialization is supposed to be the good one. Check initialization of other classes (laserSequence, bars, laserNotes, laserNote, laserTones, laserTone, laserPins) */
{
}

// parameterized
bar::bar(
  /** TODO: Here, I am not passing the __laserNotesArray by reference; 
   *        - check other constructors of other classes; 
   *            OK for laserPin; nothing to do with laserPins
   *            OK for laserTone (std::array<bool, 4> _laserPinsStatesArr)
   *            laserSequence: not done; waiting complete refacto of laserSequence
   *        - see whether this is optimal;
   *        - review history of why this is this way here (I think this was a bug correction);
   *        - check whether the solution adopted here (passing in by value)
   *          shall not be duplicated in other classes. */
  std::array<laserNote, 16> __laserNotesArray,
  const int16_t __i16IndexNumber
):
  i16IndexNumber(__i16IndexNumber),
  _laserNotesArray{__laserNotesArray} 
{
}

// copy constructor
bar::bar(const bar& __bar):
  i16IndexNumber(__bar.i16IndexNumber),
  _laserNotesArray{__bar._laserNotesArray}
{
}

// assignement operator
bar& bar::operator=(const bar& __bar)
{
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- Before assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar.ui16GetBaseNotesCountInBar() = %u\n", __bar.ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._laserNotesArray[0].getToneNumber() = %u\n", __bar._laserNotesArray[0].getToneNumber());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._laserNotesArray[0].getNote() = %u\n", __bar._laserNotesArray[0].getNote());
  // Serial.printf("bar& bar::operator=(const bar& __bar). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _laserNotesArray[0].getToneNumber() = %u\n", _laserNotesArray[0].getToneNumber());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _laserNotesArray[0].getNote() = %u\n", _laserNotesArray[0].getNote());
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- End before assignment ---------------");
  if (&__bar != this) {
    // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- Passed self test ---------------");
    i16IndexNumber = __bar.i16IndexNumber;
    _laserNotesArray = __bar._laserNotesArray;
  }
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- After assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _laserNotesArray[0].getToneNumber() = %u\n", _laserNotesArray[0].getToneNumber());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _laserNotesArray[0].getNote() = %u\n", _laserNotesArray[0].getNote());
  return *this;
}











///////////////////////////////////
// Getters
///////////////////////////////////
/** uint16_t const bar::ui16GetNotesCountInBar() const
 * 
 * Returns the effective number of laserNotes (as opposed to 
 * the base laserNote count) in a bar */
uint16_t const bar::ui16GetNotesCountInBar() const {
  // Serial.println("bar::ui16GetNotesCountInBar(): starting.");
  uint16_t __ui = 0;
  while (_laserNotesArray[__ui].getNote() != 0) {
    __ui++;
  }
  // Serial.printf("bar::ui16GetNotesCountInBar(): about to return [%u]\n", __ui);
  return __ui;
}

/** uint16_t const ui16GetBaseNotesCountInBar() const
 * 
 * Returns the base laserNote count (as opposed to the effective
 * number of laserNotes) in a bar */
uint16_t const bar::ui16GetBaseNotesCountInBar(const beat & _beat) const {
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): starting\n");
  if (_beat.getBaseNoteForBeat() == 0) {
    return 0;
  }
  uint16_t __ui = 0;
  uint16_t __ui16TotalNotesIn16th = 0;
  uint16_t __ui16Note;
  /**
   * 1. ronde                   : 16 : 16 / 1
   * 2. blanche                 : 8  : 16 / 2
   * 3. noire + croche          : 6  : 16 / 4 + (1/2)*(16 / 4) : 16 modulo 3 = 1
   * 4. noire                   : 4  : 16 / 4
   * 6. croche + double croche  : 3  : 16 / 8 + (1/2)*(16 / 8) : 16 modulo 6 = 4
   * 8. croche                  : 2  : 16 / 8
   * 16. double croche          : 1  : 16 / 16
  */
  while ((__ui16Note = _laserNotesArray[__ui].getNote()) != 0) {
    // Serial.printf("bar::ui16GetBaseNotesCountInBar: __ui16Note = [%u]\n", __ui16Note);
    // Serial.printf("bar::ui16GetBaseNotesCountInBar: __ui = [%u]\n", __ui);
    __ui++;
    // 3; noire + croche
    if (__ui16Note == 3) { 
      __ui16TotalNotesIn16th += 6; // 4 + 2;
      continue;
    }
    // 6; croche + double croche
    if (__ui16Note == 6) {
      __ui16TotalNotesIn16th += 3; // 2 + 1
      continue;
    }
    __ui16TotalNotesIn16th += 16 / __ui16Note;
  };
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): __ui16TotalNotesIn16th = [%u]\n", __ui16TotalNotesIn16th);
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): _beat.getBaseNoteForBeat() = [%u]\n", _beat.getBaseNoteForBeat());
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): about to return [%u]\n", (__ui16TotalNotesIn16th / 16 * _beat.getBaseNoteForBeat()));
  return (__ui16TotalNotesIn16th / 16 * _beat.getBaseNoteForBeat());
}


/** uint32_t const bar::ui32GetBarDuration() const
 * 
 * Returns the current bar effective duration in ms */
uint32_t const bar::ui32GetBarDuration(const beat & _beat) const {
  // Serial.println(F("bar::ui32GetBarDuration(). starting."));
  // Serial.printf("bar::ui32GetBarDuration(). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar::ui32GetBarDuration(). _beat.ui16GetBaseNoteDurationInMs() = %u\n", _beat.ui16GetBaseNoteDurationInMs());
  // Serial.printf("bar::ui32GetBarDuration(). about to return the following value: %u\n", ui16GetBaseNotesCountInBar() * _beat.ui16GetBaseNoteDurationInMs());
  return (ui16GetBaseNotesCountInBar(_beat) * _beat.ui16GetBaseNoteDurationInMs());
}


/**const std::array<laserNote, 16> & bar::getNotesArray() const
 * 
 * Instance getter.
 * 
 * Returns a pointer to the associated notes array */
const std::array<laserNote, 16> & bar::getNotesArray() const {
  return _laserNotesArray;
}







//****************************************************************//
// Bars //********************************************************//
//****************************************************************//

bars::bars(
  myMeshViews & __thisMeshViews
  // void (*_sendCurrentBar)(const int16_t __i16_current_bar_id)
):
  // sendCurrentBar(_sendCurrentBar),
  ui16IxNumbOfBarToPreload(0),  // <-- TODO: review setters method here; maybe need to cast barIndex as an int16, to initialize at -1
  nextBar(),
  barFileName("/bars.json"),
  tPlayBar(),
  _thisMeshViews(__thisMeshViews),
  _laserNotes(),
  _defaultBar(),
  _activeBar(_defaultBar)
{
  // 1. Disable and reset the Task tPlayBar
  disableAndResetTPlayBar();

  // 2. Define _barsArray, an array containing a series of hard coded bars

  /** Signature of a bar:
   *  a. std::array<laserNote, 16> & _laserNotesArray: a reference to an array of laserNotes. Each laserNote
   *     is composed of:
   *     (i) its duration (expressed in base laserNote - full, half, quarter, etc.) and;
   *     (ii) its laserTone.
   *     ex.: { 2, 1, 2, 2, {{1,7},{1,8}}}
   *     a. 2: the bar shall be played at 2 beats per minutes => the base laserNote last 30 seconds.
   *     b. 1: the base laserNote is a white
   *     c. 2: we have two laserNotes per bar;
   *     d. 2: we have two effective laserNotes in this specific bar;
   *     e. the two laserNotes are:
   *        - {1,7} -> a white with laserTone 7.
   *        - {1,8} -> a white with laserTone 8. 
   *  b. the bar's index number in the barsArray
   * 
   * */

  /** relays
   * {"nts":[{"t":7,"n":1},{"t":8,"n":1}], "ix":0}
   * 
   * */  
  // std::array<laserNote, 16> _aRelays {laserNote(7,1), laserNote(8,1)};
  // _barsArray[0] = {_aRelays, 0};

  /** twins
   * {"nts":[{"t":5,"n":1},{"t":6,"n":1}], "ix":1}
   * 
   * */
  // std::array<laserNote, 16> _aTwins {laserNote(5,1), laserNote(6,1)};
  // _barsArray[1] = {_aTwins, 1};

  /** all 
   * {"nts":[{"t":15,"n":1},{"t":1,"n":1}], "ix":2}
   * 
   * */
  // std::array<laserNote, 16> _aAll {laserNote(15,1), laserNote(0,1)};
  // _barsArray[2] = {_aAll, 2};

  /** swipeRight
   * {"nts":[{"t":1,"n":1},{"t":2,"n":1},{"t":3,"n":1},{"t":4,"n":1}], "ix":3}
   * 
   * */
  // std::array<laserNote, 16> _aSwipeR {laserNote(1,1), laserNote(2,1), laserNote(2,1), laserNote(4,1)};
  // _barsArray[3] = {_aSwipeR, 3};

  /** swipeLeft
   * {"nts":[{"t":4,"n":1},{"t":3,"n":1},{"t":2,"n":1},{"t":1,"n":1}], "ix":4}
   * 
   * */
  // std::array<laserNote, 16> _aSwipeL {laserNote(4,1), laserNote(3,1), laserNote(2,1), laserNote(1,1)};
  // _barsArray[4] = {_aSwipeL, 4};

  /** all off
   * {"nts":[{"t":5,"n":1},{"t":0,"n":1}], "ix":5}
   * 
   * */
  // std::array<laserNote, 16> _aAllOff {laserNote(5,1), laserNote(0,1)};
  // _barsArray[5] = {_aAllOff, 5};
  tPreloadNextBar.set(0, 1, [&](){ return _tcbPreloadNextBar(); }, NULL, NULL);
}



///////////////////////////////////
// Setters
///////////////////////////////////
/** bars::setActive(const bar & __activeBar): public instance setter method
 * 
 *  bars::setActive() sets the variable _activeBar (of the bars instance)
 *  using the passed-in param (which is a const bar lvalue reference).
 * 
 *  Drafting of the former bar method, which this method is supposed to replace.
 * 
 * void bar::setActive(const int16_t __i16_active_bar_id) {
 *   tPlayBar.disable();
 *   _activeBar = *this;
 *   _i16ActiveBarId = __i16_active_bar_id;
 * }
 *  */
uint16_t const bars::setActive(const bar & __activeBar) {
  disableAndResetTPlayBar();
  _activeBar = __activeBar;
  return i16GetActiveBarId();
}


/** bars::disableAndResetTPlayBar(): public setter method
 * 
 *  
 *  Disables and resets the parameters of the Task tPlayNote (by calling
 *  _disableAndResetTPlayNote) and  Task tPlayBar to its default parameters, to
 *  play bars read from a laserSequence. 
 * 
 *   Task tPlayBar default parameters:
 *   - an interval of 0 second;
 *   - one single iteration;
 *   - its default main callback;
 *   - its default onEnable callback
 * 
 *   bars::disableAndResetTPlayBar() is called by:
 *   - the constructor of the bars class, before initializing the various hard-coded bars;
 *   - setActive() in the bars class (which is itself called by playBar());
 *   - the test file (test.cpp);
 *   - the laserInterface;
 *   - playSequence in the laserSequences class;
 * 
 *   The Task tPlayBar is enabled upon entering a new bar by playBar.
 * 
 *   It is disabled:
 *   - once it has performed all of its iterations; or
 *   - by the callback of tPlayLaserSequence.
 * 
 *   task tPlayBar plays a given bar (set in bars::activeBar) 
 *   at a given beat rate.
 * */
void bars::disableAndResetTPlayBar() {
  _laserNotes._disableAndResetTPlayNote();
  tPlayBar.disable();
  tPlayBar.set(0, 1, [&](){_tcbPlayBar(beat{});}, [&](){return _oetcbPlayBar();}, NULL);
}






///////////////////////////////////
// Getters
///////////////////////////////////
/** int16_t const bars::i16GetActiveBarId() const
 * 
 * Returns the active bar index number */
int16_t const bars::i16GetActiveBarId() const {
  return _activeBar.i16IndexNumber;
}



/** laserNotes & bar::getNotes()
 * 
 * Returns a ref to the laserNotes instance stored in bars */
laserNotes & bars::getNotes() {
  return _laserNotes;
}




///////////////////////////////////
// Player
///////////////////////////////////
/** bars::playBar(beat const & __beat):
 *  
 *  play the preloaded bar.
 * 
 *  {@ params} beat const & __beat: beat to be taken into account
 *             to calculate the laserNotes duration */
uint16_t const bars::playBar(beat const & __beat) {
  // 0. Do not do anything if the beat has not been set
  if ((__beat.getBaseBeatInBpm() == 0) || (__beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // 1. set the passed-in bar as active (setActive() will also call 
  //    disableAndResetTPlayBar() and _laserNotes._disableAndResetTPlayNote())
  if (setActive(nextBar) == -2) {
    return 1;
  };
  
  // 2. set the main callback
  tPlayBar.setCallback([&](){_tcbPlayBar(__beat);});

  // 3. once all the setting have been done, play the bar
  tPlayBar.restart();

  // 4. return 2 for success
  return 2;
}




/** bars::_oetcbPlayBar(): enable callback for Task tPlayBar.
 *  
 *  Upon enabling Task tPlayBar, this _oetcbPlayBar() set the number of iterations 
 *  for Task tPlayBar.
 * 
 *  Each iteration of Task tPlayBar stops and starts the tPlayNote Task, which plays
 *  the laserNotes stored in the bar.
 * 
 *  Accordingly, the number of iterations of Task tPlayBar shall be equal to 
 *  the effective number of laserNotes in the bar.  
 * */
bool bars::_oetcbPlayBar(){
  // Serial.println("bars::_oetcbPlayBar(). starting.");

  /**1. set the number of iterations base of the effective number of laserNotes in the bar */
  tPlayBar.setIterations(_activeBar.ui16GetNotesCountInBar());

  /**2. Send the bar index number to the mesh */
  _thisMeshViews.sendBar(_activeBar.i16IndexNumber);

  // Serial.println("bars::_oetcbPlayBar(). over.");
  return true;
}




/** _tcbPlayBar(): Task tPlayBar main callback
 *  Each pass corresponds to a laserNote in the laserNotes array property of the
 *  currently active bar.
 * 
 *  Accordingly, we reset the interval of tPlayBar to the duration of the laserNote.
 * 
 *  - At this iteration of tPlayBar, _tcbPlayBar enables tPlayNote (for a
 *    default duration of 30s. -> in the definition of tPlayNote, in class laserNote.)
 * 
 *  - At the next iteration, which will occur after the interval corresponding
 *    to duration of the current laserNote, tPlayBar will disable tPlayNote (to stop 
 *    playing the former laserNote) and restart it (to start playing the next laserNote).
 * */
void bars::_tcbPlayBar(beat const & __beat){
  Serial.println("bars::_tcbPlayBar(). starting.");

  /**1. Get the run counter and decrease it by 1 (the run counter uses "human" numbering; 
   *    i.e. it starts at 1 (0 is 0).
   * */
  uint16_t _ui16Iter = tPlayBar.getRunCounter();
  _ui16Iter = ((0 == _ui16Iter) ? 0 : (_ui16Iter - 1));

  /**2. Call playNote() on the current laserNote in the bar.
   * 
   *    playNote():
   *    a. disable tPlayNote;
   *    b. set the current laserNote in the bar (_activeBar.getNotesArray().at[_ui16Iter]) 
   *       as _activeLaserNote in the laserNote class;
   *    c. restartDelayed tPlayNote. 
   * */
  _laserNotes.playNote(_activeBar.getNotesArray().at(_ui16Iter), __beat);

  /**3. Set the interval for next iteration of tPlayBar
   * 
   *    At each pass, reset the interval before the next iteration of this 
   *    Task bars::tPlayBar. This marks the duration of each laserNote played in the
   *    context of a bar. 
   * */
  tPlayBar.setInterval(_activeBar.getNotesArray().at(_ui16Iter).ui16GetNoteDurationInMs(__beat));

  Serial.println(F("bars::_tcbPlayBar(). over."));
};










///////////////////////////////////
// Bar Loader
///////////////////////////////////
void bars::preloadNextBarStraight(uint16_t _ui16IxNumbOfBarToPreload){
  // read next bar values from the file system
  ui16IxNumbOfBarToPreload = _ui16IxNumbOfBarToPreload;
  _preloadNextBar(ui16IxNumbOfBarToPreload);
}




void bars::preloadNextBarThroughTask(uint16_t _ui16IxNumbOfBarToPreload){
  // read next bar values from the file system
  ui16IxNumbOfBarToPreload = _ui16IxNumbOfBarToPreload;
  tPreloadNextBar.restart();
}




void bars::_tcbPreloadNextBar() {
  // read next bar values from the file system
  _preloadNextBar(ui16IxNumbOfBarToPreload);
}




void bars::_preloadNextBar(uint16_t _ui16IxNumbOfBarToPreload){
  Serial.printf("bars::_preloadNextBar: Reading file: %s\r\n", barFileName);

  mySpiffs __mySpiffs;
  if (!(__mySpiffs.readCollectionItemParamsInFile(barFileName, _ui16IxNumbOfBarToPreload))) {
    return;
  }

  // Get a reference to the root array
  JsonObject _joBar = __mySpiffs._jdItemParams.as<JsonObject>();

  _preloadNextBarFromJSON(_joBar);
}




void bars::_preloadNextBarFromJSON(const JsonObject& _joBar) {
  // {"nts":[{"t":7,"n":1},{"t":8,"n":1}], "ix":0}
  Serial.println("bars::_preloadNextBarFromJSON: starting");
  // Load _joBar["nts"] into an std::array
  std::array<laserNote, 16> _laserNotesArray = _parseJsonNotesArray(_joBar["nts"].as<JsonArray>());

  // Load the next laserSequence into a laserSequence instance
  nextBar = {
    _laserNotesArray,
    _joBar["ix"].as<int16_t>()
  };
}



std::array<laserNote, 16> const bars::_parseJsonNotesArray(const JsonArray& _JsonNotesArray) {
  uint16_t _noteIx = 0;
  std::array<laserNote, 16> _laserNotesArray;
  for (JsonVariant _JsonNote : _JsonNotesArray) {
    _laserNotesArray.at(_noteIx) = {
      _JsonNote["t"].as<uint16_t>(),
      _JsonNote["n"].as<uint16_t>()
    };
    _noteIx++;
  }
  return _laserNotesArray;
}