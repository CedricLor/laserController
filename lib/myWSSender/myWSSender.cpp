/*
  myWSSender.cpp - Library to handle web socket responder and sender related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include <myWSSender.h>








///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// myWSSender class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
myWSSender::myWSSender(AsyncWebSocket & __asyncWebSocketInstance):
  _asyncWebSocketInstance(__asyncWebSocketInstance)
{ }







void myWSSender::prepareAllIFDataMessage(AsyncWebSocketClient * _client) {
  Serial.printf("myWSSender::prepareAllIFDataMessage. starting.\n");

  thisControllerBox.updateThisBoxProperties();

  StaticJsonDocument<900> __doc;
  JsonObject _joMsg = __doc.to<JsonObject>();
  _joMsg["action"] = 3;

  /** {"action":3,
   * "serverIP":[192,168,43,50],
   * "wifi":{"wssid":"LTVu_dG9ydG9y","wpass":"totototo","wgw":[192,168,43,50],"wgwp":5555,"wch":6,"wfip":[192,168,43,50],"wnm":[192,168,43,50]},
   * "rootIF":{"roNNa":200,"IFNNA":200},
   * "softAP":{"sssid":"ESP32-Access-Point","spass":"123456789","sIP":[192,168,43,50],"sgw":[192,168,43,50],"snm":[192,168,43,50]},
   * "mesh":{"mssid":"laser_boxes","mpass":"somethingSneaky","mport":5555}} */
  if (thisControllerBox.globBaseVars.MY_DG_WS) {
    Serial.printf("myWSSender::_prepareAllIFDataMessage. About to allot _joMsg[\"serverIP\"] = (thisControllerBox.globBaseVars.laserControllerMesh.getStationIP()).toString()\n");
    Serial.printf("myWSSender::_prepareAllIFDataMessage. server IP: ");Serial.println((thisControllerBox.globBaseVars.laserControllerMesh.getStationIP()).toString());
  }

  // Real IP of the Interface
  _joMsg["serverIP"]        = ( thisControllerBox.globBaseVars.isRoot ? WiFi.localIP().toString() : WiFi.softAPIP().toString() );
  
  // Wifi Settings of External Network (in case the IF is served on the station interface of the ESP)
  JsonObject __wifiSettings   = _joMsg.createNestedObject("wifi");
  __wifiSettings["wssid"]     = ssid;
  __wifiSettings["wpass"]     = pass;
  __wifiSettings["wgw"]       = gatewayIP.toString();
  __wifiSettings["wgwp"]      = ui16GatewayPort;
  __wifiSettings["wch"]       = ui8WifiChannel;
  __wifiSettings["wfip"]      = fixedIP.toString();
  __wifiSettings["wnm"]       = fixedNetmaskIP.toString();

  // Root and Interface Nodes Params
  JsonObject __rootIFSettings = _joMsg.createNestedObject("rootIF");
  if (thisControllerBox.globBaseVars.isRoot) {
    __rootIFSettings["roNNa"] = thisControllerBox.thisCtrlerBox.ui16NodeName;
  } else {
    uint16_t _bxIndex           = thisControllerBox.thisLaserSignalHandler.ctlBxColl.findIndexByNodeId(thisControllerBox.globBaseVars.ui32RootNodeId);
    __rootIFSettings["roNNa"]   = ( (_bxIndex == 254) ? _bxIndex : thisControllerBox.thisLaserSignalHandler.ctlBxColl.controllerBoxesArray.at(_bxIndex).ui16NodeName );
  }
  __rootIFSettings["IFNNA"]   = thisControllerBox.thisCtrlerBox.ui16NodeName;

  // Soft AP Settings (in case the IF is served on the softAP of the ESP)
  JsonObject __softAPSettings = _joMsg.createNestedObject("softAP");
  __softAPSettings["sssid"]   = softApSsid;
  __softAPSettings["spass"]   = softApPassword;
  __softAPSettings["sIP"]     = softApMyIp.toString();
  __softAPSettings["sgw"]     = softApMeAsGatewayIp.toString();
  __softAPSettings["snm"]     = softApNetmask.toString();
  __softAPSettings["shi"]     = softApHidden;
  __softAPSettings["smc"]     = softApMaxConnection;

  // Mesh settings
  JsonObject __meshSettings   = _joMsg.createNestedObject("mesh");
  __meshSettings["mssid"]     = meshPrefix;
  __meshSettings["mpass"]     = meshPass;
  __meshSettings["mport"]     = meshPort;
  __meshSettings["mhi"]       = meshHidden;
  __meshSettings["mmc"]       = meshMaxConnection;

  // send the message
  sendWSData(_joMsg, _client);
}





