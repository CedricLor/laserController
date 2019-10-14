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
  i16boxStateRequestedFromWeb(-1)
{
  resetTasktSetBoxState();
}




/** setters */

/** Setter for i16boxStateRequestedFromWeb
 * 
 *  Called from: 
 *  - myMeshController, upon receiving a changeBox request from the web. */
void thisControlerBox::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
  i16boxStateRequestedFromWeb = _i16boxStateRequestedFromWeb;
  /** Set the Task that will check whether this change shall have an impact
   *  on thisBox boxState, add it to the Scheduler and restart it. */
  if (_tcbSetBoxStateFromWeb != nullptr) {
    tSetBoxState.set(0, 1, _tcbSetBoxStateFromWeb);
    tSetBoxState.restart();
  }
}


/** private methods */
// Task tSetBoxState resetter
void thisControlerBox::resetTasktSetBoxState() {
  tSetBoxState.set(0, 1, NULL, NULL, NULL);
}