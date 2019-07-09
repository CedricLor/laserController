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
// StaticJsonDocument<0> _empty_doc;
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
                myWSReceiver _myWSReceiver(data);
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
    myWSSender::tSendWSDataIfChangeStationIp.enable();
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
