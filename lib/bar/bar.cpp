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

bar _bar;
bar &bar::_activeBar = _bar;
uint16_t bar::_ui16ActiveBar = 0;
std::array<bar, 7> bar::_bars;



// pointer to functions to produce an interface for bar
void (*bar::sendCurrentBar)(const uint16_t __ui16_active_bar) = nullptr;




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
bar::bar() :
  _ui16BaseNotesCountInBar(0), 
  _ui16NotesCountInBar(0),
  _notesArray(std::array<note, 16> {})
{
}

// parameterized
bar::bar(
  const uint16_t __ui16_base_notes_count_in_bar, 
  const uint16_t __ui16_notes_count_in_bar,
  std::array<note, 16> __notesArray
  // std::array<std::array<uint16_t, 2>, 16> __ui16NoteTone
):
  _ui16BaseNotesCountInBar(__ui16_base_notes_count_in_bar), 
  _ui16NotesCountInBar(__ui16_notes_count_in_bar),
  _notesArray(__notesArray)
{
}

// // copy constructor
// bar::bar( const bar& )
//  // _tone(tone::_tones[_ui16Tone])
// {
// }

// // assignement operator
// bar& bar::operator=(const bar& )
// {
// }





///////////////////////////////////
// Initialisers
///////////////////////////////////
void bar::initBars() {
  Serial.println("void bar::_initBars(). Starting.");
  // define an array containing references to the note type and tones to be played in the bar

  /** Signature of a bar:
   *  a. the duration of a beat in bpm (i.e. the tempo);
   *  b. the base note for each beat (full, half, quarter, etc.) (the 4 in 2/4, for instance);
   *  c. the count of base notes per bar (the 2 in 2/4, for instance);
   *  d. the number of effective notes in the bar (all the full, half, etc. effectively in the bar);
   *  e. a bidimensional array of references to the notes.
   *     1. The first dimension represents the sequence of notes: {...},{...},{...} 
   *     2. The second dimension represents each note. Each note is composed of:
   *     (i) its duration (expressed in base note - full, half, quarter, etc.) and;
   *     (ii) its tone.
   *     ex.: { 2, 1, 2, 2, {{1,7},{1,8}}}
   *     a. 2: the bar shall be played at 2 beats per minutes => the base note last 30 seconds.
   *     b. 1: the base note is a white
   *     c. 2: we have two notes per bar;
   *     d. 2: we have two effective notes in this specific bar;
   *     e. the two notes are:
   *        - {1,7} -> a white with tone 7.
   *        - {1,8} -> a white with tone 8. */


  /** relays
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  uint16_t _ui16noteCountForThisBar = 2;
  std::array<note, 16> _aRelays {note(7,1), note(8,1)};
  _bars[0] = { 2, _ui16noteCountForThisBar, _aRelays};
  // Serial.println("bar::_initBars(). _bars[0]._ui16BaseBeatInBpm: ");Serial.println(_bars[0]._ui16BaseBeatInBpm);
  // Serial.println("bar::_initBars(). _bars[0]._iLaserPinStatusAtEachBeat[0][1]");Serial.println(_bars[0]._iLaserPinStatusAtEachBeat[0][1]);

  /** twins
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  _ui16noteCountForThisBar = 2;
  std::array<note, 16> _aTwins {note(5,1), note(6,1)};
  _bars[1] = { 2, _ui16noteCountForThisBar, _aTwins};

  /** all 
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  _ui16noteCountForThisBar = 2;
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
  _ui16noteCountForThisBar = 4;
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
/**note::playNoteStandAlone:
 *  
 *  play a single note for a given duration.
 * 
 *  {@ params} uint16_t const __ui16_base_note_for_beat: pass the base note 
 *             for a given beat. ex. 4, a black
 *  {@ params} uint16_t const __ui16_base_beat_in_bpm: pass the base beat 
 *             in bpm. ex. 120 bpm (500 ms)
 *  {@ params} bar const & __target_bar: pass a target bar reference to const
 *             ex. bar(4, 8, std::array<note, 16> _aSwipeL {note(4,8), note(3,8), note(2,8), note(1,8)}, note(2,8), note(3,8), note(4,8), note(0,8))
 *             where:
 *             - 4 stands for 4 blacks per bar;
 *             - 8 stands for 8 effective notes in the bar; and
 *             - the array of notes contained in the bar.
*/
void bar::playBarStandAlone(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, bar const & __target_bar) {
// void note::playNoteStandAlone(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, note const & __target_note) {
  tPlayBar.disable();
  setActiveBarFromBar(__target_bar);
  _setTPlayBar(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm);
  tPlayBar.restartDelayed();
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
  tPlayBar.setIterations(_bars[_ui16ActiveBar]._ui16NotesCountInBar);

  // if (MY_DG_LASER) {
  //   Serial.println("bar::_oetcbPlayBar(). After setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bar::_oetcbPlayBar(). _bars[_ui16ActiveBar]._ui16NotesCountInBar = ");Serial.println(_bars[_ui16ActiveBar]._ui16NotesCountInBar);
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

  // 2. Disable the Task tPlayNote (just in case)
  note::tPlayNote.disable();

  /**3. Set the static activeNote of the note class (note::_activeNote)
   *    using the Task iterator to select the relevant note in the _notesArray
   *    of the currently activeBar. */
  note::setActiveNoteFromNote(_bars[_ui16ActiveBar]._notesArray[_ui16_iter]);

  /**4.Reenable the tPlayNote Task
   *   Upon enabling the Task, the onEnable callback of tPlayNote will be
   *   immediately executed, starting to play the note.
   * 
   *   After expiration of the default interval of 30s. (or if disabled
   *   from this callback _tcbPlayBar (at the following iteration), the
   *   tPlayNote Task will be disabled.
   * 
   *   tPlayNote onDisable callback will then turn off all the lasers, marking
   *   the passing from one note to the other. */
  note::tPlayNote.enableDelayed();

  /**5. Set the interval for next iteration
   *   
   *    At each pass, reset the interval before the next iteration 
   *    of this Task bar::tPlayBar. This marks the duration of each note. */
  // Serial.println(F("------------- DEBUG --------- BAR --------- DEBUG -------------"));
  // Serial.printf("bar::_tcbPlayBar(). calling _bars[%i].getNoteDuration(%i)\n", _ui16ActiveBar, _ui16_iter);
  tPlayBar.setInterval(_bars[_ui16ActiveBar]._notesArray[_ui16_iter].ulGetNoteDurationInMs());

  Serial.println(F("bar::_tcbPlayBar(). Ending."));
};



void bar::setActiveBar(const uint16_t __ui16_active_bar) {
  // Serial.println("bar::setActiveBar(). Starting.");
  // if (MY_DG_LASER) {
  //   Serial.print("bar::setActiveBar(). (before setting) _ui16ActiveBar = ");Serial.println(_ui16ActiveBar);
  // }
  _ui16ActiveBar = __ui16_active_bar;
  // if (MY_DG_LASER) {
  //   Serial.print("bar::setActiveBar(). (after setting) _ui16ActiveBar = ");Serial.println(_ui16ActiveBar);
  // }
  // Serial.println("bar::setActiveBar(). Ending.");
};


/** bar::setActiveBarFromBar(): public static setter method
 * 
 *  sets the parameters of the static variable &note::_activeNote 
 *  from a passed in note reference. */
void bar::setActiveBarFromBar(const bar & __target_bar) {
  _activeBar = __target_bar;
}


/** bar::_setTPlayBar(): public static setter method
 * 
 *  sets the parameters of the Task tPlayNote. */
void bar::_setTPlayBar(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm) {
  beat::setActiveBeat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm);
  tPlayBar.setOnDisable([](){
    beat::setActiveBeat(0, 0);
  });
}


// Get the active bar
int16_t bar::getCurrentBar() {
  return _ui16ActiveBar;
}