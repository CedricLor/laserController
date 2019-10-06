/*
  bar.cpp - bars are precoded sequences of notes
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

bar globalBar{}; // TODO: <-- For the moment, it is at the global scope; find a way to have it stored somewhere else







///////////////////////////////////
// Constructors
///////////////////////////////////
// default
bar::bar() :
  i16IndexNumber(-1), /** default initialization at -1 */
  _notesArray{{}} /** TODO: This initialization is supposed to be the good one. Check initialization of other classes (sequence, bars, notes, note, tones, tone, laserPins) */
{
}

// parameterized
bar::bar(
  /** TODO: Here, I am not passing the __notesArray by reference; 
   *        - check other constructors of other classes; 
   *            OK for laserPin; nothing to do with laserPins
   *            OK for tone (std::array<bool, 4> _laserPinsStatesArr)
   *            sequence: not done; waiting complete refacto of sequence
   *        - see whether this is optimal;
   *        - review history of why this is this way here (I think this was a bug correction);
   *        - check whether the solution adopted here (passing in by value)
   *          shall not be duplicated in other classes. */
  std::array<note, 16> __notesArray,
  const int16_t __i16IndexNumber
):
  i16IndexNumber(__i16IndexNumber),
  _notesArray{__notesArray} 
{
}

// copy constructor
bar::bar(const bar& __bar):
  i16IndexNumber(__bar.i16IndexNumber),
  _notesArray{__bar._notesArray}
{
}

// assignement operator
bar& bar::operator=(const bar& __bar)
{
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- Before assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar.ui16GetBaseNotesCountInBar() = %u\n", __bar.ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._notesArray[0].getToneNumber() = %u\n", __bar._notesArray[0].getToneNumber());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._notesArray[0].getNote() = %u\n", __bar._notesArray[0].getNote());
  // Serial.printf("bar& bar::operator=(const bar& __bar). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getToneNumber() = %u\n", _notesArray[0].getToneNumber());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getNote() = %u\n", _notesArray[0].getNote());
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- End before assignment ---------------");
  if (&__bar != this) {
    // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- Passed self test ---------------");
    i16IndexNumber = __bar.i16IndexNumber;
    _notesArray = __bar._notesArray;
  }
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- After assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getToneNumber() = %u\n", _notesArray[0].getToneNumber());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getNote() = %u\n", _notesArray[0].getNote());
  return *this;
}











///////////////////////////////////
// Getters
///////////////////////////////////
/** uint16_t const bar::ui16GetNotesCountInBar() const
 * 
 * Returns the effective number of notes (as opposed to 
 * the base note count) in a bar */
uint16_t const bar::ui16GetNotesCountInBar() const {
  // Serial.println("bar::ui16GetNotesCountInBar(): starting.");
  uint16_t __ui = 0;
  while (_notesArray[__ui].getNote() != 0) {
    __ui++;
  }
  // Serial.printf("bar::ui16GetNotesCountInBar(): about to return [%u]\n", __ui);
  return __ui;
}

/** uint16_t const ui16GetBaseNotesCountInBar() const
 * 
 * Returns the base note count (as opposed to the effective
 * number of notes) in a bar */
uint16_t const bar::ui16GetBaseNotesCountInBar() const {
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): starting\n");
  if (activeBeat.getBaseNoteForBeat() == 0) {
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
  while ((__ui16Note = _notesArray[__ui].getNote()) != 0) {
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
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): activeBeat.getBaseNoteForBeat() = [%u]\n", activeBeat.getBaseNoteForBeat());
  // Serial.printf("bar::ui16GetBaseNotesCountInBar(): about to return [%u]\n", (__ui16TotalNotesIn16th / 16 * activeBeat.getBaseNoteForBeat()));
  return (__ui16TotalNotesIn16th / 16 * activeBeat.getBaseNoteForBeat());
}


/** uint32_t const bar::ui32GetBarDuration() const
 * 
 * Returns the current bar effective duration in ms */
uint32_t const bar::ui32GetBarDuration() const {
  // Serial.println(F("bar::ui32GetBarDuration(). starting."));
  // Serial.printf("bar::ui32GetBarDuration(). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar::ui32GetBarDuration(). activeBeat.ui16GetBaseNoteDurationInMs() = %u\n", activeBeat.ui16GetBaseNoteDurationInMs());
  // Serial.printf("bar::ui32GetBarDuration(). about to return the following value: %u\n", ui16GetBaseNotesCountInBar() * activeBeat.ui16GetBaseNoteDurationInMs());
  return (ui16GetBaseNotesCountInBar() * activeBeat.ui16GetBaseNoteDurationInMs());
}


