/*
  myWebServerBase.cpp - Library to handle web server controller related functions.
  Created by Cedric Lor, January 19, 2019.

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

*/

#include "Arduino.h"
#include "myWebServerBase.h"


AsyncWebServer myWebServerBase::_asyncServer(80);
AsyncWebSocket myWebServerBase::_ws("/ws"); // access at ws://[esp ip]/ws
uint32_t myWebServerBase::_ws_client_id = 0;
StaticJsonDocument<0> _empty_doc;
JsonObject myWebServerBase::_empty_obj = _empty_doc.to<JsonObject>();


myWebServerBase::myWebServerBase()
{
}


void myWebServerBase::_listAllCollectedHeaders(AsyncWebServerRequest *request) {
  int __headers = request->headers();
  for(int __i=0;__i<__headers;__i++){
    AsyncWebHeader* __h = request->getHeader(__i);
    Serial.printf("HEADER[%s]: %s\n", __h->name().c_str(), __h->value().c_str());
  }
}

void myWebServerBase::_listAllCollectedParams(AsyncWebServerRequest *request) {
  int __params = request->params();
  for(int __i=0;__i<__params;__i++){
    AsyncWebParameter* __param = request->getParam(__i);
    if(__param->isPost()){
      Serial.printf("POST[%s]: %s\n", __param->name().c_str(), __param->value().c_str());
    } else {
      Serial.printf("GET[%s]: %s\n", __param->name().c_str(), __param->value().c_str());
    }
  }
}



void myWebServerBase::startAsyncServer() {
  // starts the AsyncServer and sets a couple of callbacks, which will respond to requests
  // same as myMesh::meshSetup(), but respectively for the mesh server and the web server.

  // attach AsyncWebSocket
  _ws.onEvent(_onEvent);
  _asyncServer.addHandler(&_ws);

  // respond to GET requests
  _asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.print("myWebServerBase::startAsyncServer():: In handler of \"/\" request -------\n");

      if (MY_DEBUG) {
        // List all collected headers
        _listAllCollectedHeaders(request);
        // List all parameters
        _listAllCollectedParams(request);
      }

      // Decode request and change behavior of this controller box
      myWebServerControler __myWebServerControler;
      __myWebServerControler.decodeRequest(request);   // Call to "child" class myWebServerControler

      // Send a response (i.e. display a web page)

     //  * Response Stream (You can print/write/printf to it, up to the contentLen bytes)
     //  * */
     // AsyncResponseStream::AsyncResponseStream(const String& contentType, size_t bufferSize){
     // AsyncResponseStream * AsyncWebServerRequest::beginResponseStream(const String& contentType, size_t bufferSize){
     //   return new AsyncResponseStream(contentType, bufferSize);
     // }

      // AsyncResponseStream *__response = request->beginResponseStream("text/html");  // define a response stream
      // __response->addHeader("Server","ESP Async Web Server");                       // append stuff to header
      // __response->printf(__myWebServerView.returnTheResponse().c_str());            // converts the arduino String in C string (array of chars)
      // request->send(__response);

      // AsyncWebServerResponse * AsyncWebServerRequest::beginResponse(FS &fs, const String& path, const String& contentType, bool download, AwsTemplateProcessor callback){
      //   if(fs.exists(path) || (!download && fs.exists(path+".gz")))
      //     return new AsyncFileResponse(fs, path, contentType, download, callback);
      //   return NULL;
      // }
      // AsyncWebServerResponse *_response = request->beginResponse(SPIFFS, "/index.htm", "text/html", false, _processor);
      // request->send(_response);

      // void AsyncWebServerRequest::send(FS &fs, const String& path, const String& contentType, bool download, AwsTemplateProcessor callback){
      //   if(fs.exists(path) || (!download && fs.exists(path+".gz"))){
      //     send(beginResponse(fs, path, contentType, download, callback));
      //   } else send(404);
      // }
      request->send(SPIFFS, "/index.htm", String(), false, _processor);
  }); // end _asyncServer.on("/", ...)

  _asyncServer.on("/index.css", HTTP_GET, [](AsyncWebServerRequest *request){
     request->send(SPIFFS, "/index.css", "text/css");
   });

   _asyncServer.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.js", "text/javascript");
    });

  _asyncServer.onNotFound(&_onRequest);
  _asyncServer.onRequestBody(&_onBody);

  _asyncServer.begin();
}





