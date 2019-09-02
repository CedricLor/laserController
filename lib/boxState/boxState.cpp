/*
  boxState.cpp - precoded boxes states that will trigger various configuration of the box for various durations
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

/**
  Brief:
  - The boxState class provides a series of templates for states of this Controlerbox.
  - The step class provides a parameters for boxStates:
    - duration;
    (- number of iterations);
    - states to go to upon occurence of events:
      - signal from IR
      - signal from Mesh
      - expiration
  - The signal class provides an interface to the boxState class for events registered in
    the ControlerBox class.
  - boxState::_setBoxTargetState() is the only contact point for the signal class.
  - tPlayBoxState starts and stops the boxState and underlying sequence, depending
    on the settings (duration, associated sequence number) of the currently active
    boxState, and upon request from _setBoxTargetState().
  - boxState::_setBoxTargetState() parameterize tPlayBoxState before starting it.
  - upon being re-enabled, tPlayBoxState sends a status message to the mesh.
*/


/*
  TODO DEBUG PROBLEMS:
  VERY HIGH:
  II. SPIFFS: make the call to split files in main.cpp smarter: detect
  if there are any files to split (file which name would contain the special
  '-' character).

  MEDIUM:
  I. Create mesh and web stack for:
  - sequences
  - notes/tones
  - steps
  - steps mode switch
  II. mySpiffs and steps and sequences:
  - generalize mySpiffs step logic to sequences and notes
  - a lot of refactoring to do

  LOW:
  II. BoxState and Steps
    B. Implement array of masterBoxes
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
// STEP class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

step step::steps[8];


step::step() {

}


step::step(int16_t __i16stepBoxStateNb,
  int16_t __i16StateDuration,
  uint16_t __ui16AssociatedSequence,
  int16_t __i16onIRTrigger,
  int16_t __i16onMeshTrigger,
  int16_t __i16onExpire,
  uint16_t __ui16stepMasterBoxName,
  std::array<int16_t, 4> __i16monitoredMasterStates
)
    : _i16stepBoxStateNb(__i16stepBoxStateNb),
    _i16StateDuration(__i16StateDuration),
    _ui16AssociatedSequence(__ui16AssociatedSequence),
    _i16onIRTrigger(__i16onIRTrigger),
    _i16onMeshTrigger(__i16onMeshTrigger),
    _i16onExpire(__i16onExpire),
    _ui16stepMasterBoxName(__ui16stepMasterBoxName),
    _i16monitoredMasterStates(__i16monitoredMasterStates)
{
}


Task step::tPreloadNextStep(0, 1, &_tcbPreloadNextStep, NULL/*&mns::myScheduler*/, false, NULL, NULL);

void step::_tcbPreloadNextStep() {
  mySpiffs _mySpiffs;
  // read next step values from the file system
  char _cNodeName[4];
  snprintf(_cNodeName, 4, "%u", thisBox.ui16NodeName);
  _mySpiffs.readJSONObjLineInFile("/sessions.json", step::_preloadNextStepFromJSON, boxState::ui16stepCounter, _cNodeName);

  // load the values in memory as variables into the next step
  // steps[boxState::ui16stepCounter] = {
  //   /*set _i16stepBoxStateNb*/,
  //   /*set _i16StateDuration*/,
  //   /*_ui16AssociatedSequence*/,
  //   /*_i16onIRTrigger*/,
  //   /*_i16onMeshTrigger*/,
  //   /*_i16onExpire*/,
  //   /*_ui16stepMasterBoxName*/
  //   /*_i16monitoredMasterStates*/
  // };
}

