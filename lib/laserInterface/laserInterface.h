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
    void sendCurrentBoxState(const int16_t _i16CurrentStateNbr);

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
    void setCurrentSequence(const int16_t __i16ActiveSequence);
    void getCurrentSequence();
    void sendCurrentSequence(const int16_t __i16ActiveSequence);
    void playSequence(const int16_t __i16SequenceNb);

    // bar stack
    void initBarComm();
    void setCurrentBar(const uint16_t __ui16ActiveBar);
    void getCurrentBar();
    void sendCurrentBar(const uint16_t __ui16ActiveBar);
    void playBar(const uint16_t __ui16ActiveBar);

    // note stack
    void initNoteComm();
    void setCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void getCurrentNote();
    void sendCurrentNote(const uint16_t __ui16ActiveTone, const uint16_t __ui16ActiveNote);
    void playNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
}

#endif
