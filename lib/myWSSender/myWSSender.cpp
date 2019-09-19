/*
  myWSSender.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myWebServerControler.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerControler.h
  |  |  |  |--MasterSlaveBox.cpp (called to set some values re. master/slave box reactions in global)
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |--//LaserGroupedUnits.cpp
  |  |  |  |--//MasterSlaveBox.cpp
  |  |  |
  |  |  |--myWebServerViews.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerViews.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
  |  |  |  |--//LaserPin.cpp
  |  |  |
  |  |  |--myWebServerWS
  |  |  |  |--myWebServerWS.cpp
  |  |  |  |  |--myWebServerWS.h
  |  |  |  |--myWSReceiver
  |  |  |  |  |--myWSReceiver.cpp
  |  |  |  |  |  |--myWSReceiver.H
  |  |  |  |--myWSSender
  |  |  |  |  |--myWSSender.cpp
  |  |  |  |  |  |--myWSSender.H

*/

#include "Arduino.h"
#include <myWSSender.h>

AsyncWebSocket * myWSSender::server = nullptr;

myWSSender::myWSSender()
{
}







// Web Socket Message Senders
// Send WS message when I change Station IP
Task myWSSender::tSendWSDataIfChangeStationIp(10000, TASK_FOREVER, &_tcbSendWSDataIfChangeStationIp, NULL/*&mns::myScheduler*/, false, NULL, NULL);

void myWSSender::_tcbSendWSDataIfChangeStationIp() {
  Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp(). starting.");
  // if (MY_DG_WS) {
  //   Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp. interface station IP has changed.");
  //   Serial.printf("myWSSender::_tcbSendWSDataIfChangeStationIp. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
  // }

  myWSSender _myWSSender;
  if (MY_DG_WS) {
    Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp. about to call prepareWSData with parameter (3).");
  }
  /** TODO: treat the nullprt issue:
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
  _myWSSender.prepareWSData(3, nullptr); // 3 for message sent in case of change in station IP

  thisBox.updateThisBoxProperties();
  Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp(). starting.");
}







// Send WS message upon (i) boxState changes, (ii) appearance or (iii) disappearance of a new box
// This task runs for ever and checks whether the boxState of any of the Controller boxes connected to
// lasers has changed in the ControlerBox array of the Interface controller.
// If so, it send a WS message with the new information.
Task myWSSender::tSendWSDataIfChangeBoxState(500, TASK_FOREVER, &_tcbSendWSDataIfChangeBoxState, NULL/*&mns::myScheduler*/, false, NULL, NULL);