void step::_preloadNextStepFromJSON(JsonObject& _joStep) {
  // step &_nextStep = step::steps[boxState::ui16stepCounter];
  // _nextStep._i16stepBoxStateNb = _joStep["_i16stepBoxStateNb"];
  // _nextStep._i16StateDuration = _joStep["_i16StateDuration"];
  // _nextStep._ui16AssociatedSequence = _joStep["_ui16AssociatedSequence"];
  // _nextStep._i16onIRTrigger = _joStep["_i16onIRTrigger"];
  // _nextStep._i16onMeshTrigger = _joStep["_i16onMeshTrigger"];
  // _nextStep._i16onExpire = _joStep["_i16onExpire"];
  // _nextStep._ui16stepMasterBoxName = _joStep["_ui16stepMasterBoxName"];

  std::array<int16_t, 4> _i16monitoredMasterStates = {};
  uint16_t _i = 0;
  for (int16_t _monitoredState : _joStep["_i16monitoredMasterStates"].as<JsonArray>()) {
    // _nextStep._i16monitoredMasterStates[_i] = _monitoredState;
    _i16monitoredMasterStates[_i] = _monitoredState;
    _i++;
  }
  step::steps[boxState::ui16stepCounter] = {
    // _i16stepBoxStateNb(__i16stepBoxStateNb),
    _joStep["_i16stepBoxStateNb"].as<int16_t>(),
    // _i16StateDuration(__i16StateDuration),
    _joStep["_i16StateDuration"].as<int16_t>(),
    // _ui16AssociatedSequence(__ui16AssociatedSequence),
    _joStep["_ui16AssociatedSequence"].as<uint16_t>(),
    // _i16onIRTrigger(__i16onIRTrigger),
    _joStep["_i16onIRTrigger"].as<int16_t>(),
    // _i16onMeshTrigger(__i16onMeshTrigger),
    _joStep["_i16onMeshTrigger"].as<int16_t>(),
    // _i16onExpire(__i16onExpire),
    _joStep["_i16onExpire"].as<int16_t>(),
    // _ui16stepMasterBoxName(__ui16stepMasterBoxName),
    _joStep["_ui16stepMasterBoxName"].as<uint16_t>(),
    // _i16monitoredMasterStates(__i16monitoredMasterStates)
    _i16monitoredMasterStates
  };
}





/** step::applyStep(): applies the values of this step to the relevant boxState */
void step::applyStep() {
  Serial.println("step::applyStep(). starting");
  // set the masterBoxName which state changes shall be monitored
  // Serial.println("step::applyStep(). debug master box name setter");
  // Serial.println(_ui16stepMasterBoxName);
  thisBox.updateMasterBoxName(_ui16stepMasterBoxName);

  boxState::boxStates[_i16stepBoxStateNb] = {
    _i16StateDuration,
    _ui16AssociatedSequence,
    _i16onIRTrigger,
    _i16onMeshTrigger,
    _i16onExpire,
    _i16monitoredMasterStates
  };

  // // get handy access to boxState for this step
  // // Serial.print("step::applyStep(). debug _i16stepBoxStateNb = ");
  // // Serial.println(_i16stepBoxStateNb);
  // boxState &_thisStepBoxState = boxState::boxStates[_i16stepBoxStateNb];

  // // set the duration of the boxState for this step
  //   // Serial.print("step::applyStep(). debug _thisStepBoxState.i16Duration = ");
  //   // Serial.println(_thisStepBoxState.i16Duration);
  //   // Serial.print("step::applyStep(). debug _i16StateDuration = ");
  //   // Serial.println(_i16StateDuration);
  // _thisStepBoxState.i16Duration = _i16StateDuration;

  // // set the sequence to play at this boxState
  // _thisStepBoxState.ui16AssociatedSequence = _ui16AssociatedSequence;

  // // set the boxState in which to go upon IR getting high
  // // (IR interrupt)
  // _thisStepBoxState.i16onIRTrigger = _i16onIRTrigger;

  // // set the boxState in which to go upon detection of a mesh relevant change
  // // (i.e. a change of the state of the masterBox)
  // // (mesh interrupt)
  // _thisStepBoxState.i16onMeshTrigger = _i16onMeshTrigger;

  // // set the boxState in which to go upon expiration of the boxState
  // // (timer interrupt)
  // _thisStepBoxState.i16onExpire = _i16onExpire;

  // // set the boxStates of the masterBox that will be relevant to
  // //  trigger a mesh interrupt
  // // (mesh interrupt)
  // _thisStepBoxState.ui16monitoredMasterStatesSize = _ui16monitoredMasterStatesSize;
  // _thisStepBoxState.i16monitoredMasterStates = _i16monitoredMasterStates;

  // preload the next step from memory
  step::tPreloadNextStep.enable();

  Serial.println("step::applyStep(). ending");
}



