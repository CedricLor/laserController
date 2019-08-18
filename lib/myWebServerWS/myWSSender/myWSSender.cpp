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
#include "myWSSender.h"



myWSSender::myWSSender()
{
}







// Web Socket Message Senders
// Send WS message when I change Station IP
Task myWSSender::tSendWSDataIfChangeStationIp(10000, TASK_FOREVER, &_tcbSendWSDataIfChangeStationIp, &userScheduler, false);

void myWSSender::_tcbSendWSDataIfChangeStationIp() {
  // if (!(laserControllerMesh.getStationIP() == ControlerBoxes[gui16MyIndexInCBArray].stationIP)) {
    // if (MY_DG_WS) {
    //   Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp. interface station IP has changed.");
    // }
    // Serial.printf("myWSSender::_tcbSendWSDataIfChangeStationIp. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());

    myWSSender _myWSSender;
    if (MY_DG_WS) {
      Serial.println("myWSSender::_tcbSendWSDataIfChangeStationIp. about to call prepareWSData with parameter (3).");
    }
    _myWSSender.prepareWSData(3); // 3 for message sent in case of change in station IP

    ControlerBoxes[gui16MyIndexInCBArray].updateThisBoxProperties();
  // }
}







// Send WS message upon (i) boxState changes, (ii) appearance or (iii) disappearance of a new box
// This task runs for ever and checks whether the boxState of any of the Controller boxes connected to
// lasers has changed in the ControlerBox array of the Interface controller.
// If so, it send a WS message with the new information.
Task myWSSender::tSendWSDataIfChangeBoxState(500, TASK_FOREVER, &_tcbSendWSDataIfChangeBoxState, &userScheduler, false);

void myWSSender::_tcbSendWSDataIfChangeBoxState() {
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
      _obj["action"] = "addBox";
      _obj["boxState"] = ControlerBoxes[_ui16BoxIndex].i16BoxActiveState;
      _obj["masterbox"] = ControlerBoxes[_ui16BoxIndex].ui16MasterBoxName - gui16ControllerBoxPrefix;
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
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. A box [%u] has changed master\n", ControlerBoxes[_ui16BoxIndex].ui16NodeName);
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. (ControlerBoxes[%u].ui16MasterBoxName) == %u\n", _ui16BoxIndex, (ControlerBoxes[_ui16BoxIndex].ui16MasterBoxName));
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. New master: %u\n", (int)(ControlerBoxes[_ui16BoxIndex].ui16MasterBoxName));
      }
      _obj["action"] = "changeBox";
      _obj["key"] = "masterbox";
      _obj["val"] = ControlerBoxes[_ui16BoxIndex].ui16MasterBoxName - gui16ControllerBoxPrefix;
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

    // in each of the above cases, send a message to the browser
    if (_obj["action"] != "-1") {
      if (MY_DG_WS) {
        Serial.printf("- myWSSender::_tcbSendWSDataIfChangeBoxState. About to call sendWSData with a message [\"action\"] = %s\n", _obj["action"].as<const char*>());
      }
      _myWSSender.sendWSData(_obj);
    }
  }
}






void myWSSender::prepareWSData(const int8_t _i8messageType) {
  Serial.printf("- myWSSender::prepareWSData. Starting. Message type [%i]\n", _i8messageType);
  StaticJsonDocument<256> __doc;
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
    if (MY_DG_WS) {
      Serial.printf("- myWSSender::prepareWSData. Message type [%i]. About to allot __newObj[\"serverIP\"] = (laserControllerMesh.getStationIP()).toString()\n", _i8messageType);
      Serial.printf("- myWSSender::prepareWSData. Message type [%i]. server IP ", _i8messageType);Serial.println((laserControllerMesh.getStationIP()).toString());
    }
    __newObj["serverIP"] = ((isRoot == true) ? (WiFi.localIP().toString()) : (WiFi.softAPIP().toString()));
    __newObj["ssid"] = ssid;
    __newObj["pass"] = pass;
    __newObj["gatewayIP"] = gatewayIP.toString();
    __newObj["ui16GatewayPort"] = ui16GatewayPort;
    __newObj["ui8WifiChannel"] = ui8WifiChannel;
    __newObj["fixedIP"] = fixedIP.toString();
    __newObj["fixedNetmaskIP"] = fixedNetmaskIP.toString();
    // expected JSON obj:  {"action":3;"serverIP":"192.168.43.84"}
  }

  sendWSData(__newObj);

  if (MY_DG_WS) {
    Serial.println("- myWSSender::prepareWSData. Ending.");
  }
}














void myWSSender::sendWSData(JsonObject& _joMsg) {
    if (MY_DG_WS) {
      Serial.println("- myWSSender::sendWSData. Starting.");
    }

    size_t _len = measureJson(_joMsg);
    AsyncWebSocketMessageBuffer * _buffer = myWebServerWS::ws.makeBuffer(_len); //  creates a buffer (len + 1) for you.

    if (_buffer) {
        serializeJson(_joMsg, (char *)_buffer->get(), _len + 1);
        Serial.print("- myWSSender::sendWSData. Serialized message: ");
        serializeJson(_joMsg, Serial);
        Serial.println();
        
        // Before sending anything, check if you have any client
        if (size_t _client_count = myWebServerWS::ws.count()) {
          // check that you have the client_id
          // check that the client with the client_id you have is not pointing to nothing
          // check if the client is connected
          if (myWebServerWS::ws_client_id &&
            myWebServerWS::ws.client(myWebServerWS::ws_client_id) != nullptr &&
            myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status() == WS_CONNECTED) {
            if (MY_DG_WS) Serial.printf("- myWSSender::sendWSData. Sending the WS message to client [%i].\n", myWebServerWS::ws_client_id);
            myWebServerWS::ws.client(myWebServerWS::ws_client_id)->text(_buffer);
            if (MY_DG_WS) Serial.println("- myWSSender::sendWSData. Message sent\n");
          }  // end if (myWebServerWS::ws_client_id && ..
          else {
            // the ws_client_id I have does not match existing client
            // send the info to all the clients
            if (MY_DG_WS)  Serial.printf("- myWSSender::sendWSData. Client [%u] not found. Sending the WS message message to all %u clients.\n", myWebServerWS::ws_client_id, _client_count);
            myWebServerWS::ws.textAll(_buffer);
            if (MY_DG_WS) Serial.println(F("- myWSSender::sendWSData. Message broadcasted"));
          } // end else
        } // end if (size_t _client_count = myWebServerWS::ws.count()) {...}
        else { // there is no client connected
          if (MY_DG_WS) Serial.println(F("- myWSSender::sendWSData: The message could not be sent because there are no WS client connected."));
        }
    }
    if (MY_DG_WS) {
      Serial.println(F("- myWSSender::sendWSData. Ending."));
    }
}





void myWSSender::_resetAllControlerBoxBoolsToTrue(const uint16_t _ui16BoxIndex) {
  ControlerBoxes[_ui16BoxIndex].isNewBoxHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].boxActiveStateHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].sBoxDefaultStateChangeHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].boxDeletionHasBeenSignaled = true;
  ControlerBoxes[_ui16BoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
}
