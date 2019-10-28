/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
  Created by Cedric Lor, June 10, 2019.

*/

/**
  Brief:
  - The boxStateCollection class provides a series of templates for states of this Controlerbox.
  - The step class provides a parameters for boxStates:
    - duration;
    (- number of iterations);
    - states to go to upon occurence of events:
      - signal from IR
      - signal from Mesh
      - expiration
  - The signal class provides an interface to the boxState class for events registered in
    the ControlerBox class.
  - bxStateColl._setBoxTargetState() is the only contact point for the signal class.
  - bxStateColl.tPlayBoxState starts and stops the boxState and underlying laserSequence, depending
    on the settings (duration, associated laserSequence number) of the currently active
    boxState, and upon request from _setBoxTargetState().
  - bxStateColl._setBoxTargetState() parameterize bxStateColl.tPlayBoxState before starting it.
  - upon being re-enabled, bxStateColl.tPlayBoxState sends a status message to the mesh.
*/


/*
  TODO DEBUG PROBLEMS:
  HIGH:
  I. Create the stack for steps, multiple masters and multiple monitored states:
     - JSON
     - Mesh
     - Web
  II. Create the stack for laserSequences:
     - JSON
     - Mesh
     - Web
  III. Create the stack for laserNotes/laserTones:
     - JSON
     - Mesh
     - Web
  IV. Create the stack for steps mode switch:
     - JSON
     - Mesh
     - Web
  V. NTP for browser
  VI.SPIFFS: make the call to split files in main.cpp smarter: detect
  if there are any files to split (file which name would contain the special
  '-' character).
  VII.Create additional roles (isIrController, isRelay...)

  MEDIUM:

  LOW:
  II. BoxState and Steps
    C. Implement various reactions to various state of master
    D. Implement locking IRStartup mode to other IRless modes only
  III. WSSender: add a queue in the looped task
  IV. WSSender: by pass ControlerBox storage when receiving data from the boxes
  V. Make the whole thing more flexible to be reused for:
    1. nodes in the mesh that would only serve as relays
    2. nodes in the mesh that would serve as relays and be connected to IR or
    ultrasonic sensors providing data to the whole system
  VI. Hardware interrupt on the IR Pin
  VII. Deep sleep
*/

#include "Arduino.h"
#include "boxState.h"







///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// step class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
step::step():
  _i16stepBoxStateNb(0),
  _i16StateDuration(0),
  _ui16AssociatedSequence(0),
  _i16onIRTrigger(0),
  _i16onMeshTrigger(0),
  _i16onExpire(0),
  _ui16monitoredMasterBoxesNodeNames({}),
  _i16monitoredMasterStates({})
{}

step::step(int16_t __i16stepBoxStateNb,
  int16_t __i16StateDuration,
  uint16_t __ui16AssociatedSequence,
  int16_t __i16onIRTrigger,
  int16_t __i16onMeshTrigger,
  int16_t __i16onExpire,
  std::array<uint16_t, 4> __ui16monitoredMasterBoxesNodeNames,
  std::array<int16_t, 4> __i16monitoredMasterStates
): 
  _i16stepBoxStateNb(__i16stepBoxStateNb),
  _i16StateDuration(__i16StateDuration),
  _ui16AssociatedSequence(__ui16AssociatedSequence),
  _i16onIRTrigger(__i16onIRTrigger),
  _i16onMeshTrigger(__i16onMeshTrigger),
  _i16onExpire(__i16onExpire),
  _ui16monitoredMasterBoxesNodeNames(__ui16monitoredMasterBoxesNodeNames),
  _i16monitoredMasterStates(__i16monitoredMasterStates)
{}

// copy constructor
step::step(const step& __step):
  _i16stepBoxStateNb(__step._i16stepBoxStateNb),
  _i16StateDuration(__step._i16StateDuration),
  _ui16AssociatedSequence(__step._ui16AssociatedSequence),
  _i16onIRTrigger(__step._i16onIRTrigger),
  _i16onMeshTrigger(__step._i16onMeshTrigger),
  _i16onExpire(__step._i16onExpire),
  _ui16monitoredMasterBoxesNodeNames(__step._ui16monitoredMasterBoxesNodeNames),
  _i16monitoredMasterStates(__step._i16monitoredMasterStates)
{ }

