/*
  myWSReceiverReconcile.cpp - Library to handle reconciliation of data relating to 
  the various controller boxes between the data available in the browser and 
  and the data that the IF knows about the state of the controller boxes connected
  to the mesh.

  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include <myWSReceiverReconcile.h>




myWSReceiverReconcile::myWSReceiverReconcile(JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/, controllerBoxesCollection & _ctlBxColl)
{
  _onHandshakeCheckWhetherDOMNeedsUpdate(_obj, _ctlBxColl);
}







int16_t myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/, controllerBoxesCollection & _ctlBxColl) {
  // Declare and define a Json object to read the box numbers and box states from the nested JSON object
  JsonObject __joBoxesStatesInDOM = _obj["boxesStatesInDOM"].as<JsonObject>(); // __joBoxesStatesInDOM = {1:4;2:3}

  /** If the size of the Json object __joBoxesStatesInDOM is equal to 0,
   *  there are no boxes in the DOM. 
   * 
   *  Let's then check if there are boxes the controller boxes array and return. */
  if (__joBoxesStatesInDOM.size() == 0) {
    _handleCaseNoBoxesInDom(_ctlBxColl);
    return 0;
  }

  /** If the size of the Json object __joBoxesStatesInDOM is superior to 0,
   *  there are boxes in the DOM. 
   * 
   *  Let's then check if there are boxes the controller boxes array. */
  _handleCaseBoxesInDom(__joBoxesStatesInDOM, _ctlBxColl);
  return __joBoxesStatesInDOM.size();
}




/**void myWSReceiverReconcile::_handleCaseNoBoxesInDom(JsonObject& __joBoxesStatesInDOM)
 * 
 *  Handles the case where the browser's tells us that there are no boxes in the DOM.
 * 
 *  If there are no connected boxes to the mesh, then the DOM and the controller boxes array 
 *  are sync. Do nothing.
 * 
 *  If there are boxes connected to the mesh, then the DOM needs an update. */
void myWSReceiverReconcile::_handleCaseNoBoxesInDom(controllerBoxesCollection & _ctlBxColl) {
  /** If there are no boxes connected/registered in the CB array and no boxes in DOM, just return.*/
  if (_ctlBxColl.ui16connectedBoxesCount == 0) {
    return;
  }
  /** If there is at least 1 box connected/registered in the CB array, 
   *  mark all the boxes in the CB array as not signaled, so that the WSSender knows that
   *  it shall send the corresponding data to the browser. */
  _markAllBoxesAsUnsignaledNewBox(_ctlBxColl.controllerBoxesArray);
  return;
}





void myWSReceiverReconcile::_markAllBoxesAsUnsignaledNewBox(std::array<ControlerBox, 10U> & _controllerBoxesArray) {
  _parseCBArrayAndMarkUnsignaledCBs(_controllerBoxesArray, [&](uint16_t _iterator){
    /** If the _controllerBoxesArray.at(_i) has a networkData.nodeId, 
     *  it means that it is a connected box.
     *  Changing its isNewBoxHasBeenSignaled property to false will trigger 
     *  in the callback of task _tSendWSDataIfChangeBoxState */
    _controllerBoxesArray.at(_iterator).isNewBoxHasBeenSignaled = false;
  });
}





void myWSReceiverReconcile::_handleCaseBoxesInDom(JsonObject& __joBoxesStatesInDOM, controllerBoxesCollection & _ctlBxColl) {
  /** There are no connected boxes (and boxes in the DOM):
   *  -> send instruction to delete all the boxRows from the DOM */
  if (_ctlBxColl.ui16connectedBoxesCount == 0) {
    _handleCaseBoxesInDomAndNoBoxesInCBArray(__joBoxesStatesInDOM);
    return;
  }
  // there are boxes connected to the mesh (and boxes in the DOM):
  // -> check consistency between the DOM and thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray
  _checkConsistancyDOMDB(__joBoxesStatesInDOM, _ctlBxColl.controllerBoxesArray);
}





void myWSReceiverReconcile::_handleCaseBoxesInDomAndNoBoxesInCBArray(JsonObject& __joBoxesStatesInDOM) {
  __joBoxesStatesInDOM.clear();
  __joBoxesStatesInDOM["action"] = "deleteBox";
  __joBoxesStatesInDOM["lb"] = "a";
  myWSSender _myWSSender;
  _myWSSender.sendWSData(__joBoxesStatesInDOM);
  // __joBoxesStatesInDOM = {action: "deleteBox"; lb: "a"}
}





void myWSReceiverReconcile::_checkConsistancyDOMDB(JsonObject& _joBoxState, std::array<ControlerBox, 10U> & _controllerBoxesArray) {

  for (JsonPair _p : _joBoxState) { // for each pair boxIndex:boxState in the DOM,
    // {1:4;2:3;etc.}

    // DISCONNECTED BOXES CHECKER
    _lookForDisconnectedBoxes(_p, _controllerBoxesArray);

    // BOXSTATE CHECKER
    _checkBoxStateConsistancy(_p, _controllerBoxesArray);

  } // end for (JsonPair _p : _joBoxState)

  // MISSING BOXES CHECKER
  // look for missing boxes in the DOM and ask for an update
  _lookForDOMMissingRows(_joBoxState, _controllerBoxesArray);

  return;
}





void myWSReceiverReconcile::_lookForDisconnectedBoxes(JsonPair& _p, std::array<ControlerBox, 10U> & _controllerBoxesArray) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);

  // check if it still is connected; if not, request an update of the DOM
  if (_controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0) {
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    _controllerBoxesArray.at(_iBoxIndex).boxDeletionHasBeenSignaled = false;
  } // end if
}






void myWSReceiverReconcile::_checkBoxStateConsistancy(JsonPair& _p, std::array<ControlerBox, 10U> & _controllerBoxesArray) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);
  // check if it has the correct boxState; if not, ask for an update
  if (_controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState != (int)(_p.value().as<char*>())) {
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    _controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled = false;
  } // end if
}





void myWSReceiverReconcile::_lookForDOMMissingRows(JsonObject& _joBoxState, std::array<ControlerBox, 10U> & _controllerBoxesArray) {
  _parseCBArrayAndMarkUnsignaledCBs(_controllerBoxesArray, [&](uint16_t _iterator){
      char _c[3];  // declare an array of char of 3 characters ("   ")
      itoa(_iterator, _c, 10); // convert the iterator into a char (ex. "1")
      const char* _keyInJson = _joBoxState[_c]; // access the object of box-state by the iterator: _joBoxState["1"]
      if (_keyInJson == nullptr) {
        // if the _controllerBoxesArray.at(_i) has a networkData.nodeId and the corresponding _joBoxState[_c] is a nullprt
        // there is a missing box in the DOM
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        _controllerBoxesArray.at(_iterator).isNewBoxHasBeenSignaled = false;
      }
  });
}






template <typename F>
void myWSReceiverReconcile::_parseCBArrayAndMarkUnsignaledCBs(std::array<ControlerBox, 10U> & _controllerBoxesArray, F&& lambda) {
  // iterate over all the potentially existing laser boxes
  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    lambda(_i);
  } // end for
}