void myWSSender::sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client) {
    if (thisControllerBox.globBaseVars.MY_DG_WS) {
      Serial.println("myWSSender::sendWSData. starting");
    }

    /** 0. If no client is registered with the WebSocket, just return */
    if (!(_asyncWebSocketInstance.count())) {
      Serial.println("myWSSender::sendWSData. No clients connected. Not sending anything.");
      return;
    }
    /** The message is addressed to a specific client or to any client */

    /** 1. Serialize the JSON message into a buffer */
    AsyncWebSocketMessageBuffer * _buffer = _loadBuffer(_joMsg);

    /** 2. Send the buffered message */
    _sendMsg(_buffer, _client);

    if (thisControllerBox.globBaseVars.MY_DG_WS) {Serial.println(F("myWSSender::sendWSData. over"));}
}





AsyncWebSocketMessageBuffer * myWSSender::_loadBuffer(JsonObject& _joMsg) {
  /** 1. Measure the size of the message */
  size_t _len = measureJson(_joMsg);
  /** 2. Create a buffer to hold the message */
  AsyncWebSocketMessageBuffer * _buffer = _asyncWebSocketInstance.makeBuffer(_len); // makeBuffer creates a buffer of length == len + 1
  /** 3. Serialize the message into the buffer */
  serializeJson(_joMsg, (char *)_buffer->get(), _len + 1);
  Serial.printf("myWSSender::_loadBuffer. Serialized message: %s\n", (char *)_buffer->get());
  return _buffer;
}





void myWSSender::_sendMsg(AsyncWebSocketMessageBuffer * _buffer, AsyncWebSocketClient * _client) {
  /** 5. If the message is addressed to any connected client, send it to all the clients */
  if (_client == nullptr) {
    _asyncWebSocketInstance.textAll(_buffer);
    Serial.println("myWSSender::_sendMsg. The message was sent to all the clients.\n");
    return;
  }

  /** 6. If the message is addressed to a specific client, send it to the target client */
  _client->text(_buffer);
}
















///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// myWSResponder class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
myWSResponder::myWSResponder(AsyncWebSocket & __asyncWebSocketInstance) :
  _asyncWebSocketInstance(__asyncWebSocketInstance)
{
  tSendWSDataIfChangeStationIp.set(10000, TASK_FOREVER, [&](){ return _tcbSendWSDataIfChangeStationIp();});
  tSendWSDataIfChangeBoxState.set(500, TASK_FOREVER, [&](){ return _tcbSendWSDataIfChangeBoxState();});
}







/** myWSResponder::_tcbSendWSDataIfChangeStationIp() 
 * 
 *  Callback for Task tSendWSDataIfChangeStationIp.
 *  
 *  Sends a websocket message when detecting a change Station IP
 *  (Task tSendWSDataIfChangeStationIp iterates every 10 seconds and
 *  checks whether a change in station IP has occured). 
 * */
void myWSResponder::_tcbSendWSDataIfChangeStationIp() {
  Serial.println("myWSResponder::_tcbSendWSDataIfChangeStationIp(). starting");

  /** TODO: treat the nullptr issue:
    * This Task shall be enabled:
    * (i) if one of the IFData (see in prepareAllIFDataMessage) has changed:
    *     - an array of all the clients shall be constructed,
    *     - the new data shall be broadcasted to all the clients, 
    *     - the clients shall be removed from the array when their "received IP" message has been received.
    *     - once the array is empty, the Task shall be disabled; 
    * (ii) if a new client has connected to the websocket:
    *     - it shall work more or less as it currently works (i.e. send the data to the new client;
    *       once the data has been received ("received IP" message), the Task is disabled), except 
    *       that "nullptr" shall be replaced by the relevant client instance.
    *  
    *  Currently, the Task is enabled upon connection of a new client and it is disabled upon reception
    *  of "Received IP" message from any client.
    *   */
  myWSSender _myWSSender(_asyncWebSocketInstance);
  _myWSSender.prepareAllIFDataMessage(nullptr); // 3 for message sent in case of change in station IP

  Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp(). ending");
}






bool myWSResponder::_checkWhetherUnsignaledNewBox(ControlerBox & _controlerBox) {
  if (_controlerBox.isNewBoxHasBeenSignaled == false) {
    if (thisControllerBox.globBaseVars.MY_DG_WS) { Serial.printf("myWSSender::_checkWhetherUnsignaledNewBox. In fact, a new box [%u] has joined.\n", _controlerBox.ui16NodeName); }
    _joMsg["action"]      = "addBox";
    _joMsg["boxState"]    = _controlerBox.i16BoxActiveState;
    _joMsg["boxDefstate"] = _controlerBox.sBoxDefaultState;
    // expected _joMsg = {lb:1; action:"addBox"; boxState: 3; boxDefstate: 6}
    // reset all the booleans to true (boxState and boxDefstate have just been read; they are no longer unsignaled.)
    _resetAllControlerBoxBoolsToTrueByCB(_controlerBox);
    return true;
  }
  return false;
}






