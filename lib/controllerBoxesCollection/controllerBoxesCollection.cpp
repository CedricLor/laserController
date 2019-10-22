/**
 * controllerBoxesCollection.cpp - Library to handle the collection of ControlerBoxes
 * Created by Cedric Lor, Octobre 14, 2019.
 * */

#include "Arduino.h"
#include "controllerBoxesCollection.h"



////////////////////////////////////////////////////////////////////////////////
// Class controllerBoxesCollection
////////////////////////////////////////////////////////////////////////////////

// Constructors
controllerBoxesCollection::controllerBoxesCollection():
  connectedBoxesCount(1),
  previousConnectedBoxesCount(1),
  controllerBoxesArray(),
  thisBox(controllerBoxesArray[0])
{
  _resetTSetBoxState();
}




/** Setters */

// /** void controllerBoxesCollection::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime)
//  *  
//  * Setter for the activeState and associated variables
//  * 
//  * Called only from this class (for the other boxes) and by
//  * boxState (when an effective update has been made). 
//  * 
//  * params:  const int16_t _i16boxActiveState:
//  *          const uint32_t _ui32BoxActiveStateStartTime:
//  * */
// void controllerBoxesCollection::setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime) {
//   // Serial.println("controllerBoxesCollection::setBoxActiveState(): starting");

//   if ( (thisBox.i16BoxActiveState != _i16boxActiveState) || (thisBox.ui32BoxActiveStateStartTime != _ui32BoxActiveStateStartTime) ) {
//     thisBox.i16BoxActiveState = _i16boxActiveState;
//     // Serial.printf("controllerBoxesCollection::updateOtherBoxProperties(): controllerBoxesArray.at(%u).i16BoxActiveState: %u\n", __ui16BoxIndex, controllerBoxesArray.at(__ui16BoxIndex).i16BoxActiveState);

//     thisBox.boxActiveStateHasBeenSignaled = false;
//     // Serial.printf("controllerBoxesCollection::updateOtherBoxProperties(): controllerBoxesArray.at(%u).boxActiveStateHasBeenSignaled: %i\n", __ui16BoxIndex, controllerBoxesArray.at(__ui16BoxIndex).boxActiveStateHasBeenSignaled);
//     /** thisBox.boxActiveStateHasBeenSignaled setters:
//      *  - by default to true upon init (controlerBox constructor);
//      *  - to false here (when called from boxStateCollection::_restartPlayBoxState());
//      *  - to false in myWSReceiver and to true in myWSSender, in the IF: to track change request
//      *    coming from the web and whether the states of other boxes have been received;
//      *  - to true in myMeshViews (for this box only, upon sending a statusMsg);
//      * */

//     thisBox.ui32BoxActiveStateStartTime = _ui32BoxActiveStateStartTime;
//     // Serial.printf("controllerBoxesCollection::updateOtherBoxProperties(): controllerBoxesArray.at(%u).ui32BoxActiveStateStartTime: %u\n", __ui16BoxIndex, controllerBoxesArray.at(__ui16BoxIndex).ui32BoxActiveStateStartTime);

//     /** Set the Task that will check whether this change shall have an impact
//      *  on thisBox boxState, add it to the Scheduler and restart it.
//      * 
//      *  The following comments are coming from ControlerBox::setBoxActiveState()
//      *  -> Testing whether the ControlerBox is not thisBox (thisBox does not read its own mesh high
//      *  signals; it sends mesh high signals). */
//     tSetBoxState.set(0, 1, [&](){return signalHandlers._tcbIfMeshTriggered(thisBox);});
//     tSetBoxState.restart();
//   }

//   // Serial.println("controllerBoxesCollection::setBoxActiveState(): over");
// }


// /** Setter for i16boxStateRequestedFromWeb
//  * 
//  *  Called from: 
//  *  - myMeshController, upon receiving a changeBox request from the web. */
// void controllerBoxesCollection::setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb) {
//   i16boxStateRequestedFromWeb = _i16boxStateRequestedFromWeb;
//   /** Set the Task that will check whether this change shall have an impact
//    *  on thisBox boxState, add it to the Scheduler and restart it. */
//   // tSetBoxState.set(0, 1, [&](){return signalHandlers._tcbSetBoxStateFromWeb();}, NULL, NULL);
//   tSetBoxState.restart();
// }



/** controllerBoxesCollection::updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj)
 * 
 *  Upon receiving a statusMsg from another laser box, this static method will 
 *  try to find the corresponding box in controllerBoxesArray, using the 
 *  _ui32nodeId.
 * 
 *  If it finds it, it will overwrite the data corresponding to this box 
 *  in the relevant entry of the CB array.
 *  Else, it will save such data in the next empty slot. */