void step::initSteps() {
  Serial.println("step::initSteps(): starting");
  /* step 0: waiting IR, all Off
  - no passenger */
  steps[0] = {4, -1, 5, 6, -1, 4, 254, boxState::_monitorNoStates};
  // Serial.println("step::initSteps():");
  // Serial.println(steps[0]._ui16stepMasterBoxName);
  /* boxState: 4 - waiting IR, duration: -1 - infinite, sequence: 5 - all Off,
    onIRTrigger: apply state 6, onMeshTrigger: -1 (no mesh trigger),
    onExpire: 4 (no expiration, repeat), _ui16stepMasterBoxName: 254 */

  /* step 1: PIR High, waiting both, relays
  - passenger at box 1 (this box) */
  steps[1] = {6, 60, 0, 6, 12, 6/*repeat once*/, 202 /*and 203*/, boxState::_IRStates};
  /* boxState: 6 - PIR High, waiting both, duration: 60 seconds, sequence: 0 - relays,
    onIRTrigger: apply state 6 (repeat), onMeshTrigger: 12 (Mesh High, waiting mesh),
    onExpire: 6 (repeat)[-- TO BE IMPROVED: repeat once], _ui16stepMasterBoxName: 202 [-- TO BE IMPROVED: and 203] */

  /* step 2: Mesh High, waiting mesh, all Off
  - passenger at boxes 2 or 3, going to boxes 5 or 6 */
  steps[2] = {12, 60, 5, -1, 12, 12, 205 /*and 206*/, boxState::_IRStates};
  /* boxState: 12 - Mesh High, waiting mesh, duration: 60 seconds, sequence: 5 - all Off,
    onIRTrigger: -1, onMeshTrigger: 12 (repeat Mesh High, waiting mesh),
    onExpire: 12 (repeat), _ui16stepMasterBoxName: 205 [-- TO BE IMPROVED: and 203] */

  /* step 3: Mesh High, waiting mesh, relays
  - passenger at boxes 5 or 6, going between boxes 5 and 6 */
  steps[3] = {12, -1, 0, -1, 11, 12, 202 /* and 203*/, boxState::_IRStates};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 4: Mesh High, waiting mesh, relays
  - passenger at boxes 5 or 6, going to box 4 */
  steps[4] = {12, -1, 0, -1, 11, 12, 202 /* and 203*/, boxState::_IRStates};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 5: Mesh High, waiting mesh, relays
  - passenger at box 4, going to box 2 or 3 */
  steps[5] = {12, -1, 0, -1, 11, 12, 202 /* and 203*/, boxState::_IRStates};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 6: Mesh High, IR interrupt, relays
  - passenger at boxes 2 or 3, going to box 1 */
  steps[6] = {11, -1, 0, 9, 11, 11, 254, boxState::_monitorNoStates};
  /* boxState: 11 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: 9 (IR high, no interrupt), onMeshTrigger: 11 (repeat),
    onExpire: 11 (repeat once), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 7: IR High, no interrupt, relays
  - passenger at boxes 2 or 3, going to box 1 */
  steps[7] = {9, -1, 0, -1, -1, 9, 254, boxState::_monitorNoStates};
  /* boxState: 9 - IR High, no interrupt, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1 (IR high, no interrupt), onMeshTrigger: -1 (none),
    onExpire: 9 (repeat once), _ui16stepMasterBoxName: 254 */
  Serial.println("step::initSteps(): starting");
}








///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// boxState class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
short int boxState::_boxTargetState = 0;
const short int boxState::BOX_STATES_COUNT = 14;
boxState boxState::boxStates[BOX_STATES_COUNT];

const std::array<int16_t, 4> boxState::_monitorNoStates {-1};
const std::array<int16_t, 4> boxState::_IRStates {6, 7, 8, 9};
const std::array<int16_t, 4> boxState::_MeshStates {10, 11, 12, 13};

// ui16stepCounter starts at 0; will be used to set
// the params of the first step to be activated (after IR startup)
// step[0] in _restartPlayBoxState
uint16_t boxState::ui16stepCounter = 0;

// ui16Mode = 0 => mode automatic, boxStates use their default settings
// ui16Mode = 1 => step controlled, boxStates use the settings corresponding
// to the step they are embedded in
uint16_t boxState::ui16Mode = 0;



// Constructors
/** default constructor */
boxState::boxState()
{
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
  i16monitoredMasterStates = {};
}

