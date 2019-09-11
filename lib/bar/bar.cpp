/*
  bar.cpp - bars are two beats to four beats, precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.


Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "bar.h"

bar bar::_emptyBar;
bar &bar::_activeBar = _emptyBar;
int16_t bar::_i16ActiveBarId = -1;
std::array<bar, 7> bar::_bars;
std::array<note, 16> bar::_emptyNotesArray;


// pointer to functions to produce an interface for bar
void (*bar::sendCurrentBar)(const int16_t __i16_current_bar_id) = nullptr;




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
bar::bar() :
  _notesArray(_emptyNotesArray)
{
}

// parameterized
bar::bar(
  std::array<note, 16> & __notesArray
):
  _notesArray(__notesArray)
{
}

// copy constructor
bar::bar(const bar& __bar):
  _notesArray(__bar._notesArray)
{
}

// assignement operator
bar& bar::operator=(const bar& __bar)
{
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- Before assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar.ui16GetBaseNotesCountInBar() = %u\n", __bar.ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._notesArray[0].getTone() = %u\n", __bar._notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._notesArray[0].getNote() = %u\n", __bar._notesArray[0].getNote());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0].ui16GetBaseNotesCountInBar() = %u\n", _bars[0].ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0]._notesArray[0].getTone() = %u\n", _bars[0]._notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0]._notesArray[0].getNote() = %u\n", _bars[0]._notesArray[0].getNote());
  // Serial.printf("bar& bar::operator=(const bar& __bar). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getTone() = %u\n", _notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getNote() = %u\n", _notesArray[0].getNote());
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- End before assignment ---------------");
  if (&__bar != this) {
  _notesArray = __bar._notesArray;
  }
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- After assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getTone() = %u\n", _notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getNote() = %u\n", _notesArray[0].getNote());
  return *this;
}





///////////////////////////////////
// Initialisers
///////////////////////////////////
void bar::initBars() {
  Serial.println("void bar::_initBars(). Starting.");
  // define an array containing references to the note type and tones to be played in the bar

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
  std::array<note, 16> _aRelays {note(7,1), note(8,1)};
  _bars[0] = { _aRelays};  
  // Serial.printf("bar::_initBars(). _bars[0].ui16GetBaseNotesCountInBar() == 2 ? %i\n", _bars[0].ui16GetBaseNotesCountInBar() == 2);
  // Serial.printf("bar::_initBars(). _bars[0]._notesArray[0].getTone() = %u\n", _bars[0]._notesArray[0].getTone());
  // Serial.printf("bar::_initBars(). _bars[0]._notesArray[0].getTone() == 7 ? %i\n", _bars[0]._notesArray[0].getTone() == 7);
  // Serial.printf("bar::_initBars(). _bars[0]._notesArray[0].getNote() = %u\n", _bars[0]._notesArray[0].getNote());
  // Serial.printf("bar::_initBars(). _bars[0]._notesArray[0].getNote() == 1 ? %i\n", _bars[0]._notesArray[0].getNote() == 1);

  /** twins
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  std::array<note, 16> _aTwins {note(5,1), note(6,1)};
  _bars[1] = { _aTwins};

  /** all 
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  std::array<note, 16> _aAll {note(15,1), note(0,1)};
  _bars[2] = { _aAll};

  /** swipeRight
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  std::array<note, 16> _aSwipeR {note(1,1), note(2,1), note(2,1), note(4,1)};
  _bars[3] = { _aSwipeR};

  /** swipeLeft
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  std::array<note, 16> _aSwipeL {note(4,1), note(3,1), note(2,1), note(1,1)};
  _bars[4] = { _aSwipeL};

  /** all off
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 1
   * => 1 / 1 */
  std::array<note, 16> _aAllOff {note(5,1), note(0,1)};
  _bars[5] = { _aAllOff};

  Serial.println("void bar::_initBars(). Ending.");
}






