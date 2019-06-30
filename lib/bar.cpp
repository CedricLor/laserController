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
  |  |  |  |--tone.cpp (called to play some member functions)
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





// Constructors
bar::bar() {
  ulBaseBeatInMs = -1;
}






// Initialisers
void bar::_initBar(const char __cName[_char_count_in_name], const unsigned long __ulBaseBeatInMs, const short __iBaseNoteForBeat, const short int __iNotesCountInBar, const short int __iNoteTone[4][2]){
  // Serial.println("void bar::initBar(). Starting.");
  strcpy(_cName, __cName);
  ulBaseBeatInMs = __ulBaseBeatInMs;
  _iBaseNoteForBeat = __iBaseNoteForBeat;
  iNotesCountInBar = __iNotesCountInBar;
  for (short __thisNote = 0; __thisNote < __iNotesCountInBar; __thisNote++) {
    _note[__thisNote][0] = __iNoteTone[__thisNote][0]; // the note duration
    _note[__thisNote][1] = __iNoteTone[__thisNote][1]; // the tone
  }
  // Serial.println("void bar::initBar(). Ending.");
};

void bar::initBars() {
  Serial.println("------ void bar::_initBars(). Starting.");
  // define an array containing references to the note type and tones to be played in the bar

  short _noteCountForThisBar = 2;
  const short int aRelays[_noteCountForThisBar][2] = {{1,7},{1,8}};
  // load values into bars[0]:
  // a. the bar's name
  // b. the duration of each beats in ms (i.e. the tempo)
  // c. the base note for each beat (full, half, quarter, etc.)
  // d. the number of note in the bar
  // e. the array of references to the tones to be played in the bar
  bars[0]._initBar("relays", 30000, 1, _noteCountForThisBar, aRelays);
  // Serial.println("void bar::_initBars(). bars[0].ulBaseBeatInMs: ");
  // Serial.println(bars[0].ulBaseBeatInMs);
  // Serial.println("void bar::_initBars(). bars[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(bars[0]._iLaserPinStatusAtEachBeat[0][1]);

  _noteCountForThisBar = 2;
  const short int aTwins[_noteCountForThisBar][2] = {{1, 5},{1, 6}};
  bars[1]._initBar("twins", 30000, 1, _noteCountForThisBar, aTwins);

  _noteCountForThisBar = 2;
  const short int aAll[_noteCountForThisBar][2] = {{1, 15},{1, 0}};
  bars[2]._initBar("all", 30000, 1, _noteCountForThisBar, aAll);

  _noteCountForThisBar = 4;
  const short int aSwipeR[_noteCountForThisBar][2] = {{1,1},{1,2},{1,3},{1,4}};
  bars[3]._initBar("swipeR", 500, 1, _noteCountForThisBar, aSwipeR);

  _noteCountForThisBar = 4;
  const short int aSwipeL[_noteCountForThisBar][2] = {{1,4},{1,3},{1,2},{1,1}};
  bars[4]._initBar("swipeL", 500, 1, _noteCountForThisBar, aSwipeL);

  _noteCountForThisBar = 1;
  const short int aAllOff[_noteCountForThisBar][2] = {{1,0}};
  bars[5]._initBar("all of", 30000, 1, _noteCountForThisBar, aAllOff);

  Serial.println("------ void bar::_initBars(). Ending.");
}






// // Loop Player
// // tPlayBars plays a bar in loop, for an unlimited number of iterations,
// // until it is disabled.
// // tPlayBars is enabled and disabled by the stateBox class.
// // Upon entering a new stateBox (startup, IR signal received, etc.),
// // the stateBox tasks sets bar::_activeBar to the bar index number
// // associated with this stateBox. Then the tPlayBars is enabled, until being disabled by the boxState class
// Task bar::tPlayBars(0, TASK_FOREVER, &_tcbPlayBars, &userScheduler, false, &_oetcbPlayBars, &_odtcbPlayBars);
//
// // Upon enabling the tPlayBars task, the _activeBar is played a
// // first time and the _duration of the bar is calculated in order to
// // set the interval between each iterations of the tPlayBars task
// bool bar::_oetcbPlayBars() {
//   Serial.println("------ bool bar::_oetcbPlayBars(). Starting.");
//   // Serial.print("------ bool bar::_oetcbPlayBars(). _activeBar: ");Serial.println(_activeBar);
//
//   // Start immediately playing the bar on enable
//   bars[_activeBar]._playBar(_activeBar);
//
//   // Calculate the interval at which each iteration occur, by multiplying the tempo of the bar by the number of beats in the bar
//   unsigned long _duration = bars[_activeBar].ulBaseBeatInMs * bars[_activeBar].iNotesCountInBar;
//   // Serial.print("------ bool bar::_oetcbPlayBars(). _duration: ");Serial.println(_duration);
//
//   // Set the interval at which each iteration occur.
//   tPlayBars.setInterval(_duration);
//   // Serial.print("------ bool bar::_oetcbPlayBars(). tPlayBars.getInterval() = ");Serial.println(tPlayBars.getInterval());
//   Serial.println("------ bool bar::_oetcbPlayBars(). Ending.");
//   return true;
// }
//
// void bar::_tcbPlayBars() {
//   Serial.println("------ void bar::_tcbPlayBars(). Starting.");
//   bars[_activeBar]._playBar(_activeBar);
//   Serial.println("------ void bar::_tcbPlayBars(). Ending.");
// }
//
// // on disable tPlayBars, turn off all the laser by setting the activeBar
// // to state 5 ("all off"), then playBar 5.
// void bar::_odtcbPlayBars() {
//   Serial.println("------ void bar::_odtcbPlayBars(). Starting.");
//   bars[_activeBar]._playBar(5); // all lasers off
//   Serial.println("------ void bar::_odtcbPlayBars(). Ending.");
// };




