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
  
  ui16NodeName(254),

  i16BoxActiveState(-1),
  ui32BoxActiveStateStartTime(0),
  ui32lastRecPirHighTime(0),
  sBoxDefaultState(globalBaseVariables.gi16BoxDefaultState),

  isNewBoxHasBeenSignaled(true),
  boxDeletionHasBeenSignaled(true),
  boxActiveStateHasBeenSignaled(true), // has it been signaled to the browser by the interface?
  sBoxDefaultStateChangeHasBeenSignaled(true)
{ }




void myWSControllerBox::printProperties(const uint16_t __ui16BoxIndex) {
  const char * _methodName = "myWSControllerBox::printProperties(): Box n. ";
  Serial.printf("%s[%u] -> ui16NodeName: %u\n", _methodName, __ui16BoxIndex, ui16NodeName);

  Serial.printf("%s[%u] -> i16BoxActiveState: %u\n", _methodName, __ui16BoxIndex, i16BoxActiveState);
  Serial.printf("%s[%u] -> ui32BoxActiveStateStartTime: %u\n", _methodName, __ui16BoxIndex, ui32BoxActiveStateStartTime);
  Serial.printf("%s[%u] -> boxActiveStateHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, boxActiveStateHasBeenSignaled);

  Serial.printf("%s[%u] -> ui32lastRecPirHighTime: %u\n", _methodName, __ui16BoxIndex, ui32lastRecPirHighTime);

  Serial.printf("%s[%u] -> isNewBoxHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, isNewBoxHasBeenSignaled);
  Serial.printf("%s[%u] -> boxDeletionHasBeenSignaled: %i\n", _methodName, __ui16BoxIndex, boxDeletionHasBeenSignaled);

  Serial.printf("%s[%u] -> sBoxDefaultState: %u\n", _methodName, __ui16BoxIndex, sBoxDefaultState);
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
*/
const bool myWSControllerBox::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
  // Serial.println("myWSControllerBox::setBoxActiveState(): starting");

  if ( (i16BoxActiveState != _i16boxActiveState) || (ui32BoxActiveStateStartTime != _ui32BoxActiveStateStartTime) ) {
    i16BoxActiveState = _i16boxActiveState;

    boxActiveStateHasBeenSignaled = false;
    /** boxActiveStateHasBeenSignaled setters:
     *  - by default to true upon init (controlerBox constructor);
     *  - to false here;
     *  - to false in myWSReceiver and to true in myWSSender, in the IF: to track change request
     *    coming from the web and whether the states of other boxes have been received;
     *  - to true in myMeshViews (for this box only, upon sending a statusMsg);
     * */

    ui32BoxActiveStateStartTime = _ui32BoxActiveStateStartTime;

    return true;
  }
  // Serial.println("myWSControllerBox::setBoxActiveState(): over");
  return false;
}



// Setter for the defaultState and associated variables
// Called only from this class (for the other boxes).
void myWSControllerBox::setBoxDefaultState(const short _sBoxDefaultState) {
  // Serial.println("myWSControllerBox::setBoxDefaultState(): starting");
  sBoxDefaultState = _sBoxDefaultState;
  sBoxDefaultStateChangeHasBeenSignaled = false;
  // Serial.println("myWSControllerBox::setBoxDefaultState(): over");
}



/** Setter for ui32lastRecPirHighTime
 * 
 *  Called from: 
 *  - this class (from the method updateBoxProperties), upon receiving 
 *    an upstream information (limited for the moment to IR High)
 *    message from the other boxes; updateBoxProperties is itself 
 *    called by the _updateOrCreate() method in controllerBoxesCollection, which is 
 *    itself called from the meshController
 *  - the pirController, upon IR high. */
void myWSControllerBox::setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime) {
  if (_ui32lastRecPirHighTime != ui32lastRecPirHighTime) {
    ui32lastRecPirHighTime = _ui32lastRecPirHighTime;
  }
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

  // set the nodeId
  // if (networkData.nodeId == 0) {
  //   isNewBoxHasBeenSignaled = false;
  //   Serial.printf("%s ControlerBoxes[%u].isNewBoxHasBeenSignaled = %i\n", _subName, __ui16BoxIndex, isNewBoxHasBeenSignaled);
  // }
  // networkData.update(_ui32SenderNodeId, _obj);

  // set the ui16NodeName
  ui16NodeName = _obj["NNa"];

  // Setting activeState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  if (_obj.containsKey("actSt")) {
    const short int __senderBoxActiveState = _obj["actSt"];
    const uint32_t __uiSenderBoxActiveStateStartTime = _obj["actStStartT"];
    setBoxActiveState(__senderBoxActiveState, __uiSenderBoxActiveStateStartTime);
  }
  
  // Setting defaultState stack
  // need to send via myMeshViews and add to ControlerBox the time
  // for which the new sender boxState shall apply
  // extract the __senderBoxActiveState from the JSON
  if (_obj.containsKey("boxDefstate")) {
    const short int __senderBoxDefaultState = _obj["boxDefstate"];
    setBoxDefaultState(__senderBoxDefaultState);
  }

  if (_obj["action"] == "usi" && _obj["key"] == "IR") {
    setBoxIRTimes(_obj["time"].as<uint32_t>());
  }

  // Print out the updated properties
  if (globalBaseVariables.MY_DEBUG == true) {
    Serial.printf("%s Updated box index %u. Calling printProperties().\n", _subName, __ui16BoxIndex);
    printProperties(__ui16BoxIndex);
  }
  Serial.println("%s over");
}






void myWSControllerBox::_deleteBox(const uint16_t _ui16BoxIndex) {
  const char * _subName = "myWSControllerBox::_deleteBox():";

  // *this = myWSControllerBox{};
  boxDeletionHasBeenSignaled = false;
  Serial.printf("%s boxDeletionHasBeenSignaled set to FALSE\n", _subName);

  printProperties(_ui16BoxIndex);

  Serial.printf("%s over\n", _subName);
}