/**const std::array<note, 16> & bar::getNotesArray() const
 * 
 * Instance getter.
 * 
 * Returns a pointer to the associated bars array */
const std::array<note, 16> & bar::getNotesArray() const {
  return _notesArray;
}







//****************************************************************//
// Bars //********************************************************//
//****************************************************************//

bars::bars(
  void (*_sendCurrentBar)(const int16_t __i16_current_bar_id)
):
  sendCurrentBar(_sendCurrentBar),
  tPlayNote(_notes.tPlayNote),
  _activeBar(globalBar),
  _notes(),
  _barsArray({})
{
  // 1. Disable and reset the Task tPlayBar
  disableAndResetTPlayBar();

  // 2. Define _barsArray, an array containing a series of hard coded bars

  /** Signature of a bar:
   *  a. std::array<note, 16> & _notesArray: a reference to an array of notes. Each note
   *     is composed of:
   *     (i) its duration (expressed in base note - full, half, quarter, etc.) and;
   *     (ii) its tone.
   *     ex.: { 2, 1, 2, 2, {{1,7},{1,8}}}
   *     a. 2: the bar shall be played at 2 beats per minutes => the base note last 30 seconds.
   *     b. 1: the base note is a white
   *     c. 2: we have two notes per bar;
   *     d. 2: we have two effective notes in this specific bar;
   *     e. the two notes are:
   *        - {1,7} -> a white with tone 7.
   *        - {1,8} -> a white with tone 8. 
   *  b. the bar's index number in the barsArray
   * 
   *  It used to include:
   *  a. a beat object, which contains:
   *     1. the duration of a beat in bpm (i.e. the tempo);
   *     2. the base note for each beat (full, half, quarter, etc.) 
   *        (the 4 in 2/4, for instance; the 2 is defined at the sequence level);
   * */

  /** relays
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  
  // Serial.println("bars::bars. before creating _aRelays");
  std::array<note, 16> _aRelays {note(7,1), note(8,1)};
  // Serial.println("bars::bars. _aRelays created");
  _barsArray[0] = {_aRelays, 0};
  // Serial.println("bars::bars. _barsArray[0] copy assigned");
  // Serial.printf("bars::bars. _barsArray[0].ui16GetBaseNotesCountInBar() == 2 ? %i\n", _barsArray[0].ui16GetBaseNotesCountInBar() == 2);
  // Serial.printf("bars::bars. _barsArray[0].getNotesArray().at(0).getToneNumber() = %u\n", _barsArray[0].getNotesArray().at(0).getToneNumber());
  // Serial.printf("bars::bars. _barsArray[0].getNotesArray().at(0).getToneNumber() == 7 ? %s\n", ( (_barsArray[0].getNotesArray().at(0).getToneNumber() == 7) ? "true" : "false") );
  // Serial.printf("bars::bars. _barsArray[0].getNotesArray().at(0).getNote() = %u\n", _barsArray[0].getNotesArray().at(0).getNote());
  // Serial.printf("bars::bars. _barsArray[0].getNotesArray().at(0).getNote() == 1 ? %s\n", ( (_barsArray[0].getNotesArray().at(0).getNote() == 1) ? "true" : "false") );

  /** twins
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  std::array<note, 16> _aTwins {note(5,1), note(6,1)};
  _barsArray[1] = {_aTwins, 1};

  /** all 
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  std::array<note, 16> _aAll {note(15,1), note(0,1)};
  _barsArray[2] = {_aAll, 2};

  /** swipeRight
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  std::array<note, 16> _aSwipeR {note(1,1), note(2,1), note(2,1), note(4,1)};
  _barsArray[3] = {_aSwipeR, 3};

  /** swipeLeft
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  std::array<note, 16> _aSwipeL {note(4,1), note(3,1), note(2,1), note(1,1)};
  _barsArray[4] = {_aSwipeL, 4};

  /** all off
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 1
   * => 1 / 1 */
  std::array<note, 16> _aAllOff {note(5,1), note(0,1)};
  _barsArray[5] = {_aAllOff, 5};

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
  return _activeBar.i16IndexNumber;
}