// Single bar player
// Plays a given bar one single time.
// It is called by the Task tPlayBars
void bar::playBar(const short activeBar){
  Serial.println("------ void bar::_playBar(). Starting");
  // Serial.print("------ void bar::_playBar(). _activeBar: ");Serial.println(_activeBar);

  // set the active bar for the callbacks of _tPlayBar Task
  setActiveBar(activeBar);

  Serial.println("------ void bar::_playBar(). Just before enabling _tPlayBar");
  Serial.print("------ void bar::_playBar(). _tPlayBar.isEnabled() = ");Serial.println(_tPlayBar.isEnabled());
  Serial.print("------ void bar::_playBar(). _tPlayBar.getIterations() = ");Serial.println(_tPlayBar.getIterations());
  Serial.print("------ void bar::_playBar(). _tPlayBar.getInterval() = ");Serial.println(_tPlayBar.getInterval());

  // Enable the _tPlayBar Task
  _tPlayBar.enable();
  // Serial.println("------ void bar::_playBar(). Task _tPlayBar enabled");

  Serial.println("------ void bar::_playBar(). Ending");
};






Task bar::_tPlayBar(0, 1, &_tcbPlayBar, &userScheduler, false, &_oetcbPlayBar, &_odtcbPlayBar);

bool bar::_oetcbPlayBar(){
  // onEnable, set the number of iterations for the task to the number of notes to play
  Serial.println("------ void bar::_oetcbPlayBar(). Starting.");
  _tPlayBar.setIterations(bars[_activeBar].iNotesCountInBar);
  Serial.print("------ void bar::_oetcbPlayBar(). bars[_activeBar].iNotesCountInBar = ");Serial.println(bars[_activeBar].iNotesCountInBar);
  Serial.print("------ void bar::_oetcbPlayBar(). _tPlayBar.getIterations() = ");Serial.println(_tPlayBar.getIterations());
  Serial.print("------ void bar::_oetcbPlayBar(). _tPlayBar.getInterval() = ");Serial.println(_tPlayBar.getInterval());
  Serial.println("------ void bar::_oetcbPlayBar(). Ending.");
  return true;
}

void bar::_tcbPlayBar(){
  Serial.println("------ void bar::_tcbPlayBar(). Starting.");
  Serial.print("------ void bar::_tcbPlayBar(). _tPlayBar.getIterations() = ");Serial.println(_tPlayBar.getIterations());
  Serial.print("------ void bar::_tcbPlayBar(). _tPlayBar.getInterval() = ");Serial.println(_tPlayBar.getInterval());
  // Each pass corresponds to a note in the notes array of the currently active bar:
  // bar[_activeBar]._note[ITERATION - 1][]
  short _iter = _tPlayBar.getRunCounter() - 1;
  Serial.print("------ void bar::_tcbPlayBar(). _iter: ");Serial.println(_iter);

  // Indicate to the note class which will be the tone to be played
  Serial.print("------ void bar::_tcbPlayBar(). bars[_activeBar]._note[_iter][1] = ");Serial.println(bars[_activeBar]._note[_iter][1]);
  note::activeTone = bars[_activeBar]._note[_iter][1];
  Serial.print("------ void bar::_tcbPlayBar(). note::activeTone = ");Serial.println(note::activeTone);

  // // Disable the Task (we are about to play a new note; if tPlayNote was currently playing a note, it shall stop immediately)
  note::tPlayNote.disable();

  // Enable the Task
  /*
     On enabling the Task, the onEnable callback will be immediately executed,
     starting to play the bar.
     After expiration of the interval or if disabled from the sequence,
     the Task will automatically be disabled.
     Its onDisable callback will just turn off all the lasers.
  */
  note::tPlayNote.enableDelayed();

  // _tPlayBar
  // At each pass, reset the interval before the next iteration of the Task bar::_tPlayBar
  unsigned long __ulDurationInMs = bars[_activeBar].getIntervalForEachNote(_iter);
  Serial.print("------ void bar::_tcbPlayBar(). __ulDurationInMs = ");Serial.println(__ulDurationInMs);
  _tPlayBar.setInterval(__ulDurationInMs);
  Serial.print("------ void bar::_tcbPlayBar(). _tPlayBar.getInterval() = ");Serial.println(_tPlayBar.getInterval());

  Serial.println("------ void bar::_tcbPlayBar(). Ending.");
};

void bar::_odtcbPlayBar(){
  Serial.println("------ void bar::_odtcbPlayBar(). Starting. -----");
  Serial.println("------ void bar::_odtcbPlayBar(). Ending. -----");
}

unsigned long bar::getIntervalForEachNote(const short int _iter){
  Serial.println("------ unsigned long bar::getIntervalForEachNote(). Starting.");
  unsigned long __ulDurationInMs = bars[_activeBar]._iBaseNoteForBeat /
    bars[_activeBar]._note[_iter][0] *
    bars[_activeBar].ulBaseBeatInMs;
  Serial.print("------ unsigned long bar::getIntervalForEachNote(). __ulDurationInMs = ");Serial.println(__ulDurationInMs);
  Serial.println("------ unsigned long bar::getIntervalForEachNote(). Ending.");
  return __ulDurationInMs;
}



void bar::setActiveBar(const short activeBar) {
  Serial.println("------ void bar::setActiveBar(). Starting.");
  Serial.print("------ void bar::setActiveBar(). (before setting) _activeBar = ");Serial.println(_activeBar);
  _activeBar = activeBar;
  Serial.print("------ void bar::setActiveBar(). (after setting) _activeBar = ");Serial.println(_activeBar);
  Serial.println("------ void bar::setActiveBar(). Ending.");
};
