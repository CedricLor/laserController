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
bar bar::bars[_ui16_bar_count];



// pointer to functions to produce an interface for boxState
void (*bar::sendCurrentBar)(const int16_t __ui16_active_bar) = nullptr;




///////////////////////////////////
// Constructors
///////////////////////////////////
bar::bar() {
  ui16BaseBeatInBpm = -1;
}






///////////////////////////////////
// Initialisers
///////////////////////////////////
void bar::_initBar(const uint16_t __ui16BaseBeatInBpm, const uint16_t _ui16BaseNoteForBeat, const uint16_t _ui16BaseNotesCountInBar, const uint16_t __ui16NotesCountInBar, const short int __iNoteTone[4][2]){
  // Serial.println("void bar::initBar(). Starting.");

  ui16BaseBeatInBpm = __ui16BaseBeatInBpm; // give its base beat to the bar (1000ms would be 60 beats per minutes)
  ui16BaseNoteForBeat = _ui16BaseNoteForBeat; // give its base note to the bar (the 4 in 2/4, for instance)
  ui16BaseNotesCountInBar = _ui16BaseNotesCountInBar; // give the count of base notes in the bar (the 2 in 2/4, for instance)

  ui16NotesCountInBar = __ui16NotesCountInBar;
  // fill the bar with the actual notes
  for (short __thisNote = 0; __thisNote < ui16NotesCountInBar; __thisNote++) {
    _note[__thisNote][0] = __iNoteTone[__thisNote][0]; // the note duration (in base note)
    _note[__thisNote][1] = __iNoteTone[__thisNote][1]; // the tone (the height of the note)
  }
  // Serial.println("void bar::initBar(). Ending.");
};


