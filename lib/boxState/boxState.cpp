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

/*
  Brief:
  - _tPlayBoxStates detects boxState change request coming from the outside, depending
  on the settings (IR Trigger, Mesh Trigger) of the currently active boxState.
  - _setBoxTargetState stores the change requests from _tPlayBoxStates.
  - tPlayBoxState starts and stops the boxState and underlying sequence, depending
  on the settings (duration, associated sequence number) of the currently active
  boxState, and upon request from its parent Task _tPlayBoxStates.
*/


/*
  DEBUG PROBLEMS:
  VERY HIGH:
  I. OTA for all
  II. SPIFFS: make the call to split files in main.cpp smarter: detect
  if there are any files to split (file which name would contain the special
  '-' characteur).

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
  I. Global
    - setting and use of gui16ControllerBoxPrefix
    -> make them two different values and adapt the whole code
    -> this is a very long shot:
      - gui16ControllerBoxPrefix: 47 results in 8 files
  II. BoxState and Steps
    A. ui16MasterBoxName
      - casting ui16MasterBoxName
      - calculations of ui16MasterBoxName index which result in negative numbers
      - initial setting of ControlerBox.ui16MasterBoxName
      - empty class constructor for steps and boxStates -> should initialize
      each parameter to avoid leaving dump values
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
  VIII. mySavedPrefs: refacto -> mySavedPrefs shall be loaded before ControlerBox
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
  uint16_t __ui16stepMasterBoxName
)
    : _i16stepBoxStateNb(__i16stepBoxStateNb),
    _i16StateDuration(__i16StateDuration),
    _ui16AssociatedSequence(__ui16AssociatedSequence),
    _i16onIRTrigger(__i16onIRTrigger),
    _i16onMeshTrigger(__i16onMeshTrigger),
    _i16onExpire(__i16onExpire),
    _ui16stepMasterBoxName(__ui16stepMasterBoxName)
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
  // };
}

void step::_preloadNextStepFromJSON(JsonObject& _joStep) {
  step &_nextStep = step::steps[boxState::ui16stepCounter];
  _nextStep._i16stepBoxStateNb = _joStep["_i16stepBoxStateNb"];
  _nextStep._i16StateDuration = _joStep["_i16StateDuration"];
  _nextStep._ui16AssociatedSequence = _joStep["_ui16AssociatedSequence"];
  _nextStep._i16onIRTrigger = _joStep["_i16onIRTrigger"];
  _nextStep._i16onMeshTrigger = _joStep["_i16onMeshTrigger"];
  _nextStep._i16onExpire = _joStep["_i16onExpire"];
  _nextStep._ui16stepMasterBoxName = _joStep["_ui16stepMasterBoxName"];
}






void step::applyStep() {
  Serial.println("step::applyStep(). starting");
  // apply to the relevant boxState
  // get handy access to boxState for this step
    // Serial.print("step::applyStep(). debug _i16stepBoxStateNb = ");
    // Serial.println(_i16stepBoxStateNb);
  boxState &_thisStepBoxState = boxState::boxStates[_i16stepBoxStateNb];

  // set the duration of the boxState for this step
    // Serial.print("step::applyStep(). debug _thisStepBoxState.i16Duration = ");
    // Serial.println(_thisStepBoxState.i16Duration);
    // Serial.print("step::applyStep(). debug _i16StateDuration = ");
    // Serial.println(_i16StateDuration);
  _thisStepBoxState.i16Duration = _i16StateDuration;

  // set the sequence to play at this boxState
  _thisStepBoxState.ui16AssociatedSequence = _ui16AssociatedSequence;

  // set the boxState in which to go upon IR getting high
  // (IR interrupt)
  _thisStepBoxState.i16onIRTrigger = _i16onIRTrigger;

  // set the boxState in which to go upon detected a mesh relevant change
  // (i.e. a change of the masterBox states)
  // (mesh interrupt)
  _thisStepBoxState.i16onMeshTrigger = _i16onMeshTrigger;

  // set the boxState in which to go upon expiration of the boxState
  // (timer interrupt)
  _thisStepBoxState.i16onExpire = _i16onExpire;

  // set the masterBoxName which state changes shall be watched over
  // Serial.println("step::applyStep(). debug master box name setter");
  // Serial.println(_ui16stepMasterBoxName);
  thisBox.updateMasterBoxName(_ui16stepMasterBoxName);
  // _thisStepBoxState._ui16stepMasterBoxName = _ui16stepMasterBoxName;

  // preload the next step from memory
  step::tPreloadNextStep.enable();

  Serial.println("step::applyStep(). ending");
}



void step::initSteps() {
  Serial.println("step::initSteps(): starting");
  /* step 0: waiting IR, all Off
  - no passenger */
  steps[0] = {4, -1, 5, 6, -1, 4, 254};
  // Serial.println("step::initSteps():");
  // Serial.println(steps[0]._ui16stepMasterBoxName);
  /* boxState: 4 - waiting IR, duration: -1 - infinite, sequence: 5 - all Off,
    onIRTrigger: apply state 6, onMeshTrigger: -1 (no mesh trigger),
    onExpire: 4 (no expiration, repeat), _ui16stepMasterBoxName: 254 */

  /* step 1: PIR High, waiting both, relays
  - passenger at box 1 (this box) */
  steps[1] = {6, 60, 0, 6, 12, 6/*repeat once*/, 202 /*and 203*/};
  /* boxState: 6 - PIR High, waiting both, duration: 60 seconds, sequence: 0 - relays,
    onIRTrigger: apply state 6 (repeat), onMeshTrigger: 12 (Mesh High, waiting mesh),
    onExpire: 6 (repeat)[-- TO BE IMPROVED: repeat once], _ui16stepMasterBoxName: 202 [-- TO BE IMPROVED: and 203] */

  /* step 2: Mesh High, waiting mesh, all Off
  - passenger at boxes 2 or 3, going to boxes 5 or 6 */
  steps[2] = {12, 60, 5, -1, 12, 12, 205 /*and 206*/};
  /* boxState: 12 - Mesh High, waiting mesh, duration: 60 seconds, sequence: 5 - all Off,
    onIRTrigger: -1, onMeshTrigger: 12 (repeat Mesh High, waiting mesh),
    onExpire: 12 (repeat), _ui16stepMasterBoxName: 205 [-- TO BE IMPROVED: and 203] */

  /* step 3: Mesh High, waiting mesh, relays
  - passenger at boxes 5 or 6, going between boxes 5 and 6 */
  steps[3] = {12, -1, 0, -1, 11, 12, 202 /* and 203*/};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 4: Mesh High, waiting mesh, relays
  - passenger at boxes 5 or 6, going to box 4 */
  steps[4] = {12, -1, 0, -1, 11, 12, 202 /* and 203*/};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 5: Mesh High, waiting mesh, relays
  - passenger at box 4, going to box 2 or 3 */
  steps[5] = {12, -1, 0, -1, 11, 12, 202 /* and 203*/};
  /* boxState: 12 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: -1, onMeshTrigger: 11 (mesh high, waiting IR),
    onExpire: 12 (repeat until mesh trigger), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 6: Mesh High, IR interrupt, relays
  - passenger at boxes 2 or 3, going to box 1 */
  steps[6] = {11, -1, 0, 9, 11, 11, 254};
  /* boxState: 11 - Mesh High, waiting IR, duration: -1 - infinite, sequence: 0 - relays,
    onIRTrigger: 9 (IR high, no interrupt), onMeshTrigger: 11 (repeat),
    onExpire: 11 (repeat once), _ui16stepMasterBoxName: 202 [and 203] */

  /* step 7: IR High, no interrupt, relays
  - passenger at boxes 2 or 3, going to box 1 */
  steps[7] = {9, -1, 0, -1, -1, 9, 254};
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
bool boxState::_boxActiveStateHasBeenReset = 0;
const short int boxState::BOX_STATES_COUNT = 14;
boxState boxState::boxStates[BOX_STATES_COUNT];