void myWSSender::_tcbSendWSDataIfChangeBoxState() {
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): starting.");
  // if no client has ever connected, just return
  if (server == nullptr) { return; }
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): WS has been started.");
  // if no client is currently connected, just return
  if (!(server->count())) { return; }
  // Serial.println("myWSSender::_tcbSendWSDataIfChangeBoxState(): WS has some client connected.");
  // else check the relevant changes and inform the clients
  myWSSender _myWSSender;
  for (uint16_t _ui16BoxIndex = 1; _ui16BoxIndex < gui16BoxesCount; _ui16BoxIndex++) {
    // prepare a JSON document
    StaticJsonDocument<256> _doc;
    JsonObject _obj = _doc.to<JsonObject>();

    // populate the JSON object
    _obj["lb"] = _ui16BoxIndex;
    _obj["action"] = "-1";
    // expected _obj = {lb:1; action:-1}

    // if the box is an unsignaled new box
    if (ControlerBoxes[_ui16BoxIndex].isNewBoxHasBeenSignaled == false) {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. In fact, a new box [%u] has joined.\n", ControlerBoxes[_ui16BoxIndex].ui16NodeName);
      }
      _obj["action"]      = "addBox";
      _obj["boxState"]    = ControlerBoxes[_ui16BoxIndex].i16BoxActiveState;
      _obj["masterbox"]   = ControlerBoxes[_ui16BoxIndex].getMasterBoxNameForWeb();
      _obj["boxDefstate"] = ControlerBoxes[_ui16BoxIndex].sBoxDefaultState;
      // expected _obj = {lb:1; action:"addBox"; boxState: 3; masterbox: 4; boxDefstate: 6}
      // reset all the booleans to true
      _resetAllControlerBoxBoolsToTrue(_ui16BoxIndex);
    }

    // if the box has an unsignaled change of default state
    if (ControlerBoxes[_ui16BoxIndex].sBoxDefaultStateChangeHasBeenSignaled == false) {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. Default state of box [%u] has changed\n", ControlerBoxes[_ui16BoxIndex].ui16NodeName);
      }
      _obj["action"] = "changeBox";
      _obj["key"] = "boxDefstate";
      _obj["val"] = ControlerBoxes[_ui16BoxIndex].sBoxDefaultState;
      _obj["st"] = 2;
      // expected _obj = {lb:1; action:"changeBox"; key: "boxDefstate"; val: 4; st: 2}
      ControlerBoxes[_ui16BoxIndex].sBoxDefaultStateChangeHasBeenSignaled = true;
    }

    // if the box has an unsignaled change of state
    if (ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenSignaled == false) {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. State of box [%u] has changed\n", ControlerBoxes[_ui16BoxIndex].ui16NodeName);
      }
      _obj["action"] = "changeBox";
      _obj["key"] = "boxState";
      _obj["val"] = ControlerBoxes[_ui16BoxIndex].i16BoxActiveState;
      _obj["st"] = 2;
      // expected _obj = {lb:1; action:"changeBox"; key: "boxState"; val: 6; st: 2}
      ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenSignaled = true;
    }

    // if the box master has changed
    if (ControlerBoxes[_ui16BoxIndex].bMasterBoxNameChangeHasBeenSignaled == false) {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. Box [%u] has changed master: ", ControlerBoxes[_ui16BoxIndex].ui16NodeName);
        Serial.printf("ControlerBoxes[%u].ui16MasterBoxName == %u\n", _ui16BoxIndex, ControlerBoxes[_ui16BoxIndex].ui16MasterBoxName);
      }
      _obj["action"] = "changeBox";
      _obj["key"] = "masterbox";
      _obj["val"] = ControlerBoxes[_ui16BoxIndex].getMasterBoxNameForWeb();
      _obj["st"] = 2; // "st" for status, 2 for executed
      // expected _obj = {lb:1; action:"changeBox"; key: "masterbox"; val: 9; st: 2}
      ControlerBoxes[_ui16BoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
    }

    // if the box is an unsignaled deleted box
    if (ControlerBoxes[_ui16BoxIndex].boxDeletionHasBeenSignaled == false) {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. A box [%i] has disconnected\n", ControlerBoxes[_ui16BoxIndex].ui16NodeName);
      }
      _obj["action"] = "deleteBox";
      _resetAllControlerBoxBoolsToTrue(_ui16BoxIndex);
      // expected _obj = {lb:1; action:"deleteBox"}
    }

    // in each of the above cases, send a message to the clients
    if (_obj["action"] != "-1") {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. About to call sendWSData with a message [\"action\"] = %s\n", _obj["action"].as<const char*>());
      }
      _myWSSender.sendWSData(_obj);
    }
  }
}






