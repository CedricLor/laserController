/*
  myWebServerWS.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include "myWebServerWS.h"


myWSServer::myWSServer():
  _asyncWebSocketInstance("/"), // access at ws://[esp ip]/
  _myWSResponder(_asyncWebSocketInstance)
{ 
  /** set _myWSServer.onEvent as the "onEvent callback" 
   *  of the _asyncWebSocketInstance*.
   *  (i.e. setting _myWSServer.onEvent() as the callback that will handle 
   *  websocket calls from the browsers) 
   * 
   *  * _asyncWebSocketInstance is a member of _myWSServer, the myWSServer instance.
   *  */
  _asyncWebSocketInstance.onEvent([&](AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsEventType __type, void * __arg, uint8_t *__data, size_t __len){
    onEvent(__server, __client, __type, __arg, __data, __len);
  });
}






AsyncWebSocket & myWSServer::getAsyncWebSocketInstance() {
  return _asyncWebSocketInstance;
}






myWSResponder & myWSServer::getMyWSResponder() {
  return _myWSResponder;
}






/** myWSServer::onEvent(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsEventType type, void * __arg, uint8_t *__data, size_t __len)
 * 
 *  Callback on websocket events (i.e. on messages from the browser).
*/
void myWSServer::onEvent(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsEventType type, void * __arg, uint8_t *__data, size_t __len){
    //Handle WebSocket event
    if(type == WS_EVT_CONNECT){
        //client connected
        if (globalBaseVariables.MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: type == WS_EVT_CONNECT; __server->url(): [%s], __client->id(): [%i] connect\n", __server->url(), __client->id());
          Serial.println("- myWebServerWS::onEvent: type == WS_EVT_CONNECT; About to call myWSResponder::prepareWSData");
        }
        myWSSender _myWSSender(_asyncWebSocketInstance, _myWSResponder.getMyWSSenderTasks().tSendWSDataIfChangeStationIp);
        _myWSSender.prepareWSData(0, __client); // 0 for messageType "handshake"

        __client->ping();
    } else if(type == WS_EVT_DISCONNECT){
        //client disconnected
        if (globalBaseVariables.MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: ws[%s] disconnect %i\n", __server->url(), __client->id());
        }
    } else if(type == WS_EVT_ERROR){
        //error was received from the other end
        if (globalBaseVariables.MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] error(%u): %s\n", __server->url(), __client->id(), *((uint16_t*)__arg), (char*)__data);
        }
    } else if(type == WS_EVT_PONG){
        //pong message was received (in response to a ping request maybe)
        if (globalBaseVariables.MY_DG_WS) {
          Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] pong[%u]: %s\n", __server->url(), __client->id(), __len, (__len)?(char*)__data:"");
        }
    // receiving __data from __client
    } else if(type == WS_EVT_DATA){
        //data packet
        AwsFrameInfo * info = (AwsFrameInfo*)__arg;

        if(info->final && info->index == 0 && info->len == __len){
            //the whole message is in a single frame and we got all of it's data
            if (globalBaseVariables.MY_DG_WS) {
              Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] %s-message length[%llu]: \n", __server->url(), __client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
            }
            if(info->opcode == WS_TEXT) {

                // myWSSender _myWSSender(_webSocketServer, _myWSResponder._myWSSenderTasks.tSendWSDataIfChangeStationIp);
                // Serial.print("- myWebServerWS::onEvent: WS_EVT_DATA: about to call prepareWSData(1)\n");
                // _myWSSender.prepareWSData(1, __client); // text message confirmation

            }
            else {
              __client->binary("I got your WS binary message");
            }
            if(info->opcode == WS_TEXT){
              // message is a text message
                __data[__len] = 0;
                // Serial.println("----------------------- TEXT ------------------");
                // Serial.printf("%s\n", (char*)__data);
                // Serial.println(__len);
                if (__len == 1) { // this is a ping
                  __client->text((char*)__data);
                  return;
                }
                myWSReceiver _myWSReceiver(__data, _asyncWebSocketInstance, _myWSResponder.getMyWSSenderTasks());

            } else {
              // message is a binary message
              if (globalBaseVariables.MY_DG_WS) {
                  // Serial.println("----------------------- BINARY ------------------");
                  // Serial.printf("- myWebServerWS::onEvent: this is a binary message (single frame)\n");
                for(size_t i=0; i < info->len; i++){
                  Serial.printf("%02x ", __data[i]);
                }
                Serial.printf("\n");
              }
            }
        }

        else {
            //message is comprised of multiple frames or the frame is split into multiple packets
          if (globalBaseVariables.MY_DG_WS) {
            Serial.printf("- myWebServerWS::onEvent: this is a multiple frames message\n");
            if(info->index == 0){
                if(info->num == 0)
                    Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] %s-message start\n", __server->url(), __client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] frame[%u] start[%llu]\n", __server->url(), __client->id(), info->num, info->len);
            }

                Serial.printf("- myWebServerWS::onEvent: ws[%s][%u] frame[%u] %s[%llu - %llu]: ", __server->url(), __client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + __len);
          }
            if(info->message_opcode == WS_TEXT){
              // message is a text message
                __data[__len] = 0;
                if (globalBaseVariables.MY_DG_WS) {
                  Serial.printf("- myWebServerWS::onEvent: this is a multiple frames text message\n");
                  Serial.printf("%s\n", (char*)__data);
                }
            } else {
              // message is a binary message
              if (globalBaseVariables.MY_DG_WS) {
                Serial.printf("- myWebServerWS::onEvent: this is a multiple frames text message\n");
                for(size_t i=0; i < __len; i++){
                  Serial.printf("%02x ", __data[i]);
                }
                Serial.printf("\n");
              }
            }

          if((info->index + __len) == info->len){
            // received the final frame or packet
              if (globalBaseVariables.MY_DG_WS) {
                Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames message\n");
                Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", __server->url(), __client->id(), info->num, info->len);
              }
              if(info->final){
                  if (globalBaseVariables.MY_DG_WS) {
                    Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames message\n");
                    Serial.printf("ws[%s][%u] %s-message end\n", __server->url(), __client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                  }
                  if(info->message_opcode == WS_TEXT) {
                    if (globalBaseVariables.MY_DG_WS) {
                      Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames text message\n");
                      Serial.print("- myWebServerWS::onEvent: WS_EVT_DATA (2nd): about to call prepareWSData(1)\n");
                    }
                      myWSSender _myWSSender(_asyncWebSocketInstance, _myWSResponder.getMyWSSenderTasks().tSendWSDataIfChangeStationIp);
                      _myWSSender.prepareWSData(1, __client); // text message confirmation

                      // __client->text("I got your WS text message");
                  }
                  else {
                    if (globalBaseVariables.MY_DG_WS) {
                      Serial.printf("- myWebServerWS::onEvent: this is the final frames of a multiple frames binary message\n");
                    }
                    __client->binary("I got your WS binary message");
                  }
              }
          }
        }
    }
}
