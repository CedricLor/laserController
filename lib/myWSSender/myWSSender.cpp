/*
  myWSSender.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include <myWSSender.h>








///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// myWSSender class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
myWSSender::myWSSender(AsyncWebSocket & __server):
  _server(__server)
{ }







void myWSSender::prepareAllIFDataMessage(AsyncWebSocketClient * _client) {
  Serial.printf("myWSSender::prepareAllIFDataMessage. starting.\n");

  StaticJsonDocument<900> __doc;
  JsonObject _joMsg = __doc.to<JsonObject>();
  _joMsg["action"] = 3;

  /** {"action":3,
   * "serverIP":[192,168,43,50],
   * "wifi":{"wssid":"LTVu_dG9ydG9y","wpass":"totototo","wgw":[192,168,43,50],"wgwp":5555,"wch":6,"wfip":[192,168,43,50],"wnm":[192,168,43,50]},
   * "rootIF":{"roNNa":200,"IFNNA":200},
   * "softAP":{"sssid":"ESP32-Access-Point","spass":"123456789","sIP":[192,168,43,50],"sgw":[192,168,43,50],"snm":[192,168,43,50]},
   * "mesh":{"mssid":"laser_boxes","mpass":"somethingSneaky","mport":5555}} */
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSSender::_prepareAllIFDataMessage. About to allot _joMsg[\"serverIP\"] = (globalBaseVariables.laserControllerMesh.getStationIP()).toString()\n");
    Serial.printf("myWSSender::_prepareAllIFDataMessage. server IP: ");Serial.println((globalBaseVariables.laserControllerMesh.getStationIP()).toString());
  }

  // Real IP of the Interface
  _joMsg["serverIP"]        = ( globalBaseVariables.isRoot ? WiFi.localIP().toString() : WiFi.softAPIP().toString() );
  
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
  if (globalBaseVariables.isRoot) {
    __rootIFSettings["roNNa"] = thisControllerBox.thisCtrlerBox.ui16NodeName;
  } else {
    uint16_t _bxIndex           = thisControllerBox.thisSignalHandler.ctlBxColl.findIndexByNodeId(globalBaseVariables.ui32RootNodeId);
    __rootIFSettings["roNNa"]   = ( (_bxIndex == 254) ? _bxIndex : thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_bxIndex).ui16NodeName );
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
    if (globalBaseVariables.MY_DG_WS) {
      Serial.println("myWSSender::sendWSData. starting");
    }

    // If no client is registered with the WebSocket, just return
    if (!(_server.count())) {
      Serial.println("myWSSender::sendWSData. No clients connected. Not sending anything.");
      return;
    }

    // The message is addressed to a specific client or to any client
    // Measure the size of the message
    size_t _len = measureJson(_joMsg);
    // Make a buffer to hold it
    AsyncWebSocketMessageBuffer * _buffer = _server.makeBuffer(_len); //  creates a buffer (len + 1) for you.
    // Serialize the message into the buffer
    serializeJson(_joMsg, (char *)_buffer->get(), _len + 1);
    // Display the serialized message to the console
    Serial.print("myWSSender::sendWSData. Serialized message: ");serializeJson(_joMsg, Serial);Serial.println();

    // If the message is addressed to any connected client, send it to all the clients
    if (_client == nullptr) {
      _server.textAll(_buffer);
      Serial.println("myWSSender::sendWSData. The message was sent to all the clients.\n");
      return;
    }

    // The message is addressed to a specific client, send it to the targeted client
    _client->text(_buffer);

    if (globalBaseVariables.MY_DG_WS) {
      Serial.println(F("myWSSender::sendWSData. over"));
    }
}















///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// myWSResponder class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
myWSResponder::myWSResponder(AsyncWebSocket & __asyncWebSocketInstance) :
  _asyncWebSocketInstance(__asyncWebSocketInstance),
  _myWSSenderTasks({__asyncWebSocketInstance})
{
}






myWSSenderTasks & myWSResponder::getMyWSSenderTasks() {
  return _myWSSenderTasks;
}







///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// myWSSenderTasks class
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
myWSSenderTasks::myWSSenderTasks(AsyncWebSocket & __asyncWebSocketInstance) :
  _asyncWebSocketInstance(__asyncWebSocketInstance)
{
  tSendWSDataIfChangeStationIp.set(10000, TASK_FOREVER, [&](){ return _tcbSendWSDataIfChangeStationIp();});
  tSendWSDataIfChangeBoxState.set(500, TASK_FOREVER, [&](){ return _tcbSendWSDataIfChangeBoxState();});
}






/** myWSSenderTasks::_tcbSendWSDataIfChangeStationIp() 
 * 
 *  Callback for Task tSendWSDataIfChangeStationIp.
 *  
 *  Sends a websocket message when detecting a change Station IP
 *  (Task tSendWSDataIfChangeStationIp iterates every 10 seconds and
 *  checks whether a change in station IP has occured). 
 * */