// assignement operator
step& step::operator=(const step& __step)
{
  if (&__step != this) {
    _i16stepBoxStateNb = __step._i16stepBoxStateNb;
    _i16StateDuration = __step._i16StateDuration;
    _ui16AssociatedSequence = __step._ui16AssociatedSequence;
    _i16onIRTrigger = __step._i16onIRTrigger;
    _i16onMeshTrigger = __step._i16onMeshTrigger;
    _i16onExpire = __step._i16onExpire;
    _ui16monitoredMasterBoxesNodeNames = __step._ui16monitoredMasterBoxesNodeNames;
    _i16monitoredMasterStates = __step._i16monitoredMasterStates;
  }
  return *this;
}

step::step(step&& __step):
  _i16stepBoxStateNb(0),
  _i16StateDuration(0),
  _ui16AssociatedSequence(0),
  _i16onIRTrigger(0),
  _i16onMeshTrigger(0),
  _i16onExpire(0),
  _ui16monitoredMasterBoxesNodeNames({}),
  _i16monitoredMasterStates({})
{
  *this = std::move(__step);
}

step & step::operator=(step&& __step) {
  if (this != &__step) {
    _i16stepBoxStateNb = 0;
    _i16StateDuration = 0;
    _ui16AssociatedSequence = 0;
    _i16onIRTrigger = 0;
    _i16onMeshTrigger = 0;
    _i16onExpire = 0;
    _ui16monitoredMasterBoxesNodeNames = {};
    _i16monitoredMasterStates = {};

    _i16stepBoxStateNb = __step._i16stepBoxStateNb;
    _i16StateDuration = __step._i16StateDuration;
    _ui16AssociatedSequence = __step._ui16AssociatedSequence;
    _i16onIRTrigger = __step._i16onIRTrigger;
    _i16onMeshTrigger = __step._i16onMeshTrigger;
    _i16onExpire = __step._i16onExpire;
    _ui16monitoredMasterBoxesNodeNames = __step._ui16monitoredMasterBoxesNodeNames;
    _i16monitoredMasterStates = __step._i16monitoredMasterStates;

    __step._i16stepBoxStateNb = 0;
    __step._i16StateDuration = 0;
    __step._ui16AssociatedSequence = 0;
    __step._i16onIRTrigger = 0;
    __step._i16onMeshTrigger = 0;
    __step._i16onExpire = 0;
    __step._ui16monitoredMasterBoxesNodeNames = {};
    __step._i16monitoredMasterStates = {};
  }
  return *this;
}







