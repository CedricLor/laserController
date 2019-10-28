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







void myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/) {

  // Declare and define a Json object to read the box numbers and box states from the nested JSON object
  JsonObject __joBoxesStatesInDOM = _obj["boxesStatesInDOM"].as<JsonObject>(); // __joBoxesStatesInDOM = {1:4;2:3}
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): JSON Object _obj available containing the boxState of each boxRow in the DOM \n"); }

  /** If the size of the Json object __joBoxesStatesInDOM is equal to 0,
   *  there are no boxes in the DOM. 
   * 
   *  Let's then check if there are boxes the controller boxes array.
   * */
  if (__joBoxesStatesInDOM.size() == 0) {
    _handleCaseNoBoxesInDom(__joBoxesStatesInDOM);
    return;
  }

  else // re. (__joBoxesStatesInDOM.size() != 0)
  // there are boxRows in DOM
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): There are boxRows in the DOM \n");
    Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount ==  %u.\n", thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount);
  }
  {
    if (thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount == 0) {
      // there are no connected boxes (and boxes in the DOM):
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): There are boxRows in DOM but no connectedBoxes.\n");
        Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): About to instruct to delete all boxRows in DOM.\n");
      }
      // -> send instruction to delete all the boxRows from the DOM
      _obj.clear();
      _obj["action"] = "deleteBox";
      _obj["lb"] = "a";
      myWSSender _myWSSender;
      _myWSSender.sendWSData(_obj);
      // _obj = {action: "deleteBox"; lb: "a"}
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending after instructing to delete all boxRows in DOM.\n");
      }
      return;
    }
    else // re. if (ControlerBox::ui16connectedBoxesCount == 0)
    // there are boxes connected to the mesh (and boxes in the DOM):
    // -> check consistency between the DOM and thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray
    {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): There are boxRows in DOM and connectedBoxes.\n");
        Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): About to check consistency.\n");
      }
      _checkConsistancyDOMDB(__joBoxesStatesInDOM);
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiverReconcile::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending after consistency check done.\n");
      }
    } // end else
  } // end if (__joBoxesStatesInDOM.size() != 0)

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
  if (thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount == 1) {
    // if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiverReconcile::_handleCaseNoBoxesInDom(): Ending action type \"handshake\", because there are no boxRow in DOM nor connectedBoxes.\n"); }
    return;
  }
  /** If there is more than 1 box connected, look in the Json object 
   *  received from the browser to detect which boxes are missing in 
   *  the DOM and send to the browser the relevant information.
  */
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiverReconcile::_handleCaseNoBoxesInDom(): No boxRow in DOM but connectedBoxes.\n");
    Serial.printf("myWSReceiverReconcile::_handleCaseNoBoxesInDom(): Calling _lookForDOMMissingRows().\n");
  }
  _lookForDOMMissingRows(__joBoxesStatesInDOM);
  // if (globalBaseVariables.MY_DG_WS) {
  //   Serial.printf("myWSReceiverReconcile::_handleCaseNoBoxesInDom(): Ending after checking missing boxRows.\n");
  // }
  return;
}





void myWSReceiverReconcile::_checkConsistancyDOMDB(JsonObject& _joBoxState) {
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiverReconcile::_checkConsistancyDOMDB(): JSON Object _joBoxState.size: %i. There are currently boxRow(s) in the DOM.\n", _joBoxState.size());
    Serial.printf("myWSReceiverReconcile::_checkConsistancyDOMDB(): thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount ==  %u. There are currently boxes connected to the mesh.\n", thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount);
    Serial.printf("myWSReceiverReconcile::_checkConsistancyDOMDB(): about to iterate over the boxRows, looking for the existing boxRow and boxState in DOM\n");
  }

  for (JsonPair _p : _joBoxState) { // for each pair boxIndex:boxState in the DOM,
    // {1:4;2:3;etc.}

    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiverReconcile::_checkConsistancyDOMDB(): new iteration.\n");
      Serial.printf("myWSReceiverReconcile::_checkConsistancyDOMDB(): about to use the key of the current pair of the JSON object to check whether the ControlerBox corresponding to the boxRow in the DOM really exists in thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.\n");
      Serial.printf("myWSReceiverReconcile::_checkConsistancyDOMDB(): testing p.key().c_str(): %s\n", _p.key().c_str());
    }

    // DISCONNECTED BOXES CHECKER
    _lookForDisconnectedBoxes(_p);

    // BOXSTATE CHECKER
    _checkBoxStateConsistancy(_p);

  } // end for (JsonPair _p : _joBoxState)

  // MISSING BOXES CHECKER
  // look for missing boxes in the DOM and ask for an update
  _lookForDOMMissingRows(_joBoxState);

  if (globalBaseVariables.MY_DG_WS) {
    Serial.println("myWSReceiverReconcile::_checkConsistancyDOMDB. Ending on action type 0 (received handshake message with list of boxRows in DOM).");
  }
  return;
}





