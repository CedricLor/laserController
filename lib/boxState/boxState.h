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
    boxState(int16_t *__i16monitoredMasterStates={}, ControlerBox * __masterBox = nullptr); // default constructor
    boxState(const int16_t _i16Duration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const uint16_t _ui16onMeshTrigger, const uint16_t _ui16onExpire, const int16_t *__i16monitoredMasterStates={}, ControlerBox * __masterBox = nullptr);

    static void initBoxStates(); // initializer of the array of boxState

    static void switchToStepControlled();
    static uint16_t ui16stepCounter;
    static uint16_t ui16Mode;

    static Task tPlayBoxStates;
    static const short int BOX_STATES_COUNT;

    static boxState boxStates[];

    int16_t i16Duration; // duration for which the status shall stay active before automatically returning to default
    uint16_t ui16AssociatedSequence;  // sequence associated to a given state
    int16_t i16onIRTrigger;
    int16_t i16onMeshTrigger;
    int16_t i16onExpire;
    int16_t *i16monitoredMasterStates;

    static Task tPlayBoxState;

  private:
    static short int _boxTargetState;
    static bool _boxActiveStateHasBeenReset;

    static void _tcbPlayBoxStates();
    static bool _oetcbPlayBoxStates();

    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

    // _tcbPlayBoxStates() sub functions
    void _setBoxTargetStateFromSignalCatchers(boxState& _currentBoxState);
    static void _resetSignalCatchers(boxState& _currentBoxState);
    static void _restart_tPlayBoxState(boxState& _currentBoxState);

    static void _setBoxTargetState(const short int targetBoxState);

    static unsigned long _ulCalcInterval(int16_t _i16IntervalInS);
    static uint16_t ui16mToS(uint16_t _minutes);

    void _setMasterBox();
    ControlerBox * _masterBox;

    void _initBoxState(const int16_t _i16Duration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire);
    bool _hasBothTriggers();
    bool _checkBothTriggersAndAct();
    void _checkMeshTriggerAndAct();
    void _checkIRTriggerAndAct();
    bool _meshHasBeenTriggered();
    bool _testIfMasterIsInMonitoredState();
    void _resolveTriggersConflictAndAct();


    void _changeBoxState(const int16_t _i16StepIxNb, const int16_t _i16Duration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire);

};

#endif
