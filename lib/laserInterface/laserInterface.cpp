/*
  laserInterface.cpp - Library to handle communications between laser stack and mesh
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include <laserInterface.h>


// boxStates stack
void laserInterface::initBoxStatesComm() {
    boxState::sendBoxState = sendBoxState;
}

void laserInterface::sendBoxState(const int16_t _currentStateNbr) {
    myMeshViews _myMeshViews;
    _myMeshViews.statusMsg();
}