///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// stepCollection class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
stepCollection::stepCollection():
  ui16stepCounter(0),
  stepFileName("/sessions.json"),
  i16maxStepIndexNb(-1)
{
  Serial.println("stepCollection::stepCollection(): starting");
  /* step 0: waiting IR, all Off
  - no passenger */
  // stepsArray[0] = {4, -1, 5, 6, -1, 4, bxStateColl._monitorNoMaster, bxStateColl._monitorNoStates};
  // Serial.println("stepCollection::stepCollection():");
  /* boxState: 4 - waiting IR, duration: -1 - infinite, laserSequence: 5 - all Off,
    onIRTrigger: apply state 6, onMeshTrigger: -1 (no mesh trigger),
    onExpire: 4 (no expiration, repeat), 
    _ui16monitoredMasterBoxesNodeNames: [254] (_monitorNoMaster),
    _i16monitoredMasterStates:  [-1] _monitorNoStates */

  /* step 1: PIR High, waiting both, relays
  - passenger at box 1 (this box) */
  // std::array<uint16_t, 4> _arrMonitor_202_203 {202, 203};
  // stepsArray[1] = {6, 60, 0, 6, 12, 6/*repeat once*/, _arrMonitor_202_203, bxStateColl._IRStates};
  /* boxState: 6 - PIR High, waiting both, duration: 60 seconds, laserSequence: 0 - relays,
    onIRTrigger: apply state 6 (repeat), onMeshTrigger: 12 (Mesh High, waiting mesh),
    onExpire: 6 (repeat)[-- TO BE IMPROVED: repeat once], 
    _ui16monitoredMasterBoxesNodeNames: [202, 203],
    _i16monitoredMasterStates: _IRStates [6, 7, 8, 9] */

  /* step 2: Mesh High, waiting mesh, all Off
  - passenger at boxes 2 or 3, going to boxes 5 or 6 */
  // std::array<uint16_t, 4> _arrMonitor_205_206 {205, 206};
  // stepsArray[2] = {12, 60, 5, -1, 12, 12, _arrMonitor_205_206, bxStateColl._IRStates};
  /* boxState: 12 - Mesh High, waiting mesh, duration: 60 seconds, laserSequence: 5 - all Off,
    onIRTrigger: -1, onMeshTrigger: 12 (repeat Mesh High, waiting mesh),
    onExpire: 12 (repeat), 
    _ui16stepMasterBoxName: [205, 206],
    _i16monitoredMasterStates: _IRStates [6, 7, 8, 9] */

  /* step 3: Mesh High, waiting mesh, relays
  - passenger at boxes 5 or 6, going between boxes 5 and 6 */
  // stepsArray[3] = {12, -1, 0, -1, 11, 12, _arrMonitor_202_203, bxStateColl._IRStates};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, laserSequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), 
    _ui16stepMasterBoxName: [202, 203],
    _i16monitoredMasterStates: _IRStates [6, 7, 8, 9] */

  /* step 4: Mesh High, waiting mesh, relays
  - passenger at boxes 5 or 6, going to box 4 */
  // stepsArray[4] = {12, -1, 0, -1, 11, 12, _arrMonitor_202_203, bxStateColl._IRStates};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, laserSequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), 
    _ui16stepMasterBoxName: [202, 203],
    _i16monitoredMasterStates: _IRStates [6, 7, 8, 9] */

  /* step 5: Mesh High, waiting mesh, relays
  - passenger at box 4, going to box 2 or 3 */
  // stepsArray[5] = {12, -1, 0, -1, 11, 12, _arrMonitor_202_203, bxStateColl._IRStates};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, laserSequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), 
    _ui16stepMasterBoxName: [202, 203],
    _i16monitoredMasterStates: _IRStates [6, 7, 8, 9] */

  /* step 6: Mesh High, IR interrupt, relays
  - passenger at boxes 2 or 3, going to box 1 */
  // stepsArray[6] = {11, -1, 0, 9, 11, 11, bxStateColl._monitorNoMaster, bxStateColl._monitorNoStates};
  /* boxState: 11 - Mesh High, waiting IR, duration: -1 - infinite, laserSequence: 0 - relays,
    onIRTrigger: 9 (IR high, no interrupt), onMeshTrigger: 11 (repeat),
    onExpire: 11 (repeat once), 
    _ui16stepMasterBoxName: [254] (_monitorNoMaster),
    _i16monitoredMasterStates:  [-1] _monitorNoStates */

  /* step 7: IR High, no interrupt, relays
  - passenger at boxes 2 or 3, going to box 1 */
  // stepsArray[7] = {9, -1, 0, -1, -1, 9, bxStateColl._monitorNoMaster, bxStateColl._monitorNoStates};
  /* boxState: 9 - IR High, no interrupt, duration: -1 - infinite, laserSequence: 0 - relays,
    onIRTrigger: -1 (IR high, no interrupt), onMeshTrigger: -1 (none),
    onExpire: 9 (repeat once), 
    _ui16stepMasterBoxName: [254] (_monitorNoMaster), 
    _i16monitoredMasterStates:  [-1] _monitorNoStates */

  mySpiffs __mySpiffs;
  i16maxStepIndexNb = __mySpiffs.numberOfLinesInFile(stepFileName);
  
  tPreloadNextStep.set(0, 1, [&](){ return _tcbPreloadNextStep(); }, NULL, NULL);
  tPreloadNextStep.restart();

  Serial.println("stepCollection::stepCollection(): ending");
}



void stepCollection::reset() {
  ui16stepCounter = 0;
  _preloadNextStep(ui16stepCounter);
}



void stepCollection::_tcbPreloadNextStep() {
  Serial.printf("stepCollection::_tcbPreloadNextStep(): starting\n");
  // read next step values from the file system
  _preloadNextStep(ui16stepCounter);
  Serial.printf("stepCollection::_tcbPreloadNextStep(): ending\n");
}