// ui16stepCounter starts at 0; will be used to set
// the params of the first step to be activated (after IR startup)
// step[0] in _tPlayBoxStates main callback (_tcbPlayBoxStates)
// in its sub _restart_tPlayBoxState
uint16_t boxState::ui16stepCounter = 0;

// ui16Mode = 0 => mode automatic, boxStates use their default settings
// ui16Mode = 1 => step controlled, boxStates use the settings corresponding
// to the step they are embedded in
uint16_t boxState::ui16Mode = 0;



// Constructors
boxState::boxState() {
}


// boxState::boxState(const int16_t _i16Duration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire)
//     : i16Duration(_i16Duration), ui16AssociatedSequence(_ui16AssociatedSequence), i16onIRTrigger(_i16onIRTrigger), i16onMeshTrigger(_i16onMeshTrigger), i16onExpire(_i16onExpire)
// {
// }





// Initialisers
void boxState::_initBoxState(const int16_t _i16Duration, const uint16_t _ui16AssociatedSequence, const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire){
  // Serial.println("boxState::_initBoxState(). Starting.");
  i16Duration = _i16Duration;
  ui16AssociatedSequence = _ui16AssociatedSequence;
  i16onIRTrigger = _i16onIRTrigger;
  i16onMeshTrigger = _i16onMeshTrigger;
  i16onExpire = _i16onExpire;
  // Serial.println("boxState::_initBoxState(). Ending.");
};



