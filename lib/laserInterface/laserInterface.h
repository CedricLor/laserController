/*
  laserInterface.h - Library to handle communications between laser stack and mesh
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef laserInterface_h
#define laserInterface_h

#include "Arduino.h"
#include "globalBasementVars.h"
#include "myMeshViews.h"
#include "laserSequence.h"

namespace laserInterface {

  extern laserSequences globalSequences;

  // void init(controllerBoxesCollection * __cntrllerBoxesCollection);

  enum class _Mode : uint16_t { step_controlled, automatic, sequence_manual, bar_manual, note_manual };

  class laserScheduler {
    // friend step;
    // friend boxState;

    public:
      laserScheduler(_Mode __mode=_Mode::automatic);
    private:
      // laserSchedule mode: 1. automatic (laserSequences connected to boxStates), 2. laserSequence manual, 3. bar manual, 4. laserNote manual
      uint16_t _activeMode;

      // beat & _activeBeat;

      // players
      laserNotes _laserNotes;
      bars _bars;
      laserSequences _laserSequences;
      
      // default item
      beat const _defaultBeat;
      laserNote const _defaultLaserNote;
      bar const _defaultBar;
      laserSequence const _defaultSequence;

      // active item
      // laserSequence & _activeSequence;
      // bar & _activeBar;
      // laserNote & _activeLaserNote;
  };

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

  // namespace boxStateNS {
  //   // boxStates stack
  //   void initComm();
  //   /** not being implemented for the moment; activeState set by direct call from meshController to ControlerBox */
  //   // void setBoxState();
  //   void sendCurrent(const int16_t _i16CurrentStateNbr);
  // }

  namespace monitoredMasterBoxes {
    // masterBoxes stack
    void initComm();
    void add();
    void remove();
    void set();
    void send();
  }

  namespace monitoredStates {
    // monitored states stack
    void initComm();
    void add();
    void remove();
    void set();
    void send();
  }

  // namespace beatNS {
  //   void init();
  //   void sendCurrent(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat);
  // }

  namespace sequenceNS {
    // laserSequence stack
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
    // laserNote stack
    void initComm();
    void setCurrent(const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note);
    void getCurrent();
    void sendCurrent(const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note);
    void play(uint16_t const __ui16_base_note_for_beat, uint16_t const __ui16_base_beat_in_bpm, const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note);
    void lockStack();
  }
}

#endif
