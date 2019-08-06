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

short int bar::_activeBar = 0;
const short int bar::_bar_count = 7;
bar bar::bars[_bar_count];
const short int bar::_char_count_in_name = 7;




///////////////////////////////////
// Constructors
///////////////////////////////////
bar::bar() {
  ui16BaseBeatInBpm = -1;
}






///////////////////////////////////
// Initialisers
///////////////////////////////////
void bar::_initBar(const char __cName[_char_count_in_name], const uint16_t __ui16BaseBeatInBpm, const uint16_t _ui16BaseNoteForBeat, const uint16_t _ui16BaseNotesCountInBar, const uint16_t __ui16NotesCountInBar, const short int __iNoteTone[4][2]){
  // Serial.println("void bar::initBar(). Starting.");
  strcpy(_cName, __cName);  // give its name to the bar

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

  short _noteCountForThisBar = 2;
  const short int aRelays[_noteCountForThisBar][2] = {{1,7},{1,8}};
  // load values into bars[0]:
  // a. the bar's name
  // b. the beat in bpm (i.e. the tempo)
  // c. which one is the base note for each beat (full, half, quarter, etc.) (the 4 in 2/4, for instance)
  // d. how many base notes does the bar count (the 2 in 2/4, for instance)
  // e. the number of effective notes in the bar (all the full, half, etc. effectively in the bar)
  // f. the array of references to the notes (i.e. duration of each
  // note (in base note)) and the tones to be played in the bar {relDuration, tone}
  bars[0]._initBar("relays" /*a.*/, 2 /*bpm*/, 1/*base note*/, 2/*base note count*/, _noteCountForThisBar/*e.*/, aRelays/*f.*/);
  // => 2 / 1
  // Serial.println("void bar::_initBars(). bars[0].ui16BaseBeatInBpm: ");
  // Serial.println(bars[0].ui16BaseBeatInBpm);
  // Serial.println("void bar::_initBars(). bars[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(bars[0]._iLaserPinStatusAtEachBeat[0][1]);

  _noteCountForThisBar = 2;
  // => 2 / 1
  const short int aTwins[_noteCountForThisBar][2] = {{1, 5},{1, 6}};
  bars[1]._initBar("twins", 2/*bpm*/, 1/*base note*/, 2/*base note count*/, _noteCountForThisBar, aTwins);

  _noteCountForThisBar = 2;
  // => 2 / 1
  const short int aAll[_noteCountForThisBar][2] = {{1, 15},{1, 0}};
  bars[2]._initBar("all", 2/*bpm*/, 1/*base note*/, 2/*base note count*/, _noteCountForThisBar, aAll);

  _noteCountForThisBar = 4;
  // => 4 / 1
  const short int aSwipeR[_noteCountForThisBar][2] = {{1,1},{1,2},{1,3},{1,4}};
  bars[3]._initBar("swipeR", 120/*bpm*/, 1/*base note*/, 4/*base note count*/, _noteCountForThisBar, aSwipeR);

  _noteCountForThisBar = 4;
  // => 4 / 1
  const short int aSwipeL[_noteCountForThisBar][2] = {{1,4},{1,3},{1,2},{1,1}};
  bars[4]._initBar("swipeL", 120/*bpm*/, 1/*base note*/, 4/*base note count*/, _noteCountForThisBar, aSwipeL);

  _noteCountForThisBar = 1;
  // => 1 / 1
  const short int aAllOff[_noteCountForThisBar][2] = {{1,0}};
  bars[5]._initBar("all of", 2/*bpm*/, 1/*base note*/, 1/*base note count*/, _noteCountForThisBar, aAllOff);

  Serial.println("void bar::_initBars(). Ending.");
}






///////////////////////////////////
// Player
///////////////////////////////////





Task bar::tPlayBar(0, 1, &_tcbPlayBar, &myTaskScheduler, false, &_oetcbPlayBar, &_odtcbPlayBar);



