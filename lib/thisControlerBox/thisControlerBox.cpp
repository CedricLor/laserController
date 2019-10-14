/*
  thisControlerBox.cpp - Library to handle this particular ControlerBox (as opposed to the others in the collection)
  Created by Cedric Lor, Octobre 14, 2019.
*/

#include "Arduino.h"
#include "thisControlerBox.h"



////////////////////////////////////////////////////////////////////////////////
// Class thisControlerBox
////////////////////////////////////////////////////////////////////////////////

// Constructors
thisControlerBox::thisControlerBox():
  i16boxStateRequestedFromWeb(-1),
  thisBox(ControlerBoxes[0])
{
  resetTSetBoxState();
}




/** Setters */

/** void thisControlerBox::setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime)
 *  
 * Setter for the activeState and associated variables
 * 
 * Called only from this class (for the other boxes) and by
 * boxState (when an effective update has been made). 
 * 
 * params:  const short _sBoxActiveState:
 *          const uint32_t _ui32BoxActiveStateStartTime:
 * */
void thisControlerBox::setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
  // Serial.println("thisControlerBox::setBoxActiveState(): starting");

  if ( (thisBox.i16BoxActiveState != _sBoxActiveState) || (thisBox.ui32BoxActiveStateStartTime != _ui32BoxActiveStateStartTime) ) {
    thisBox.i16BoxActiveState = _sBoxActiveState;
    // Serial.printf("thisControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].i16BoxActiveState: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].i16BoxActiveState);

    thisBox.boxActiveStateHasBeenSignaled = false;
    // Serial.printf("thisControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].boxActiveStateHasBeenSignaled);
    // setters:
    // - by default to true upon init (controlerBox constructor);
    // - to false here (usefull for the IF, for the received states of other boxes);
    // - to true in myMeshViews (for this box only, upon sending a statusMsg);
    // - to true and false in myWebServerBase (by the IF, for the other boxes) --> tracing if it has sent an update to the browser
    // used by the interface mostly

    thisBox.ui32BoxActiveStateStartTime = _ui32BoxActiveStateStartTime;
    // Serial.printf("thisControlerBox::updateOtherBoxProperties(): ControlerBoxes[%u].ui32BoxActiveStateStartTime: %u\n", __ui16BoxIndex, ControlerBoxes[__ui16BoxIndex].ui32BoxActiveStateStartTime);

    /** Set the Task that will check whether this change shall have an impact
     *  on thisBox boxState, add it to the Scheduler and restart it.
     * 
     *  Testing whether the ControlerBox is not thisBox (thisBox does not read its own mesh high
     *  signals; it sends mesh high signals). */
    tSetBoxState.set(0, 1, [&](){return signalHandlers._tcbIfMeshTriggered(thisBox);});
    tSetBoxState.restart();
  }

  // Serial.println("thisControlerBox::setBoxActiveState(): over");
}


/** Setter for i16boxStateRequestedFromWeb
 * 
 *  Called from: 
 *  - myMeshController, upon receiving a changeBox request from the web. */
void thisControlerBox::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
  i16boxStateRequestedFromWeb = _i16boxStateRequestedFromWeb;
  /** Set the Task that will check whether this change shall have an impact
   *  on thisBox boxState, add it to the Scheduler and restart it. */
  tSetBoxState.set(0, 1, [&](){return signalHandlers._tcbSetBoxStateFromWeb();});
  tSetBoxState.restart();
}


/** private methods */
// Task tSetBoxState resetter
void thisControlerBox::resetTSetBoxState() {
  tSetBoxState.set(0, 1, NULL, NULL, NULL);
}