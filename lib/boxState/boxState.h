/*
  boxState.h - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.

  |--main.cpp
  |  |
  |  |--boxState.cpp
  |  |  |--boxState.h
  |  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--sequence.cpp
  |  |  |  |--sequence.h
  |  |  |  |--global.cpp (called to start some tasks and play some functions)
  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
  |  |  |  |  |--bar.h
  |  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
  |  |  |  |  |  |--note.h
  |  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
  |  |  |  |  |  |  |--tone.h
  |  |  |  |
  |  |  |  |--tone.cpp
  |  |  |  |  |--tone.h
  |  |  |  |  |--global.cpp (called to retrieve some values)
  |  |  |  |  |  |--global.h
  |  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h


reverse dependency graph
  |--boxState.h
  |  |--boxState.cpp
  |  |  |--main.cpp

*/

#ifndef boxState_h
#define boxState_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <mns.h>
#include <mySpiffs.h>
#include <myMeshViews.h>
#include <sequence.h>

class step
{
  public:
    static step steps[];

    step();
    step(int16_t __i16stepBoxStateNb,
      int16_t __i16StateDuration,
      uint16_t __ui16AssociatedSequence,
      int16_t __i16onIRTrigger,
      int16_t __i16onMeshTrigger,
      int16_t __i16onExpire,
      uint16_t __ui16stepMasterBoxName,
      uint16_t __ui16monitoredMasterStatesSize,
      int16_t *__i16monitoredMasterStates
    );

    // boxState criteria (variables) setter
    int16_t _i16stepBoxStateNb;
    int16_t _i16StateDuration;
    uint16_t _ui16AssociatedSequence;
    int16_t _i16onIRTrigger;
    // int16_t _i16onIRTriggerNewMaster;
    int16_t _i16onMeshTrigger;
    // int16_t i16onMeshTriggerNewMaster;
    int16_t _i16onExpire;
    // int16_t i16onExpireNewMaster;
    uint16_t _ui16stepMasterBoxName;
    // int16_t masterStatesToWatch[];
    uint16_t _ui16monitoredMasterStatesSize;
    int16_t *_i16monitoredMasterStates;

    void applyStep();

    static Task tPreloadNextStep;
    static void _tcbPreloadNextStep();
    static void _preloadNextStepFromJSON(JsonObject& _joStep);

    static void initSteps();
  private:

};



class boxState
{
  public:
    /** constructors*/
    /** default constructor: used to define the boxStates array */
    boxState();
    /** "little constructor": used to initialize the default boxStates */
    boxState(const int16_t _i16Duration, 
      const uint16_t _ui16AssociatedSequence, 
      const int16_t _i16onIRTrigger, 
      const int16_t _i16onMeshTrigger, 
      const int16_t _i16onExpire);
    /** "full blown constructor": used to reinitialize the boxStates from the steps class */
    boxState(const int16_t _i16Duration, 
      const uint16_t _ui16AssociatedSequence, 
      const int16_t _i16onIRTrigger, 
      const int16_t _i16onMeshTrigger, 
      const int16_t _i16onExpire, 
      const uint16_t _ui16monitoredMasterStatesSize, 
      int16_t *_i16monitoredMasterStates);

    // boxStates array
    static const short int BOX_STATES_COUNT;
    static boxState boxStates[];
    static void initBoxStates();

    // step mode switch stack
    static void switchToStepControlled();
    static uint16_t ui16stepCounter;
    static uint16_t ui16Mode;

    /** main boxState Task: iterating all the time every 500 ms to monitor
     *  the occurence of triggers */
    static Task tPlayBoxStates;

    // instance variables
    int16_t i16Duration; // duration for which the state shall stay active before automatically returning to default
    uint16_t ui16AssociatedSequence;  // sequence associated to a given state
    int16_t i16onIRTrigger;
    int16_t i16onMeshTrigger;
    int16_t i16onExpire;
    uint16_t ui16monitoredMasterStatesSize;
    int16_t *i16monitoredMasterStates;

    /** individual boxState Task: iterating once (unless explicitly restarted) 
     *  for the duration of a single boxState */
    static Task tPlayBoxState;

  private:
    friend class signal;

    static short int _boxTargetState;

    static void _tcbPlayBoxStates();
    static bool _oetcbPlayBoxStates();

    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

    static void _setBoxTargetState(const short int targetBoxState);

    static unsigned long _ulCalcInterval(int16_t _i16IntervalInS);
};

#endif
