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
uint16_t bar::_ui16ActiveBar = 0;
std::array<bar, 7> bar::_bars;
std::array<note, 16> bar::_emptyNotesArray;


// pointer to functions to produce an interface for bar
void (*bar::sendCurrentBar)(const uint16_t __ui16_active_bar) = nullptr;




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
bar::bar() :
  _ui16BaseNotesCountInBar(0), 
  _ui16NotesCountInBar(0),
  _notesArray(_emptyNotesArray)
{
}

// parameterized
bar::bar(
  const uint16_t __ui16_base_notes_count_in_bar, 
  const uint16_t __ui16_notes_count_in_bar,
  std::array<note, 16> & __notesArray
):
  _ui16BaseNotesCountInBar(__ui16_base_notes_count_in_bar), 
  _ui16NotesCountInBar(__ui16_notes_count_in_bar),
  _notesArray(__notesArray)
{
}

// copy constructor
bar::bar(const bar& __bar):
  _ui16BaseNotesCountInBar(__bar._ui16BaseNotesCountInBar), 
  _ui16NotesCountInBar(__bar._ui16NotesCountInBar),
  _notesArray(__bar._notesArray)
{
}

// assignement operator
bar& bar::operator=(const bar& __bar)
{
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- Before assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._ui16BaseNotesCountInBar = %u\n", __bar._ui16BaseNotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._ui16NotesCountInBar = %u\n", __bar._ui16NotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._notesArray[0].getTone() = %u\n", __bar._notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). __bar._notesArray[0].getNote() = %u\n", __bar._notesArray[0].getNote());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0]._ui16BaseNotesCountInBar = %u\n", _bars[0]._ui16BaseNotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0]._ui16NotesCountInBar = %u\n", _bars[0]._ui16NotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0]._notesArray[0].getTone() = %u\n", _bars[0]._notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _bars[0]._notesArray[0].getNote() = %u\n", _bars[0]._notesArray[0].getNote());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _ui16BaseNotesCountInBar = %u\n", _ui16BaseNotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). _ui16NotesCountInBar = %u\n", _ui16NotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getTone() = %u\n", _notesArray[0].getTone());
  // Serial.printf("bar& bar::operator=(const bar& __bar). _notesArray[0].getNote() = %u\n", _notesArray[0].getNote());
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- End before assignment ---------------");
  _ui16BaseNotesCountInBar = __bar._ui16BaseNotesCountInBar;
  _ui16NotesCountInBar = __bar._ui16NotesCountInBar;
  _notesArray = __bar._notesArray;
  // Serial.println("bar& bar::operator=(const bar& __bar). ----------------------- After assignment ---------------");
  // Serial.printf("bar& bar::operator=(const bar& __bar). _ui16BaseNotesCountInBar = %u\n", _ui16BaseNotesCountInBar);
  // Serial.printf("bar& bar::operator=(const bar& __bar). _ui16NotesCountInBar = %u\n", _ui16NotesCountInBar);
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
   *  a. _ui16BaseNotesCountInBar: the count of base notes per bar (the 2 in 2/4, 
   *     for instance);
   *     TODO: this parameter is never used. It is only kept because it would be usefull
   *     to have it when incorporating bars of a given time length (n1 base notes) into
   *     sequence requiring bars of another time length (n2 base notes).
   *     ex. sequence 
   *  b. _ui16NotesCountInBar: the number of effective notes in the bar (all the full,
   *      half, etc. effectively in the bar);
   *  c. std::array<note, 16> & _notesArray: a reference to an array of notes. Each note
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
  uint16_t _ui16noteCountForThisBar = 2;
  std::array<note, 16> _aRelays {note(7,1), note(8,1)};
  _bars[0] = { 2, _ui16noteCountForThisBar, _aRelays};  
  // Serial.printf("bar::_initBars(). _bars[0]._ui16BaseNotesCountInBar == 2 ? %i\n", _bars[0]._ui16BaseNotesCountInBar == 2);
  // Serial.printf("bar::_initBars(). _bars[0]._ui16NotesCountInBar == _ui16noteCountForThisBar ? %i\n", _bars[0]._ui16NotesCountInBar == _ui16noteCountForThisBar);
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
  _bars[1] = { 2, _ui16noteCountForThisBar, _aTwins};

  /** all 
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  std::array<note, 16> _aAll {note(15,1), note(0,1)};
  _bars[2] = { 2, _ui16noteCountForThisBar, _aAll};

  /** swipeRight
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  _ui16noteCountForThisBar = 4;
  std::array<note, 16> _aSwipeR {note(1,1), note(2,1), note(2,1), note(4,1)};
  _bars[3] = { 4, _ui16noteCountForThisBar, _aSwipeR};

  /** swipeLeft
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  std::array<note, 16> _aSwipeL {note(4,1), note(3,1), note(2,1), note(1,1)};
  _bars[4] = { 4, _ui16noteCountForThisBar, _aSwipeL};

  /** all off
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 1
   * => 1 / 1 */
  _ui16noteCountForThisBar = 1;
  std::array<note, 16> _aAllOff {note(5,1), note(0,1)};
  _bars[5] = { 1, _ui16noteCountForThisBar, _aAllOff};

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
  this->setActive();
  beat(__beat).setActive();
  tPlayBar.setOnDisable([](){
    beat(0, 0).setActive();
    tPlayBar.setOnDisable(NULL);
  });
  tPlayBar.restart();
}


