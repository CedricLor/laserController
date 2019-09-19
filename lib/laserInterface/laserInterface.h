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
    
  void init();

  namespace stepsNS {
    void play();

    namespace collection {
      void initComm();
      void set();
      void send();
    }

    namespace single {
      void add();
      void remove();
      void setCurrent();
      void sendCurrent();
    }
  }

  namespace boxStateNS {
    // boxStates stack
    void initComm();
    /** not being implemented for the moment; activeState set by direct call from meshController to ControlerBox */
    // void setBoxState();
    void sendCurrent(const int16_t _i16CurrentStateNbr);
  }

  namespace monitoredMasterBoxes {
    // masterBoxes stack
    void initMonitoredMasterBoxComm();
    void addMonitoredMasterBox();
    void removeMonitoredMasterBox();
    void setMonitoredMasterBoxes();
    void sendMonitoredMasterBoxes();
  }

  namespace monitoredStates {
    // monitored states stack
    void initMonitoredBoxesStatesComm();
    void addMonitoredBoxState();
    void removeMonitoredBoxState();
    void setMonitoredBoxesStates();
    void sendMonitoredBoxesStates();
  }

  namespace beatNS {
    void init();
    void sendCurrentBeat(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat);
  }

  namespace sequenceNS {
    // sequence stack
    void initComm();
    void setCurrent(const int16_t __i16ActiveSequenceNb);
    void getCurrent();
    void sendCurrent(const int16_t __i16ActiveSequenceNb);
    void play(const int16_t __i16SequenceNb);
    void lockStack();
  }

  namespace barNS {
    // bar stack
    void initComm();
    void setCurrent(const int16_t __i16_target_bar_id);
    void getCurrent();
    void sendCurrent(const int16_t __i16_current_bar_id);
    void play(const uint16_t __ui16_base_note_for_beat, const uint16_t __ui16_base_beat_in_bpm, const int16_t __i16_target_bar=-1);
    void lockStack();
  }

  namespace noteNS {
    // note stack
    void initComm();
    void setCurrent(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void getCurrent();
    void sendCurrent(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void play(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);
    void lockStack();
  }
}

#endif
