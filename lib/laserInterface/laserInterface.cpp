/*
  laserInterface.cpp - Library to handle communications between laser stack and mesh
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include <laserInterface.h>

using namespace laserInterface;



/*******************/
// boxStates stack
/*******************/
void laserInterface::initBoxStatesComm() {
    boxState::sendCurrentBoxState = sendCurrentBoxState;
}


void laserInterface::sendCurrentBoxState(const int16_t _i16CurrentStateNbr) {
    myMeshViews _myMeshViews;
    _myMeshViews.statusMsg();
}


/*******************/
// sequences stack
/*******************/
void laserInterface::initSequenceComm() {
    sequence::sendCurrentSequence = sendCurrentSequence;
}

// TODO: for the moment, setCurrentSequence can only be called with a sequence_id
// no way to dynamically pass an ad hoc sequence. Refacto to give possibility to
// pass non static sequences
// Same issue with setCurrentBar
void laserInterface::setCurrentSequence(const int16_t __i16_sequence_id) {
    sequence::getSequenceFromSequenceArray(__i16_sequence_id).setActive(__i16_sequence_id);
}


void laserInterface::getCurrentSequence() {
    sendCurrentSequence(sequence::getCurrentSequence());
}


void laserInterface::sendCurrentSequence(const int16_t _i16CurrentStateNbr) {
    /** TODO: either draft a call to myMeshViews, or include
     *  sending sequence with statusMsg (since one single sequence
     *  is associated to one single boxState).  */
}


void laserInterface::playSequence(const int16_t __i16SequenceNb) {
  lockSequenceStack();
  setCurrentSequence(__i16SequenceNb); // <-- TODO: this does not do anything. Draft sthing similar to playBar
  sequence::tPlaySequence.restartDelayed();
}


void laserInterface::lockSequenceStack() {
  ControlerBox::setBoxActiveStateFromWeb(0);
  sequence::tPlaySequenceInLoop.disable();
}





/*******************/
// bars stack
/*******************/
void laserInterface::initBarComm() {
    // TODO: initBarComm(), in the end, this namespace shall be used to initialize the whole laser stack
    // change its name
    sequence::globalBars.sendCurrentBar = sendCurrentBar;
}


void laserInterface::setCurrentBar(const int16_t __i16_target_bar_id) {
    sequence::globalBars.setActive(sequence::globalBars.getBarFromBarArray(__i16_target_bar_id));
}


void laserInterface::getCurrentBar() {
    sequence::globalBars.sendCurrentBar(sequence::globalBars.i16GetCurrentBarId());
}


void laserInterface::sendCurrentBar(const int16_t __i16_current_bar_id) {
    /** TODO: draft a call to myMeshViews.  */
}


/** laserInterface::playBar():
 * 
 *  {@ params} uint16_t const __ui16_base_note_for_beat: pass the base note 
 *             for a given beat. ex. 4, a black
 *  {@ params} uint16_t const __ui16_base_beat_in_bpm: pass the base beat 
 *             in bpm. ex. 120 bpm (500 ms) */
void laserInterface::playBar(const uint16_t __ui16_base_note_for_beat, const uint16_t __ui16_base_beat_in_bpm, const int16_t __i16_target_bar) {
  /** 1. lock bar to avoid getting signal from a boxState or sequence player*/
  lockBarStack();

  /** 2. set the active bar from the passed-in parameters or create a bar
   *     TODO: check how the activeBarId is set and where it is used; maybe get rid of it
   *     TODO: we need to be able to pass an std::array<note, 16> to this method to create new bars
   * 
   *     two ways ---> 2. (i) create a bar or (ii) select a bar in the hard coded bars array
   * 
   *     TODO: make a choice. And try to change the bar factory in the bars class */
  bar __target_bar;
  if (__i16_target_bar != -1) {
    // 2.a: select a bar in the hard coded bars array and set it as active
    __target_bar = sequence::globalBars.getBarFromBarArray(__i16_target_bar);
  } else {
    // 2.a: create a bar
    __target_bar = {std::array<note, 16>{note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)}};
  }

  // 3. play it
  sequence::globalBars.playBarStandAlone(__target_bar, beat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm));
}


void laserInterface::lockBarStack() {
  lockSequenceStack();
  sequence::tPlaySequence.disable();
}


/*******************/
// notes stack
/*******************/
void laserInterface::initNoteComm() {
    // TODO: initNoteComm(), in the end, this namespace shall be used to initialize the whole laser stack
    // change its name
    notes _notes{sendCurrentNote};
    /**
     *  TODO: This expression won't be very usefull -> the _notes instance that is created here 
     *        will be automatically deleted when this method will terminate...
     * 
     *        Such an expression shall be used on a global variable or at least, on a notes
     *        instance that is going to last and be used for sthg. 
     * */
}


void laserInterface::setCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    sequence::globalBars.getNotes().setActive(note(__ui16_target_tone, __ui16_target_note));
}


void laserInterface::getCurrentNote() {
    const note & _note = sequence::globalBars.getNotes().getCurrentNote();
    sendCurrentNote(_note.getToneNumber(), _note.getNote());
}


void laserInterface::sendCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    /** TODO: draft a call to myMeshViews.  */
}


void laserInterface::playNote(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
  // 1. lock notes to avoid getting signal from a boxState, sequence or bar player
  lockNoteStack();
  // 2. set the note and play it
  notes _notes;
  _notes.playNoteStandAlone(note(__ui16_target_tone, __ui16_target_note), beat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm));
}


void laserInterface::lockNoteStack() {
  lockBarStack();
  sequence::globalBars.tPlayBar.disable();
}