void myWSSender::prepareWSData(const int8_t _i8messageType, AsyncWebSocketClient * _client) {
  Serial.printf("- myWSSender::prepareWSData. starting. Message type [%i]\n", _i8messageType);
  StaticJsonDocument<900> __doc;
  JsonObject __newObj = __doc.to<JsonObject>();
  __newObj["action"] = _i8messageType;

  // message 0 on handshake: activate the exchange of station IP, ssid and pass
  if (_i8messageType == 0) {
    if (MY_DG_WS) {
      Serial.printf("- myWSSender::prepareWSData. Message type [%i]\n", _i8messageType);
      Serial.printf("- myWSSender::prepareWSData. Message type %i received. About to enable _tSendWSDataIfChangeStationIp\n", _i8messageType);
    }
    tSendWSDataIfChangeStationIp.enable();
    if (MY_DG_WS) {
      Serial.printf("- myWSSender::prepareWSData.  _tSendWSDataIfChangeStationIp enabled.\n");
    }
    // expected JSON obj: {"action":0}
  }

  // small confirmation messages (type 0 to 2)
  if (_i8messageType == 0 /*|| _i8messageType == 1*/ || _i8messageType == 2) {
    const char _messages_array[][30] = {"Hello WS Client","I got your WS text message","I got your WS binary message"};
    __newObj["message"] = _messages_array[_i8messageType];
    if (MY_DG_WS) {
      Serial.printf("- myWSSender::prepareWSData. _messages_array[%i] = %s\n", _i8messageType, _messages_array[_i8messageType]);
    }
    // expected JSON obj: {"action":0;"message":"Hello WS Client"}
    // expected JSON obj: {"action":1;"message":"I got your WS text message"}
    // expected JSON obj: {"action":2;"message":"I got your WS binary message"}
  }

  // message type 3: change in station IP
  if (_i8messageType == 3) {
    /** {"action":3,
     * "serverIP":[192,168,43,50],
     * "wifi":{"wssid":"LTVu_dG9ydG9y","wpass":"totototo","wgw":[192,168,43,50],"wgwp":5555,"wch":6,"wfip":[192,168,43,50],"wnm":[192,168,43,50]},
     * "rootIF":{"roNNa":200,"IFNNA":200},
     * "softAP":{"sssid":"ESP32-Access-Point","spass":"123456789","sIP":[192,168,43,50],"sgw":[192,168,43,50],"snm":[192,168,43,50]},
     * "mesh":{"mssid":"laser_boxes","mpass":"somethingSneaky","mport":5555}} */
    if (MY_DG_WS) {
      Serial.printf("- myWSSender::prepareWSData. Message type [%i]. About to allot __newObj[\"serverIP\"] = (laserControllerMesh.getStationIP()).toString()\n", _i8messageType);
      Serial.printf("- myWSSender::prepareWSData. Message type [%i]. server IP ", _i8messageType);Serial.println((laserControllerMesh.getStationIP()).toString());
    }

    // Real IP of the Interface
    __newObj["serverIP"]        = ( isRoot ? WiFi.localIP().toString() : WiFi.softAPIP().toString() );
    
    // Wifi Settings of External Network (in case the IF is served on the station interface of the ESP)
    JsonObject __wifiSettings   = __newObj.createNestedObject("wifi");
    __wifiSettings["wssid"]     = ssid;
    __wifiSettings["wpass"]     = pass;
    __wifiSettings["wgw"]       = gatewayIP.toString();
    __wifiSettings["wgwp"]      = ui16GatewayPort;
    __wifiSettings["wch"]       = ui8WifiChannel;
    __wifiSettings["wfip"]      = fixedIP.toString();
    __wifiSettings["wnm"]       = fixedNetmaskIP.toString();

    // Root and Interface Nodes Params
    JsonObject __rootIFSettings = __newObj.createNestedObject("rootIF");
    if (isRoot) {
      __rootIFSettings["roNNa"] = thisBox.ui16NodeName;
    } else {
      uint16_t _bxIndex           = ControlerBox::findIndexByNodeId(ui32RootNodeId);
      __rootIFSettings["roNNa"]   = ( (_bxIndex == 254) ? _bxIndex : ControlerBoxes[_bxIndex].ui16NodeName );
    }
    __rootIFSettings["IFNNA"]   = thisBox.ui16NodeName;

    // Soft AP Settings (in case the IF is served on the softAP of the ESP)
    JsonObject __softAPSettings = __newObj.createNestedObject("softAP");
    __softAPSettings["sssid"]   = softApSsid;
    __softAPSettings["spass"]   = softApPassword;
    __softAPSettings["sIP"]     = softApMyIp.toString();
    __softAPSettings["sgw"]     = softApMeAsGatewayIp.toString();
    __softAPSettings["snm"]     = softApNetmask.toString();
    __softAPSettings["shi"]     = softApHidden;
    __softAPSettings["smc"]     = softApMaxConnection;

    // Mesh settings
    JsonObject __meshSettings   = __newObj.createNestedObject("mesh");
    __meshSettings["mssid"]     = meshPrefix;
    __meshSettings["mpass"]     = meshPass;
    __meshSettings["mport"]     = meshPort;
    __meshSettings["mhi"]       = meshHidden;
    __meshSettings["mmc"]       = meshMaxConnection;
  }

  // Send a message to the newly connected client
  sendWSData(__newObj, _client);

  if (MY_DG_WS) {
    Serial.println("- myWSSender::prepareWSData. over.");
  }
}














void myWSSender::sendWSData(JsonObject& _joMsg, AsyncWebSocketClient * _client) {
    if (MY_DG_WS) {
      Serial.println("- myWSSender::sendWSData. starting.");
    }

    // If no client has ever even tried to connect, the websocket server is not yet started. Return.
    if (server == nullptr) {
      Serial.println("- myWSSender::sendWSData. Websocket server not yet started. Not sending anything.");
      return;
    }

    // If no client is registered with the WebSocket, just return
    if (!(server->count())) {
      Serial.println("- myWSSender::sendWSData. No clients connected. Not sending anything.");
      return;
    }

    // The message is addressed to a specific client or to any client
    // Measure the size of the message
    size_t _len = measureJson(_joMsg);
    // Make a buffer to hold it
    AsyncWebSocketMessageBuffer * _buffer = server->makeBuffer(_len); //  creates a buffer (len + 1) for you.
    // Serialize the message into the buffer
    serializeJson(_joMsg, (char *)_buffer->get(), _len + 1);
    // Display the serialized message to the console
    Serial.print("- myWSSender::sendWSData. Serialized message: ");serializeJson(_joMsg, Serial);Serial.println();

    // If the message is addressed to any connected client, send it to all the clients
    if (_client == nullptr) {
      server->textAll(_buffer);
      Serial.println("- myWSSender::sendWSData. The message was sent to all the clients.\n");
      return;
    }

    // The message is addressed to a specific client, send it to the targeted client
    _client->text(_buffer);

    if (MY_DG_WS) {
      Serial.println(F("- myWSSender::sendWSData. over."));
    }
}





void myWSSender::_resetAllControlerBoxBoolsToTrue(const uint16_t _ui16BoxIndex) {
  ControlerBoxes[_ui16BoxIndex].isNewBoxHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].sBoxDefaultStateChangeHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].boxDeletionHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
}
