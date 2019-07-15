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


StaticJsonDocument<0> _empty_doc;
JsonObject myWSSender::_empty_obj = _empty_doc.to<JsonObject>();


myWSSender::myWSSender()
{
}







// Web Socket Message Senders
// Send WS message when I change Station IP
Task myWSSender::tSendWSDataIfChangeStationIp(10000, TASK_FOREVER, &_tcbSendWSDataIfChangeStationIp, &userScheduler, false);

void myWSSender::_tcbSendWSDataIfChangeStationIp() {
  // if (!(laserControllerMesh.getStationIP() == ControlerBoxes[myIndexInCBArray].stationIP)) {
    Serial.println("myWebServerWS::_tcbSendWSDataIfChangeStationIp. interface station IP has changed.");
    // Serial.printf("myWebServerWS::_tcbSendWSDataIfChangeStationIp. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());

    myWSSender _myWSSender;
    _myWSSender.prepareWSData(3); // 3 for message sent in case of change in station IP

    ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();
  // }
}







// Send WS message upon (i) boxState changes, (ii) appearance or (iii) disappearance of a new box
// This task runs for ever and checks whether the boxState of any of the Controller boxes connected to
// lasers has changed in the ControlerBox array of the Interface controller.
// If so, it send a WS message with the new information.
Task myWSSender::tSendWSDataIfChangeBoxState(500, TASK_FOREVER, &_tcbSendWSDataIfChangeBoxState, &userScheduler, false);

