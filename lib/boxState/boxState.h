/*
  boxState.h - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.

*/

#ifndef boxState_h
#define boxState_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <mns.h>
#include <mySpiffs.h>
#include <laserInterface.h>

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
      std::array<uint16_t, 4> _ui16monitoredMasterBoxesNodeNames,
      const std::array<int16_t, 4> __i16monitoredMasterStates
    );

    // boxState criteria (variables) setter
    int16_t _i16stepBoxStateNb;
    int16_t _i16StateDuration;
    uint16_t _ui16AssociatedSequence;
    int16_t _i16onIRTrigger;
    int16_t _i16onMeshTrigger;
    int16_t _i16onExpire;
    std::array<uint16_t, 4> _ui16monitoredMasterBoxesNodeNames;
    std::array<int16_t, 4> _i16monitoredMasterStates;

    void applyStep();

    static Task tPreloadNextStep;
    static void _tcbPreloadNextStep();
    static void _preloadNextStepFromJSON(JsonObject& _joStep);

    static void initSteps();
  private:

};


class stepCollection
{
  public:
    stepCollection();

    std::array<step, 8> stepsArray;

    Task tPreloadNextStep;
    void _tcbPreloadNextStep();
    void _preloadNextStepFromJSON(JsonObject& _joStep);
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
      std::array<uint16_t, 4> _ui16monitoredMasterBoxesNodeNames,
      std::array<int16_t, 4> _i16monitoredMasterStates);

    // boxStates array
    static const short int BOX_STATES_COUNT;
    static boxState boxStates[];
    static void initBoxStates();

    // step mode switch stack
    static void switchToStepControlled();
    static uint16_t ui16stepCounter;
    static uint16_t ui16Mode;

    // instance variables
    int16_t i16Duration; // duration for which the state shall stay active before automatically returning to default
    uint16_t ui16AssociatedSequence;  // sequence associated to a given state
    int16_t i16onIRTrigger;
    int16_t i16onMeshTrigger;
    int16_t i16onExpire;
    std::array<uint16_t, 4> ui16monitoredMasterBoxesNodeNames;
    std::array<int16_t, 4> i16monitoredMasterStates;

    /** individual boxState Task: iterating once (unless explicitly restarted) 
     *  for the duration of a single boxState */
    static Task tPlayBoxState;

    /** interface to mesh */
    static void (*sendCurrentBoxState)(const int16_t _i16CurrentStateNbr);

  private:
    friend class signal;
    friend class step;
    friend class stepCollection;

    static const std::array<uint16_t, 4> _monitorNoMaster; // {254}
    static const std::array<int16_t, 4> _monitorNoStates;// {-1};
    static const std::array<int16_t, 4> _IRStates;// {6, 7, 8, 9};
    static const std::array<int16_t, 4> _MeshStates;// {10, 11, 12, 13};

    static short int _boxTargetState;

    static void _restartPlayBoxState();

    static bool _oetcbPlayBoxState();
    static void _odtcbPlayBoxState();

    static void _setBoxTargetState(const short int targetBoxState);

    static unsigned long _ulCalcInterval(int16_t _i16IntervalInS);
};


class boxStateCollection
{
  public:
    /** constructors*/
    /** default constructor: used to define the boxStates array */
    boxStateCollection(void (*_sendCurrentBoxState)(const int16_t _i16CurrentStateNbr)=nullptr);

    // boxStates array
    std::array<boxState, 14> boxStatesArray;

    // step mode switch stack
    void switchToStepControlled();
    uint16_t ui16stepCounter;
    uint16_t ui16Mode;

    /** individual boxState Task: iterating once (unless explicitly restarted) 
     *  for the duration of a single boxState */
    Task tPlayBoxState;

    /** interface to mesh */
    void (*sendCurrentBoxState)(const int16_t _i16CurrentStateNbr);

  private:
    friend class signal;
    friend class step;
    friend class stepCollection;

    const std::array<uint16_t, 4> _monitorNoMaster; // {254}
    const std::array<int16_t, 4> _monitorNoStates;// {-1};
    const std::array<int16_t, 4> _IRStates;// {6, 7, 8, 9};
    const std::array<int16_t, 4> _MeshStates;// {10, 11, 12, 13};

    short int _boxTargetState;

    void _restartPlayBoxState();

    bool _oetcbPlayBoxState();
    void _odtcbPlayBoxState();

    void _setBoxTargetState(const short int targetBoxState);

    unsigned long _ulCalcInterval(int16_t _i16IntervalInS);
};
#endif
