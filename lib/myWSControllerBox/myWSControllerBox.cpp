/*
  myWSControllerBox.cpp - Library wrapping around the ControlerBox class, to keep track of changes
  requests and confirmations exchanged between the ControlerBoxes, the Interface and the Browser.
  Created by Cedric Lor, October 26, 2019.
*/

#include "Arduino.h"
#include "myWSControllerBox.h"


// PUBLIC
// Instance Methods

// Constructor
myWSControllerBox::myWSControllerBox(ControlerBox & _controllerBox):
  _controllerBox(_controllerBox),
  
  isNewBoxHasBeenSignaled(true),
  boxDeletionHasBeenSignaled(true),
  boxActiveStateHasBeenSignaled(true), // has it been signaled to the browser by the interface?
  sBoxDefaultStateChangeHasBeenSignaled(true)
{ }




void myWSControllerBox::printProperties(const uint16_t __ui16BoxIndex) {
  const char * _methodName = "myWSControllerBox::printProperties(): Box n. ";
  _controllerBox._printProperties(__ui16BoxIndex);

  Serial.printf("%s[%u] -> isNewBoxHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("%s[%u] -> boxDeletionHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, boxDeletionHasBeenSignaled);
  Serial.printf("%s[%u] -> boxActiveStateHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, boxActiveStateHasBeenSignaled);
  Serial.printf("%s[%u] -> sBoxDefaultStateChangeHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, sBoxDefaultStateChangeHasBeenSignaled);
}




/** const bool myWSControllerBox::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime)
 *  
 *  Setter for the activeState and associated variables.
 * 
 *  Called from:
 *  - boxStateCollection::_restartTaskPlayBoxState(): for this box, when this box's boxState has changed;
 *  - myWSControllerBox::updateBoxProperties(): for the other boxes, upon receiving a notification of a boxState
 *    change from the mesh.
 * 
 *  TODO: Check whether this function will ever be called from anywhere
*/
const bool myWSControllerBox::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
  // Serial.println("myWSControllerBox::setBoxActiveState(): starting");

  if (_controllerBox._setBoxActiveState(_i16boxActiveState, _ui32BoxActiveStateStartTime)) {
    boxActiveStateHasBeenSignaled = false;
    /** boxActiveStateHasBeenSignaled setters:
     *  - by default to true upon init (controlerBox constructor);
     *  - to false here;
     *  - to false in myWSReceiver and to true in myWSSender, in the IF: to track change request
     *    coming from the web and whether the states of other boxes have been received;
     *  - to true in myMeshViews (for this box only, upon sending a statusMsg);
     * */
    return true;
  }
  // Serial.println("myWSControllerBox::setBoxActiveState(): over");
  return false;
}



/** Setter for the defaultState and associated variables
 *  Called only from this class (for the other boxes). */
void myWSControllerBox::setBoxDefaultState(const short _sBoxDefaultState) {
  // Serial.println("myWSControllerBox::setBoxDefaultState(): starting");
  _controllerBox._setBoxDefaultState(_sBoxDefaultState);
  sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.println("myWSControllerBox::setBoxDefaultState(): over");
}



/**void myWSControllerBox::updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex)
 * 
 *  Updater of the properties of the controller boxes in the mesh
 *  (including this one, at index 0 of the controller boxes array).
 *  
 *  Called from controllerBoxesCollection::_updateOrCreate() exclusively
*/
void myWSControllerBox::updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex) {
  const char * _subName = "myWSControllerBox::updateOtherBoxProperties():";
  Serial.printf("%s starting\n", _subName);
  _controllerBox._updateBoxProperties(_ui32SenderNodeId, _obj, __ui16BoxIndex);
  Serial.printf("%s over\n", _subName);
}






void myWSControllerBox::_deleteBox(const uint16_t _ui16BoxIndex) {
  const char * _subName = "myWSControllerBox::_deleteBox():";

  // *this = myWSControllerBox{};
  boxDeletionHasBeenSignaled = false;
  Serial.printf("%s boxDeletionHasBeenSignaled set to FALSE\n", _subName);

  printProperties(_ui16BoxIndex);

  Serial.printf("%s over\n", _subName);
}