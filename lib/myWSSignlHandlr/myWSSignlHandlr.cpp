/*
  myWSSignlHandlr.cpp - Library to handle the transmission of signals read from the mesh 
  to the browser via the web socket
  Created by Cedric Lor, November 1, 2019.
*/

#include "Arduino.h"
#include <myWSSignlHandlr.h>








///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// myWSSignlHandlr class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
myWSSignlHandlr::myWSSignlHandlr(AsyncWebSocket & __asyncWebSocketInstance):
  _asyncWebSocketInstance(__asyncWebSocketInstance)
{
  tSendWSData.set(0, 1, NULL, NULL, NULL);
}







/** void myWSSignlHandlr::_tcbSendWSData()
 * 
 *  Template Callback for Task tSendWSData. 
 * */
void myWSSignlHandlr::_tcbSendWSData() {
  /** 1. check wether any client is currently connected */
  if (!(_asyncWebSocketInstance.count())) { return; }
  // Serial.println("myWSSignlHandlr::_tcbSendWSData(): WS has some client connected.");

  /** 2. Prepare ArduinoJson stuffs */
  StaticJsonDocument<256> _doc;
  JsonObject _obj = _doc.to<JsonObject>();

  /** 3. Prepare ArduinoJson stuffs */
  // Load sthg into the JsonObject

  /** 4. Instantiate the myWSSender and broadcast this to any connected browser */
  myWSSender _myWSSender(_asyncWebSocketInstance);
  _myWSSender.sendWSData(_obj);
}













// bool myWSResponder::_checkWhetherUnsignaledNewBox(ControlerBox & _controlerBox, JsonObject & _obj) {
//   if (_controlerBox.isNewBoxHasBeenSignaled == false) {
//     if (thisControllerBox.globBaseVars.MY_DG_WS) {
//       Serial.printf("myWSSender::_checkWhetherUnsignaledNewBox. In fact, a new box [%u] has joined.\n", _controlerBox.ui16NodeName);
//     }
//     _obj["action"]      = "addBox";
//     _obj["boxState"]    = _controlerBox.i16BoxActiveState;
//     _obj["boxDefstate"] = _controlerBox.sBoxDefaultState;
//     // expected _obj = {lb:1; action:"addBox"; boxState: 3; boxDefstate: 6}
//     // reset all the booleans to true (boxState and boxDefstate have just been read; they are no longer unsignaled.)
//     _resetAllControlerBoxBoolsToTrueByCB(_controlerBox);
//     return true;
//   }
//   return false;
// }






// bool myWSResponder::_checkWhetherUnsignaledDefaultStateChange(ControlerBox & _controlerBox, JsonObject & _obj) {
//   if (_controlerBox.sBoxDefaultStateChangeHasBeenSignaled == false) {
//     if (thisControllerBox.globBaseVars.MY_DG_WS) {
//       Serial.printf("myWSSender::_checkWhetherUnsignaledDefaultStateChange. Default state of box [%u] has changed\n", _controlerBox.ui16NodeName);
//     }
//     _obj["action"] = "changeBox";
//     _obj["key"] = "boxDefstate";
//     _obj["val"] = _controlerBox.sBoxDefaultState;
//     _obj["st"] = 2;
//     // expected _obj = {lb:1; action:"changeBox"; key: "boxDefstate"; val: 4; st: 2}
//     _controlerBox.sBoxDefaultStateChangeHasBeenSignaled = true;
//     return true;
//   }
//   return false;
// }






// bool myWSResponder::_checkWhetherUnsignaledBoxStateChange(ControlerBox & _controlerBox, JsonObject & _obj) {
//   if (_controlerBox.boxActiveStateHasBeenSignaled == false) {
//     if (thisControllerBox.globBaseVars.MY_DG_WS) {
//       Serial.printf("myWSSender::_checkWhetherUnsignaledBoxStateChange. State of box [%u] has changed\n", _controlerBox.ui16NodeName);
//     }
//     _obj["action"] = "changeBox";
//     _obj["key"] = "boxState";
//     _obj["val"] = _controlerBox.i16BoxActiveState;
//     _obj["st"] = 2;
//     // expected _obj = {lb:1; action:"changeBox"; key: "boxState"; val: 6; st: 2}
//     _controlerBox.boxActiveStateHasBeenSignaled = true;
//     return true;
//   }
//   return false;
// }