void myWSSenderTasks::_tcbSendWSDataIfChangeStationIp() {
  Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp(). starting");
  // if (globalBaseVariables.MY_DG_WS) {
  //   Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp. interface station IP has changed.");
  //   Serial.printf("myWSSender::_tcbSendWSDataIfChangeStationIp. globalBaseVariables.laserControllerMesh.subConnectionJson() = %s\n",globalBaseVariables.laserControllerMesh.subConnectionJson().c_str());
  // }

  if (globalBaseVariables.MY_DG_WS) {
    Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp. about to call prepareWSData with parameter (3).");
  }
  /** TODO: treat the nullptr issue:
    * This Task shall be enabled:
    * (i) if one of the network parameter has changed, an array of all the clients shall be constructed,
    *  the new data shall be broadcasted to all the clients, the clients shall be removed from the array
    *  when their "received IP" message has been received and when the array is empty, the Task shall 
    *  be disabled; 
    * (ii) if a new client has connected and it shall send to the specific new client (make an 
    *  array of new clients, send the data to the new clients only, and the received IP message
    *  sent by the clients shall serve to remove clients from this array). Once all the clients have
    *  received the data, the Task shall be disabled.
    *  
    *  Currently, the Task is enabled upon connection of a new client and it is disabled upon reception
    *  of "Received IP" message from any client.
    *   */
  myWSSender _myWSSender(_asyncWebSocketInstance);
  _myWSSender.prepareAllIFDataMessage(nullptr); // 3 for message sent in case of change in station IP

  thisControllerBox.updateThisBoxProperties();
  Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp(). ending");
}






/** myWSSenderTasks::_tcbSendWSDataIfChangeBoxState(AsyncWebSocket & __server)
 * 
 *  _tcbSendWSDataIfChangeBoxState() iterates indefinitely every second,
 *  to check any changes in the controller boxes array.
 * 
 *  If it detects a change, it sends a websocket message to the browser with the relevant info.
 * 
 *  Changes supervised:
 *  (i) boxState changes;
 *  (ii) appearance or (iii) disappearance of a new box
*/
void myWSSenderTasks::_tcbSendWSDataIfChangeBoxState() {
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): starting");

  /** 1. check wether any client is currently connected
   * 
   *     If no client is currently connected, just return, basta. */
  if (!(_asyncWebSocketInstance.count())) { return; }
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): WS has some client connected.");

  /** 2. else: there is at least one client currently connected:
   * 
   *      a. instantiate a myWSSender instance;
   *      b. iterate over the controller boxes array and look for any relevant change. */
  myWSSender _myWSSender(_asyncWebSocketInstance);
  for (uint16_t _ui16BoxIndex = 0; _ui16BoxIndex < globalBaseVariables.gui16BoxesCount; _ui16BoxIndex++) {
    // prepare a JSON document
    StaticJsonDocument<256> _doc;
    JsonObject _obj = _doc.to<JsonObject>();

    // populate the JSON object
    _obj["lb"] = _ui16BoxIndex;
    _obj["action"] = "-1";
    // expected _obj = {lb:1; action:-1}

    // if the box is an unsignaled new box
    if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).isNewBoxHasBeenSignaled == false) {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. In fact, a new box [%u] has joined.\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).ui16NodeName);
      }
      _obj["action"]      = "addBox";
      _obj["boxState"]    = thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).i16BoxActiveState;
      _obj["boxDefstate"] = thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).sBoxDefaultState;
      // expected _obj = {lb:1; action:"addBox"; boxState: 3; boxDefstate: 6}
      // reset all the booleans to true
      _resetAllControlerBoxBoolsToTrue(_ui16BoxIndex);
    }

    // if the box has an unsignaled change of default state
    if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).sBoxDefaultStateChangeHasBeenSignaled == false) {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. Default state of box [%u] has changed\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).ui16NodeName);
      }
      _obj["action"] = "changeBox";
      _obj["key"] = "boxDefstate";
      _obj["val"] = thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).sBoxDefaultState;
      _obj["st"] = 2;
      // expected _obj = {lb:1; action:"changeBox"; key: "boxDefstate"; val: 4; st: 2}
      thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).sBoxDefaultStateChangeHasBeenSignaled = true;
    }

    // if the box has an unsignaled change of state
    if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).boxActiveStateHasBeenSignaled == false) {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. State of box [%u] has changed\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).ui16NodeName);
      }
      _obj["action"] = "changeBox";
      _obj["key"] = "boxState";
      _obj["val"] = thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).i16BoxActiveState;
      _obj["st"] = 2;
      // expected _obj = {lb:1; action:"changeBox"; key: "boxState"; val: 6; st: 2}
      thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).boxActiveStateHasBeenSignaled = true;
    }

    // if the box is an unsignaled deleted box
    if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).boxDeletionHasBeenSignaled == false) {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. A box [%i] has disconnected\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).ui16NodeName);
      }
      _obj["action"] = "deleteBox";
      _resetAllControlerBoxBoolsToTrue(_ui16BoxIndex);
      // expected _obj = {lb:1; action:"deleteBox"}
    }

    // in each of the above cases, send a message to the clients
    if (_obj["action"] != "-1") {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. About to call sendWSData with a message [\"action\"] = %s\n", _obj["action"].as<const char*>());
      }
      _myWSSender.sendWSData(_obj);
    }
  }
}






void myWSSenderTasks::_resetAllControlerBoxBoolsToTrue(const uint16_t _ui16BoxIndex) {
  thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).isNewBoxHasBeenSignaled = true;
  thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).boxActiveStateHasBeenSignaled = true;
  thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).sBoxDefaultStateChangeHasBeenSignaled = true;
  thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_ui16BoxIndex).boxDeletionHasBeenSignaled = true;
}