///////////////////////////////////
// Player
///////////////////////////////////
/** bar::playBarStandAlone():
 *  
 *  play a single bar calculating the durations
 *  on the basis of the passed-in beat.
 * 
 *  {@ params} beat const & __beat: pass a beat to be taken into account
 *             to calculate the notes duration */
void bar::playBarStandAlone(beat const & __beat) {
  // 1. set the bar as active
  this->setActive();
  // 2. set the active beat from the passed in beat
  beat(__beat).setActive();
  /**3. set the tPlayNote Task to its default when playing notes read from a bar.
   *    tPlayNote will be managed from tPlayBar => set tPlayNote to play each note (in the bar)
   *    for one single iteration and with the maximum available interval (30000). */
  note::resetTPlayNoteToPlayNotesInBar();
  /**4. set the onDisable callback of tPlayBar to reset the active beat to (0,0) once
   *    the stand alone bar has been read, so that (i) bar can be read again as part of a
   *    sequence and (ii) any other class that may depend on beat finds a clean beat to start
   *    with, as required. */
  tPlayBar.setOnDisable([](){
    beat(0, 0).setActive();
    tPlayBar.setOnDisable(NULL);
  });
  // 5. once all the setting have been done, play the bar
  tPlayBar.restart();
}


/** bar::playBarInSequence():
 *  
 *  play a single bar calculating the durations
 *  on the basis of the beat set by tPlaySequence. */
void bar::playBarInSequence() {
  setActive();
  tPlayBar.restart();
}









Task bar::tPlayBar(0, 1, &_tcbPlayBar, NULL/*&mns::myScheduler*/, false, &_oetcbPlayBar, NULL);



/** bar::_oetcbPlayBar(): enable callback for Task tPlayBar.
 *  
 *  Upon enabling, set the number of iterations. 
 *  Each iteration stops and starts the tPlayNote Task.
 *  Accordingly, the number of iterations shall be equal to 
 *  the effective number of notes in the bar.  */