uint16_t controllerBoxesCollection::updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj) {
  Serial.printf("controllerBoxesCollection::updateOrCreate(): starting with _ui32nodeId = %u\n", _ui32nodeId);
  /** 1. look for the relevant box by nodeId.
   * 
   *  Why looking by NodeID rather than by NodeName?
   *  If I decide to change the nodeName at runtime, the nodeID will stay the same and I
   *  will find my box and would just need to update the data relating to it.
   *  If my nodeId has changed for the same nodeName, it means that I have unplugged an ESP
   *  and physically changed the device. In such a case, the nodeName will probably have been
   *  deleted anyway. */
  uint16_t __ui16BoxIndex = 254;

  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    if ((controllerBoxesArray.at(_i).nodeId == 0) && (__ui16BoxIndex == 254)) { 
      __ui16BoxIndex = _i;
      continue;
    }
    if (controllerBoxesArray.at(_i).nodeId == _ui32nodeId) {
      printSearchResults(_i, _ui32nodeId, "_ui32nodeId");
      __ui16BoxIndex = _i;
      break;
    }
  }

  /** If we found an existing box or if we have a slot where to save the data from the JSON object 
   *  => save the data */
  if (__ui16BoxIndex != 254) {
    if (controllerBoxesArray.at(__ui16BoxIndex).nodeId == 0) {
      updateConnectedBoxCount(connectedBoxesCount + 1);
    }
    controllerBoxesArray.at(__ui16BoxIndex).updateBoxProperties(_ui32nodeId, _obj, __ui16BoxIndex);
  }

  /** In any case, return the index number to the caller. 
   *  Returning 254 means that: 
   *  (i) the box was not found; and 
   *  (ii) there was no empty slot left in the CB array to save the new data. */
  return __ui16BoxIndex;
}



/** Task tSetBoxState resetter
 * 
 *  private method
 */
void controllerBoxesCollection::_resetTSetBoxState() {
  tSetBoxState.set(0, 1, NULL, NULL, NULL);
}



uint16_t controllerBoxesCollection::findIndexByNodeId(uint32_t _ui32nodeId) {
  Serial.printf("controllerBoxesCollection::findIndexByNodeId(): looking for ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  uint16_t __ui16BoxIndex = 254;
  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    if (controllerBoxesArray.at(_i).nodeId == _ui32nodeId) {
      printSearchResults(_i, _ui32nodeId, "_ui32nodeId");
      __ui16BoxIndex = _i;
      break;
    }
  }
  Serial.printf("controllerBoxesCollection::findIndexByNodeId(): did not find ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  return __ui16BoxIndex;
}



uint16_t controllerBoxesCollection::findIndexByNodeName(uint16_t _ui16NodeName) {
  const char * _subName = "controllerBoxesCollection::findIndexByNodeName():";
  Serial.printf("%s looking for ControlerBox with uint16_t ui16NodeName = %u\n", _subName, _ui16NodeName);
  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    if (controllerBoxesArray.at(_i).ui16NodeName == _ui16NodeName) {
      printSearchResults(_i, (uint32_t)_ui16NodeName, "_ui16NodeName");
      return _i;
    }
  }
  Serial.printf("%s did not find ControlerBox with _ui16NodeName = %u\n", _subName, _ui16NodeName);
  return 254;
}



void controllerBoxesCollection::printSearchResults(uint16_t _index, uint32_t _ui32saughtTerm, const char * _saughtIdentifier) {
  Serial.printf("controllerBoxesCollection::printSearchResults(): found ControlerBox with %s = %u\n", _saughtIdentifier, _ui32saughtTerm);
  Serial.printf("controllerBoxesCollection::printSearchResults(): ControlerBox with %s %u has index: %u\n", _saughtIdentifier, _ui32saughtTerm, _index);
}






void controllerBoxesCollection::deleteBoxByNodeId(uint32_t _ui32nodeId) {
  Serial.println("controllerBoxesCollection::deleteBoxByNodeId(): starting");
  for (uint16_t __it = 0; __it < globalBaseVariables.gui16BoxesCount; __it++) {
    if (controllerBoxesArray[__it].nodeId == _ui32nodeId) {
      controllerBoxesArray[__it]._deleteBox();
      updateConnectedBoxCount(connectedBoxesCount - 1);
      Serial.printf("controllerBoxesCollection::deleteBoxByNodeId(): updated ConnectedBoxCount\n");

      break;
    }
  }
  Serial.println("controllerBoxesCollection::deleteBoxByNodeId(): over");
}





void controllerBoxesCollection::deleteBoxByBoxIndex(uint16_t _ui16index) {
  Serial.println("controllerBoxesCollection::deleteBoxByBoxIndex(): starting");
  controllerBoxesArray[_ui16index]._deleteBox();
  updateConnectedBoxCount(connectedBoxesCount - 1);
  Serial.println("controllerBoxesCollection::deleteBoxByBoxIndex(): over");
}





uint16_t const controllerBoxesCollection::updateConnectedBoxCount(uint16_t _ui16newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  return connectedBoxesCount = _ui16newConnectedBoxesCount;
}