void stepCollection::_preloadNextStep(uint16_t _ui16stepCounter){
  Serial.printf("stepCollection::_preloadNextStep: Reading file: %s\r\n", stepFileName);

  mySpiffs __mySpiffs;
  if (!(__mySpiffs.readCollectionItemParamsInFile(stepFileName, _ui16stepCounter))) {
    return;
  }

  // DeserializationError err = deserializeJson(_jdStep, _cStep);
  // if (err) {
  //   Serial.print(F("stepCollection::_preloadNextStep: deserializeJson() failed: "));
  //   Serial.println(err.c_str());
  //   return;
  // }

  // Get a reference to the root object
  JsonObject _joStep = __mySpiffs._jdItemParams.as<JsonObject>();

  _preloadNextStepFromJSON(_joStep);
}




void stepCollection::_preloadNextStepFromJSON(const JsonObject& _joStep) {
  Serial.println("stepCollection::_preloadNextStepFromJSON: starting");
  // Load the the monitored master states into an std::array
  // uint16_t _i = 0;
  std::array<int16_t, 4> _i16monitoredMasterStates = {};
  _parseJsonArray(_joStep, "_i16mastStates", _i16monitoredMasterStates);
  // uint16_t _i = 0;
  // for (auto _it : _i16monitoredMasterStates) {
  //   Serial.printf("stepCollection::_preloadNextStepFromJSON: _i16monitoredMasterStates[%u]: [%i]\n", _i, _it);
  //   _i++;
  // }

  // Load the the monitored master names into an std::array
  // _i = 0;
  std::array<uint16_t, 4> _ui16monitoredMasterBoxesNodeNames = {};
  _parseJsonArray(_joStep, "_ui16mastBxsNN", _ui16monitoredMasterBoxesNodeNames);
  // _i = 0;
  // for (auto _it : _ui16monitoredMasterBoxesNodeNames) {
  //   Serial.printf("stepCollection::_preloadNextStepFromJSON: _ui16monitoredMasterBoxesNodeNames[%u]: [%u]\n", _i, _it);
  //   _i++;
  // }

  // Load the next step into a step instance
  nextStep = {
    // _i16stepBoxStateNb(__i16stepBoxStateNb),
    _joStep["_i16BoxStateNb"].as<int16_t>(),
    // _i16StateDuration(__i16StateDuration),
    _joStep["_i16StDur"].as<int16_t>(),
    // _ui16AssociatedSequence(__ui16AssociatedSequence),
    _joStep["_ui16AssSeq"].as<uint16_t>(),
    // _i16onIRTrigger(__i16onIRTrigger),
    _joStep["_i16onIR"].as<int16_t>(),
    // _i16onMeshTrigger(__i16onMeshTrigger),
    _joStep["_i16onMesh"].as<int16_t>(),
    // _i16onExpire(__i16onExpire),
    _joStep["_i16onExp"].as<int16_t>(),
    // _ui16monitoredMasterBoxesNodeNames(__ui16monitoredMasterBoxesNodeNames),
    _ui16monitoredMasterBoxesNodeNames,
    // _i16monitoredMasterStates(__i16monitoredMasterStates),
    _i16monitoredMasterStates
  };
}


template<typename ARRAY>
void stepCollection::_parseJsonArray(const JsonObject& _joStep, const char * key, ARRAY & _array)
{
  // Serial.println("stepCollection::_parseJsonArray(): starting");
  uint16_t _i = 0;
  // int16_t _firstJsonArrayElt = _joStep[key][0].as<int16_t>();
  // // Serial.print("stepCollection::_parseJsonArray(): _firstJsonArrayElt: ");Serial.print(_firstJsonArrayElt);Serial.println();
  // // Serial.printf("stepCollection::_parseJsonArray(): _firstJsonArrayElt == _nullValueConvention: [%s]\n", ((_firstJsonArrayElt == _nullValueConvention) ? "true" : "false"));
  // if (_firstJsonArrayElt == _nullValueConvention) {
  //   for (_i = 0; _i < _array.size(); _i++) {
  //     _array.at(_i) = _firstJsonArrayElt;
  //   }
  //   return;
  // }
  for (JsonVariant _it : _joStep[key].as<JsonArray>()) {
    _array.at(_i) = _it;
    _i++;
  }
}













///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// boxState class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////


// Constructors
/** default constructor */
boxState::boxState()
{
  ui16monitoredMasterBoxesNodeNames = {};
  i16monitoredMasterStates = {};
}

