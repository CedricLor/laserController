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
  controllerBoxesArray()
{ }




/** controllerBoxesCollection::_updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj)
 * 
 *  Upon receiving a statusMsg or an upstream information message from another controller box
 *  (the calling box), this method will try to find, by node id, an entry corresponding to such
 *  calling box in the controller boxes array.
 * 
 *  If this method finds an entry with the calling boxe's node id in the controller boxes' array, 
 *  it will overwrite the data in the corresponding entry of the controller boxes array, with the data
 *  contained in the JsonObject.
 * 
 *  Otherwise, it will save such data in the controller boxes array in the first empty slot it finds. */
uint16_t controllerBoxesCollection::_updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj) {
  Serial.printf("controllerBoxesCollection::_updateOrCreate(): starting with _ui32nodeId = %u\n", _ui32nodeId);
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
    if ((controllerBoxesArray.at(_i).networkData.nodeId == 0) && (__ui16BoxIndex == 254)) { 
      __ui16BoxIndex = _i;
      continue;
    }
    if (controllerBoxesArray.at(_i).networkData.nodeId == _ui32nodeId) {
      printSearchResults(_i, _ui32nodeId, "_ui32nodeId");
      __ui16BoxIndex = _i;
      break;
    }
  }

  /** If we found an existing box or if we have a slot where to save the data from the JSON object 
   *  => save the data */
  if (__ui16BoxIndex != 254) {
    if (controllerBoxesArray.at(__ui16BoxIndex).networkData.nodeId == 0) {
      updateConnectedBoxCount(connectedBoxesCount + 1);
    }
    controllerBoxesArray.at(__ui16BoxIndex)._updateBoxProperties(_ui32nodeId, _obj, __ui16BoxIndex);
  }

  /** In any case, return the index number to the caller. 
   *  Returning 254 means that: 
   *  (i) the box was not found; and 
   *  (ii) there was no empty slot left in the CB array to save the new data. */
  return __ui16BoxIndex;
}



uint16_t controllerBoxesCollection::findIndexByNodeId(uint32_t _ui32nodeId) {
  Serial.printf("controllerBoxesCollection::findIndexByNodeId(): looking for ControlerBox with _ui32nodeId = %u\n", _ui32nodeId);
  uint16_t __ui16BoxIndex = 254;
  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    if (controllerBoxesArray.at(_i).networkData.nodeId == _ui32nodeId) {
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
  Serial.printf("%s did not find ControlerBox with _ui16NodeName == %u\n", _subName, _ui16NodeName);
  return 254;
}



void controllerBoxesCollection::printSearchResults(uint16_t _index, uint32_t _ui32saughtTerm, const char * _saughtIdentifier) {
  Serial.printf("controllerBoxesCollection::printSearchResults(): found ControlerBox with %s = %u\n", _saughtIdentifier, _ui32saughtTerm);
  Serial.printf("controllerBoxesCollection::printSearchResults(): ControlerBox with %s %u has index: %u\n", _saughtIdentifier, _ui32saughtTerm, _index);
}






void controllerBoxesCollection::deleteBoxByNodeId(uint32_t _ui32nodeId) {
  Serial.println("controllerBoxesCollection::deleteBoxByNodeId(): starting");
  for (uint16_t __it = 0; __it < globalBaseVariables.gui16BoxesCount; __it++) {
    if (controllerBoxesArray[__it].networkData.nodeId == _ui32nodeId) {
      deleteBoxByBoxIndex(__it);
      Serial.printf("controllerBoxesCollection::deleteBoxByNodeId(): updated ConnectedBoxCount\n");

      break;
    }
  }
  Serial.println("controllerBoxesCollection::deleteBoxByNodeId(): over");
}





void controllerBoxesCollection::deleteBoxByBoxIndex(uint16_t _ui16index) {
  Serial.println("controllerBoxesCollection::deleteBoxByBoxIndex(): starting");
  controllerBoxesArray[_ui16index]._deleteBox(_ui16index);
  updateConnectedBoxCount(connectedBoxesCount - 1);
  Serial.println("controllerBoxesCollection::deleteBoxByBoxIndex(): over");
}





uint16_t const controllerBoxesCollection::updateConnectedBoxCount(uint16_t _ui16newConnectedBoxesCount) {
  previousConnectedBoxesCount = connectedBoxesCount;
  return connectedBoxesCount = _ui16newConnectedBoxesCount;
}