bool myWSResponder::_checkWhetherUnsignaledDefaultStateChange(ControlerBox & _controlerBox) {
  if (_controlerBox.sBoxDefaultStateChangeHasBeenSignaled == false) {
    if (thisControllerBox.globBaseVars.MY_DG_WS) { Serial.printf("myWSSender::_checkWhetherUnsignaledDefaultStateChange. Default state of box [%u] has changed\n", _controlerBox.ui16NodeName); }
    _joMsg["action"] = "changeBox";
    _joMsg["key"] = "boxDefstate";
    _joMsg["val"] = _controlerBox.sBoxDefaultState;
    _joMsg["st"] = 2;
    // expected _joMsg = {lb:1; action:"changeBox"; key: "boxDefstate"; val: 4; st: 2}
    _controlerBox.sBoxDefaultStateChangeHasBeenSignaled = true;
    return true;
  }
  return false;
}






bool myWSResponder::_checkWhetherUnsignaledBoxStateChange(ControlerBox & _controlerBox) {
  if (thisControllerBox.globBaseVars.MY_DG_WS) { Serial.printf("myWSSender::_checkWhetherUnsignaledBoxStateChange. State of box [%u] has changed\n", _controlerBox.ui16NodeName); }
  _joMsg["action"] = "changeBox";
  _joMsg["key"] = "boxState";
  _joMsg["val"] = _controlerBox.i16BoxActiveState;
  _joMsg["st"] = 2;
  // expected _joMsg = {lb:1; action:"changeBox"; key: "boxState"; val: 6; st: 2}
  return true;
}






bool myWSResponder::_checkWhetherUnsignaledDeletedBox(ControlerBox & _controlerBox) {
  if (_controlerBox.boxDeletionHasBeenSignaled == false) {
    if (thisControllerBox.globBaseVars.MY_DG_WS) {
      Serial.printf("myWSSender::_checkWhetherUnsignaledDeletedBox. A box [%i] has disconnected\n", _controlerBox.ui16NodeName);
    }
    _joMsg["action"] = "deleteBox";
    _resetAllControlerBoxBoolsToTrueByCB(_controlerBox);
    // expected _joMsg = {lb:1; action:"deleteBox"}
    return true;
  }
  return false;
}






void myWSResponder::_checkBoxStateAndSendMsgATCMB(uint16_t _ui16BoxIndex, controllerBoxesCollection & _ctlBxColl, myWSSender & _myWSSender) {
    _jDoc.clear();

    // populate the JSON object
    _joMsg["lb"] = _ui16BoxIndex;
    _joMsg["action"] = "-1";
    // expected _joMsg = {lb:1; action:-1}

    // if the box is an unsignaled new box
    _checkWhetherUnsignaledNewBox(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex));

    // if the box has an unsignaled change of default state
    _checkWhetherUnsignaledDefaultStateChange(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex));

    // if the box is an unsignaled deleted box
    _checkWhetherUnsignaledDeletedBox(_ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex));

    // in each of the above cases, send a message to the clients
    if (_joMsg["action"] != "-1") {
      if (thisControllerBox.globBaseVars.MY_DG_WS) {
        Serial.printf("myWSSender::_checkBoxStateAndSendMsgATCMB. About to call sendWSData with a message [\"action\"] = %s\n", _joMsg["action"].as<const char*>());
      }
      _myWSSender.sendWSData(_joMsg);
    }

}






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






void myWSResponder::_resetAllControlerBoxBoolsToTrueByIdNumber(const uint16_t _ui16BoxIndex) {
  _resetAllControlerBoxBoolsToTrueByCB(thisControllerBox.thisLaserSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex));
}






void myWSResponder::_resetAllControlerBoxBoolsToTrueByCB(ControlerBox & _controlerBox) {
  _controlerBox.isNewBoxHasBeenSignaled = true;
  _controlerBox.sBoxDefaultStateChangeHasBeenSignaled = true;
  _controlerBox.boxDeletionHasBeenSignaled = true;
}





void myWSResponder::signalNewBox(ControlerBox & _controlerBox) {
  _checkWhetherUnsignaledBoxStateChange(_controlerBox);
}