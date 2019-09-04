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
  ControlerBox::setBoxActiveStateFromWeb(0);
  setCurrentSequence(__i16SequenceNb);
  sequence::tPlaySequence.restartDelayed();
}




/*******************/
// bars stack
/*******************/
void laserInterface::initBarComm() {
    bar::sendCurrentBar = sendCurrentBar;
}


void laserInterface::setCurrentBar(const uint16_t __ui16ActiveBar) {
    bar::setActiveBar(__ui16ActiveBar);
}


void laserInterface::getCurrentBar() {
    sendCurrentBar(bar::getCurrentBar());
}


void laserInterface::sendCurrentBar(const uint16_t __ui16ActiveBar) {
    /** TODO: draft a call to myMeshViews.  */
}


void laserInterface::playBar(const uint16_t __ui16ActiveBar) {
  ControlerBox::setBoxActiveStateFromWeb(0);
  // TODO: lock bar to avoid getting signal from a manually started sequence player
  setCurrentBar(__ui16ActiveBar);
  bar::tPlayBar.restartDelayed();
}




/*******************/
// notes stack
/*******************/
void laserInterface::initNoteComm() {
    note::sendCurrentNote = sendCurrentNote;
}


void laserInterface::setCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    // note::setActiveTone(__ui16ActiveTone);
    note _note(__ui16_target_tone, __ui16_target_note);
    // set the note duration
}


void laserInterface::getCurrentNote() {
    const note &_note = note::getCurrentNote();
    sendCurrentNote(_note.getTone(), _note.getNote());
}


void laserInterface::sendCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    /** TODO: draft a call to myMeshViews.  */
}


void laserInterface::playNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
  ControlerBox::setBoxActiveStateFromWeb(0);
  // TODO: lock note to avoid getting signal from a manually started sequence or bar player
  setCurrentNote(__ui16_target_tone, __ui16_target_note);
  // TODO: tPlayNote shall read its parameters in an instance of note
  note::tPlayNote.restartDelayed();
}