/** bars::disableAndResetTPlayBar(): public setter method
 * 
 *  
 *  Disables and resets the parameters of the Task tPlayNote (by calling
 *  _disableAndResetTPlayNote) and  Task tPlayBar to its default parameters, to
 *  play bars read from a sequence. 
 * 
 *   Task tPlayBar default parameters:
 *   - an interval of 0 second;
 *   - one single iteration;
 *   - its default main callback;
 *   - its default onEnable callback
 * 
 *   bars::disableAndResetTPlayBar() is called by:
 *   - the constructor of the bars class, before initializing the various hard-coded bars;
 *   - setActive() in the bars class (which is itself called by playBarStandAlone() and playBarInSequence());
 *   - the test file (test.cpp);
 *   - the laserInterface;
 *   - playSequenceStandAlone and playSequenceInBoxState in the sequence class;
 * 
 *   The Task tPlayBar is enabled upon entering a new bar by playBarInSequence
 *   or by playBarStandAlone().
 * 
 *   It is disabled:
 *   - once it has performed all of its iterations; or
 *   - by the callback of tPlaySequence.
 * 
 *   task tPlayBar plays a given bar (set in bars::activeBar) 
 *   at a given beat rate.
 * */
void bars::disableAndResetTPlayBar() {
  tPlayBar.disable();
  tPlayBar.set(0, 1, [&](){_tcbPlayBar();}, [&](){return _oetcbPlayBar();});
}






///////////////////////////////////
// Getters
///////////////////////////////////
/** int16_t const bars::i16GetCurrentBarId() const
 * 
 * Returns the active bar index number */
int16_t const bars::i16GetCurrentBarId() const {
  return _activeBar.i16IndexNumber;
}



/** bar & bars::getCurrentBar() const
 * 
 * Returns the active bar as a bar */
bar const & bars::getCurrentBar() const {
  return _activeBar;
}



/** bar const & bars::getBarFromBarArray(const uint16_t __ui16_bar_id)
 * 
 * Returns a ref to one of the hard coded bars given its index number */
bar const & bars::getBarFromBarArray(const uint16_t __ui16_bar_id) const {
  return _barsArray.at(__ui16_bar_id);
}



/** notes & bar::getNotes()
 * 
 * Returns a ref to the notes instance stored in bars */
notes & bars::getNotes() {
  return _notes;
}




///////////////////////////////////
// Player
///////////////////////////////////
/** bars::playBarStandAlone():
 *  
 *  play a single bar calculating the durations
 *  on the basis of the passed-in beat.
 * 
 *  {@ params} const bar & __target_bar: bar to be played
 *  {@ params} beat const & __beat: beat to be taken into account
 *             to calculate the notes duration */