/** "little" constructor */
boxState::boxState(const int16_t _i16Duration, 
  const uint16_t _ui16AssociatedSequence, 
  const int16_t _i16onIRTrigger, 
  const int16_t _i16onMeshTrigger, 
  const int16_t _i16onExpire) 
    : i16Duration(_i16Duration), 
    ui16AssociatedSequence(_ui16AssociatedSequence), 
    i16onIRTrigger(_i16onIRTrigger), 
    i16onMeshTrigger(_i16onMeshTrigger), 
    i16onExpire(_i16onExpire)
{
  ui16monitoredMasterBoxesNodeNames = {};
  i16monitoredMasterStates = {};
}

/** "full blown" constructor */
boxState::boxState(const int16_t _i16Duration, 
  const uint16_t _ui16AssociatedSequence, 
  const int16_t _i16onIRTrigger, 
  const int16_t _i16onMeshTrigger, 
  const int16_t _i16onExpire,
  std::array<uint16_t, 4> _ui16monitoredMasterBoxesNodeNames,
  std::array<int16_t, 4> _i16monitoredMasterStates)
    : i16Duration(_i16Duration), 
    ui16AssociatedSequence(_ui16AssociatedSequence), 
    i16onIRTrigger(_i16onIRTrigger), 
    i16onMeshTrigger(_i16onMeshTrigger), 
    i16onExpire(_i16onExpire), 
    ui16monitoredMasterBoxesNodeNames(_ui16monitoredMasterBoxesNodeNames),
    i16monitoredMasterStates(_i16monitoredMasterStates)
{
  /** TODO: for the moment, not doing anything with the following tests.
   *        in the future:
   *        - add i16onMasterIRTrigger property to boxState to handle the 
   *        masterBox(es) IR signals;
   *        - think about what to do with the _MeshStates (for the moment, not in use) */
  /**
   * if (__i16monitoredMasterStates == boxState::_IRStates) {
   *   // do sthg smart
   *  }
   *  if (__i16monitoredMasterStates == boxState::_MeshStates) {
   *   // do sthg smart
   *  } */
}

























///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// boxStateCollection class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void (boxStateCollection::*restartPBS)() = nullptr;  

