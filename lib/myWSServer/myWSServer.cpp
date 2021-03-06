/*
  myWSServer.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include "myWSServer.h"


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






void myWSServer::_handleEventTypeConnect(AsyncWebSocket * __server, AsyncWebSocketClient * __client) {
  //client connected
  if (thisControllerBox.globBaseVars.MY_DG_WS) {
    Serial.printf("- myWSServer::onEvent: type == WS_EVT_CONNECT; __server->url(): [%s], __client->id(): [%i] connect\n", __server->url(), __client->id());
    Serial.println("- myWSServer::onEvent: type == WS_EVT_CONNECT; About to call myWSResponder::prepareWSData");
  }
  _myWSResponder.tSendWSDataIfChangeStationIp.enable();
  __client->ping();
}







void myWSServer::_handleEventTypeDisconnected(AsyncWebSocket * __server, AsyncWebSocketClient * __client) {
  //client disconnected
  if (thisControllerBox.globBaseVars.MY_DG_WS) {
    Serial.printf("- myWSServer::onEvent: ws[%s] disconnect %i\n", __server->url(), __client->id());
  }
}







void myWSServer::_handleEventTypeError(AsyncWebSocket * __server, AsyncWebSocketClient * __client, void * __arg, uint8_t *__data) {
  //error was received from the other end
  if (thisControllerBox.globBaseVars.MY_DG_WS) {
    Serial.printf("- myWSServer::onEvent: ws[%s][%u] error(%u): %s\n", __server->url(), __client->id(), *((uint16_t*)__arg), (char*)__data);
  }
}







void myWSServer::_handleEventTypePong(AsyncWebSocket * __server, AsyncWebSocketClient * __client, uint8_t *__data, size_t __len) {
  //pong message was received (in response to a ping request maybe)
  if (thisControllerBox.globBaseVars.MY_DG_WS) {
    Serial.printf("- myWSServer::onEvent: ws[%s][%u] pong[%u]: %s\n", __server->url(), __client->id(), __len, (__len)?(char*)__data:"");
  }
}







void myWSServer::_handleSingleFrameMessage(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsFrameInfo * __info, uint8_t *__data, size_t __len) {
  //the whole message is in a single frame and we got all of it's data
  if (thisControllerBox.globBaseVars.MY_DG_WS) {
    Serial.printf("- myWSServer::onEvent: ws[%s][%u] %s-message length[%llu]: \n", __server->url(), __client->id(), (__info->opcode == WS_TEXT)?"text":"binary", __info->len);
  }
  if(__info->opcode == WS_TEXT){
    // message is a text message
    __data[__len] = 0;
    // Serial.println("----------------------- TEXT ------------------");
    // Serial.printf("%s\n", (char*)__data);
    // Serial.println(__len);
    if (__len == 1) { // this is a ping
      __client->text((char*)__data);
      return;
    }
    myWSReceiver _myWSReceiver(__data, _asyncWebSocketInstance, _myWSResponder);
  } else {
    // message is a binary message
    if (thisControllerBox.globBaseVars.MY_DG_WS) {
        // Serial.println("----------------------- BINARY ------------------");
        // Serial.printf("- myWSServer::onEvent: this is a binary message (single frame)\n");
      for(size_t i=0; i < __info->len; i++){
        Serial.printf("%02x ", __data[i]);
      }
      Serial.printf("\n");
    }
  }
}







void myWSServer::_handleMultipleFrameMessage(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsFrameInfo * __info, uint8_t *__data, size_t __len) {
  //   //message is comprised of multiple frames or the frame is split into multiple packets
  // if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //   Serial.printf("- myWSServer::onEvent: this is a multiple frames message\n");
  //   if(__info->index == 0){
  //     if(__info->num == 0)
  //       Serial.printf("- myWSServer::onEvent: ws[%s][%u] %s-message start\n", __server->url(), __client->id(), (__info->message_opcode == WS_TEXT)?"text":"binary");
  //     Serial.printf("- myWSServer::onEvent: ws[%s][%u] frame[%u] start[%llu]\n", __server->url(), __client->id(), __info->num, __info->len);
  //   }
  //   Serial.printf("- myWSServer::onEvent: ws[%s][%u] frame[%u] %s[%llu - %llu]: ", __server->url(), __client->id(), __info->num, (__info->message_opcode == WS_TEXT)?"text":"binary", __info->index, __info->index + __len);
  // }
  
  // if(__info->message_opcode == WS_TEXT){
  //   // message is a text message
  //   __data[__len] = 0;
  //   if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //     Serial.printf("- myWSServer::onEvent: this is a multiple frames text message\n");
  //     Serial.printf("%s\n", (char*)__data);
  //   }
  // } else {
  //   // message is a binary message
  //   if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //     Serial.printf("- myWSServer::onEvent: this is a multiple frames text message\n");
  //     for(size_t i=0; i < __len; i++){
  //       Serial.printf("%02x ", __data[i]);
  //     }
  //     Serial.printf("\n");
  //   }
  // }

  // if((__info->index + __len) == __info->len){
  //   // received the final frame or packet
  //   if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //     Serial.printf("- myWSServer::onEvent: this is the final frames of a multiple frames message\n");
  //     Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", __server->url(), __client->id(), __info->num, __info->len);
  //   }
  //   if(__info->final){
  //     if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //       Serial.printf("- myWSServer::onEvent: this is the final frames of a multiple frames message\n");
  //       Serial.printf("ws[%s][%u] %s-message end\n", __server->url(), __client->id(), (__info->message_opcode == WS_TEXT)?"text":"binary");
  //     }
  //     if(__info->message_opcode == WS_TEXT) {
  //       if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //         Serial.printf("- myWSServer::onEvent: this is the final frames of a multiple frames text message\n");
  //         Serial.print("- myWSServer::onEvent: WS_EVT_DATA (2nd): about to call prepareWSData(1)\n");
  //       }
  //         myWSSender _myWSSender(_asyncWebSocketInstance);
  //         _myWSSender.prepareWSData(1, __client); // text message confirmation

  //         // __client->text("I got your WS text message");
  //     }
  //     else {
  //       if (thisControllerBox.globBaseVars.MY_DG_WS) {
  //         Serial.printf("- myWSServer::onEvent: this is the final frames of a multiple frames binary message\n");
  //       }
  //       __client->binary("I got your WS binary message");
  //     }
  //   }
  // }
}







void myWSServer::_handleEventTypeData(AsyncWebSocket * __server, AsyncWebSocketClient * __client, void * __arg, uint8_t *__data, size_t __len) {
  //data packet
  AwsFrameInfo * _info = (AwsFrameInfo*)__arg;

  if(_info->final && _info->index == 0 && _info->len == __len){
    _handleSingleFrameMessage(__server, __client, _info, __data, __len);
  }

  else {
    _handleMultipleFrameMessage(__server, __client, _info, __data, __len);
  }
}







/** myWSServer::onEvent(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsEventType type, void * __arg, uint8_t *__data, size_t __len)
 * 
 *  Callback on websocket events (i.e. on messages from the browser).
*/
void myWSServer::onEvent(AsyncWebSocket * __server, AsyncWebSocketClient * __client, AwsEventType __type, void * __arg, uint8_t *__data, size_t __len){
    //Handle WebSocket event
    if(__type == WS_EVT_CONNECT){
      _handleEventTypeConnect(__server, __client);
    } else if(__type == WS_EVT_DISCONNECT){
      _handleEventTypeDisconnected(__server, __client);
    } else if(__type == WS_EVT_ERROR){
      _handleEventTypeError(__server, __client, __arg, __data);
    } else if(__type == WS_EVT_PONG){
      _handleEventTypePong(__server, __client, __data, __len);
    // receiving __data from __client
    } else if(__type == WS_EVT_DATA){
      _handleEventTypeData(__server, __client, __arg, __data, __len);
    } // end if(type == WS_EVT_DATA)
}
