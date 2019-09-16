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
    void setCurrentSequence(const int16_t __i16ActiveSequenceNb);
    void getCurrentSequence();
    void sendCurrentSequence(const int16_t __i16ActiveSequenceNb);
    void playSequence(const int16_t __i16SequenceNb);
    void lockSequenceStack();

    // bar stack
    void initBarComm();
    void setCurrentBar(const int16_t __i16_target_bar_id);
    void getCurrentBar();
    void sendCurrentBar(const int16_t __i16_current_bar_id);
    void playBar(const uint16_t __ui16_base_note_for_beat, const uint16_t __ui16_base_beat_in_bpm, const int16_t __i16_target_bar=-1);
    void lockBarStack();

    // note stack
    void initNoteComm();
    void setCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void getCurrentNote();
    void sendCurrentNote(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void playNote(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void lockNoteStack();
}

#endif