/** bar::playBarInSequence():
 *  
 *  play a single bar calculating the durations
 *  on the basis of the beat set by tPlaySequence. */
void bar::playBarInSequence() {
  this->setActive();
  // TODO: in sequence, set the active beat
  // TODO: in sequence, when starting 
  //        tPlaySequence (or tPlaySequenceInLoop) 
  //        reset the onDisable callback to NULL
  // tPlayBar.setOnDisable([](){
  //   beat(0, 0).setActive();
  //   tPlayBar.setOnDisable(NULL);
  // });

  tPlayBar.restart();
}


/** bar::playBar():
 *  
 *  play a single bar for a given duration 
 * (calculated using the passed-in beat).
 * 
 *  {@ params} beat const & __beat: pass a beat to be taken into account
 *             to calculate the notes duration */
void bar::playBar(beat const & __beat) {
  this->setActive();
  _activeBar._setTPlayBar(beat(__beat));
  tPlayBar.enable();
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
  tPlayBar.setIterations(_activeBar._ui16NotesCountInBar);

  /**2. tPlayNote will be managed from tPlayBar => set tPlayNote to play each note (in the bar)
   *    for one single iteration and with the maximum available interval (30000).
   */
  //    TODO: no need to call this repetitively if the bar is played within a sequence. Adopt the same 
  //          approach as in playNoteStandAlone and playNoteInBar
  note::tPlayNote.setInterval(30000);
  note::tPlayNote.setOnDisable(note::_odtcbPlayNote);

  // if (MY_DG_LASER) {
  //   Serial.println("bar::_oetcbPlayBar(). After setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bar::_oetcbPlayBar(). _activeBar._ui16NotesCountInBar = ");Serial.println(_activeBar._ui16NotesCountInBar);
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
void bar::setActive() {
  tPlayBar.disable();
  _activeBar = *this;
}



/** bar::_setTPlayBar(): public static setter method
 * 
 *  sets the parameters of the Task tPlayNote. */
void bar::_setTPlayBar(const beat & __beat) {
  beat(__beat).setActive();
  tPlayBar.setOnDisable([](){
    beat(0, 0).setActive();
    tPlayBar.setOnDisable(NULL);
  });
}




///////////////////////////////////
// Getters
///////////////////////////////////
/** int16_t const bar::i16GetCurrentBarId()
 * 
 * Returns the active bar index number */
int16_t const bar::i16GetCurrentBarId() {
  return _ui16ActiveBar;
}

/** bar & bar::getCurrentBar()
 * 
 * Returns the active bar as a bar */
bar & bar::getCurrentBar() {
  return _activeBar;
}

/** bar const & bar::getBarFromBarArray(const uint16_t __ui16_bar_index_number) 
 * 
 * Returns a const ref to one of the hard coded bars given its index number */
bar & bar::getBarFromBarArray(const uint16_t __ui16_bar_index_number) {
  return _bars[__ui16_bar_index_number];
}