/** Constructors */
/** default constructor */
boxStateCollection::boxStateCollection(
    ControlerBox & __thisCtrlerBox,
    myMeshViews & __thisMeshViews
    // void (*_sendCurrentBoxState)(const int16_t _i16CurrentStateNbr)
  ):
  boxStatesArray{},
  _thisCtrlerBox(__thisCtrlerBox),
  ui16Mode(0),
  // sendCurrentBoxState(_sendCurrentBoxState),
  _thisMeshViews(__thisMeshViews),
  _stepColl(),
  _laserSequences(_thisMeshViews),
  _monitorNoMaster({254}),
  _monitorNoStates({-1}),
  _IRStates({6, 7, 8, 9}),
  _MeshStates({10, 11, 12, 13}),
  _boxTargetState(0)
{
  Serial.println("boxStateCollection::initBoxStates(). starting.");
  /** Constructor signature (using "little constructor")
   * _i16Duration, _ui16AssociatedSequence, _i16onIRTrigger, _i16onMeshTrigger, _i16onExpire */

  // ********* TECHNICAL STATES ***********************************************
  // ----- 0, 1, 2
  // --> STATE 0: manual / off
  /** {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = -1, onMeshTrigger = -1, onExpire = 0 (repeat)} */
  boxStatesArray.at(0) = {-1, 5, -1, -1, 0};
  // Serial.println("void boxStateCollection::_initBoxStates(). bxStateColl.boxStatesArray.at(0).i16Duration");
  // Serial.println(bxStateColl.boxStatesArray.at(0).i16Duration);

  // const uint16_t _i16Duration, const uint16_t _ui16AssociatedSequence
  // const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire

  // --> STATE 1: align lasers
  /** laserSequence "twins" for indefinite time, without interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 1 ("twins"), 
   *   onIRTrigger = -1, 
   *   onMeshTrigger = -1, 
   *   onExpire = 1 (repeat)} */
  boxStatesArray.at(1) = {-1, 1, -1, -1, 1};

  // --> STATE 2: pir startup waiting mesh
  /** laserSequence "twins" for 60 seconds, no interrupt from IR, interrupts from mesh */
  /** {Duration = 60 sec., AssocSeqce = 1 ("twins"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 2 (possible val: 0-2 12-13 with restrictions)
   *   (restrictions --> no subsequent IR before expiration of 60 seconds startup delay)),
   *   onExpire = 3 (repeat)} */
  boxStatesArray.at(2) = {60, 1, -1, 2, 3};


  // ********* WAITING STATES (3 STATES + STATE 0) ****************************
  // ----- 3, 4, 5, 0
  // --> STATE 3: Waiting Both
  /** laserSequence "all of" for indefinite time, IR and mesh interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 3 (repeat)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(3) = {-1, 5, 6, 10, 3}; 

  // --> STATE 4: Waiting IR
  /** laserSequence "all of" for indefinite time, IR interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (repeat)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(4) = {-1, 5, 6, -1, 4}; 

  // --> STATE 5: Waiting Mesh
  /** laserSequence "all of" for indefinite time, mesh interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 5 (repeat)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(5) = {-1, 5, -1, 10, 5}; 


  // ********* PIR HIGH STATES (4 STATES DEPENDING ON THEIR INTERRUPTS) *******
  // ----- 6, 7, 8, 9
  // --> STATE 6: PIR High Both interrupt
  /** laserSequence "relays" for 120 seconds, IR and mesh interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 3 (fall back to waiting both)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(6) = {120, 0, 6, 10, 3}; 

  // --> STATE 7: PIR High IR interrupt
  /** laserSequence "relays" for 120 seconds, IR interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(7) = {120, 0, 6, -1, 4}; 
  /** laserSequence "relays" for 2 minutes with "interrupt/restart" triggers 
   * from IR only */

  // --> STATE 8: PIR High Mesh interrupt
  /** laserSequence "relays" for 120 seconds, mesh interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 5 (fall back to waiting mesh)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(8) = {120, 0, -1, 10, 5}; 

  // --> STATE 9: PIR High no interrupt
  /** laserSequence "relays" for 120 seconds, no interrupt.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(9) = {120, 0, -1, -1, 4}; 


  // ********* MESH HIGH STATES (4 STATES DEPENDING ON THEIR INTERRUPTS) ******
  // ----- 10, 11, 12, 13
  // --> STATE 10: Mesh High Both interrupt
  /** laserSequence "relays" for 120 seconds, IR and mesh interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 3 (fall back to waiting both)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(10) = {120, 0, 6, 10, 3}; 

  // --> STATE 11: Mesh High IR interrupt
  /** laserSequence "relays" for 120 seconds, IR interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(11) = {120, 0, 6, -1, 4}; 

  // --> STATE 12: Mesh High Mesh interrupt
  /** laserSequence "relays" for 120 seconds, mesh interrupt.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 5 (fall back to waiting mesh)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(12) = {120, 0, -1, 10, 5/*possible val: 0 3-13*/}; 
  /** laserSequence "relays" for 2 minutes with "interrupt/restart" triggers 
   * from mesh only */

  // --> STATE 13: mesh High no interrupt
  /** laserSequence "relays" for 120 seconds, no interrupt.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStatesArray.at(13) = {120, 0, -1, -1, 4}; 

  // Set Task tPlayBoxState
  tPlayBoxState.set(0, 1, NULL, [&](){ return _oetcbPlayBoxState();}, [&](){ return _odtcbPlayBoxState();});

  restartPBS = &boxStateCollection::_restartTaskPlayBoxState;

  Serial.println("boxStateCollection::initBoxStates(). over.\n");
}








//////////////////////////////////////////////
// Switch to Step Controlled Mode
//////////////////////////////////////////////
void boxStateCollection::toggleStepControlled(uint16_t _ui16Mode) {
  Serial.println("void boxStateCollection::toggleStepControlled(). starting.");
  ui16Mode = _ui16Mode;
  _stepColl.reset();
  if (ui16Mode) {
    restartPBS = &boxStateCollection::_restartTaskPlayBoxStateInStepControlledMode;
  } else {
    restartPBS = &boxStateCollection::_restartTaskPlayBoxState;
  }
  (*this.*restartPBS)();
  Serial.println("void boxStateCollection::toggleStepControlled(). ending.");
}










//////////////////////////////////////////////
// Task _tPlayBoxStates and its callbacks
//////////////////////////////////////////////
/** void boxStateCollection::_restartTaskPlayBoxStateInStepControlledMode()
 * 
 *  Called from boxStateCollection::_setBoxTargetState(const short __boxTargetState) only.
*/
void boxStateCollection::_restartTaskPlayBoxStateInStepControlledMode() {
  // Serial.println("void boxStateCollection::_restartTaskPlayBoxStateInStepControlledMode(). starting.");
  if (_stepColl.i16maxStepIndexNb > -1) {
    /** 1. configure the params for the next boxState. */
    _applyNextStep();
    if ((_stepColl.ui16stepCounter < _stepColl.i16maxStepIndexNb)) {
      /** 2. increment the step counter. */
      _stepColl.ui16stepCounter = _stepColl.ui16stepCounter + 1;
      /** 3. preload the next step from flash memory. */
      _stepColl.tPreloadNextStep.restart();
    }
  }
  /** 4. call the generic _restartTaskPlayBoxState()*/
  _restartTaskPlayBoxState();
}