/** "full blown" constructor */
boxState::boxState(const int16_t _i16Duration, 
  const uint16_t _ui16AssociatedSequence, 
  const int16_t _i16onIRTrigger, 
  const int16_t _i16onMeshTrigger, 
  const int16_t _i16onExpire,
  std::array<int16_t, 4> __i16monitoredMasterStates)
    : i16Duration(_i16Duration), 
    ui16AssociatedSequence(_ui16AssociatedSequence), 
    i16onIRTrigger(_i16onIRTrigger), 
    i16onMeshTrigger(_i16onMeshTrigger), 
    i16onExpire(_i16onExpire), 
    i16monitoredMasterStates(__i16monitoredMasterStates)
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





// boxState array initialiser
void boxState::initBoxStates() {
  Serial.println("boxState::_initBoxStates(). Starting.");
  /** Constructor signature (using "little constructor")
   * _i16Duration, _ui16AssociatedSequence, _i16onIRTrigger, _i16onMeshTrigger, _i16onExpire */

  // ********* TECHNICAL STATES ***********************************************
  // ----- 0, 1, 2
  // --> STATE 0: manual / off
  /** {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = -1, onMeshTrigger = -1, onExpire = 0 (repeat)} */
  boxStates[0] = {-1, 5, -1, -1, 0};
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].cName: ");
  // Serial.println(boxStates[0].cName);
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].i16Duration");
  // Serial.println(boxStates[0].i16Duration);

  // const uint16_t _i16Duration, const uint16_t _ui16AssociatedSequence
  // const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire

  // --> STATE 1: align lasers
  /** sequence "twins" for indefinite time, without interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 1 ("twins"), 
   *   onIRTrigger = -1, 
   *   onMeshTrigger = -1, 
   *   onExpire = 1 (repeat)} */
  boxStates[1] = {-1, 1, -1, -1, 1};

  // --> STATE 2: pir startup waiting mesh
  /** sequence "twins" for 60 seconds, no interrupt from IR, interrupts from mesh */
  /** {Duration = 60 sec., AssocSeqce = 1 ("twins"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 12 (possible val: 0-2 12-13 with restrictions 
   *   (restrictions --> no subsequent IR before expiration of 60 seconds startup delay)),
   *   onExpire = 3 (repeat)} */
  boxStates[2] = {60, 1, -1, 8, 3};


  // ********* WAITING STATES (3 STATES + STATE 0) ****************************
  // ----- 3, 4, 5, 0
  // --> STATE 3: Waiting Both
  /** sequence "all of" for indefinite time, IR and mesh interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 3 (repeat)(possible val: any except technical: 0 3-13)} */
  boxStates[3] = {-1, 5, 6, 10, 3}; 

  // --> STATE 4: Waiting IR
  /** sequence "all of" for indefinite time, IR interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (repeat)(possible val: any except technical: 0 3-13)} */
  boxStates[4] = {-1, 5, 6, -1, 4}; 

  // --> STATE 5: Waiting Mesh
  /** sequence "all of" for indefinite time, mesh interrupts.
   *  {Duration = -1 (indefinite), AssocSeqce = 5 ("all of"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 5 (repeat)(possible val: any except technical: 0 3-13)} */
  boxStates[5] = {-1, 5, -1, 10, 5}; 


  // ********* PIR HIGH STATES (4 STATES DEPENDING ON THEIR INTERRUPTS) *******
  // ----- 6, 7, 8, 9
  // --> STATE 6: PIR High Both interrupt
  /** sequence "relays" for 120 seconds, IR and mesh interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 3 (fall back to waiting both)(possible val: any except technical: 0 3-13)} */
  boxStates[6] = {120, 0, 6, 10, 3}; 

  // --> STATE 7: PIR High IR interrupt
  /** sequence "relays" for 120 seconds, IR interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStates[7] = {120, 0, 6, -1, 4}; 
  /** sequence "relays" for 2 minutes with "interrupt/restart" triggers 
   * from IR only */

  // --> STATE 8: PIR High Mesh interrupt
  /** sequence "relays" for 120 seconds, mesh interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 5 (fall back to waiting mesh)(possible val: any except technical: 0 3-13)} */
  boxStates[8] = {120, 0, -1, 10, 5}; 

  // --> STATE 9: PIR High no interrupt
  /** sequence "relays" for 120 seconds, no interrupt.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStates[9] = {120, 0, -1, -1, 4}; 


  // ********* MESH HIGH STATES (4 STATES DEPENDING ON THEIR INTERRUPTS) ******
  // ----- 10, 11, 12, 13
  // --> STATE 10: Mesh High Both interrupt
  /** sequence "relays" for 120 seconds, IR and mesh interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 3 (fall back to waiting both)(possible val: any except technical: 0 3-13)} */
  boxStates[10] = {120, 0, 6, 10, 3}; 

  // --> STATE 11: Mesh High IR interrupt
  /** sequence "relays" for 120 seconds, IR interrupts.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = 6 (possible val: IR High: 6-9), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStates[11] = {120, 0, 6, -1, 4}; 

  // --> STATE 12: Mesh High Mesh interrupt
  /** sequence "relays" for 120 seconds, mesh interrupt.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = 10 (possible val: Mesh High: 10-13), 
   *   onExpire = 5 (fall back to waiting mesh)(possible val: any except technical: 0 3-13)} */
  boxStates[12] = {120, 0, -1, 10, 5/*possible val: 0 3-13*/}; 
  /** sequence "relays" for 2 minutes with "interrupt/restart" triggers 
   * from mesh only */

  // --> STATE 13: mesh High no interrupt
  /** sequence "relays" for 120 seconds, no interrupt.
   *  {Duration = 120 seconds, AssocSeqce = 0 ("relays"), 
   *   onIRTrigger = -1 (possible val: no IR High: -1), 
   *   onMeshTrigger = -1 (possible val: no Mesh High: -1), 
   *   onExpire = 4 (fall back to waiting IR)(possible val: any except technical: 0 3-13)} */
  boxStates[13] = {120, 0, -1, -1, 4}; 

  Serial.println("boxState::_initBoxStates(). Ending.");
}





