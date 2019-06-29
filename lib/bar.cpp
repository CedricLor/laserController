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
}






// Initialisers
void bar::_initBar(const char cName[_char_count_in_name], const unsigned long ulTempo, const short int iNumberOfBeatsInBar, const short int iLaserPinStatusAtEachBeat[4]){
  // Serial.println("void bar::initBar(). Starting.");
  strcpy(_cName, cName);
  _ulTempo = ulTempo;
  _iNumberOfBeatsInBar = iNumberOfBeatsInBar;
  for (short __thisBeat = 0; __thisBeat < iNumberOfBeatsInBar; __thisBeat++) {
    _iLaserPinStatusAtEachBeat[__thisBeat] = iLaserPinStatusAtEachBeat[__thisBeat];
  }
  // Serial.println("void bar::initBar(). Ending.");
};

void bar::initBars() {
  Serial.println("void bar::_initBars(). Starting.");
  // define an array containing references to the tones to be played in the bar
  const short int aRelays[2] = {7,8};
  // load values into bars[0]:
  // a. the bar's name
  // b. the duration of each beats (i.e. the tempo)
  // c. the number of beats in the bar
  // d. the array of references to the tones to be played in the bar
  bars[0]._initBar("relays", 30000, 2, aRelays);
  // Serial.println("void bar::_initBars(). bars[0]._ulTempo: ");
  // Serial.println(bars[0]._ulTempo);
  // Serial.println("void bar::_initBars(). bars[0]._iLaserPinStatusAtEachBeat[0][1]");
  // Serial.println(bars[0]._iLaserPinStatusAtEachBeat[0][1]);
  const short int aTwins[2] = {5,6};
  bars[1]._initBar("twins", 30000, 2, aTwins);
  const short int aAll[2] = {15,0};
  bars[2]._initBar("all", 30000, 2, aAll);
  const short int aSwipeR[4] = {1,2,3,4};
  bars[3]._initBar("swipeR", 500, 4, aSwipeR);
  const short int aSwipeL[4] = {4,3,2,1};
  bars[4]._initBar("swipeL", 500, 4, aSwipeL);
  const short int aAllOff[1] = {0};
  bars[5]._initBar("all of", 0, 1, aAllOff);
  Serial.println("void bar::_initBars(). Ending.");
}






// Loop Player
// tPlayBarInLoop plays a bar in loop, for an unlimited number of iterations,
// until it is disabled.
// tPlayBarInLoop is enabled and disabled by the stateBox class.
// Upon entering a new stateBox (startup, IR signal received, etc.),
// the stateBox tasks sets bar::_activeBar to the bar index number
// associated with this stateBox. Then the tPlayBarInLoop is enabled, until being disabled by the boxState class
Task bar::tPlayBarInLoop(0, TASK_FOREVER, &_tcbPlayBarInLoop, &userScheduler, false, &_oetcbPlayBarInLoop, &_odtcbPlayBarInLoop);

// Upon enabling the tPlayBarInLoop task, the _activeBar is played a
// first time and the _duration of the bar is calculated in order to
// set the interval between each iterations of the tPlayBarInLoop task
bool bar::_oetcbPlayBarInLoop() {
  Serial.println("----- bool bar::_oetcbPlayBarInLoop(). Starting.");
  // Serial.print("----- bool bar::_oetcbPlayBarInLoop(). _activeBar: ");Serial.println(_activeBar);

  // Start immediately playing the bar on enable
  _playBar();

  // Calculate the interval at which each iteration occur, by multiplying the tempo of the bar by the number of beats in the bar
  unsigned long _duration = bars[_activeBar]._ulTempo * bars[_activeBar]._iNumberOfBeatsInBar;
  // Serial.print("----- bool bar::_oetcbPlayBarInLoop(). _duration: ");Serial.println(_duration);

  // Set the interval at which each iteration occur.
  // Before doing, check if _duration == 0.
  // If _duration == 0, this actually means infinite,
  // which means that we are trying to play the bar "all lasers off"
  // Instead of starting an infinite loop, turning all lasers off,
  // let's just disable tPlayBarInLoop
  if (_duration == 0) {
    Serial.println("----- bool bar::_oetcbPlayBarInLoop(). Ending on _duration == 0");
    return false;
  }
  tPlayBarInLoop.setInterval(_duration);
  // Serial.print("----- bool bar::_oetcbPlayBarInLoop(). tPlayBarInLoop.getInterval() = ");Serial.println(tPlayBarInLoop.getInterval());
  Serial.println("----- bool bar::_oetcbPlayBarInLoop(). Ending.");
  return true;
}

void bar::_tcbPlayBarInLoop() {
  Serial.println("----- void bar::_tcbPlayBarInLoop(). Starting.");
  _playBar();
  Serial.println("----- void bar::_tcbPlayBarInLoop(). Ending.");
}

// on disable tPlayBarInLoop, turn off all the laser by setting the activeBar
// to state 5 ("all off"), then playBar 5.
void bar::_odtcbPlayBarInLoop() {
  Serial.println("----- void bar::_odtcbPlayBarInLoop(). Starting.");
  setActiveBar(5); // all lasers off
  _playBar();
  Serial.println("----- void bar::_odtcbPlayBarInLoop(). Ending.");
};




// Single bar player
// Plays a given bar one single time.
// It is called by the Task tPlayBarInLoop
// 1. sets the interval of the _tPlayBar task from the tempo of the relevant bar
// this tempo corresponds to the duration for which each tone will be played
// 2. sets the number of iterations of the _tPlayBar task from the number of
// beats (i.e. tones) in the bar
// 3. enables the _tPlayBar task
void bar::_playBar(){
  Serial.println("----- void bar::_playBar(). Starting");
  // Serial.print("----- void bar::_playBar(). _activeBar: ");Serial.println(_activeBar);
  _tPlayBar.setInterval(bars[_activeBar]._ulTempo);
  // Serial.print("----- void bar::_playBar(). Tempo: ");Serial.println(bars[barNumber]._ulTempo);
  _tPlayBar.setIterations(bars[_activeBar]._iNumberOfBeatsInBar);
  // Serial.print("----- void bar::_playBar(). Beats: ");Serial.println(bars[barNumber]._iNumberOfBeatsInBar);
  _tPlayBar.enable();
  // Serial.println("----- void bar::_playBar(). Task _tPlayBar enabled");
  Serial.println("----- void bar::_playBar(). Ending");
};

Task bar::_tPlayBar(0, 0, &_tcbPlayBar, &userScheduler, false);

void bar::_tcbPlayBar(){
  // Serial.println("----- void bar::_tcbPlayBar(). Starting.");
  short _iter = _tPlayBar.getRunCounter() - 1;
  // Serial.print("----- void bar::_tcbPlayBar(). _iter: ");Serial.println(_iter);
  // Look for the tone number to read at this tempo
  short int _activeTone = bars[_activeBar]._iLaserPinStatusAtEachBeat[_iter];
  // Play tone
  tone::tones[_activeTone].playTone();
  // Serial.println("----- void bar::_tcbPlayBar(). Ending.");
};

void bar::setActiveBar(const short activeBar) {
  // Serial.println("void bar::setActiveBar(). Starting.");
  _activeBar = activeBar;
  // Serial.println("void bar::setActiveBar(). Ending.");
};
