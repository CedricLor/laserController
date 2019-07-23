/*
  step.cpp - interface handling the sequences of boxstate in given session
  Created by Cedric Lor, July 23, 2019.


*/

#include "Arduino.h"
#include "step.h"

const short int step::STEPS_COUNT = 14;
step step::steps[STEPS_COUNT];
const short int step::_NAME_CHAR_COUNT = 25;
const short int step::_HTML_TAG_CHAR_COUNT = 4;





// Constructors
step::step() {
}






// Initialisers
void step::_initStep(const char __cpName[_NAME_CHAR_COUNT], const char __cpHtmlTag[_NAME_CHAR_COUNT], const unsigned long __ulDuration, const uint16_t __ui16AssociatedSequence, const uint16_t __ui16associatedBoxState, const uint16_t __ui16onIRTrigger, const uint16_t __ui16onMeshTrigger, const uint16_t __ui16onExpire){
  // Serial.println("void step::_initBoxState(). Starting.");
  // strcpy(cName, cpName);
  // strcpy(cHtmlTag, cpHtmlTag);
  // _ulDuration = __ulDuration;
  // _bAssociatedSequence = __bAssociatedSequence;
  // _ui16associatedBoxState = __ui16associatedBoxState;
  // _ui16onIRTrigger = __ui16onIRTrigger;
  // _ui16onMeshTrigger = __ui16onMeshTrigger;
  // _ui16onExpire = __ui16onExpire;
  // Serial.println("void step::_initBoxState(). Ending.");
}

void step::initSteps() {
  Serial.println("void step::initSteps(). Starting.");
  // boxStates[0]._initBoxState("manual / off", "man", 1000000, 5, 0, 0); // sequence "all of" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // // Serial.println("void step::_initBoxStates(). boxStates[0].cName: ");
  // // Serial.println(boxStates[0].cName);
  // // Serial.println("void step::_initBoxStates(). boxStates[0]._ulDuration");
  // // Serial.println(boxStates[0]._ulDuration);
  // boxStates[1]._initBoxState("align lasers", "ali", 1000000, 1, 0, 0); // sequence "twins" for indefinite time, without "interrupt/restart" triggers from mesh or IR
  // boxStates[2]._initBoxState("pir Startup", "irs", 60000, 1, 0, 1); // sequence "twins" for 60 seconds, without "interrupt/restart" triggers from IR, but triggers from mesh
  // boxStates[3]._initBoxState("waiting both", "wab", 1000000, 5, 1, 1); // sequence "all of" for indefinite time until trigger from either IR or mesh
  // boxStates[4]._initBoxState("waiting ir", "wai", 1000000, 5, 1, 0); // sequence "all of" for indefinite time until trigger from IR
  // boxStates[5]._initBoxState("waiting mesh", "wam", 1000000, 5, 0, 1); // sequence "all of" for indefinite time until trigger from mesh
  // boxStates[6]._initBoxState("pir High both interrupt", "ihb", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  // boxStates[7]._initBoxState("pir High ir interrupt", "ihr", 120000, 0, 1, 0); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  // boxStates[8]._initBoxState("pir High mesh interrupt", "ihm", 120000, 0, 0, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  // boxStates[9]._initBoxState("pir High no interrupt", "ihn", 120000, 0, 0, 0); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh
  // boxStates[10]._initBoxState("mesh High both interrupt", "mhb", 120000, 0, 1, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from both IR and mesh
  // boxStates[11]._initBoxState("mesh High ir interrupt", "mhi", 120000, 0, 1, 0); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from IR only
  // boxStates[12]._initBoxState("mesh High mesh interrupt", "mhm", 120000, 0, 0, 1); // sequence "relays" for 2 minutes with "interrupt/restart" triggers from mesh only
  // boxStates[13]._initBoxState("mesh High no interrupt", "mhn", 120000, 0, 0, 0); // sequence "relays" for 2 minutes with no "interrupt/restart" triggers from IR or mesh

  Serial.println("void step::initSteps(). Ending.");
}





// step player wrapper
// tPlaySteps starts the execution of the various steps.
// It is enabled when starting a session.
// It then iterates indefinitely until disabled.
// TODO: iteration frequency to be determined
// TODO: actions on iterations to be determined
// TODO: determine what to do on disable (put in deep sleep, probably)
// onEnable, it plays the first step of the session.
// then on each pass, it ???
// Task step::tPlaySteps(1000L, -1, &_tcbPlaySteps, &userScheduler, false, &_oetcbPlaySteps);

/*
*/

