/*
  laserInterface.h - Library to handle communications between laser stack and mesh
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef laserInterface_h
#define laserInterface_h

#include "Arduino.h"
#include "myMeshController.h"
#include "boxState.h"

namespace laserInterface {
    // steps stack
    void playSteps();

    void initStepsComm();
    void addStep();
    void removeStep();
    void setCurrentStep();
    void sendCurrentStep();
    void setSteps();
    void sendSteps();

    // boxStates stack
    void initBoxStatesComm();
    /** not being implemented for the moment; activeState set by direct call from meshController to ControlerBox */
    // void setBoxState();
    void sendBoxState(const int16_t _currentStateNbr);

    // masterBoxes stack
    void initMonitoredMasterBoxComm();
    void addMonitoredMasterBox();
    void removeMonitoredMasterBox();
    void setMonitoredMasterBoxes();
    void sendMonitoredMasterBoxes();

    // monitored states stack
    void initMonitoredBoxesStatesComm();
    void addMonitoredBoxState();
    void removeMonitoredBoxState();
    void setMonitoredBoxesStates();
    void sendMonitoredBoxesStates();

    // sequence stack
    void initSequenceComm();
    void setCurrentSequence();
    void sendCurrentSequence();

    // bar stack
    void initBarComm();
    void setCurrentBar();
    void sendCurrentBar();

    // note stack
    void initNoteComm();
    void setCurrentNote();
    void sendCurrentNote();
}

#endif
