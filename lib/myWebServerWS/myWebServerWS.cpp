/*
  myWebServerWS.cpp - Library web socket related functions.
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
#include "myWebServerWS.h"


AsyncWebSocket myWebServerWS::ws("/ws"); // access at ws://[esp ip]/ws
uint32_t myWebServerWS::_ws_client_id = 0;
StaticJsonDocument<0> _empty_doc;
JsonObject myWebServerWS::_empty_obj = _empty_doc.to<JsonObject>();


myWebServerWS::myWebServerWS()
{
}





// Callback on websocket events
void myWebServerWS::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    //Handle WebSocket event
    if(type == WS_EVT_CONNECT){
        //client connected
        _ws_client_id = client->id();
        Serial.printf("ws[%s][%i] connect\n", server->url(), client->id());
        _prepareWSData(0); // 0 for messageType "hand shake"
        client->ping();
    } else if(type == WS_EVT_DISCONNECT){
        //client disconnected
        Serial.printf("ws[%s] disconnect %i\n", server->url(), client->id());
    } else if(type == WS_EVT_ERROR){
        //error was received from the other end
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else if(type == WS_EVT_PONG){
        //pong message was received (in response to a ping request maybe)
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");

    // receiving data from client
    } else if(type == WS_EVT_DATA){
        //data packet
        AwsFrameInfo * info = (AwsFrameInfo*)arg;

        if(info->final && info->index == 0 && info->len == len){
            //the whole message is in a single frame and we got all of it's data
            Serial.printf("ws[%s][%u] %s-message length[%llu]: \n", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
            if(info->opcode == WS_TEXT)
                _prepareWSData(1); // text message confirmation
            else
                client->binary("I got your WS binary message");
            if(info->opcode == WS_TEXT){
              // message is a text message
                data[len] = 0;
                // Serial.printf("%s\n", (char*)data);
                _decodeWSMessage(data); // uint8_t *data
            } else {
              // message is a binary message
                for(size_t i=0; i < info->len; i++){
                  Serial.printf("%02x ", data[i]);
                }
                Serial.printf("\n");
            }
        }

        else {
            //message is comprised of multiple frames or the frame is split into multiple packets
            if(info->index == 0){
                if(info->num == 0)
                    Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
            }

            Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
            if(info->message_opcode == WS_TEXT){
              // message is a text message
                data[len] = 0;
                Serial.printf("%s\n", (char*)data);
            } else {
              // message is a binary message
                for(size_t i=0; i < len; i++){
                  Serial.printf("%02x ", data[i]);
                }
                Serial.printf("\n");
            }

          if((info->index + len) == info->len){
            // received the final frame or packet
              Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
              if(info->final){
                  Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                  if(info->message_opcode == WS_TEXT)
                    _prepareWSData(1); // text message confirmation
                    // client->text("I got your WS text message");
                  else
                    client->binary("I got your WS binary message");
              }
          }
        }
    }
}






// Web Socket Receiver
void myWebServerWS::_decodeWSMessage(uint8_t *data) {
  Serial.println("myWebServerWS::_decodeWSMessage. Starting.");

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<256> doc;
  Serial.print("myWebServerWS::_decodeWSMessage(): jsonDocument created\n");

  // deserialize the message msg received from the mesh into the StaticJsonDocument doc
  DeserializationError err = deserializeJson(doc, data);
  Serial.print("myWebServerWS::_decodeWSMessage(): message msg deserialized into JsonDocument doc\n");
  Serial.print("myWebServerWS::_decodeWSMessage(): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");

  // read the type of message (0 for handshake, 3 for confirmation that change IP adress has been received, 4 for change boxState)
  const int _type = doc["type"]; // correspondings to root[action] in meshController
  Serial.printf("myWebServerWS::_decodeWSMessage(): The message _type is %i \n", _type);


  // choose the type of reaction depending on the message type
  // if type 0, handshake and comparing the number of boxRow vs the number of connected boxes
  if (_type == 0) {           // 0 for hand shake message
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i - converting doc[\"message\"] to JSON Object \n", _type);
    // Declare and define a JSONObject
    JsonObject _obj = doc["message"].as<JsonObject>();
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i - JSON Object _obj available containing the boxState of each boxRow in the DOM \n", _type);

    // if no boxRow in DOM and no boxes connected to the mesh, just return
    if (_obj.size() == 0 && ControlerBox::connectedBoxesCount == 1) {
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object _obj.size: %i. There are currently no boxRow in the DOM.\n", _type, (_obj.size() == 0));
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently no boxes connected to the mesh.\n", _type, (ControlerBox::connectedBoxesCount == 1));
      Serial.printf("myWebServerWS::_decodeWSMessage(): Ending on message type [%i], because there are no boxRow, nor connectedBoxes.\n", _type);
      return;
    }
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object _obj.size: %i. There are currently boxRow(s) in the DOM.\n", _type, _obj.size());
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently boxes connected to the mesh.\n", _type, ControlerBox::connectedBoxesCount);


    //else, there is a JSON Object of this type: {1:3,4:5,7:2}
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to iterate over the boxRows, looking for the existing boxRow and boxState in DOM\n", _type);
    for (JsonPair p : _obj) { // for each pair boxIndex:boxState in the DOM,
      Serial.printf("myWebServerWS::_decodeWSMessage(): Mew iteration.\n");
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to use the key of the current pair of the JSON object to check whether the ControlerBox corresponding to the boxRow in the DOM really exists in ControlerBoxes.\n", _type);
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, testing (int)p.key().c_str(): %i\n", _type, (int)p.key().c_str());

      // DELETED BOXES CHECKER
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, using this value to select a ControlerBoxes[]\n", _type);
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[(int)p.key().c_str()].nodeId == 0 is equal to %i\n", _type, (ControlerBoxes[(int)p.key().c_str()].nodeId == 0));
      // check if it still is connected; if not, ask for an update
      if (ControlerBoxes[(int)p.key().c_str()].nodeId == 0) {
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, the ControlerBox corresponding to the current boxRow has a nodeId of: %i. It is no longer connected to the mesh. Delete from the DOM.", _type, ControlerBoxes[(int)p.key().c_str()].nodeId);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to turn [boxDeletionHasBeenSignaled] of ControlerBoxes[%i] to false.\n", _type, (int)p.key().c_str());
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        ControlerBoxes[(int)p.key().c_str()].boxDeletionHasBeenSignaled = false;
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[%i].boxDeletionHasBeenSignaled turned to %i.\n", _type, (int)p.key().c_str(), ControlerBoxes[(int)p.key().c_str()].boxDeletionHasBeenSignaled);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _type);
      } // if


      // BOXSTATE CHECKER
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[(int)p.key().c_str()].boxActiveState = %i\n", _type, ControlerBoxes[(int)p.key().c_str()].boxActiveState);
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, (int)p.value().as<char*>() = %i\n.", _type, (int)p.value().as<char*>());
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, comparison between the two: %i\n.", _type, (ControlerBoxes[(int)p.key().c_str()].boxActiveState != (int)p.value().as<char*>()));
      // check if it has the correct boxState; if not, ask for an update
      if (ControlerBoxes[(int)p.key().c_str()].boxActiveState != (int)p.value().as<char*>()) {
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, the state of the ControlerBox corresponding to the current boxRow is different than its boxState in the DOM. Update it in the DOM.\n", _type);
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled = false;
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled = %i.\n", _type, ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _type);
      } // if
    } // for

    // MISSING BOXES CHECKER
    // look for the missing items and ask for an update
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to iterate over the ControlerBoxes to look if any is missing from the JSON object containing the boxRows from the DOM.\n", _type);
    for (short _i = 1; _i < sBoxesCount; _i++) {
      char _c[3];
      itoa(_i, _c, 10);
      const char* _keyInJson = _obj[_c];
      if ((ControlerBoxes[_i].nodeId != 0) &&_keyInJson == nullptr) {
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, there is a missing box in the DOM. Add it.\n", _type);
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        ControlerBoxes[_i].isNewBoxHasBeenSignaled = false;
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[_i].isNewBoxHasBeenSignaled = %i\n", _type, ControlerBoxes[_i].isNewBoxHasBeenSignaled);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _type);
      } // if
    } // for
    Serial.println("myWebServerWS::_decodeWSMessage. Ending on type 0 (received handshake message with list of boxRows in DOM).");
    return;
  }
  if (_type == 3) {           // 3 for confirmation that change IP adress has been received
    Serial.println("myWebServerWS::_decodeWSMessage. Ending on type 3 (received confirmation that new station IP has been received).");
    _tSendWSDataIfChangeStationIp.disable();
    return;
  }
  if (_type == 4) {           // 4 for change boxState
    // send a mesh request to the other box
    // convert the box name to a char array box name
    int __iNodeName = doc["lb"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): (from JSON) __iNodeName = %i \n", __iNodeName);
    char _cNodeName[4];
    itoa((__iNodeName + bControllerBoxPrefix), _cNodeName, 10);
    Serial.printf("myWebServerWS::_decodeWSMessage(): _cNodeName = %s \n", _cNodeName);
    // convert the box state to a char array
    const char* _boxState = doc["boxState"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): _boxState = %s \n", _boxState);
    myMeshViews __myMeshViews;
    // instantiate a mesh view
    Serial.printf("myWebServerWS::_decodeWSMessage(): about to call __myMeshViews.changeBoxTargetState().\n");
    __myMeshViews.changeBoxTargetState(_boxState, _cNodeName);

    // send a response telling the instruction is in course of being executed
    StaticJsonDocument<64> _sub_doc;
    JsonObject _sub_obj = _sub_doc.to<JsonObject>();
    Serial.printf("---------------------- %i -------------------\n", __iNodeName);
    _sub_obj["lb"] = __iNodeName;
    _sub_obj["boxState"] = _boxState;
    _prepareWSData(4, _sub_obj);
  }
  if (_type == 8) {             // 8 for change master
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i - starting \n", _type);
    // send a mesh request to the other box
    // convert the box name to a char array box name
    int __iNodeName = doc["lb"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): (from JSON) __iNodeName = %i \n", __iNodeName);
    char _cNodeName[4];
    itoa((__iNodeName + bControllerBoxPrefix), _cNodeName, 10);
    Serial.printf("myWebServerWS::_decodeWSMessage(): _cNodeName = %s \n", _cNodeName);
    // get the masterbox number
    int _iMasterBox = doc["masterbox"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): _boxState = %i \n", _iMasterBox);
    myMeshViews __myMeshViews;
    // instantiate a mesh view
    Serial.printf("myWebServerWS::_decodeWSMessage(): about to call __myMeshViews.changeMasterBox().\n");
    __myMeshViews.changeMasterBoxMsg(_iMasterBox, __iNodeName);

    // send a response telling the instruction is in course of being executed
    StaticJsonDocument<64> _sub_doc;
    JsonObject _sub_obj = _sub_doc.to<JsonObject>();
    Serial.printf("---------------------- %i -------------------\n", __iNodeName);
    _sub_obj["lb"] = __iNodeName;
    _sub_obj["ms"] = _iMasterBox;
    _sub_obj["st"] = 1; // "st" for status, 1 for sent to laser controller; waiting execution
    _prepareWSData(8, _sub_obj);
  }
  Serial.println("myWebServerWS::_decodeWSMessage. Ending.");
}




// Web Socket Message Senders
// Send WS message when I change Station IP
Task myWebServerWS::_tSendWSDataIfChangeStationIp(10000, TASK_FOREVER, &_tcbSendWSDataIfChangeStationIp, &userScheduler, false);

void myWebServerWS::_tcbSendWSDataIfChangeStationIp() {
  // if (!(laserControllerMesh.getStationIP() == ControlerBoxes[myIndexInCBArray].stationIP)) {
    Serial.println("myWebServerWS::_tcbSendWSDataIfChangeStationIp. interface station IP has changed.");
    // Serial.printf("myWebServerWS::_tcbSendWSDataIfChangeStationIp. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
    _prepareWSData(3); // 3 for message sent in case of change in station IP
    ControlerBoxes[myIndexInCBArray].updateThisBoxProperties();
  // }
}

// Send WS message upon (i) boxState changes, (ii) appearance or (iii) disappearance of a new box
// This task runs for ever and checks whether the boxState of any of the Controller boxes connected to
// lasers has changed in the ControlerBox array of the Interface controller.
// If so, it send a WS message with the new information.
Task myWebServerWS::_tSendWSDataIfChangeBoxState(500, TASK_FOREVER, &_tcbSendWSDataIfChangeBoxState, &userScheduler, false);

void myWebServerWS::_tcbSendWSDataIfChangeBoxState() {
  for (short int _iBoxIndex = 1; _iBoxIndex < sBoxesCount; _iBoxIndex++) {
    // prepare a JSON document
    StaticJsonDocument<64> _doc;
    JsonObject _obj = _doc.to<JsonObject>();
    // holder for type of message
    short int _messageType = -1;

    // populate the JSON object
    _obj["lb"] = _iBoxIndex;

    // if the box has an unsignaled change of state
    if (ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. State of box [%i] has changed\n", (_iBoxIndex + bControllerBoxPrefix));
      _messageType = 5;
      _obj["boxState"] = ControlerBoxes[_iBoxIndex].boxActiveState;
      _obj["ms"] = (int)ControlerBoxes[_iBoxIndex].bMasterBoxName - bControllerBoxPrefix;
      ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled = true;
    }

    // if the box is an unsignaled new box
    if (ControlerBoxes[_iBoxIndex].isNewBoxHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. In fact, a new box [%i] has joined.\n", (_iBoxIndex + bControllerBoxPrefix));
      _messageType = 6;
      _obj["boxState"] = ControlerBoxes[_iBoxIndex].boxActiveState;
      _obj["ms"] = (int)ControlerBoxes[_iBoxIndex].bMasterBoxName - bControllerBoxPrefix;
      ControlerBoxes[_iBoxIndex].isNewBoxHasBeenSignaled = true;
    }

    if (ControlerBoxes[_iBoxIndex].boxDeletionHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. A box [%i] has disconnected\n", (_iBoxIndex + bControllerBoxPrefix));
      _messageType = 7;
      ControlerBoxes[_iBoxIndex].boxDeletionHasBeenSignaled = true;
    }

    if (ControlerBoxes[_iBoxIndex].bMasterBoxNameChangeHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. A box [%i] has changed master\n", (_iBoxIndex + int(bControllerBoxPrefix)));
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. (int)(ControlerBoxes[%i].bMasterBoxName) == %i\n", _iBoxIndex, (int)(ControlerBoxes[_iBoxIndex].bMasterBoxName));
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. (ControlerBoxes[%i].bMasterBoxName) == %i\n", _iBoxIndex, (ControlerBoxes[_iBoxIndex].bMasterBoxName));
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. New master: %i\n", (int)(ControlerBoxes[_iBoxIndex].bMasterBoxName));
      _messageType = 8;
      _obj["ms"] = (int)(ControlerBoxes[_iBoxIndex].bMasterBoxName);
      _obj["st"] = 2; // "st" for status, 2 for executed
      ControlerBoxes[_iBoxIndex].bMasterBoxNameChangeHasBeenSignaled = true;
    }

    // in all cases
    if (_messageType != -1) {
      // pass it on, with the type of message (5, 6 or 7) we want to add
      if (MY_DEBUG) {Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. About to call _prepareWSData with message of type %i.\n", _messageType);}
      _prepareWSData(_messageType, _obj);
    }
  }
}

// void somefunction(void (*fptr)(void*, int, int), void* context) {
//     fptr(context, 17, 42);
// }





void myWebServerWS::_prepareWSData(const short int _iMessageType, JsonObject& _subdoc) {
  Serial.printf("- myWebServerWS::_prepareWSData. Starting with message type [%i]\n", _iMessageType);
  Serial.printf("- myWebServerWS::_prepareWSData. Preparing JSON document\n");
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
  else if (_iMessageType == 4 || _iMessageType == 5 || _iMessageType == 6 || _iMessageType == 7 || _iMessageType == 8) {
    doc["message"] = _subdoc;
  }

  // message 0 on handshake: activate the exchange of station IP
  else if (_iMessageType == 0) {
    Serial.printf("- myWebServerWS::_prepareWSData. Message type [%i] was none of 3 to 7\n", _iMessageType);
    if (MY_DEBUG) {
      Serial.printf("- myWebServerWS::_prepareWSData. Message type %i received. About to enable _tSendWSDataIfChangeStationIp\n", _iMessageType);
    }
    _tSendWSDataIfChangeStationIp.enable();
    if (MY_DEBUG) {
      Serial.printf("- myWebServerWS::_prepareWSData.  _tSendWSDataIfChangeStationIp enabled.\n");
    }
  }

  // small confirmation messages (type 1 to 3)
  else {
    const char _messages_array[][30] = {"Hello WS Client","I got your WS text message","I got your WS binary message"};
    doc["message"] = _messages_array[_iMessageType];
    Serial.printf("- myWebServerWS::_prepareWSData. _messages_array[%i] = %s\n", _iMessageType, (char*)_messages_array);
  }

  // message ready. sending it to the send function
  Serial.println("- myWebServerWS::_prepareWSData. About to send JSON to sender function.");
  sendWSData(doc);
  Serial.println("- myWebServerWS::_prepareWSData. Ending.");
}





void myWebServerWS::sendWSData(JsonDocument& doc) {
    Serial.println("- myWebServerWS::_sendWSData. Starting.");

    size_t _len = measureJson(doc);
    Serial.printf("- myWebServerWS::_sendWSData. _len of JSON Document [%i]\n", _len);

    AsyncWebSocketMessageBuffer * _buffer = ws.makeBuffer(_len); //  creates a buffer (len + 1) for you.

    if (_buffer) {
        serializeJson(doc, (char *)_buffer->get(), _len + 1);

        Serial.print("- myWebServerWS::_sendWSData: _ws_client_id = ");Serial.println(_ws_client_id);
        if (_ws_client_id) {
          Serial.printf("- myWebServerWS::_sendWSData. About to send a WS message message to [%i].\n", _ws_client_id);
          ws.client(_ws_client_id)->text(_buffer);
          Serial.println("- myWebServerWS::_sendWSData. Message sent");
        }
        // else {
        //   Serial.printf("- myWebServerWS::_sendWSData. About to send a WS message message to all.\n");
        //   ws.textAll(_buffer);
        //   Serial.println("- myWebServerWS::_sendWSData. Message sent");
        // }
    }
    Serial.println("- myWebServerWS::_sendWSData. Ending.");
}