void step::_tcbPlaySteps() {
  // Serial.println("void step::_tcbPlaySteps(). Starting.");
  // Serial.print("void step::_tcbPlaySteps(). Iteration:");
  // Serial.println(_tcbPlaySteps.getRunCounter());

  // _restart_tPlayBoxState();
  // Serial.println("void step::_tcbPlaySteps(). Ending.");
};

// Upon _oetcbPlaySteps being enabled (at the beginning of the session), the _boxTargetState is being
// changed to 2 (pir Startup).
bool step::_oetcbPlaySteps() {
  // Serial.println("void step::_oetcbPlaySteps(). Starting.");

  // Serial.println("void step::_oetcbPlaySteps(). Ending.");
  return true;
}






/*
  _tPlayStep plays a step once (it iterates only once).
  It is enabled by tPlaySteps.
  Its main iteration is delayed until aInterval has expired. aInterval is set in its onEnable callback.
  It is equal to the duration of the boxState selected by ControlerBoxes[gui16MyIndexInCBArray].boxActiveState.
  Upon being enabled, its onEnable callback:
  1. sets the aInterval of _tPlayBoxState, based on the _ulDuration of the currently active boxState;
  2. looks for the associated sequence using the ControlerBoxes[gui16MyIndexInCBArray].boxActiveState variable to select the relevant boxState in boxStates[];
  3. sets the active sequence by a call to sequence::setActiveSequence();
  4. enables the task sequence::tPlaySequenceInLoop.
  Task sequence::tPlaySequenceInLoop is set to run indefinitely, for so long as it is not disabled.
  - Task sequence::tPlaySequenceInLoop is equivalent to Task tLED(TASK_IMMEDIATE, TASK_FOREVER, NULL, &ts, false, NULL, &LEDOff)
  in the third example provided with the taskScheduler library.
  - Task _tPlayBoxState is equivalent to Task tBlink(5000, TASK_ONCE, NULL, &ts, false, &BlinkOnEnable, &BlinkOnDisable)
  in the third example provided with the taskScheduler library.
  After expiration of aInterval, its onDisable callback disables sequence::tPlaySequenceInLoop.
*/
// Task step::_tPlayStep(0, 1, NULL, &userScheduler, false, &_oetcbPlayStep, &_odtcbPlayStep);

bool step::_oetcbPlayStep(){
  // Serial.println("bool step::_oetcbPlayStep(). Starting.");
  // Serial.print("bool step::_oetcbPlayStep(). Box State Number: ");Serial.println(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState);
  // // Look for the sequence number to read when in this state
  // short int _activeSequence = boxStates[ControlerBoxes[gui16MyIndexInCBArray].boxActiveState]._bAssociatedSequence;
  // Serial.print("bool step::_oetcbPlayStep() _activeSequence: ");
  // Serial.println(_activeSequence);
  // // set the active sequence
  // Serial.println("bool step::_oetcbPlayStep() calling sequence::setActiveSequence(_activeSequence)");
  // sequence::setActiveSequence(_activeSequence);
  //
  // // Play sequence in loop until end
  // Serial.println("bool step::_oetcbPlayStep() sequence::tPlaySequenceInLoop about to be enabled");
  // sequence::tPlaySequenceInLoop.enable();
  // myMeshViews __myMeshViews;
  // __myMeshViews.statusMsg();
  // Serial.println("bool step::_oetcbPlayStep(). Ending.");
  return true;
}

void step::_odtcbPlayStep(){
  // Upon disabling the task which plays a given boxState,
  // (i) disable the associated sequence player; and
  // (ii) if the state which was being played was not the default state, set it to its default state

  // Serial.println("void step::_odtcbPlayStep(). Starting.");
  // // Serial.print("void step::_odtcbPlayBoxState() _tPlayBoxState.getInterval(): ");
  // // Serial.println(_tPlayBoxState.getInterval());
  // // disable the associated sequence player
  // sequence::tPlaySequenceInLoop.disable();
  // // Serial.println("void step::_odtcbPlayBoxState(): ControlerBoxes[gui16MyIndexInCBArray].boxActiveState");
  // // Serial.println(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState);
  // // Serial.println("void step::_odtcbPlayBoxState(): _boxTargetState");
  // // Serial.println(_boxTargetState);
  // if (!(ControlerBoxes[gui16MyIndexInCBArray].boxActiveState == ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState)) {
  //   _setBoxTargetState(ControlerBoxes[gui16MyIndexInCBArray].sBoxDefaultState);
  // }
  // Serial.println("void step::_odtcbPlayStep(). Ending.");
}