/** void boxStateCollection::_applyNextStep()
 * 
 *  Applies the next step to the relevant boxState for such next step
 * 
 *  Called from boxStateCollection::_restartTaskPlayBoxStateInStepControlledMode() only.
*/
void boxStateCollection::_applyNextStep() {
  step & _nextStep = _stepColl.nextStep;
  boxState _nextBoxState{
    _nextStep._i16StateDuration,
    _nextStep._ui16AssociatedSequence,
    _nextStep._i16onIRTrigger,
    _nextStep._i16onMeshTrigger,
    _nextStep._i16onExpire,
    _nextStep._ui16monitoredMasterBoxesNodeNames,
    _nextStep._i16monitoredMasterStates
  };
  boxStatesArray.at(_nextStep._i16stepBoxStateNb) = _nextBoxState;
}



/** void boxStateCollection::_restartTaskPlayBoxState()
 * 
 *  Called from boxStateCollection::_setBoxTargetState(const short __boxTargetState) only.
*/
void boxStateCollection::_restartTaskPlayBoxState() {
  // Serial.println("void boxStateCollection::_restartTaskPlayBoxState(). starting.");
  // Serial.print("void boxStateCollection::_restartTaskPlayBoxState(). Iteration:");
  // Serial.println(tPlayBoxStates.getRunCounter());

  /** If the targetState has been reset, start playing the corresponding state:
   *  1. set the duration to stay in the new boxState (by setting the aInterval
   *     of the "children" Task tPlayBoxState; tPlayBoxState.setInterval), to
   *     the i16Duration of the target boxState (bxStateColl.boxStatesArray.at(_)oxTargetState].i16Duration);
   *  2. set the i16BoxActiveState property (and related properties) of this box;
   *  3. restart/enable the "children" Task tPlayBoxState.*/

  // 1. Set the duration of Task tPlayBoxState
  // Serial.print("void boxStateCollection::_restartTaskPlayBoxState() bxStateColl.boxStatesArray.at(_)oxTargetState].i16Duration: "); Serial.println(bxStateColl.boxStatesArray.at(_)oxTargetState].i16Duration);
  tPlayBoxState.setInterval(_ulCalcInterval(boxStatesArray.at(_boxTargetState).i16Duration));
  // Serial.print("void boxStateCollection::_restartTaskPlayBoxState() tPlayBoxState.getInterval(): "); Serial.println(tPlayBoxState.getInterval());

  // 2. Set the i16BoxActiveState of _thisCtrlerBox (ControllerBox) to the _boxTargetState
  _thisCtrlerBox._setBoxActiveState(_boxTargetState, globalBaseVariables.laserControllerMesh.getNodeTime());
  // Serial.println("void boxStateCollection::_restartTaskPlayBoxState() tPlayBoxState about to be enabled");

  // 3. Restart tPlayBoxState
  tPlayBoxState.restartDelayed();
  // Serial.println("void boxStateCollection::_restartTaskPlayBoxState() tPlayBoxState enabled");
  // Serial.print("void boxStateCollection::_restartTaskPlayBoxState() tPlayBoxState.getInterval(): ");Serial.println(tPlayBoxState.getInterval());
  // Serial.println("*********************************************************");


  // Serial.println("void boxStateCollection::_restartTaskPlayBoxState(). over.");
};