// Template processor
// Looks for placeholders in template
// If it meets a placeholder, replace it with a given value
String myWebServerBase::_processor(const String& var) {

  if(var == "B_NODE_NAME") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"B_NODE_NAME\")\n");
    char _cNodeName[4];         // the ASCII of the integer will be stored in this char array
    itoa(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].bNodeName,_cNodeName,10); //(integer, yourBuffer, base)
    return F(_cNodeName);
  }
  if(var == "STATION_IP") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"STATION_IP\")\n");
    return F(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].stationIP.toString().c_str());
  }
  if(var == "AP_IP") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"AP_IP\")\n");
    return F(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].APIP.toString().c_str());
  }
  // if(var == "BOX_SETTER") {
  //   // Serial.print("myWebServerBase::_processor(): if(var == \"BOX_SETTER\")\n");
  //   myWebServerViews __myWebServerView;  // Call to "child" class myWebServerViews
  //   // Serial.print("myWebServerBase::_processor(): if(var == \"BOX_SETTER\"): Just after instantiating __myWebServerView\n");
  //   // Serial.print("myWebServerBase::_processor(): if(var == \"BOX_SETTER\"): __myWebServerView.cBoxArray  =");Serial.println(__myWebServerView.cBoxArray);
  //   // Serial.print("myWebServerBase::_processor(): if(var == \"BOX_SETTER\"):  Just after __myWebServerView.loadBoxArray()\n");
  //   return F(__myWebServerView.cBoxArray);
  // }
  if(var == "NETWORK_SETTER") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"NETWORK_SETTER\")\n");
    return F(ControlerBoxes[MY_INDEX_IN_CB_ARRAY].APIP.toString().c_str());
  }
  Serial.print("myWebServerBase::_processor(): no condition met. Returning String()\n");
  return String();
}





// Callback on websocket events
void myWebServerBase::_onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
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




// Call back on unknown request
void myWebServerBase::_onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

// Call back on body request
void myWebServerBase::_onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
  if(!index){
    Serial.printf("myWebServerBase::_onBody. BodyStart: %u B\n", total);
  }
  for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
  }
  if(index + len == total){
    Serial.printf("BodyEnd: %u B\n", total);
  }
}




// Web Socket Receiver
void myWebServerBase::_decodeWSMessage(uint8_t *data) {
  Serial.println("- myWebServerBase::_decodeWSMessage. Starting.");

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<256> doc;
  Serial.print("myWebServerBase::_decodeWSMessage(): jsonDocument created\n");

  // deserialize the message msg received from the mesh into the StaticJsonDocument doc
  DeserializationError err = deserializeJson(doc, data);
  Serial.print("myWebServerBase::_decodeWSMessage(): message msg deserialized into JsonDocument doc\n");
  Serial.print("myWebServerBase::_decodeWSMessage(): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");

  const int _type = doc["type"]; // correspondings to root[action] in meshController
  Serial.printf("myWebServerBase::_decodeWSMessage(): (from JSON) message _type = %i \n", _type);

  if (_type == 0) {           // 0 for hand shake message
    Serial.println("myWebServerBase::_decodeWSMessage. Ending on type 0 (received handshake message).");
    // Declare and define a JSONObject
    JsonObject _obj = doc["message"].as<JsonObject>();
    // if no boxRow in DOM and no boxes connected to the mesh, just return
    if (_obj.size() == 0 && ControlerBox::connectedBoxesCount == 1) {
      return;
    }
    //else, there is a JSON Object of this type: {1:3,4:5,7:2}
    for (JsonPair p : _obj) { // for each pair boxIndex:boxState in the DOM,
      if (ControlerBoxes[(int)p.key().c_str()].nodeId == 0) {
        ControlerBoxes[(int)p.key().c_str()].boxDeletionHasBeenSignaled = false;
      } // if
      if (ControlerBoxes[(int)p.key().c_str()].boxActiveState != (int)p.value().as<char*>()) {
        ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled = false;
      } // if
    } // for
    for (short _i = 1; _i < BOXES_COUNT; _i++) {
      char _c[3];
      itoa(_i, _c, 10);
      const char* _keyInJson = _obj[_c];
      if ((ControlerBoxes[_i].nodeId != 0) &&_keyInJson == nullptr) {
        ControlerBoxes[_i].isNewBoxHasBeenSignaled = false;
      } // if
    } // for
    return;
  }
  if (_type == 3) {           // 3 for confirmation that change IP adress has been received
    Serial.println("myWebServerBase::_decodeWSMessage. Ending on type 3 (received confirmation that new station IP has been received).");
    _tSendWSDataIfChangeStationIp.disable();
    return;
  }
  if (_type == 4) {           // 4 for change boxState
    // send a mesh request to the other boxes
    // convert the box name to a char array box name
    int __iNodeName = doc["lb"];
    Serial.printf("myWebServerBase::_decodeWSMessage(): (from JSON) __iNodeName = %i \n", __iNodeName);
    char _cNodeName[4];
    itoa((__iNodeName + B_CONTROLLER_BOX_PREFIX), _cNodeName, 10);
    Serial.printf("myWebServerBase::_decodeWSMessage(): _cNodeName = %s \n", _cNodeName);
    // convert the box state to a char array box state
    const char* _boxState = doc["boxState"];
    Serial.printf("myWebServerBase::_decodeWSMessage(): _boxState = %s \n", _boxState);
    myMeshViews __myMeshViews;
    // instantiate a mesh view
    Serial.printf("myWebServerBase::_decodeWSMessage(): about to call __myMeshViews.changeBoxTargetState().\n");
    __myMeshViews.changeBoxTargetState(_boxState, _cNodeName);

    // send a response telling the instruction is in course of being executed
    StaticJsonDocument<64> _sub_doc;
    JsonObject _sub_obj = _sub_doc.to<JsonObject>();
    Serial.printf("---------------------- %i -------------------\n", __iNodeName);
    _sub_obj["lb"] = __iNodeName;
    _sub_obj["boxState"] = _boxState;
    _prepareWSData(4, _sub_obj);
  }
  Serial.println("myWebServerBase::_decodeWSMessage. Ending.");
}