void myWSReceiverReconcile::_lookForDisconnectedBoxes(JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);

  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiverReconcile::_lookForDisconnectedBoxes(): using this value to select a ControlerBox in thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray\n");
    Serial.printf("myWSReceiverReconcile::_lookForDisconnectedBoxes(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0 is %s\n", 
      ((thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0) ? "true" : "false")
    );
  }

  // check if it still is connected; if not, request an update of the DOM
  if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiverReconcile::_lookForDisconnectedBoxes(): the ControlerBox corresponding to the current boxRow has a networkData.nodeId of: %i. It is no longer connected to the mesh. Delete from the DOM.", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId);
      Serial.printf("myWSReceiverReconcile::_lookForDisconnectedBoxes(): about to turn thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i).boxDeletionHasBeenSignaled to false.\n", _iBoxIndex);
    }
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxDeletionHasBeenSignaled = false;
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiverReconcile::_lookForDisconnectedBoxes(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i).boxDeletionHasBeenSignaled turned to %i.\n", _iBoxIndex, thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxDeletionHasBeenSignaled);
      Serial.printf("myWSReceiverReconcile::_lookForDisconnectedBoxes(): this shall be caught by the task _tSendWSDataIfChangeBoxState at next pass.\n");
    }
  } // if
}






void myWSReceiverReconcile::_checkBoxStateConsistancy(JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiverReconcile::_checkBoxStateConsistancy(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState == %i\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState);
    Serial.printf("myWSReceiverReconcile::_checkBoxStateConsistancy(): (int)(_p.value().as<char*>()) = %i\n.", (int)(_p.value().as<char*>()));
    Serial.printf("myWSReceiverReconcile::_checkBoxStateConsistancy(): comparison between the two: %i\n.", (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState == (int)(_p.value().as<char*>())));
  }
  // check if it has the correct boxState; if not, ask for an update
  if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState != (int)(_p.value().as<char*>())) {
    if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiverReconcile::_checkBoxStateConsistancy(): the state of the ControlerBox corresponding to the current boxRow is different than its boxState in the DOM. Update it in the DOM.\n");}
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled = false;
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiverReconcile::_checkBoxStateConsistancy(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled = %i.\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled);
      Serial.printf("myWSReceiverReconcile::_checkBoxStateConsistancy(): this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n");
    }
  } // end if
}





void myWSReceiverReconcile::_lookForDOMMissingRows(JsonObject& _joBoxState) {
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiverReconcile::_lookForDOMMissingRows(): about to iterate over thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray to look if any is missing from the JSON object containing the boxRows from the DOM.\n");
  }

  // iterate over all the potentially existing laser boxes (note that it starts at 1)
  for (uint16_t _i = 0; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    char _c[3];  // declare an array of char of 3 characters ("   ")
    itoa(_i, _c, 10); // convert the iterator into a char (ex. "1")
    const char* _keyInJson = _joBoxState[_c]; // access the object of box-state by the iterator: _joBoxState["1"]
    if ((thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).networkData.nodeId != 0) && _keyInJson == nullptr) {
      // if the thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i) has a networkData.nodeId and the corresponding _joBoxState[_c] is a nullprt
      // there is a missing box in the DOM
      if (globalBaseVariables.MY_DG_WS) {Serial.printf("myWSReceiverReconcile::_lookForDOMMissingRows(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i) is missing box in the DOM. Add it.\n", _i);}
      // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
      thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).isNewBoxHasBeenSignaled = false;
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiverReconcile::_lookForDOMMissingRows(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i).isNewBoxHasBeenSignaled == %i\n", _i, thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).isNewBoxHasBeenSignaled);
        Serial.printf("myWSReceiverReconcile::_lookForDOMMissingRows(): this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n");
      }
    } // if
  } // for
}