//////////////////////////////////////////////
// Switch to Step Controlled Mode
//////////////////////////////////////////////
void boxState::switchToStepControlled() {
  ui16Mode = 1;
  ui16stepCounter = 0;
  step::initSteps();
}



//////////////////////////////////////////////
// Task _tPlayBoxStates and its callbacks
//////////////////////////////////////////////
void boxState::_restartPlayBoxState() {
  // Serial.println("void boxState::_restartPlayBoxState(). Starting.");
  // Serial.print("void boxState::_restartPlayBoxState(). Iteration:");
  // Serial.println(tPlayBoxStates.getRunCounter());

  /** If the targetState has been reset, start playing the corresponding state:
   *  1. get the params for the new state in case we are in step controlled mode;
   *  2. set the duration to stay in the new boxState (by setting the aInterval
   *     of the "children" Task tPlayBoxState; tPlayBoxState.setInterval), to
   *     the i16Duration of the target boxState (boxStates[_boxTargetState].i16Duration);
   *  3. set the i16BoxActiveState property (and related properties) of this box;
   *  4. restart/enable the "children" Task tPlayBoxState.*/

  /** 1. If we are in step controlled mode (mode 1),
   *     configure the params of the new boxState. */
  if (ui16Mode == 1) {
    step::steps[ui16stepCounter].applyStep();
  }

  // 2. Set the duration of Task tPlayBoxState
  // Serial.print("void boxState::_restartPlayBoxState() boxStates[_boxTargetState].i16Duration: "); Serial.println(boxStates[_boxTargetState].i16Duration);
  tPlayBoxState.setInterval(_ulCalcInterval(boxStates[_boxTargetState].i16Duration));
  // Serial.print("void boxState::_restartPlayBoxState() tPlayBoxState.getInterval(): "); Serial.println(tPlayBoxState.getInterval());

  // 3. Set the i16BoxActiveState of thisBox in ControlerBox to the _boxTargetState
  thisBox.setBoxActiveState(_boxTargetState, laserControllerMesh.getNodeTime());
  // Serial.println("void boxState::_restartPlayBoxState() tPlayBoxState about to be enabled");

  // 4. Restart/enable tPlayBoxState
  tPlayBoxState.restartDelayed();
  // Serial.println("void boxState::_restartPlayBoxState() tPlayBoxState enabled");
  // Serial.print("void boxState::_restartPlayBoxState() tPlayBoxState.getInterval(): ");Serial.println(tPlayBoxState.getInterval());
  // Serial.println("*********************************************************");


  // Serial.println("void boxState::_restartPlayBoxState(). Ending.");
};


