bool bar::_oetcbPlayBar(){
  // onEnable, set the number of iterations for the task to the number of notes to play
  Serial.println("void bar::_oetcbPlayBar(). Starting.");

  // if (MY_DG_LASER) {
  //   Serial.println("void bar::_oetcbPlayBar(). Before setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.println("void bar::_oetcbPlayBar(). _tPlaySequence execution parameters:");
  //   Serial.print("void bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("void bar::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("void bar::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("void bar::_oetcbPlayBar(). myTaskScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(myTaskScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("void bar::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("void bar::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  tPlayBar.setIterations(bars[_activeBar].ui16NotesCountInBar);

  // if (MY_DG_LASER) {
  //   Serial.println("void bar::_oetcbPlayBar(). After setting the iterations for this bar: *!*!*!*!*!");
  //   Serial.print("void bar::_oetcbPlayBar(). tPlayBar.isEnabled() = ");Serial.println(tPlayBar.isEnabled());
  //   Serial.print("void bar::_oetcbPlayBar(). bars[_activeBar].ui16NotesCountInBar = ");Serial.println(bars[_activeBar].ui16NotesCountInBar);
  //   Serial.print("void bar::_oetcbPlayBar(). tPlayBar.getIterations() = ");Serial.println(tPlayBar.getIterations());
  //   Serial.print("void bar::_oetcbPlayBar(). tPlayBar.getInterval() = ");Serial.println(tPlayBar.getInterval());
  //   Serial.print("void bar::_oetcbPlayBar(). myTaskScheduler.timeUntilNextIteration(_tPlaySequence) = ");Serial.println(myTaskScheduler.timeUntilNextIteration(tPlayBar));
  //   Serial.print("void bar::_oetcbPlayBar(). millis() = ");Serial.println(millis());
  //   Serial.println("void bar::_oetcbPlayBar(). *!*!*!*!*!");
  // }

  Serial.println("void bar::_oetcbPlayBar(). Ending.");

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
  Serial.println("void bar::_tcbPlayBar(). Starting.");

  // 1. get the run counter
  const short _iter = tPlayBar.getRunCounter() - 1;

  // 2. Disable the Task tPlayNote (just in case)
  note::tPlayNote.disable();

  // 3. Set the relevant note in the note class (note::activeTone)
  //    using the iterator
  note::activeTone = bars[_activeBar]._note[_iter][1];

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
  tPlayBar.setInterval(bars[_activeBar].getNoteDuration(_iter));

  Serial.println("void bar::_tcbPlayBar(). Ending.");
};




void bar::_odtcbPlayBar(){
  if (MY_DG_LASER) {
    Serial.print("void bar::_odtcbPlayBar(). millis() = ");Serial.println(millis());
    Serial.println("void bar::_odtcbPlayBar(). Task tPlayBar BYE BYE");
  }
}





unsigned long bar::getNoteDuration(const short int _iter){
  Serial.println("unsigned long bar::getNoteDuration(). Starting.");
  // Serial.print("unsigned long bar::getNoteDuration(). _activeBar = ");Serial.println(_activeBar);
  // Serial.print("unsigned long bar::getNoteDuration(). bars[_activeBar].ui16BaseNoteForBeat = ");Serial.println(bars[_activeBar].ui16BaseNoteForBeat);
  // Serial.print("unsigned long bar::getNoteDuration(). _iter = ");Serial.println(_iter);
  // Serial.print("unsigned long bar::getNoteDuration(). bars[_activeBar]._note[_iter][0] = ");Serial.println(bars[_activeBar]._note[_iter][0]);
  // Serial.print("unsigned long bar::getNoteDuration(). bars[_activeBar].ui16BaseBeatInBpm = ");Serial.println(bars[_activeBar].ui16BaseBeatInBpm);
  unsigned long __ulDurationInMs = (bars[_activeBar].ui16BaseNoteForBeat / bars[_activeBar]._note[_iter][0])
                                  *(60 / bars[_activeBar].ui16BaseBeatInBpm * 1000);
  if (__ulDurationInMs > 30000) {
    __ulDurationInMs = 30000;
  }
  // Serial.print("unsigned long bar::getNoteDuration(). __ulDurationInMs = ");Serial.println(__ulDurationInMs);
  Serial.println("unsigned long bar::getNoteDuration(). Ending.");
  return __ulDurationInMs;
}





void bar::setActiveBar(const short activeBar) {
  Serial.println("void bar::setActiveBar(). Starting.");
  // if (MY_DG_LASER) {
  //   Serial.print("void bar::setActiveBar(). (before setting) _activeBar = ");Serial.println(_activeBar);
  // }
  _activeBar = activeBar;
  // if (MY_DG_LASER) {
  //   Serial.print("void bar::setActiveBar(). (after setting) _activeBar = ");Serial.println(_activeBar);
  // }
  Serial.println("void bar::setActiveBar(). Ending.");
};