bool bar::_oetcbPlayBar(){
  // Serial.println("bar::_oetcbPlayBar(). Starting.");

  // if (MY_DG_LASER) {
  //   Serial.println("bar::_oetcbPlayBar(). Before setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.println("bar::_oetcbPlayBar(). tPlaySequence execution parameters:");
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("bar::_oetcbPlayBar(). userScheduler.timeUntilNextIteration(tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("bar::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("bar::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  /**1. set the number of iterations base of the effective number of notes in the bar*/
  tPlayBar.setIterations(_activeBar.ui16GetNotesCountInBar());


  // if (MY_DG_LASER) {
  //   Serial.println("bar::_oetcbPlayBar(). After setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("bar::_oetcbPlayBar(). userScheduler.timeUntilNextIteration(tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("bar::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("bar::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  // Serial.println("bar::_oetcbPlayBar(). Ending.");

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
void bar::_tcbPlayBar(){
  Serial.println("bar::_tcbPlayBar(). Starting.");

  // 1. get the run counter
  const uint16_t _ui16_iter = tPlayBar.getRunCounter() - 1;

  /**2. Call playNoteInBar() on the current note in the bar.
   * 
   *    playNoteInBar():
   *    a. disable tPlayNote;
   *    b. set the current note in the bar (_activeBar._notesArray[_ui16_iter]) 
   *       as _activeNote in the note class;
   *    c. restartDelayed tPlayNote. */
  _activeBar._notesArray[_ui16_iter].playNoteInBar();

  /**3. Set the interval for next iteration of tPlayBar
   * 
   *    At each pass, reset the interval before the next iteration of this 
   *    Task bar::tPlayBar. This marks the duration of each note played in the
   *    context of a bar. */
  Serial.println(F("------------- DEBUG --------- BAR --------- DEBUG -------------"));
  Serial.printf("bar::_tcbPlayBar(). calling _activeBar._notesArray[_ui16_iter].ui16GetNoteDurationInMs(%u)\n", _ui16_iter);
  tPlayBar.setInterval(_activeBar._notesArray[_ui16_iter].ui16GetNoteDurationInMs());

  Serial.println(F("bar::_tcbPlayBar(). Ending."));
};





///////////////////////////////////
// Setters
///////////////////////////////////
/** bar::setActive(): public instance setter method
 * 
 *  sets the caller bar as the static variable &bar::_activeBar. */
void bar::setActive(const int16_t __i16_active_bar_id) {
  tPlayBar.disable();
  _activeBar = *this;
  _i16ActiveBarId = __i16_active_bar_id;
}







///////////////////////////////////
// Getters
///////////////////////////////////
/** int16_t const bar::i16GetCurrentBarId()
 * 
 * Returns the active bar index number */
int16_t const bar::i16GetCurrentBarId() {
  return _i16ActiveBarId;
}

/** bar & bar::getCurrentBar()
 * 
 * Returns the active bar as a bar */
bar & bar::getCurrentBar() {
  return _activeBar;
}

/** bar const & bar::getBarFromBarArray(const int16_t __i16_bar_id) 
 * 
 * Returns a const ref to one of the hard coded bars given its index number */
bar & bar::getBarFromBarArray(const int16_t __i16_bar_id) {
  return _bars[__i16_bar_id];
}

/** uint16_t const bar::ui16GetNotesCountInBar() const
 * 
 * Returns the effective number of notes (as opposed to 
 * the base note count) in a bar */
uint16_t const bar::ui16GetNotesCountInBar() const {
  uint16_t __ui = 0;
  while (_notesArray[__ui].getNote() != 0) {
    __ui++;
  }
  return __ui;
}

/** uint16_t const ui16GetBaseNotesCountInBar() const
 * 
 * Returns the base note count (as opposed to the effective
 * number of notes) in a bar */
uint16_t const bar::ui16GetBaseNotesCountInBar() const {
  uint16_t __ui = 0;
  uint16_t __ui16TotalNotesIn16th = 0;
  /**
   * 1. ronde                   : 16 : 16 / 1
   * 2. blanche                 : 8  : 16 / 2
   * 3. noire + croche          : 6  : 16 / 4 + (1/2)*(16 / 4) : 16 modulo 3 = 1
   * 4. noire                   : 4  : 16 / 4
   * 6. croche + double croche  : 3  : 16 / 8 + (1/2)*(16 / 8) : 16 modulo 6 = 4
   * 8. croche                  : 2  : 16 / 8
   * 16. double croche          : 1  : 16 / 16
  */
  while (_notesArray[__ui].getNote() != 0) {
    uint16_t __note = _notesArray[__ui].getNote();
      __ui++;
    // 3; noire + croche
    if (__note == 3) { 
      __ui16TotalNotesIn16th += 6; // 4 + 2;
      continue;
    }
    // 6; croche + double croche
    if (__note == 6) {
      __ui16TotalNotesIn16th += 3; // 2 + 1
      continue;
    }
    __ui16TotalNotesIn16th += 16 / __note;
  }
  return (__ui16TotalNotesIn16th / beat::getCurrentBeat().getBaseNoteForBeat());
}

/** uint32_t const bar::ui32GetBarDuration() const
 * 
 * Returns the current bar effective duration in ms */
uint32_t const bar::ui32GetBarDuration() const {
  // Serial.println(F("bar::ui32GetBarDuration(). Starting."));
  // Serial.printf("bar::ui32GetBarDuration(). ui16GetBaseNotesCountInBar() = %u\n", ui16GetBaseNotesCountInBar());
  // Serial.printf("bar::ui32GetBarDuration(). beat::getCurrentBeat().ui16GetBaseNoteDurationInMs() = %u\n", beat::getCurrentBeat().ui16GetBaseNoteDurationInMs());
  // Serial.printf("bar::ui32GetBarDuration(). about to return the following value: %u\n", ui16GetBaseNotesCountInBar() * beat::getCurrentBeat().ui16GetBaseNoteDurationInMs());
  return (ui16GetBaseNotesCountInBar() * beat::getCurrentBeat().ui16GetBaseNoteDurationInMs());
}