/*
  tPlayBoxState starts and stops new boxStates.
  It iterates only once. It is enabled by _restartPlayBoxState.
  Its main iteration is delayed until aInterval has expired. aInterval is set in
  _restartPlayBoxState. Such interval is equal to the duration of the new boxState.

  Upon being enabled, its onEnable callback:
  1. looks for the new boxState number, stored in this ControlerBox's
  i16BoxActiveState property and set by _restart_tPlayBoxState.
  Using this number, its selects the currently active boxState:
  2. in the currently active boxState, it reads the associated sequence number in its properties;
  3. sets the new sequence to be played (by calling sequence::setActiveSequence());
  4. starts playing the sequence (by enabling the task sequence::tPlaySequenceInLoop.

  It iterates only once and does not have a main callback.

  Upon expiration of the Task, its onDisable callback disables
  Task tPlaySequenceInLoop.
*/
Task boxState::tPlayBoxState(0, 1, NULL, NULL/*&mns::myScheduler*/, false, &_oetcbPlayBoxState, &_odtcbPlayBoxState);


bool boxState::_oetcbPlayBoxState(){
  Serial.println("boxState::_oetcbPlayBoxState(). Starting.");
  // Serial.print("boxState::_oetcbPlayBoxState(). Box State Number: ");Serial.println(thisBox.i16BoxActiveState);

  // 1. select the currently active state
  boxState& _currentBoxState = boxStates[thisBox.i16BoxActiveState];

  // 2. Set the active sequence
  sequence::setActiveSequence(_currentBoxState.ui16AssociatedSequence);

  // 3. Enable the sequence player, to play the sequence in loop
  // until tPlayBoxState expires, for the duration mentionned in the activeState
  // Serial.println("boxState::_oetcbPlayBoxState() sequence::tPlaySequenceInLoop about to be enabled");
  sequence::tPlaySequenceInLoop.enable();

  // 4. Signal the change of state to the mesh
  myMeshViews __myMeshViews;
  __myMeshViews.statusMsg();

  // 5. If we are in step controlled mode (mode 1):
  // - increment the step counter; and
  // - enable the Task that will read the params for the next step
  if (ui16Mode == 1) {
    ui16stepCounter = ui16stepCounter + 1;
    step::tPreloadNextStep.restart();
  }

  Serial.println("boxState::_oetcbPlayBoxState(). Ending.");
  return true;
}


/* Upon disabling the task which plays a given boxState,
  (i) disable the associated sequence player; and
  (ii) if the state which was being played was not the default state,
  set it to its default state.
*/
void boxState::_odtcbPlayBoxState(){
  Serial.println("boxState::_odtcbPlayBoxState(). Starting.");
  // Serial.print("boxState::_odtcbPlayBoxState() tPlayBoxState.getInterval(): ");
  // Serial.println(tPlayBoxState.getInterval());

  boxState& _currentBoxState = boxStates[thisBox.i16BoxActiveState];

  // 1. Disable the associated sequence player
  sequence::tPlaySequenceInLoop.disable();
  // Serial.println("boxState::_odtcbPlayBoxState(): thisBox i16BoxActiveState number");
  // Serial.println(_thisBox.i16BoxActiveState);
  // Serial.println("boxState::_odtcbPlayBoxState(): _boxTargetState");
  // Serial.println(_boxTargetState);

  // 2. Start the following state (timer interrupt)
  // or reset the boxState to default if no following state
  if (_currentBoxState.i16onExpire != -1) {
    _setBoxTargetState(_currentBoxState.i16onExpire);
  } else {
    _setBoxTargetState(thisBox.sBoxDefaultState);

  }
  Serial.println("boxState::_odtcbPlayBoxState(). Ending.");
}





// _setBoxTargetState is the central entry point to request a boxState change.
void boxState::_setBoxTargetState(const short __boxTargetState) {
  Serial.println("boxState::_setBoxTargetState(). Starting.");
  Serial.printf("boxState::_setBoxTargetState(). __boxTargetState: %u\n", __boxTargetState);
  _boxTargetState = __boxTargetState;
  _restartPlayBoxState();
  Serial.printf("boxState::_setBoxTargetState(). _boxTargetState: %u\n", _boxTargetState);
  Serial.println("boxState::_setBoxTargetState(). Ending.");
};



//////////////////////////////
// helpers
//////////////////////////////
unsigned long boxState::_ulCalcInterval(int16_t _i16IntervalInS) {
  if (_i16IntervalInS == -1) {
    return 4294967290;
  }
  return (_i16IntervalInS * 1000);
}

