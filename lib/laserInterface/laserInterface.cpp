/*
  laserInterface.cpp - Library to handle communications between laser stack and mesh
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include <laserInterface.h>

using namespace laserInterface;


beat laserInterface::activeBeat;
sequences laserInterface::globalSequences(sequenceNS::sendCurrent);

void laserInterface::init() {
  beatNS::init();
  mns::myScheduler.addTask(globalSequences.tPreloadNextSequence);
  mns::myScheduler.addTask(globalSequences.tPlaySequence);
  mns::myScheduler.addTask(globalSequences._bars.tPreloadNextBar);
  mns::myScheduler.addTask(globalSequences._bars.tPlayBar);
  mns::myScheduler.addTask(globalSequences._bars.getNotes().tPlayNote);
  globalSequences.sendCurrentSequence = sequenceNS::sendCurrent;
}





/*******************/
// laserScheduler
/*******************/
laserInterface::laserScheduler::laserScheduler(_Mode __mode)
  // _activeBeat(activeBeat),
  // _sequences(globalSequences)
  // _activeSequence(sequence::globalSequence)
{ }




/*******************/
// boxStates stack
/*******************/
void laserInterface::boxStateNS::initComm() {
    // boxState::sendCurrentBoxState = sendCurrent;
}


void laserInterface::boxStateNS::sendCurrent(const int16_t _i16CurrentStateNbr) {
    myMeshViews _myMeshViews;
    _myMeshViews.statusMsg();
}





/*******************/
// beat stack
/*******************/
void laserInterface::beatNS::init() {
    activeBeat = beat(2, 1, beatNS::sendCurrent);
}


void laserInterface::beatNS::sendCurrent(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat) {
    // Do something smart
}





/*******************/
// sequences stack
/*******************/
void laserInterface::sequenceNS::initComm() {
    globalSequences.sendCurrentSequence = sequenceNS::sendCurrent;
}

// TODO: for the moment, setCurrentSequence can only be called with a sequence_id
// no way to dynamically pass an ad hoc sequence. Refacto to give possibility to
// pass non static sequences
// Same issue with setCurrentBar
void laserInterface::sequenceNS::setCurrent(const int16_t __i16_sequence_id) {
    globalSequences.setActive(globalSequences.sequencesArray.at(__i16_sequence_id));
}


void laserInterface::sequenceNS::getCurrent() {
    sequenceNS::sendCurrent(globalSequences.getCurrentSequence().i16IndexNumber);
}


void laserInterface::sequenceNS::sendCurrent(const int16_t _i16CurrentStateNbr) {
    /** TODO: either draft a call to myMeshViews, or include
     *  sending sequence with statusMsg (since one single sequence
     *  is associated to one single boxState).  */
}


void laserInterface::sequenceNS::play(const int16_t __i16SequenceNb) {
  sequenceNS::lockStack();
  sequenceNS::setCurrent(__i16SequenceNb); // <-- TODO: this does not do anything. Draft sthing similar to playBar
  globalSequences.tPlaySequence.restartDelayed();
}


void laserInterface::sequenceNS::lockStack() {
  ControlerBox::setBoxActiveStateFromWeb(0);
  globalSequences.setStopCallbackForTPlaySequence();
  globalSequences.tPlaySequence.disable();
}





/*******************/
// bars stack
/*******************/
void laserInterface::barNS::initComm() {
    // TODO: initBarComm(), in the end, this namespace shall be used to initialize the whole laser stack
    // change its name
    globalSequences._bars.sendCurrentBar = barNS::sendCurrent;
}


void laserInterface::barNS::setCurrent(const int16_t __i16_target_bar_id) {
    globalSequences._bars.ui16IxNumbOfBarToPreload = __i16_target_bar_id;
    globalSequences._bars._preloadNextBar(__i16_target_bar_id);
    globalSequences._bars.setActive(globalSequences._bars.nextBar);
}


void laserInterface::barNS::getCurrent() {
    globalSequences._bars.sendCurrentBar(globalSequences._bars.i16GetCurrentBarId());
}


void laserInterface::barNS::sendCurrent(const int16_t __i16_current_bar_id) {
    /** TODO: draft a call to myMeshViews.  */
}


/** laserInterface::playBar():
 * 
 *  {@ params} uint16_t const __ui16_base_note_for_beat: pass the base note 
 *             for a given beat. ex. 4, a black
 *  {@ params} uint16_t const __ui16_base_beat_in_bpm: pass the base beat 
 *             in bpm. ex. 120 bpm (500 ms) */
void laserInterface::barNS::play(const uint16_t __ui16_base_note_for_beat, const uint16_t __ui16_base_beat_in_bpm, const int16_t __i16_target_bar_id) {
  /** 1. lock bar to avoid getting signal from a boxState or sequence player*/
  barNS::lockStack();

  /** 2. set the active bar from the passed-in parameters or create a bar
   *     TODO: check how the activeBarId is set and where it is used; maybe get rid of it
   *     TODO: we need to be able to pass an std::array<note, 16> to this method to create new bars
   * 
   *     two ways ---> 2. (i) create a bar or (ii) select a bar in the hard coded bars array
   * 
   *     TODO: make a choice. And try to change the bar factory in the bars class */
  bar __target_bar;
  if (__i16_target_bar_id != -1) {
    // 2.a: set the number of the bar to preload from SPIFFS and then preloaded it from SPIFFS
    globalSequences._bars.ui16IxNumbOfBarToPreload = __i16_target_bar_id;
    globalSequences._bars._preloadNextBar(globalSequences._bars.ui16IxNumbOfBarToPreload);
  } else {
    // 2.a: create a bar
    __target_bar = bar{std::array<note, 16>{note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)}, -2};
    globalSequences._bars.nextBar = __target_bar;
  }

  activeBeat = beat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm);

  // 3. play it
  globalSequences._bars.playBar(activeBeat);
}


void laserInterface::barNS::lockStack() {
  sequenceNS::lockStack();
  globalSequences.tPlaySequence.disable();
}


/*******************/
// notes stack
/*******************/
void laserInterface::noteNS::initComm() {
    // TODO: initNoteComm(), in the end, this namespace shall be used to initialize the whole laser stack
    // change its name
    notes _notes{noteNS::sendCurrent};
    /**
     *  TODO: This expression won't be very usefull -> the _notes instance that is created here 
     *        will be automatically deleted when this method will terminate...
     * 
     *        Such an expression shall be used on a global variable or at least, on a notes
     *        instance that is going to last and be used for sthg. 
     * */
}


void laserInterface::noteNS::setCurrent(const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note) {
    globalSequences._bars.getNotes().setActive(note(__ui16_target_laser_tone, __ui16_target_note));
}


void laserInterface::noteNS::getCurrent() {
    const note & _note = globalSequences._bars.getNotes().getCurrentNote();
    noteNS::sendCurrent(_note.getLaserToneNumber(), _note.getNote());
}


void laserInterface::noteNS::sendCurrent(const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note) {
    /** TODO: draft a call to myMeshViews.  */
}


void laserInterface::noteNS::play(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note) {
  // 1. lock notes to avoid getting signal from a boxState, sequence or bar player
  noteNS::lockStack();
  // 2. set the note and play it
  notes _notes;
  activeBeat = beat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm);
  _notes.playNote(note(__ui16_target_laser_tone, __ui16_target_note), activeBeat);
}


void laserInterface::noteNS::lockStack() {
  barNS::lockStack();
  globalSequences._bars.disableAndResetTPlayBar();
}