// Web Socket Message Senders
// Send WS message when I change Station IP
Task myWebServerBase::_tSendWSDataIfChangeStationIp(10000, TASK_FOREVER, &_tcbSendWSDataIfChangeStationIp, &userScheduler, false);

void myWebServerBase::_tcbSendWSDataIfChangeStationIp() {
  // if (!(laserControllerMesh.getStationIP() == ControlerBoxes[0].stationIP)) {
    Serial.println("myWebServerBase::_tcbSendWSDataIfChangeStationIp. interface station IP has changed.");
    // Serial.printf("myWebServerBase::_tcbSendWSDataIfChangeStationIp. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
    _prepareWSData(3); // 3 for message sent in case of change in station IP
    ControlerBoxes[0].updateThisBoxProperties();
  // }
}

// Send WS message upon (i) boxState changes, (ii) appearance or (iii) disappearance of a new box
// This task runs for ever and checks whether the boxState of any of the Controller boxes connected to
// lasers has changed in the ControlerBox array of the Interface controller.
// If so, it send a WS message with the new information.
Task myWebServerBase::_tSendWSDataIfChangeBoxState(500, TASK_FOREVER, &_tcbSendWSDataIfChangeBoxState, &userScheduler, false);

void myWebServerBase::_tcbSendWSDataIfChangeBoxState() {
  for (short int _boxIndex = 1; _boxIndex < BOXES_COUNT; _boxIndex++) {
    // prepare a JSON document
    StaticJsonDocument<64> _doc;
    JsonObject _obj = _doc.to<JsonObject>();
    // holder for type of message
    short int _messageType = -1;

    // populate the JSON object
    _obj["lb"] = _boxIndex;

    // if the box has an unsignaled change of state
    if (ControlerBoxes[_boxIndex].boxActiveStateHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. State of box [%i] has changed\n", (_boxIndex + B_CONTROLLER_BOX_PREFIX));
      _messageType = 5;
      _obj["boxState"] = ControlerBoxes[_boxIndex].boxActiveState;
      ControlerBoxes[_boxIndex].boxActiveStateHasBeenSignaled = true;
    }

    // if the box is an unsignaled new box
    if (ControlerBoxes[_boxIndex].isNewBoxHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. In fact, a new box [%i] has joined\n", (_boxIndex + B_CONTROLLER_BOX_PREFIX));
      _messageType = 6;
      _obj["boxState"] = ControlerBoxes[_boxIndex].boxActiveState;
      ControlerBoxes[_boxIndex].isNewBoxHasBeenSignaled = true;
    }

    if (ControlerBoxes[_boxIndex].boxDeletionHasBeenSignaled == false) {
      Serial.printf("_tcbSendWSDataIfChangeBoxState::_tcbSendWSDataIfChangeBoxState. A box [%i] has disconnected\n", (_boxIndex + B_CONTROLLER_BOX_PREFIX));
      _messageType = 7;
      ControlerBoxes[_boxIndex].boxDeletionHasBeenSignaled = true;
    }

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





void myWebServerBase::_prepareWSData(const short int _iMessageType, JsonObject& _subdoc) {
  Serial.printf("- myWebServerBase::_prepareWSData. Starting with message type [%i]\n", _iMessageType);
  Serial.printf("- myWebServerBase::_prepareWSData. Preparing JSON document\n");
  StaticJsonDocument<256> doc;
  doc["type"] = _iMessageType;

  if (MY_DEBUG) {
    bool _test = (_iMessageType == 1);
    Serial.printf("- myWebServerBase::_prepareWSData. (_iMessageType == 1) = %i\n", _test);
  }

  if (_iMessageType == 3) { // message type 3: change in station IP
    doc["message"] = (laserControllerMesh.getStationIP()).toString();
    if (MY_DEBUG) {
      const char* __stationIp = doc["stationIp"];
      Serial.print("- myWebServerBase::_prepareWSData. doc[\"stationIp\"] contains ");Serial.println(__stationIp);
    }
  } else if (_iMessageType == 4 || _iMessageType == 5 || _iMessageType == 6 || _iMessageType == 7) { // type 4: change state request sent to destination box
    doc["message"] = _subdoc;                      // type 5: state of a box has effectively changed // type 6: a new box has joined // tye 7: a box has disconnected
  } else {
    Serial.printf("- myWebServerBase::_prepareWSData. Message type [%i] was none of 3 to 7\n", _iMessageType);
    if (_iMessageType == 0) {
      if (MY_DEBUG) {
        Serial.printf("- myWebServerBase::_prepareWSData. Message type %i received. About to enable _tSendWSDataIfChangeStationIp\n", _iMessageType);
      }
      _tSendWSDataIfChangeStationIp.enable();
      if (MY_DEBUG) {
        Serial.printf("- myWebServerBase::_prepareWSData.  _tSendWSDataIfChangeStationIp enabled.\n");
      }
    }
    const char _messages_array[][30] = {"Hello WS Client","I got your WS text message","I got your WS binary message"};
    doc["message"] = _messages_array[_iMessageType];
    Serial.printf("- myWebServerBase::_prepareWSData. _messages_array[%i] = %s\n", _iMessageType, (char*)_messages_array);
  }
  Serial.println("- myWebServerBase::_prepareWSData. About to send JSON to sender function.");
  sendWSData(doc);
  Serial.println("- myWebServerBase::_prepareWSData. Ending.");
}





void myWebServerBase::sendWSData(JsonDocument& doc) {
    Serial.println("- myWebServerBase::_sendWSData. Starting.");

    size_t _len = measureJson(doc);
    Serial.printf("- myWebServerBase::_sendWSData. _len of JSON Document [%i]\n", _len);

    AsyncWebSocketMessageBuffer * _buffer = _ws.makeBuffer(_len); //  creates a buffer (len + 1) for you.

    if (_buffer) {
        serializeJson(doc, (char *)_buffer->get(), _len + 1);

        Serial.print("- myWebServerBase::_sendWSData: _ws_client_id = ");Serial.println(_ws_client_id);
        if (_ws_client_id) {
          Serial.printf("- myWebServerBase::_sendWSData. About to send a WS message message to [%i].\n", _ws_client_id);
          _ws.client(_ws_client_id)->text(_buffer);
          Serial.println("- myWebServerBase::_sendWSData. Message sent");
        }
        // else {
        //   Serial.printf("- myWebServerBase::_sendWSData. About to send a WS message message to all.\n");
        //   _ws.textAll(_buffer);
        //   Serial.println("- myWebServerBase::_sendWSData. Message sent");
        // }
    }
    Serial.println("- myWebServerBase::_sendWSData. Ending.");
}
