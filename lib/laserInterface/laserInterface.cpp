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


void laserInterface::setCurrentSequence(const int16_t __i16ActiveSequence) {
    sequence::setActiveSequence(__i16ActiveSequence);
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
  setCurrentSequence(__i16SequenceNb);
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
    bar::sendCurrentBar = sendCurrentBar;
}


void laserInterface::setCurrentBar(const uint16_t __ui16_target_bar_ix_numb) {
  bar::getBarFromBarArray(__ui16_target_bar_ix_numb).setActive();
}


void laserInterface::getCurrentBar() {
    sendCurrentBar(bar::i16GetCurrentBarId());
}


void laserInterface::sendCurrentBar(const uint16_t __ui16_active_bar_ix_numb) {
    /** TODO: draft a call to myMeshViews.  */
}

/** laserInterface::playBar():
 * 
 *  {@ params} uint16_t const __ui16_base_note_for_beat: pass the base note 
 *             for a given beat. ex. 4, a black
 *  {@ params} uint16_t const __ui16_base_beat_in_bpm: pass the base beat 
 *             in bpm. ex. 120 bpm (500 ms) */
void laserInterface::playBar(const uint16_t __ui16_base_note_for_beat, const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_target_bar) {
  // 1. lock bar to avoid getting signal from a boxState or sequence player
  lockBarStack();
  // two ways ---> 
  // TODO: see which way is more coherent from an interface stand point and either remove one of them or split functions
  // 2. create a bar
  std::array<note, 16> _arrayOfNotes;
  _arrayOfNotes.fill(note(0,0));
  _arrayOfNotes = {note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)};
  /**  bar __target_bar(4, 8, _arrayOfNotes)
   *      ex. bar(4, 8, std::array<note, 16> {note(4,8), note(3,8), note(2,8), note(1,8)}, note(2,8), note(3,8), note(4,8), note(0,8))
   *      where:
   *      - 4 stands for 4 blacks per bar;
   *      - 8 stands for 8 effective notes in the bar; and
   *      - the array of notes contained in the bar. */
  bar __target_bar(4, 8, _arrayOfNotes);
  __target_bar.playBar(beat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm));
  // 2. Or set the active bar
  setCurrentBar(__ui16_target_bar);
  bar & __bar_ref = bar::getCurrentBarAsBar();
  // 3. play the bar
  __bar_ref.playBar(beat(__ui16_base_note_for_beat, __ui16_base_beat_in_bpm));
}


void laserInterface::lockBarStack() {
  lockSequenceStack();
  sequence::tPlaySequence.disable();
}


/*******************/
// notes stack
/*******************/
void laserInterface::initNoteComm() {
    note::sendCurrentNote = sendCurrentNote;
}


void laserInterface::setCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    note(__ui16_target_tone, __ui16_target_note).setActive();
}


void laserInterface::getCurrentNote() {
    const note &_note = note::getCurrentNote();
    sendCurrentNote(_note.getTone(), _note.getNote());
}


void laserInterface::sendCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    /** TODO: draft a call to myMeshViews.  */
}


void laserInterface::playNote(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
  // 1. lock notes to avoid getting signal from a boxState, sequence or bar player
  lockNoteStack();
  // 2. set the note and play it
  note(__ui16_target_tone, __ui16_target_note).playNoteStandAlone( __ui16_base_note_for_beat, __ui16_base_beat_in_bpm);
}


void laserInterface::lockNoteStack() {
  lockBarStack();
  bar::tPlayBar.disable();
}