void boxState::initBoxStates() {
  Serial.println("boxState::_initBoxStates(). Starting.");

  // manual / off
  boxStates[0]._initBoxState(1000, 5, -1, -1, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].cName: ");
  // Serial.println(boxStates[0].cName);
  // Serial.println("void boxState::_initBoxStates(). boxStates[0].i16Duration");
  // Serial.println(boxStates[0].i16Duration);

  // const uint16_t _i16Duration, const uint16_t _ui16AssociatedSequence
  // const int16_t _i16onIRTrigger, const int16_t _i16onMeshTrigger, const int16_t _i16onExpire

  // align lasers
  boxStates[1]._initBoxState(1000, 1, -1, -1, 1); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // pir startup waiting mesh
  boxStates[2]._initBoxState(60, 1, -1, 8/*0-1 8-9 with restriction*/, 3); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh

  // waiting both
  boxStates[3]._initBoxState(1000, 5, 6/*6-9*/, 10/*10-13*/, 3/*0 3-13*/); // sequence "all of" for indefinite time until trigger from either IR or mesh
  // waiting ir
  boxStates[4]._initBoxState(1000, 5, 6/*6-9*/, -1, 4/*0 3-13*/); // sequence "all of" for indefinite time until trigger from IR
  // waiting mesh
  boxStates[5]._initBoxState(1000, 5, -1, 10/*10-13*/, 5/*0 3-13*/); // sequence "all of" for indefinite time until trigger from mesh

  // pir High both interrupt
  boxStates[6]._initBoxState(120, 0, 6/*6-9*/, 10/*10-13*/, 3/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  // pir High ir interrupt
  boxStates[7]._initBoxState(120, 0, 6/*6-9*/, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  // pir High mesh interrupt
  boxStates[8]._initBoxState(120, 0, -1, 10/*10-13*/, 5/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  // pir High no interrupt
  boxStates[9]._initBoxState(120, 0, -1, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

  // mesh High both interrupt
  boxStates[10]._initBoxState(120, 0, 6/*6-9*/, 10/*10-13*/, 3/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  // mesh High ir interrupt
  boxStates[11]._initBoxState(120, 0, 6/*6-9*/, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  // mesh High mesh interrupt
  boxStates[12]._initBoxState(120, 0, -1, 10/*10-13*/, 5/*0 3-13*/); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  // mesh High no interrupt
  boxStates[13]._initBoxState(120, 0, -1, -1, 4/*0 3-13*/); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

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

/* Task _tPlayBoxStates:
    tPlayBoxStates starts at the end of the setup and
    iterates indefinitely.
    Every second:
    1. it checks whether some events may trigger a boxState change
    and if so, requests such boxState changes;
    2. it then resets the event catchers to their default values;
    3. finally, if a boxState change has been requested (by itself or
    by tPlayBoxState), it restarts tPlayBoxState.

    onEnable, it puts the box into IR Startup boxState (which is
    set to last for 1 minute).
*/
Task boxState::tPlayBoxStates(1000L, -1, &_tcbPlayBoxStates, NULL/*&mns::myScheduler*/, false, &_oetcbPlayBoxStates, NULL);

/*
  At each pass of tPlayBoxStates, _tcbPlayBoxStates() will check whether the
  following values have changed (the catchers):
  - thisBox.ui16hasLastRecPirHighTimeChanged (when the current boxState is set to react to signals from the PIR);
  - ControlerBoxes[PARENT].i16BoxActiveState (when the current boxState is set to react to signals from the mesh);
  - _boxActiveStateHasBeenReset;
  - _boxTargetState;
  Depending on the changes, it will:
  - either start a new boxState or extend the duration of the current boxState; or
  - reset the catchers' values to their origin value.
  If the catchers have not changed, nothing happens.
*/

void boxState::_tcbPlayBoxStates() {
  // Serial.println("void boxState::_tcbPlayBoxStates(). Starting.");
  // Serial.print("void boxState::_tcbPlayBoxStates(). Iteration:");
  // Serial.println(tPlayBoxStates.getRunCounter());

  // A. Analyse the signal catchers and set the box target state accordingly
  _setBoxTargetStateFromSignalCatchers();

  // B. Once read, reset all the signal catchers
  _resetSignalCatchers();

  // C. If the active state (actually, the targetState) has been reset, start playing
  // the corresponding state
  _restart_tPlayBoxState();
  // Serial.println("void boxState::_tcbPlayBoxStates(). Ending.");
};


// Upon tPlayBoxStates being enabled (at startup), the _boxTargetState is being
// changed to 2 (pir Startup).
bool boxState::_oetcbPlayBoxStates() {
  // Serial.println("void boxState::_oetcbPlayBoxStates(). Starting.");
  _setBoxTargetState(2); // 2 for pir Startup; upon enabling the task tPlayBoxStates, play the pirStartup boxState
  // Serial.println("void boxState::_oetcbPlayBoxStates(). Ending.");
  return true;
}





//////////////////////////////////////////////
// _tcbPlayBoxStates() sub functions
//////////////////////////////////////////////




/*
    _setBoxTargetStateFromSignalCatchers:
    1. reads whether the web, the IR or the mesh have been modified in a way
    that could require a change of boxState;
    2. if so, requests a boxState change by calling _setBoxTargetState()
*/
void boxState::_setBoxTargetStateFromSignalCatchers() {
  // Read the signal catchers and change the targetState accordingly

  // 1. Check the web signal catcher. If it has changed, set the new targetState
  // and return
  if (ControlerBox::valFromWeb != -1) {
    Serial.println("--------------------- valFromWeb ----------");
    _setBoxTargetState(ControlerBox::valFromWeb);
    return;
  }

  // give handy access to thisBox and the _currentBoxState
  boxState& _currentBoxState = boxStates[thisBox.i16BoxActiveState];

  /** look for the index number of the master box in the CB array, using  
   *  the node name (201, for instance) registered in thisBox (thisBox.ui16MasterBoxName). */
  uint16_t _ui16masterBoxIndex = ControlerBox::findIndexByNodeName(thisBox.ui16MasterBoxName);
  /** If the masterBoxIndex is equal to 254, the targeted masterBox registered in thisBox
   *  has not joined the mesh (or at least has not been in contact with thisBox).
   *  If it is equal to something else than 254, it has been in contact with thisBox
   *  and might have registered a given state to which thisBox is programmed to react. */
  if (_ui16masterBoxIndex != 254) {
    // select the relevant masterBox in the CB array
    ControlerBox& _masterBox = ControlerBoxes[_ui16masterBoxIndex];

    // 2. Check whether the current state has both IR and mesh triggers
    if (_currentBoxState._hasBothTriggers()) {

      /** check whether both triggers (IR and mesh) have been triggered (this will not catch the case 
       * where only one of them has been triggered). */
      if (thisBox.ui16hasLastRecPirHighTimeChanged && _currentBoxState._meshHasBeenTriggered(_masterBox)) {
        // if both have been triggered, resolve the conflict and change the boxState accordingly.
        _currentBoxState._resolveTriggersConflictAndAct(_masterBox);
        return;
      }
      /** If none or only one has been triggered, continue (we need to check
       *  whether the IR only or the mesh only has been triggered) */
    }

    // 3. If the current boxState has Mesh trigger and
    // its parent box has a state other than -1 and
    // its activeState has not been taken into account
    if (_currentBoxState.i16onMeshTrigger != -1){
      _currentBoxState._checkMeshTriggerAndAct();
    }
  }

  // 4. If the current boxState has IR trigger
  if (_currentBoxState.i16onIRTrigger != -1){
    _currentBoxState._checkIRTriggerAndAct();
    return;
  }
}




/*
  _resetSignalCatchers resets all the signals catchers
  to their initial values, once the new _boxTargetState
  has been set by _setBoxTargetStateFromSignalCatchers
*/
void boxState::_resetSignalCatchers() {
  ControlerBox::valFromWeb = -1;
  thisBox.ui16hasLastRecPirHighTimeChanged = 0;
  if (thisBox.ui16MasterBoxName != 254) {
    uint16_t _ui16masterBoxIndex = ControlerBox::findIndexByNodeName(thisBox.ui16MasterBoxName);
    if (_ui16masterBoxIndex != 254) {
      ControlerBoxes[_ui16masterBoxIndex].boxActiveStateHasBeenTakenIntoAccount = true;
    }
  }
}




/*
    _restart_tPlayBoxState() starts a new boxState, if
    the _boxActiveStateHasBeenReset has somehow been reset, and
    accomplishes a couple of other technical tasks.

    If the _boxActiveStateHasBeenReset, it will:
      1. reset the witness _boxActiveStateHasBeenReset to 0;
      2. get the params for the new state in case we are in step controlled mode;
      3. set the duration to stay in the new boxState (by setting the
      aInterval of the "children" Task tPlayBoxState; tPlayBoxState.setInterval), to
      the i16Duration of the target boxState (boxStates[_boxTargetState].i16Duration);
      4. set the i16BoxActiveState property (and related properties) of this box;
      5. restart/enable the "children" Task tPlayBoxState.
*/
void boxState::_restart_tPlayBoxState() {
  // if the _boxActiveStateHasBeenReset,
  if (_boxActiveStateHasBeenReset == 1) {
    // 1. Resets the witness to 0 (false)
    _boxActiveStateHasBeenReset = 0;

    // 2. If we are in step controlled mode (mode 1),
    // configure the params of the new boxState.
    if (ui16Mode == 1) {
      step::steps[ui16stepCounter].applyStep();
    }

    // 3. Set the duration of Task tPlayBoxState
    // Serial.print("void boxState::_tcbPlayBoxStates() boxStates[_boxTargetState].i16Duration: "); Serial.println(boxStates[_boxTargetState].i16Duration);
    tPlayBoxState.setInterval(_ulCalcInterval(boxStates[_boxTargetState].i16Duration));
    // Serial.print("void boxState::_tcbPlayBoxStates() tPlayBoxState.getInterval(): "); Serial.println(tPlayBoxState.getInterval());

    // 4. Set the i16BoxActiveState to the _boxTargetState
    thisBox.setBoxActiveState(_boxTargetState, laserControllerMesh.getNodeTime());
    // Serial.println("void boxState::_tcbPlayBoxStates() tPlayBoxState about to be enabled");

    // 5. Restart/enable tPlayBoxState
    tPlayBoxState.restartDelayed();
    // Serial.println("void boxState::_tcbPlayBoxStates() tPlayBoxState enabled");
    // Serial.print("void boxState::_tcbPlayBoxStates() tPlayBoxState.getInterval(): ");Serial.println(tPlayBoxState.getInterval());
    // Serial.println("*********************************************************");
  }
}



/*
    The following subs are helpers for _setBoxTargetStateFromSignalCatchers
*/
bool boxState::_hasBothTriggers() {
  return (i16onIRTrigger != -1
    && i16onMeshTrigger != -1);
}



void boxState::_checkIRTriggerAndAct() {
  // check whether IR has been triggered;
  // if so, set the boxTarget state accordingly
  if (thisBox.ui16hasLastRecPirHighTimeChanged) {
    Serial.println("--------------------- IR triggered ----------");
    _setBoxTargetState(i16onIRTrigger);
  }
}



void boxState::_checkMeshTriggerAndAct() {
  /** look for the index number of the master box in the CB array, using  
   *  the node name (201, for instance) registered in thisBox (thisBox.ui16MasterBoxName). */
  uint16_t _ui16masterBoxIndex = ControlerBox::findIndexByNodeName(thisBox.ui16MasterBoxName);
  /** If the masterBoxIndex is equal to 254, the targeted masterBox registered in thisBox
   *  has not joined the mesh (or at least has not been in contact with thisBox).
   *  If it is equal to something else than 254, it has been in contact with thisBox
   *  and might have registered a given state to which thisBox is programmed to react. */
  if (_ui16masterBoxIndex != 254) {
    // select the relevant masterBox in the CB array
    ControlerBox& _masterBox = ControlerBoxes[_ui16masterBoxIndex];
    if (_meshHasBeenTriggered(_masterBox)) {
      Serial.println("--------------------- Mesh triggered ----------");
      _setBoxTargetState(i16onMeshTrigger);
    }
  }
}



bool boxState::_meshHasBeenTriggered(ControlerBox& _masterBox) {
  // check whether masterBox has been set (the masterBox
  // could have been disconnected, or forgotten to be set)
  // Serial.println("---------------- DEBUG ------------ BOX STATE ----------- DEBUG -------------");
  // Serial.printf("boxState::_meshHasBeenTriggered(): thisBox.ui16MasterBoxName: %u\n", thisBox.ui16MasterBoxName);
  // Serial.printf("boxState::_meshHasBeenTriggered(): (thisBox.ui16MasterBoxName == 254): %i\n", thisBox.ui16MasterBoxName == 254);

  // Serial.printf("boxState::_meshHasBeenTriggered(): _masterBox.boxActiveStateHasBeenTakenIntoAccount: %s\n", (_masterBox.boxActiveStateHasBeenTakenIntoAccount ? "true" : "false"));
  // Serial.printf("boxState::_meshHasBeenTriggered(): _masterBox.i16BoxActiveState: %i\n", _masterBox.i16BoxActiveState);
  // Serial.printf("boxState::_meshHasBeenTriggered(): _masterBox.boxActiveStateHasBeenTakenIntoAccount: %s\n", ((_masterBox.boxActiveStateHasBeenTakenIntoAccount || _masterBox.i16BoxActiveState == -1) ? "true" : "false"));
  // check whether the parent box active state
  // has been taken into account
  if (_masterBox.boxActiveStateHasBeenTakenIntoAccount || _masterBox.i16BoxActiveState == -1){
    return false;
  }
  
  // TO DO: check whether the parent box active state corresponds to one of
  // the state to which this box shall react
  // if (_masterBox.i16BoxActiveState == _monitoredMasterStates()) {
  //   return true;
  // }
  // return false

  // else, we have a mesh trigger!
  return true;
}



void boxState::_resolveTriggersConflictAndAct(ControlerBox& _masterBox) {
  // check whether both IR and Mesh have been triggered
  Serial.println("--------------------- double trigger ----------");
  // if so, compare the times at which each signal catcher has been set
  // and give priority to the most recent one
  if (thisBox.ui16hasLastRecPirHighTimeChanged > _masterBox.ui32BoxActiveStateStartTime) {
    _setBoxTargetState(i16onIRTrigger);
  } else {
    _setBoxTargetState(i16onMeshTrigger);
  }
}







/*
  tPlayBoxState starts and stops new boxStates.
  It iterates only once. It is enabled by tPlayBoxStates.
  Its main iteration is delayed until aInterval has expired. aInterval is set in
  the main callback of _tPlayBoxStates (the "parent" Task). Such interval is
  equal to the duration of the new boxState.

  Upon being enabled, its onEnable callback:
  1. looks for the new boxState number, stored in this ControlerBox's
  i16BoxActiveState property and set in the main callback of _tPlayBoxStates,
  sub: _restart_tPlayBoxState.
  Using this number, its selects the currently active boxState
  2. in the currently active boxState, it reads the associated sequence number in its properties;
  3. sets the new sequence to be played (by calling sequence::setActiveSequence());
  4. starts playing the sequence (by enabling the task sequence::tPlaySequenceInLoop.

  It iterates only once and does not have a main callback.

  Upon expiration of the Task, its onDisable callback disables
  Task tPlaySequenceInLoop.

  The interplay between Task _tPlayBoxStates, Task _tPlayBoxStates and
  Task sequence::tPlaySequenceInLoop is inspired by the third example provided
  with the taskScheduler library.
  - Task sequence::tPlaySequenceInLoop is equivalent to
  Task tLED(TASK_IMMEDIATE, TASK_FOREVER, NULL, &ts, false, NULL, &LEDOff)
  - Task tPlayBoxState is equivalent to
  Task tBlink(5000, TASK_ONCE, NULL, &ts, false, &BlinkOnEnable, &BlinkOnDisable).

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
  Serial.print("boxState::_setBoxTargetState(). __boxTargetState: ");Serial.println(__boxTargetState);
  _boxActiveStateHasBeenReset = 1;
  _boxTargetState = __boxTargetState;
  // Serial.print("boxState::_setBoxTargetState(). _boxTargetState: "); Serial.println(_boxTargetState);
  Serial.println("boxState::_setBoxTargetState(). Ending.");
};



//////////////////////////////
// helpers
//////////////////////////////
unsigned long boxState::_ulCalcInterval(int16_t _i16IntervalInS) {
  if (boxStates[_boxTargetState].i16Duration == -1) {
    return 4294967290;
  }
  return boxStates[_boxTargetState].i16Duration * 1000;
}

uint16_t boxState::ui16mToS(uint16_t _minutes) {
  uint16_t _ui16seconds = _minutes * 10;
  return _ui16seconds;
}