void myWSSender::_tcbSendWSDataIfChangeBoxState() {
  for (short _sBoxIndex = 1; _sBoxIndex < sBoxesCount; _sBoxIndex++) {
    // prepare a JSON document
    StaticJsonDocument<64> _doc;
    JsonObject _obj = _doc.to<JsonObject>();
    // holder for type of message
    short _messageType = -1;

    // populate the JSON object
    _obj["lb"] = _sBoxIndex;

    // if the box is an unsignaled new box
    if (ControlerBoxes[_sBoxIndex].isNewBoxHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. In fact, a new box [%i] has joined.\n", (_sBoxIndex + bControllerBoxPrefix));
      _messageType = 6;
      _obj["boxState"] = ControlerBoxes[_sBoxIndex].boxActiveState;
      _obj["ms"] = (int)ControlerBoxes[_sBoxIndex].bMasterBoxName - bControllerBoxPrefix;
      _obj["boxDefstate"] = ControlerBoxes[_sBoxIndex].sBoxDefaultState;
      // reset all the booleans to true
      _resetAllControlerBoxBoolsToTrue(_sBoxIndex);
    }

    // if the box has an unsignaled change of default state
    if (ControlerBoxes[_sBoxIndex].sBoxDefaultStateChangeHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. Default state of box [%i] has changed\n", (_sBoxIndex + bControllerBoxPrefix));
      _messageType = 10;
      _obj["boxDefstate"] = ControlerBoxes[_sBoxIndex].sBoxDefaultState;
      _obj["ms"] = (int)ControlerBoxes[_sBoxIndex].bMasterBoxName - bControllerBoxPrefix;
      ControlerBoxes[_sBoxIndex].sBoxDefaultStateChangeHasBeenSignaled = true;
    }

    // if the box has an unsignaled change of state
    if (ControlerBoxes[_sBoxIndex].boxActiveStateHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. State of box [%i] has changed\n", (_sBoxIndex + bControllerBoxPrefix));
      _messageType = 5;
      _obj["boxState"] = ControlerBoxes[_sBoxIndex].boxActiveState;
      _obj["ms"] = (int)ControlerBoxes[_sBoxIndex].bMasterBoxName - bControllerBoxPrefix;
      ControlerBoxes[_sBoxIndex].boxActiveStateHasBeenSignaled = true;
    }

    // if the box master has changed
    if (ControlerBoxes[_sBoxIndex].bMasterBoxNameChangeHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. A box [%i] has changed master\n", (_sBoxIndex + int(bControllerBoxPrefix)));
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. (int)(ControlerBoxes[%i].bMasterBoxName) == %i\n", _sBoxIndex, (int)(ControlerBoxes[_sBoxIndex].bMasterBoxName));
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. (ControlerBoxes[%i].bMasterBoxName) == %i\n", _sBoxIndex, (ControlerBoxes[_sBoxIndex].bMasterBoxName));
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. New master: %i\n", (int)(ControlerBoxes[_sBoxIndex].bMasterBoxName));
      _messageType = 8;
      _obj["ms"] = (int)(ControlerBoxes[_sBoxIndex].bMasterBoxName);
      _obj["st"] = 2; // "st" for status, 2 for executed
      ControlerBoxes[_sBoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
    }

    // if the box is an unsignaled deleted box
    if (ControlerBoxes[_sBoxIndex].boxDeletionHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. A box [%i] has disconnected\n", (_sBoxIndex + bControllerBoxPrefix));
      _messageType = 7;
      _resetAllControlerBoxBoolsToTrue(_sBoxIndex);
    }

    // in each of the above cases, send a message to the browser
    if (_messageType != -1) {
      if (MY_DEBUG) {Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. About to call _prepareWSData with message of type %i.\n", _messageType);}

      myWSSender _myWSSender;
      _myWSSender.prepareWSData(_messageType, _obj);
    }
  }
}






void myWSSender::prepareWSData(const short int _iMessageType, JsonObject& _subdoc) {
    Serial.printf("- myWebServerWS::_prepareWSData. Starting with message type [%i]\n", _iMessageType);
    Serial.printf("- myWebServerWS::_prepareWSData. Preparing JSON document\n");

    // if (MY_DEBUG) {
    //   Serial.printf("- myWebServerWS::_prepareWSData. Keys in _subdoc\n");
    //   for(JsonPair kvp : _subdoc) {
    //     Serial.println(kvp.key().c_str());
    //   }
    // }

    StaticJsonDocument<256> doc;
    doc["type"] = _iMessageType;

    if (MY_DEBUG) {
      bool _test = (_iMessageType == 1);
      Serial.printf("- myWebServerWS::_prepareWSData. (_iMessageType == 1) = %i\n", _test);
    }

    if (_iMessageType == 3) { // message type 3: change in station IP
      doc["message"] = (laserControllerMesh.getStationIP()).toString();
      if (MY_DEBUG) {
        const char* __stationIp = doc["stationIp"];
        Serial.print("- myWebServerWS::_prepareWSData. doc[\"stationIp\"] contains ");Serial.println(__stationIp);
      }
    }

    // messages 4 to 8:
    // (4): change boxState request being processed;
    // (5): change boxState executed
    // (6): a new box has joined the mesh
    // (7): a box has been deleted from the mesh
    // (8): master for a given is being processed, then has been changed
    else if (_iMessageType == 4 || _iMessageType == 5 || _iMessageType == 6 || _iMessageType == 7 || _iMessageType == 8 || _iMessageType == 9 || _iMessageType == 10) {
      doc["message"] = _subdoc;
    }

    // message 0 on handshake: activate the exchange of station IP
    else if (_iMessageType == 0) {
      Serial.printf("- myWebServerWS::_prepareWSData. Message type [%i] was none of 3 to 10\n", _iMessageType);
      if (MY_DEBUG) {
        Serial.printf("- myWebServerWS::_prepareWSData. Message type %i received. About to enable _tSendWSDataIfChangeStationIp\n", _iMessageType);
      }
      tSendWSDataIfChangeStationIp.enable();
      if (MY_DEBUG) {
        Serial.printf("- myWebServerWS::_prepareWSData.  _tSendWSDataIfChangeStationIp enabled.\n");
      }
    }

    // small confirmation messages (type 0 to 2)
    else {
      const char _messages_array[][30] = {"Hello WS Client","I got your WS text message","I got your WS binary message"};
      doc["message"] = _messages_array[_iMessageType];
      Serial.printf("- myWebServerWS::_prepareWSData. _messages_array[%i] = %s\n", _iMessageType, (char*)_messages_array);
    }

    // message ready. sending it to the send function
    Serial.println("- myWebServerWS::_prepareWSData. About to send JSON to sender function.");
    _sendWSData(doc);
    Serial.println("- myWebServerWS::_prepareWSData. Ending.");
}







void myWSSender::_sendWSData(JsonDocument& doc) {
    Serial.println("- myWebServerWS::_sendWSData. Starting.");

    size_t _len = measureJson(doc);
    Serial.printf("- myWebServerWS::_sendWSData. _len of JSON Document [%i]\n", _len);

    AsyncWebSocketMessageBuffer * _buffer = myWebServerWS::ws.makeBuffer(_len); //  creates a buffer (len + 1) for you.

    if (_buffer) {
        serializeJson(doc, (char *)_buffer->get(), _len + 1);

        // Before sending anything, check if you have any client
        if (size_t _client_count = myWebServerWS::ws.count()) {
          // if (MY_DEBUG) {
          //   Serial.print("- myWebServerWS::_sendWSData: AsyncWebSocket::count() = ");Serial.println(_client_count);
          //   Serial.print("- myWebServerWS::_sendWSData: myWebServerWS::ws_client_id = ");Serial.println(myWebServerWS::ws_client_id);
          //   Serial.print("- myWebServerWS::_sendWSData: (myWebServerWS::ws.client(myWebServerWS::ws_client_id) == nullptr) = ");Serial.println((myWebServerWS::ws.client(myWebServerWS::ws_client_id) != nullptr));
          //   Serial.print("- myWebServerWS::_sendWSData: (myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status() == WS_CONNECTED) = ");Serial.println(myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status() == WS_CONNECTED);
          //   Serial.print("- myWebServerWS::_sendWSData: (myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status()) = ");Serial.println(myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status());
          // }

          // check that you have the client_id
          // check that the client with the client_id you have is not pointing to nothing
          // check if the client is connected
          if (myWebServerWS::ws_client_id &&
            myWebServerWS::ws.client(myWebServerWS::ws_client_id) != nullptr &&
            myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status() == WS_CONNECTED) {
            if (MY_DEBUG) { Serial.printf("- myWebServerWS::_sendWSData. About to send a WS message message to client [%i].\n", myWebServerWS::ws_client_id); }
            myWebServerWS::ws.client(myWebServerWS::ws_client_id)->text(_buffer);
            if (MY_DEBUG) { Serial.println("- myWebServerWS::_sendWSData. Message sent"); }
          }  // end if (myWebServerWS::ws_client_id && ..
          else {
            if (MY_DEBUG) {
              Serial.print("- myWebServerWS::_sendWSData: AsyncWebSocket::count() = ");Serial.println(_client_count);
              Serial.print("- myWebServerWS::_sendWSData: myWebServerWS::ws_client_id = ");Serial.println(myWebServerWS::ws_client_id);
              Serial.print("- myWebServerWS::_sendWSData: (myWebServerWS::ws.client(myWebServerWS::ws_client_id) == nullptr) = ");Serial.println((myWebServerWS::ws.client(myWebServerWS::ws_client_id) != nullptr));
              Serial.print("- myWebServerWS::_sendWSData: (myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status() == WS_CONNECTED) = ");Serial.println(myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status() == WS_CONNECTED);
              Serial.print("- myWebServerWS::_sendWSData: (myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status()) = ");Serial.println(myWebServerWS::ws.client(myWebServerWS::ws_client_id)->status());
            }
            Serial.printf("- myWebServerWS::_sendWSData. Client not found. About to send a WS message message to all.\n");
            myWebServerWS::ws.textAll(_buffer);
            Serial.println("- myWebServerWS::_sendWSData. Message sent");
          } // end else
        } // end if client.count > 0
        else { // there is no client connected
          if (MY_DEBUG) {
            Serial.println("- myWebServerWS::_sendWSData: The message could not be sent.");
          }
        }
    }
    Serial.println("- myWebServerWS::_sendWSData. Ending.");
}





void myWSSender::_resetAllControlerBoxBoolsToTrue(const short _sBoxIndex) {
  ControlerBoxes[_sBoxIndex].isNewBoxHasBeenSignaled = true;
  ControlerBoxes[_sBoxIndex].boxActiveStateHasBeenSignaled = true;
  ControlerBoxes[_sBoxIndex].sBoxDefaultStateChangeHasBeenSignaled = true;
  ControlerBoxes[_sBoxIndex].boxDeletionHasBeenSignaled = true;
  ControlerBoxes[_sBoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
}
