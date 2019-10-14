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
  friend class boxStateCollection;
  
  public:
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

    void applyStep();

  private:
    // boxState criteria (variables) setter
    int16_t _i16stepBoxStateNb;
    int16_t _i16StateDuration;
    uint16_t _ui16AssociatedSequence;
    int16_t _i16onIRTrigger;
    int16_t _i16onMeshTrigger;
    int16_t _i16onExpire;
    std::array<uint16_t, 4> _ui16monitoredMasterBoxesNodeNames;
    std::array<int16_t, 4> _i16monitoredMasterStates;
};


class stepCollection
{
  public:
    stepCollection();

    std::array<step, 8> stepsArray;

    void readJSONObjLineInFile(mySpiffs & __mySpiffs, const char * path, uint16_t _ui16stepCounter, const char * _cNodeName);

    Task tPreloadNextStep;
    void _tcbPreloadNextStep();
    void _preloadNextStepFromJSON(JsonObject& _joStep);
};

extern stepCollection stepColl;

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

    // instance variables
    int16_t i16Duration; // duration for which the state shall stay active before automatically returning to default
    uint16_t ui16AssociatedSequence;  // sequence associated to a given state
    int16_t i16onIRTrigger;
    int16_t i16onMeshTrigger;
    int16_t i16onExpire;
    std::array<uint16_t, 4> ui16monitoredMasterBoxesNodeNames;
    std::array<int16_t, 4> i16monitoredMasterStates;

  private:
};


class boxStateCollection
{
  friend class signal;
  friend class stepCollection;

  public:
    /** constructors*/
    /** default constructor: used to define the boxStates array */
    boxStateCollection(void (*_sendCurrentBoxState)(const int16_t _i16CurrentStateNbr)=nullptr);

    // boxStates array
    std::array<boxState, 14> boxStatesArray;

    // step mode switch stack
    void switchToStepControlled();
    uint16_t ui16stepCounter;
    /** ui16Mode = 0 => mode automatic, boxStates use their default settings
     *  ui16Mode = 1 => step controlled, boxStates use the settings corresponding
     *  to the step they are embedded in.
     *  */
    uint16_t ui16Mode;

    /** individual boxState Task: iterating once (unless explicitly restarted) 
     *  for the duration of a single boxState */
    Task tPlayBoxState;

    /** interface to mesh */
    void (*sendCurrentBoxState)(const int16_t _i16CurrentStateNbr);

  private:
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

extern boxStateCollection bxStateColl;

#endif
