/*
  bar.cpp - bars are two beats to four beats, precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h



Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "bar.h"

uint16_t bar::_ui16ActiveBar = 0;
const uint16_t bar::_ui16_bar_count = 7;
std::array<bar, 5> bar::bars;



// pointer to functions to produce an interface for bar
void (*bar::sendCurrentBar)(const uint16_t __ui16_active_bar) = nullptr;




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
bar::bar()
{
}

// parameterized
bar::bar(
  const uint16_t __ui16_base_beat_in_bpm, 
  const uint16_t __ui16_base_note_for_beat, 
  const uint16_t __ui16_base_notes_count_in_bar, 
  const uint16_t __ui16_notes_count_in_bar,
  std::array<note, 16> __notesArray
  // std::array<std::array<uint16_t, 2>, 16> __ui16NoteTone
):
  ui16BaseBeatInBpm(__ui16_base_beat_in_bpm), 
  ui16BaseNoteForBeat(__ui16_base_note_for_beat), 
  ui16BaseNotesCountInBar(__ui16_base_notes_count_in_bar), 
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
  bars[0] = { 2, 1, 2, _ui16noteCountForThisBar, _aRelays};
  // Serial.println("bar::_initBars(). bars[0].ui16BaseBeatInBpm: ");Serial.println(bars[0].ui16BaseBeatInBpm);
  // Serial.println("bar::_initBars(). bars[0]._iLaserPinStatusAtEachBeat[0][1]");Serial.println(bars[0]._iLaserPinStatusAtEachBeat[0][1]);

  /** twins
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  _ui16noteCountForThisBar = 2;
  std::array<note, 16> _aTwins {note(5,1), note(6,1)};
  bars[1] = { 2, 1, 2, _ui16noteCountForThisBar, _aTwins};

  /** all 
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 2
   * => 2 / 1 */
  _ui16noteCountForThisBar = 2;
  std::array<note, 16> _aAll {note(15,1), note(0,1)};
  bars[2] = { 2, 1, 2, _ui16noteCountForThisBar, _aAll};

  /** swipeRight
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  _ui16noteCountForThisBar = 4;
  std::array<note, 16> _aSwipeR {note(1,1), note(1,2), note(1,3), note(1,4)};
  bars[3] = { 120, 1, 4, _ui16noteCountForThisBar, _aSwipeR};

  /** swipeLeft
   * duration of a beat in bpm: 120
   * base note: 1 (a full)
   * count of base notes per bar: 4
   * => 4 / 1 */
  _ui16noteCountForThisBar = 4;
  std::array<note, 16> _aSwipeL {note(1,4), note(1,3), note(1,2), note(1,1)};
  bars[4] = { 120, 1, 4, _ui16noteCountForThisBar, _aSwipeL};

  /** all off
   * duration of a beat in bpm: 2
   * base note: 1 (a full)
   * count of base notes per bar: 1
   * => 1 / 1 */
  _ui16noteCountForThisBar = 1;
  std::array<note, 16> _aAllOff {note(5,1), note(0,1)};
  bars[5] = { 2, 1, 1, _ui16noteCountForThisBar, _aAllOff};

  Serial.println("void bar::_initBars(). Ending.");
}






///////////////////////////////////
// Player
///////////////////////////////////





Task bar::tPlayBar(0, 1, &_tcbPlayBar, NULL/*&mns::myScheduler*/, false, &_oetcbPlayBar, &_odtcbPlayBar);



bool bar::_oetcbPlayBar(){
  // onEnable, set the number of iterations for the task to the number of notes to play
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
  tPlayBar.setIterations(bars[_ui16ActiveBar]._ui16NotesCountInBar);

  /**2. set the static time parameters in the note class to enable the calculation of note duration*/
  note::_setTimeParams(
    bars[_ui16ActiveBar].ui16BaseNoteForBeat, 
    bars[_ui16ActiveBar].ui16BaseBeatInBpm
  );

  // if (MY_DG_LASER) {
  //   Serial.println("bar::_oetcbPlayBar(). After setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bar::_oetcbPlayBar(). bars[_ui16ActiveBar]._ui16NotesCountInBar = ");Serial.println(bars[_ui16ActiveBar]._ui16NotesCountInBar);
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
  note::setActiveNoteFromNote(bars[_ui16ActiveBar]._notesArray[_ui16_iter]);

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
  // Serial.printf("bar::_tcbPlayBar(). calling bars[%i].getNoteDuration(%i)\n", _ui16ActiveBar, _ui16_iter);
  tPlayBar.setInterval(bars[_ui16ActiveBar]._notesArray[_ui16_iter].ulGetNoteDurationInMs());

  Serial.println(F("bar::_tcbPlayBar(). Ending."));
};


void bar::_odtcbPlayBar() {
  note::_setTimeParams(
    0, 
    0
  );
}


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



// Get the active bar
int16_t bar::getCurrentBar() {
  return _ui16ActiveBar;
}