// bool myWSResponder::_checkWhetherUnsignaledDeletedBox(ControlerBox & _controlerBox, JsonObject & _obj) {
//   if (_controlerBox.boxDeletionHasBeenSignaled == false) {
//     if (thisControllerBox.globBaseVars.MY_DG_WS) {
//       Serial.printf("myWSSender::_checkWhetherUnsignaledDeletedBox. A box [%i] has disconnected\n", _controlerBox.ui16NodeName);
//     }
//     _obj["action"] = "deleteBox";
//     _resetAllControlerBoxBoolsToTrueByCB(_controlerBox);
//     // expected _obj = {lb:1; action:"deleteBox"}
//     return true;
//   }
//   return false;
// }






// void myWSResponder::_checkBoxStateAndSendMsgATCMB(uint16_t _ui16BoxIndex, controllerBoxesCollection & _ctlBxColl, myWSSender & _myWSSender) {
//     // prepare a JSON document
//     StaticJsonDocument<256> _doc;
//     JsonObject _obj = _doc.to<JsonObject>();

//     // populate the JSON object
//     _obj["lb"] = _ui16BoxIndex;
//     _obj["action"] = "-1";
//     // expected _obj = {lb:1; action:-1}

//     // if the box is an unsignaled new box
//     _checkWhetherUnsignaledNewBox(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex), _obj);

//     // if the box has an unsignaled change of default state
//     _checkWhetherUnsignaledDefaultStateChange(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex), _obj);

//     // if the box has an unsignaled change of state
//     _checkWhetherUnsignaledBoxStateChange(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex), _obj);

//     // if the box is an unsignaled deleted box
//     _checkWhetherUnsignaledDeletedBox(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex), _obj);

//     // in each of the above cases, send a message to the clients
//     if (_obj["action"] != "-1") {
//       if (thisControllerBox.globBaseVars.MY_DG_WS) {
//         Serial.printf("myWSSender::_checkBoxStateAndSendMsgATCMB. About to call sendWSData with a message [\"action\"] = %s\n", _obj["action"].as<const char*>());
//       }
//       _myWSSender.sendWSData(_obj);
//     }

// }






/** myWSResponder::_tcbSendWSDataIfChangeBoxState(AsyncWebSocket & __server)
 * 
 *  _tcbSendWSDataIfChangeBoxState() iterates indefinitely every second,
 *  to check any changes in the controller boxes array.
 * 
 *  If it detects a change, it sends a websocket message to the browser with the relevant info.
 * 
 *  Tracked changes:
 *  (i) boxState changes;
 *  (ii) appearance or (iii) disappearance of a new box
*/
void myWSResponder::_tcbSendWSDataIfChangeBoxState() {
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): starting");

  /*********
   *  1. check wether any client is currently connected
   * 
   *     If no client is currently connected, just return, basta. */
  if (!(_asyncWebSocketInstance.count())) { return; }
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): WS has some client connected.");

  /**********
   *  2. else: there is at least one client currently connected:
   * 
   *      a. instantiate an myWSSender instance;
   *      b. iterate over the controller boxes array and look for any relevant change. */
  myWSSender _myWSSender(_asyncWebSocketInstance);
  for (uint16_t _ui16BoxIndex = 0; _ui16BoxIndex < thisControllerBox.globBaseVars.gui16BoxesCount; _ui16BoxIndex++) {
    _checkBoxStateAndSendMsgATCMB(_ui16BoxIndex, thisControllerBox.thisLaserSignalHandler.ctlBxColl, _myWSSender);
  }
}