/*
  tPlayBoxState starts and stops new boxStates.
  It iterates only once. It is enabled by _restartTaskPlayBoxState.
  Its main iteration is delayed until aInterval has expired. aInterval is set in
  _restartTaskPlayBoxState. Such interval is equal to the duration of the new boxState.

  Upon being enabled, its onEnable callback:
  1. looks for the new boxState number, stored in this ControlerBox's
  i16BoxActiveState property and set by _restart_tPlayBoxState.
  Using this number, its selects the currently active boxState:
  2. in the currently active boxState, it reads the associated laserSequence number in its properties;
  3. sets the new laserSequence to be played (by calling _laserSequences.playSequence()).

  It iterates only once and does not have a main callback.

  Upon expiration of the Task, its onDisable callback disables
  Task tPlayLaserSequence.
*/

bool boxStateCollection::_oetcbPlayBoxState(){
  Serial.println("boxStateCollection::_oetcbPlayBoxState(). starting.");
  // Serial.print("boxStateCollection::_oetcbPlayBoxState(). Box State Number: ");Serial.println(_thisCtrlerBox.i16BoxActiveState);

  // 1. select the currently active state
  boxState& _currentBoxState = boxStatesArray[_thisCtrlerBox.i16BoxActiveState];

  // 2. Select the desired laserSequence and play it in loop
  //    until tPlayBoxState expires, for the duration mentionned in the activeState
  _laserSequences.playSequence(_currentBoxState.ui16AssociatedSequence);
  // _laserSequences.playSequence(
  //   _laserSequences.sequencesArray.at(_currentBoxState.ui16AssociatedSequence));
  
  // 3. Signal the change of state to the mesh
  _thisMeshViews.sendBoxState(_thisCtrlerBox.i16BoxActiveState);
  // if (sendCurrentBoxState != nullptr) {
  //   sendCurrentBoxState(_thisCtrlerBox.i16BoxActiveState);
  // }

  Serial.println("boxStateCollection::_oetcbPlayBoxState(). over.");
  return true;
}




/* Upon disabling the task which plays a given boxState,
  (i) disable the associated laserSequence player; and
  (ii) if the state which was being played was not the default state,
  set it to its default state.
*/
void boxStateCollection::_odtcbPlayBoxState(){
  Serial.println("boxStateCollection::_odtcbPlayBoxState(). starting.");
  // Serial.print("boxStateCollection::_odtcbPlayBoxState() tPlayBoxState.getInterval(): ");
  // Serial.println(tPlayBoxState.getInterval());

  boxState& _currentBoxState = boxStatesArray[_thisCtrlerBox.i16BoxActiveState];

  // 1. Disable the associated laserSequence player
  _laserSequences.setStopCallbackForTPlayLaserSequence();
  _laserSequences.tPlayLaserSequence.disable();
  // Serial.println("boxStateCollection::_odtcbPlayBoxState(): _thisCtrlerBox i16BoxActiveState number");
  // Serial.println(_thisCtrlerBox.i16BoxActiveState);
  // Serial.println("boxStateCollection::_odtcbPlayBoxState(): _boxTargetState");
  // Serial.println(_boxTargetState);

  // 2. Start the following state (timer interrupt)
  // or reset the boxState to default if no following state
  if (_currentBoxState.i16onExpire != -1) {
    _setBoxTargetState(_currentBoxState.i16onExpire);
  } else {
    _setBoxTargetState(_thisCtrlerBox.sBoxDefaultState);

  }
  Serial.println("boxStateCollection::_odtcbPlayBoxState(). over.");
}





// _setBoxTargetState is the central entry point to request a boxState change.
void boxStateCollection::_setBoxTargetState(const short __boxTargetState) {
  Serial.println("boxStateCollection::_setBoxTargetState(). starting.");
  Serial.printf("boxStateCollection::_setBoxTargetState(). __boxTargetState (passed in parameters): %u\n", __boxTargetState);
  _boxTargetState = __boxTargetState;
  (*this.*restartPBS)();
  Serial.printf("boxStateCollection::_setBoxTargetState(). _boxTargetState (boxStateCollection instance variable): %u\n", _boxTargetState);
  Serial.println("boxStateCollection::_setBoxTargetState(). over.");
};



//////////////////////////////
// helpers
//////////////////////////////
unsigned long boxStateCollection::_ulCalcInterval(int16_t _i16IntervalInS) {
  if (_i16IntervalInS == -1) {
    return 4294967290;
  }
  return (_i16IntervalInS * 1000);
}

