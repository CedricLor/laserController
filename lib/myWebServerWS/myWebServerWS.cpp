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
#include "./myWSSender/myWSSender.cpp"
#include "./myWSReceiver/myWSReceiver.cpp"


AsyncWebSocket myWebServerWS::ws("/"); // access at ws://[esp ip]/
uint32_t myWebServerWS::ws_client_id = 0;


myWebServerWS::myWebServerWS()
{
}





// Callback on websocket events
// void _handleEvent(AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void myWebServerWS::onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    //Handle WebSocket event
    if(type == WS_EVT_CONNECT){
        //client connected
        ws_client_id = client->id();
        if (MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: type == WS_EVT_CONNECT; server->url(): [%s], client->id(): [%i] connect\n", server->url(), client->id());
          Serial.println("- myWebServerWS::onEvent: type == WS_EVT_CONNECT; About to call myWSSender::prepareWSData");
        }
        myWSSender _myWSSender;
        _myWSSender.prepareWSData(0); // 0 for messageType "handshake"

        client->ping();
    } else if(type == WS_EVT_DISCONNECT){
        //client disconnected
        if (MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: ws[%s] disconnect %i\n", server->url(), client->id());
        }
    } else if(type == WS_EVT_ERROR){
        //error was received from the other end
        if (MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
        }
    } else if(type == WS_EVT_PONG){
        //pong message was received (in response to a ping request maybe)
        if (MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
        }
    // receiving data from client
    } else if(type == WS_EVT_DATA){
        //data packet
        AwsFrameInfo * info = (AwsFrameInfo*)arg;

        if(info->final && info->index == 0 && info->len == len){
            //the whole message is in a single frame and we got all of it's data
            if (MY_DG_WS) {
              Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] %s-message length[%llu]: \n", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
            }
            if(info->opcode == WS_TEXT) {

                // myWSSender _myWSSender;
                // Serial.print("- myWebServerWS::onEvent: WS_EVT_DATA: about to call prepareWSData(1)\n");
                // _myWSSender.prepareWSData(1); // text message confirmation

            }
            else {
              client->binary("I got your WS binary message");
            }
            if(info->opcode == WS_TEXT){
              // message is a text message
                data[len] = 0;
                // Serial.println("----------------------- TEXT ------------------");
                // Serial.printf("%s\n", (char*)data);
                // Serial.println(len);
                if (len == 1) { // this is a ping
                  client->text((char*)data);
                  return;
                }
                myWSReceiver _myWSReceiver(data);

            } else {
              // message is a binary message
              if (MY_DG_WS) {
                  // Serial.println("----------------------- BINARY ------------------");
                  // Serial.printf("- myWebServerWS::onEvent: this is a binary message (single frame)\n");
                for(size_t i=0; i < info->len; i++){
                  Serial.printf("%02x ", data[i]);
                }
                Serial.printf("\n");
              }
            }
        }

        else {
            //message is comprised of multiple frames or the frame is split into multiple packets
          if (MY_DG_WS) {
            Serial.printf("- myWebServerWS::onEvent: this is a multiple frames message\n");
            if(info->index == 0){
                if(info->num == 0)
                    Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
            }

                Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
          }
            if(info->message_opcode == WS_TEXT){
              // message is a text message
                data[len] = 0;
                if (MY_DG_WS) {
                  Serial.printf("- myWebServerWS::onEvent: this is a multiple frames text message\n");
                  Serial.printf("%s\n", (char*)data);
                }
            } else {
              // message is a binary message
              if (MY_DG_WS) {
                Serial.printf("- myWebServerWS::onEvent: this is a multiple frames text message\n");
                for(size_t i=0; i < len; i++){
                  Serial.printf("%02x ", data[i]);
                }
                Serial.printf("\n");
              }
            }

          if((info->index + len) == info->len){
            // received the final frame or packet
              if (MY_DG_WS) {
                Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames message\n");
                Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
              }
              if(info->final){
                  if (MY_DG_WS) {
                    Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames message\n");
                    Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                  }
                  if(info->message_opcode == WS_TEXT) {
                    if (MY_DG_WS) {
                      Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames text message\n");
                      Serial.print("- myWebServerWS::onEvent: WS_EVT_DATA (2nd): about to call prepareWSData(1)\n");
                    }
                      myWSSender _myWSSender;
                      _myWSSender.prepareWSData(1); // text message confirmation

                      // client->text("I got your WS text message");
                  }
                  else {
                    if (MY_DG_WS) {
                      Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames binary message\n");
                    }
                    client->binary("I got your WS binary message");
                  }
              }
          }
        }
    }
}
