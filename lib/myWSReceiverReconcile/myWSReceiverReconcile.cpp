/*
  myWSReceiverReconcile.cpp - Library to handle reconciliation of data relating to 
  the various controller boxes between the data available in the browser and 
  and the data that the IF knows about the state of the controller boxes connected
  to the mesh.

  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include <myWSReceiverReconcile.h>




myWSReceiverReconcile::myWSReceiverReconcile(JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/)
{
  _onHandshakeCheckWhetherDOMNeedsUpdate(_obj);
}







int16_t myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/) {

  // Declare and define a Json object to read the box numbers and box states from the nested JSON object
  JsonObject __joBoxesStatesInDOM = _obj["boxesStatesInDOM"].as<JsonObject>(); // __joBoxesStatesInDOM = {1:4;2:3}
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): JSON Object _obj available containing the boxState of each boxRow in the DOM \n"); }

  /** If the size of the Json object __joBoxesStatesInDOM is equal to 0,
   *  there are no boxes in the DOM. 
   * 
   *  Let's then check if there are boxes the controller boxes array and return.
   * */
  if (__joBoxesStatesInDOM.size() == 0) {
    _handleCaseNoBoxesInDom(__joBoxesStatesInDOM);
    return 0;
  }

  /** If the size of the Json object __joBoxesStatesInDOM is superior to 0,
   *  there are boxes in the DOM. 
   * 
   *  Let's then check if there are boxes the controller boxes array.
   * */
  _handleCaseBoxesInDom(__joBoxesStatesInDOM);
  return __joBoxesStatesInDOM.size();
}




/**void myWSReceiverReconcile::_handleCaseNoBoxesInDom(JsonObject& __joBoxesStatesInDOM)
 * 
 *  Handles the case where the browser's tells us that there are no boxes in the DOM.
 * 
 *  If there are no connected boxes to the mesh, then the DOM and the controller boxes array 
 *  are sync. Do nothing.
 * 
 *  If there are boxes connected to the mesh, then the DOM needs an update.
*/
void myWSReceiverReconcile::_handleCaseNoBoxesInDom(JsonObject& __joBoxesStatesInDOM) {
  Serial.printf("myWSReceiverReconcile::_handleCaseNoBoxesInDom(): starting \n");
  Serial.printf("myWSReceiverReconcile::_handleCaseNoBoxesInDom(): thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount ==  %u.\n", thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount);
  /** If there is only 1 box connected, it is this box. 
   * 
   *  Just return.*/
  if (thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount == 0) {
    return;
  }
  /** If there is more than 1 box connected, look in the Json object 
   *  received from the browser to detect which boxes are missing in 
   *  the DOM and send to the browser the relevant information.
  */
  _lookForDOMMissingRows(__joBoxesStatesInDOM);
  return;
}





void myWSReceiverReconcile::_handleCaseBoxesInDom(JsonObject& __joBoxesStatesInDOM) {
  // there are no connected boxes (and boxes in the DOM):
  // -> send instruction to delete all the boxRows from the DOM
  if (thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount == 0) {
    _handleCaseBoxesInDomAndNoBoxesInCBArray(__joBoxesStatesInDOM);
    return;
  }
  // there are boxes connected to the mesh (and boxes in the DOM):
  // -> check consistency between the DOM and thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray
  _checkConsistancyDOMDB(__joBoxesStatesInDOM);
}





void myWSReceiverReconcile::_handleCaseBoxesInDomAndNoBoxesInCBArray(JsonObject& __joBoxesStatesInDOM) {
  __joBoxesStatesInDOM.clear();
  __joBoxesStatesInDOM["action"] = "deleteBox";
  __joBoxesStatesInDOM["lb"] = "a";
  myWSSender _myWSSender;
  _myWSSender.sendWSData(__joBoxesStatesInDOM);
  // __joBoxesStatesInDOM = {action: "deleteBox"; lb: "a"}
}





void myWSReceiverReconcile::_checkConsistancyDOMDB(JsonObject& _joBoxState) {

  for (JsonPair _p : _joBoxState) { // for each pair boxIndex:boxState in the DOM,
    // {1:4;2:3;etc.}

    // DISCONNECTED BOXES CHECKER
    _lookForDisconnectedBoxes(_p);

    // BOXSTATE CHECKER
    _checkBoxStateConsistancy(_p);

  } // end for (JsonPair _p : _joBoxState)

  // MISSING BOXES CHECKER
  // look for missing boxes in the DOM and ask for an update
  _lookForDOMMissingRows(_joBoxState);

  return;
}





void myWSReceiverReconcile::_lookForDisconnectedBoxes(JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);

  // check if it still is connected; if not, request an update of the DOM
  if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0) {
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxDeletionHasBeenSignaled = false;
  } // end if
}






void myWSReceiverReconcile::_checkBoxStateConsistancy(JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);
  // check if it has the correct boxState; if not, ask for an update
  if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState != (int)(_p.value().as<char*>())) {
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled = false;
  } // end if
}





void myWSReceiverReconcile::_lookForDOMMissingRows(JsonObject& _joBoxState) {

  // iterate over all the potentially existing laser boxes (note that it starts at 1)
  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    char _c[3];  // declare an array of char of 3 characters ("   ")
    itoa(_i, _c, 10); // convert the iterator into a char (ex. "1")
    const char* _keyInJson = _joBoxState[_c]; // access the object of box-state by the iterator: _joBoxState["1"]
    if ((thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).networkData.nodeId != 0) && _keyInJson == nullptr) {
      // if the thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i) has a networkData.nodeId and the corresponding _joBoxState[_c] is a nullprt
      // there is a missing box in the DOM
      // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
      thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).isNewBoxHasBeenSignaled = false;
    } // end if
  } // end for
}