void bar::initBars() {
  Serial.println("void bar::_initBars(). Starting.");
  // define an array containing references to the note type and tones to be played in the bar

  /** Signature of a bar:
   *  a. the beat in bpm (i.e. the tempo)
   *  b. which one is the base note for each beat (full, half, quarter, etc.) (the 4 in 2/4, for instance)
   *  c. how many base notes does the bar count (the 2 in 2/4, for instance)
   *  d. the number of effective notes in the bar (all the full, half, etc. effectively in the bar)
   *  e. the array of references to the notes taking the form of a bidimensional array.
   *     Each item in the first dimension represent a note. Each note is composed 
   *     of (i) the duration of each note (expressed in base note - full, half, quarter, etc.)
   *     and (ii) its tone. Each note takes the form of a two-item array (second dimension of
   *     the bidimensional array): {duration, tone}. */


  /** relays
   * => 2 / 1 */
  uint16_t _ui16noteCountForThisBar = 2;
  const short int aRelays[_ui16noteCountForThisBar][2] = {{1,7},{1,8}};
  bars[0]._initBar(2 /*bpm*/, 1/*base note*/, 2/*base note count*/, _ui16noteCountForThisBar/*e.*/, aRelays/*f.*/);
  // => 2 / 1
  // Serial.println("bar::_initBars(). bars[0].ui16BaseBeatInBpm: ");Serial.println(bars[0].ui16BaseBeatInBpm);
  // Serial.println("bar::_initBars(). bars[0]._iLaserPinStatusAtEachBeat[0][1]");Serial.println(bars[0]._iLaserPinStatusAtEachBeat[0][1]);

  /** twins
   * => 2 / 1 */
  _ui16noteCountForThisBar = 2;
  const short int aTwins[_ui16noteCountForThisBar][2] = {{1, 5},{1, 6}};
  bars[1]._initBar(2/*bpm*/, 1/*base note*/, 2/*base note count*/, _ui16noteCountForThisBar, aTwins);

  /** all 
   * => 2 / 1 */
  _ui16noteCountForThisBar = 2;
  const short int aAll[_ui16noteCountForThisBar][2] = {{1, 15},{1, 0}};
  bars[2]._initBar(2/*bpm*/, 1/*base note*/, 2/*base note count*/, _ui16noteCountForThisBar, aAll);

  /** swipeRight
   * => 4 / 1 */
  _ui16noteCountForThisBar = 4;
  const short int aSwipeR[_ui16noteCountForThisBar][2] = {{1,1},{1,2},{1,3},{1,4}};
  bars[3]._initBar(120/*bpm*/, 1/*base note*/, 4/*base note count*/, _ui16noteCountForThisBar, aSwipeR);

  /** swipeLeft
   * => 4 / 1 */
  _ui16noteCountForThisBar = 4;
  const short int aSwipeL[_ui16noteCountForThisBar][2] = {{1,4},{1,3},{1,2},{1,1}};
  bars[4]._initBar(120/*bpm*/, 1/*base note*/, 4/*base note count*/, _ui16noteCountForThisBar, aSwipeL);

  /** all off
   * => 1 / 1 */
  _ui16noteCountForThisBar = 1;
  const short int aAllOff[_ui16noteCountForThisBar][2] = {{1,0}};
  bars[5]._initBar(2/*bpm*/, 1/*base note*/, 1/*base note count*/, _ui16noteCountForThisBar, aAllOff);

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

  tPlayBar.setIterations(bars[_ui16ActiveBar].ui16NotesCountInBar);

  // if (MY_DG_LASER) {
  //   Serial.println("bar::_oetcbPlayBar(). After setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("bar::_oetcbPlayBar(). bars[_ui16ActiveBar].ui16NotesCountInBar = ");Serial.println(bars[_ui16ActiveBar].ui16NotesCountInBar);
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("bar::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("bar::_oetcbPlayBar(). userScheduler.timeUntilNextIteration(tPlaySequence) = ");Serial.println(userScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("bar::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("bar::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  // Serial.println("bar::_oetcbPlayBar(). Ending.");

  return true;
}




// Task tPlayBar main callback
// Each pass corresponds to a note in the notes array property of the
// currently active bar.
// We leverage that to reset the interval of tPlayBar to the duration
// of the note.
// - At this iteration of tPlayBar, _tcbPlayBar enables tPlayNote (for a
//  default duration of 30s. -> in the definition of tPlayNote, in class note.)
// - At the next iteration, which will occur after the interval corresponding
// to duration of the current note, tPlayBar will disable tPlayNote.
void bar::_tcbPlayBar(){
  Serial.println("bar::_tcbPlayBar(). Starting.");

  // 1. get the run counter
  const uint16_t _ui16_iter = tPlayBar.getRunCounter() - 1;

  // 2. Disable the Task tPlayNote (just in case)
  note::tPlayNote.disable();

  // 3. Set the relevant note in the note class (note::activeTone)
  //    using the iterator
  note::activeTone = bars[_ui16ActiveBar]._note[_ui16_iter][1];

  // 4. Reenable the tPlayNote Task
  /*
     On enabling the Task, the onEnable callback of tPlayNote will be
     immediately executed, starting to play the note.
     After expiration of the default interval of 30s. or if disabled
     from this callback _tcbPlayBar (at the next iteration), the
     tPlayNote Task will be disabled.
     tPlayNote onDisable callback will just turn off all the lasers, marking
     the passing from one note to the other.
  */
  note::tPlayNote.enableDelayed();

  // 5. Set the interval for next iteration
  // At each pass, reset the interval before the next iteration of the Task bar::tPlayBar
  // Serial.println(F("------------- DEBUG --------- BAR --------- DEBUG -------------"));
  // Serial.printf("bar::_tcbPlayBar(). calling bars[%i].getNoteDuration(%i)\n", _ui16ActiveBar, _ui16_iter);
  tPlayBar.setInterval(bars[_ui16ActiveBar].getNoteDuration(_ui16_iter));

  Serial.println(F("bar::_tcbPlayBar(). Ending."));
};




void bar::_odtcbPlayBar(){
  // if (MY_DG_LASER) {
  //   Serial.print("bar::_odtcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println(F("bar::_odtcbPlayBar(). Task tPlayBar BYE BYE"));
  // }
}





unsigned long bar::getNoteDuration(const uint16_t _ui16_iter){
  // Serial.println("bar::getNoteDuration(). Starting.");
  // Serial.print("bar::getNoteDuration(). _ui16ActiveBar = ");Serial.println(_ui16ActiveBar);
  // Serial.print("bar::getNoteDuration(). bars[_ui16ActiveBar].ui16BaseNoteForBeat = ");Serial.println(bars[_ui16ActiveBar].ui16BaseNoteForBeat);
  // Serial.print("bar::getNoteDuration(). _ui16_iter = ");Serial.println(_ui16_iter);
  // Serial.println(F("------------- DEBUG --------- BAR --------- DEBUG -------------"));
  // Serial.println("bar::getNoteDuration(). bars[" + String(_ui16ActiveBar) + "]._note[" + String(_ui16_iter) + "][0] = "+ String(bars[_ui16ActiveBar]._note[_ui16_iter][0]));
  // Serial.println("bar::getNoteDuration(). bars["+ String(_ui16ActiveBar) + "].ui16BaseBeatInBpm = "+ String(bars[_ui16ActiveBar].ui16BaseBeatInBpm));
  unsigned long __ulDurationInMs = (bars[_ui16ActiveBar].ui16BaseNoteForBeat / bars[_ui16ActiveBar]._note[_ui16_iter][0])
                                  *(60 / bars[_ui16ActiveBar].ui16BaseBeatInBpm * 1000);
  if (__ulDurationInMs > 30000) {
    __ulDurationInMs = 30000;
  }
  // Serial.print("unsigned long bar::getNoteDuration(). __ulDurationInMs = ");Serial.println(__ulDurationInMs);
  // Serial.println("unsigned long bar::getNoteDuration(). Ending.");
  return __ulDurationInMs;
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



// Get the active sequence
int16_t bar::getCurrentBar() {
  return _ui16ActiveBar;
}