uint16_t const bars::playBarStandAlone(const bar & __target_bar, beat const & __beat) {
  // 0. Do not do anything if the beat has not been set
  if ((__beat.getBaseBeatInBpm() == 0) || (__beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // 1. set the passed-in bar as active
  if (setActive(__target_bar) == -2) {
    return 1;
  };
  
  /** 2. set the active beat from the passed-in beat */
  activeBeat = __beat;

  /**3. set the tPlayNote Task to its default by calling _notes._disableAndResetTPlayNote()
   *    
   *    The tPlayNote Task will then be enabled and disabled from 
   *    tPlayBar's onEnable and main callbacks (respectively _oetcbPlayBar and _tcbPlayBar).
   * 
   *    The following instruction resets tPlayNote to play a note 
   *    for one single iteration for a maximum duration of 30000 ms. */
  _notes._disableAndResetTPlayNote();

  /**4. set the onDisable callback of tPlayBar: 
   * 
   *    once the standalone bar has been read/played,
   *    the active beat shall be reset to (0,0) (to leave the beat 
   *    in a clean state). 
   *    In addition, tPlayBar does not, in its default state, have 
   *    an onDisable callback. So we need to leave it also clean.
   *    
   *    This way:
   *    (i) bars can be read again as part of a sequence,
   *    (ii) bars can be read again as stand-alone with any new beat, or
   *    (iii) any other class that may depend on a beat (note, notes, bar or sequences) finds a
   *    clean beat state to start with, as necessary. */
  tPlayBar.setOnDisable([&](){
    activeBeat = beat(0, 0);
    tPlayBar.setOnDisable(NULL);
  });

  // 5. once all the setting have been done, play the bar
  tPlayBar.restart();

  // 6. return 2 for success
  return 2;
}




/** bars::playBarInSequence():
 *  
 *  play a single bar calculating the durations
 *  on the basis of the beat set by tPlaySequence. */
bool const bars::playBarInSequence(const bar & __target_bar) {
  if ((activeBeat.getBaseNoteForBeat() == 0) || (activeBeat.getBaseBeatInBpm() == 0)) {
    return false;
  }
  setActive(__target_bar);
  tPlayBar.restart();
  return true;
}




/** bars::_oetcbPlayBar(): enable callback for Task tPlayBar.
 *  
 *  Upon enabling Task tPlayBar, this _oetcbPlayBar() set the number of iterations 
 *  for Task tPlayBar.
 * 
 *  Each iteration of Task tPlayBar stops and starts the tPlayNote Task, which plays
 *  the notes stored in the bar.
 * 
 *  Accordingly, the number of iterations of Task tPlayBar shall be equal to 
 *  the effective number of notes in the bar.  
 * */
bool bars::_oetcbPlayBar(){
  // Serial.println("bars::_oetcbPlayBar(). starting.");

  // if (MY_DG_LASER) {
  //   Serial.println("bars::_oetcbPlayBar(). Before setting the iterations for this bars: *!*!*!*!*!");
  //   Serial.println("bars::_oetcbPlayBar(). tPlaySequence execution parameters:");
  //   Serial.print("bars::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bars::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("bars::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("bars::_oetcbPlayBar(). userScheduler.timeUntilNextIteration(tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("bars::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("bars::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  /**1. set the number of iterations base of the effective number of notes in the bar*/
  tPlayBar.setIterations(_activeBar.ui16GetNotesCountInBar());


  // if (MY_DG_LASER) {
  //   Serial.println("bars::_oetcbPlayBar(). After setting the iterations for this bars: *!*!*!*!*!");
  //   Serial.print("bars::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bars::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("bars::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("bars::_oetcbPlayBar(). userScheduler.timeUntilNextIteration(tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("bars::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("bars::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  // Serial.println("bars::_oetcbPlayBar(). over.");

  return true;
}




/** _tcbPlayBar(): Task tPlayBar main callback
 *  Each pass corresponds to a note in the notes array property of the
 *  currently active bar.
 *   We leverage that to reset the interval of tPlayBar to the duration
 *   of the note.
 *   - At this iteration of tPlayBar, _tcbPlayBar enables tPlayNote (for a
 *     default duration of 30s. -> in the definition of tPlayNote, in class note.)
 *   - At the next iteration, which will occur after the interval corresponding
 *     to duration of the current note, tPlayBar will disable tPlayNote. */
void bars::_tcbPlayBar(){
  Serial.println("bars::_tcbPlayBar(). starting.");

  // 1. get the run counter
  uint16_t _ui16Iter = tPlayBar.getRunCounter();
  _ui16Iter = ((0 == _ui16Iter) ? 0 : (_ui16Iter - 1));
  // Serial.printf("bars::_tcbPlayBar(). tPlayBar.getRunCounter() == [%lu]\n", tPlayBar.getRunCounter());
  // Serial.printf("bars::_tcbPlayBar(). _ui16Iter == [%i]\n", _ui16Iter);

  /**2. Call playNoteInBar() on the current note in the bar.
   * 
   *    playNoteInBar():
   *    a. disable tPlayNote;
   *    b. set the current note in the bar (_activeBar.getNotesArray().at[_ui16Iter]) 
   *       as _activeNote in the note class;
   *    c. restartDelayed tPlayNote. */
  // _activeBar.getNotesArray();
  // Serial.printf("bars::_tcbPlayBar(). _activeBar.getNotesArray() passed\n");
  // _activeBar.getNotesArray().at(_ui16Iter);
  // Serial.printf("bars::_tcbPlayBar(). _activeBar.getNotesArray().at(_ui16Iter) passed\n");
  // Serial.printf("bars::_tcbPlayBar(). _activeBar.getNotesArray().at(_ui16Iter).getNote(): [%u]\n", _activeBar.getNotesArray().at(_ui16Iter).getNote());
  // Serial.printf("bars::_tcbPlayBar(). _activeBar.getNotesArray().at(_ui16Iter).getToneNumber(): [%u]\n", _activeBar.getNotesArray().at(_ui16Iter).getToneNumber());
  _notes.playNoteInBar(_activeBar.getNotesArray().at(_ui16Iter));

  /**3. Set the interval for next iteration of tPlayBar
   * 
   *    At each pass, reset the interval before the next iteration of this 
   *    Task bars::tPlayBar. This marks the duration of each note played in the
   *    context of a bar. */
  // Serial.println(F("------------- DEBUG --------- BAR --------- DEBUG -------------"));
  // Serial.printf("bars::_tcbPlayBar(). setting the interval, by calling _activeBar.getNotesArray().at(%u).ui16GetNoteDurationInMs() = %u\n", _ui16Iter, _activeBar.getNotesArray().at(_ui16Iter).ui16GetNoteDurationInMs());
  tPlayBar.setInterval(_activeBar.getNotesArray().at(_ui16Iter).ui16GetNoteDurationInMs());

  Serial.println(F("bars::_tcbPlayBar(). over."));
};