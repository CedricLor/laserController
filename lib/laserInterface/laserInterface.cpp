/*
  laserInterface.cpp - Library to handle communications between laser stack and mesh
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include <laserInterface.h>

using namespace laserInterface;



// boxStates stack
void laserInterface::initBoxStatesComm() {
    boxState::sendCurrentBoxState = sendCurrentBoxState;
}


void laserInterface::sendCurrentBoxState(const int16_t _i16CurrentStateNbr) {
    myMeshViews _myMeshViews;
    _myMeshViews.statusMsg();
}



// sequences stack
void laserInterface::initSequenceComm() {
    sequence::sendCurrentSequence = sendCurrentSequence;
}


void laserInterface::setCurrentSequence(const int16_t __i16ActiveSequence) {
    sequence::setActiveSequence(__i16ActiveSequence);
}


int16_t laserInterface::getCurrentSequence() {
    sendCurrentSequence(sequence::getCurrentSequence());
}


void laserInterface::sendCurrentSequence(const int16_t _i16CurrentStateNbr) {
    // TODO: draft a call to myMeshViews
}


void laserInterface::playSequence(const int16_t __i16SequenceNb) {
  // TODO: switch to a manual boxState first
  setCurrentSequence(__i16SequenceNb);
  sequence::tPlaySequence.restartDelayed();
}
