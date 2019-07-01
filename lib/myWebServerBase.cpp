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
AsyncEventSource myWebServerBase::_events("/events"); // event source (Server-Sent events)
uint32_t myWebServerBase::_ws_client_id = 0;


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

  // // Web Events (to post events to the browser)
  // _events.onConnect([](AsyncEventSourceClient *client){
  //   if(client->lastId()){
  //     Serial.printf("WE Client reconnected! Last message ID that it got is: %u\n", client->lastId());
  //   }
  //   // send event with message "hello!", id, current millis
  //   // and set reconnect delay to 1 second
  //   client->send("Hello client WE!",NULL,millis(),1000);
  // });
  //
  // // attach AsyncEventSource
  // _asyncServer.addHandler(&_events);

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
        _prepareDataWs(0); // 0 for messageType "hand shake"
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
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
            if(info->opcode == WS_TEXT){
              // message is a text message
                data[len] = 0;
                Serial.printf("%s\n", (char*)data);
            } else {
              // message is a binary message
                for(size_t i=0; i < info->len; i++){
                  Serial.printf("%02x ", data[i]);
                }
                Serial.printf("\n");
            }
            if(info->opcode == WS_TEXT)
                _prepareDataWs(1); // text message confirmation
                // client->text("I got your WS text message");
            else
                client->binary("I got your WS binary message");
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
                    _prepareDataWs(1); // text message confirmation
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



// Task myWebServerBase::tSendWEventData(10000, TASK_FOREVER, &_tcbSendWEventData, &userScheduler, true);
//
// void myWebServerBase::_tcbSendWEventData() {
//   if (!(laserControllerMesh.getStationIP() == ControlerBoxes[0].stationIP)) {
//     Serial.println("- myWebServerBase::_tcbSendWEventData. interface stationIp has changed.");
//     Serial.println("- myWebServerBase::_tcbSendWEventData. about to send new stationIp.");
//
//     // Serial.print("- myWebServerBase::_tcbSendWEventData. Number of connected clients: ");Serial.println(_events.count());
//     if (_events.count()) {
//       _events.send((laserControllerMesh.getStationIP()).toString().c_str(),"STATIONIP",millis());
//       Serial.println("- myWebServerBase::_tcbSendWEventData. new StationIp sent.");
//       Serial.println("- myWebServerBase::_tcbSendWEventData. about to update this box properties.");
//     } else {
//       Serial.println("- myWebServerBase::_tcbSendWEventData. No clients connected.");
//     }
//
//     ControlerBoxes[0].updateThisBoxProperties();
//   } // if (!(laserControllerMesh.getStationIP() == ControlerBoxes[0].stationIP))
// } // function


Task myWebServerBase::tSendWsData(10000, TASK_FOREVER, &_tcbSendWsData, &userScheduler, true);

void myWebServerBase::_tcbSendWsData() {
  if (!(laserControllerMesh.getStationIP() == ControlerBoxes[0].stationIP)) {
    Serial.println("- myWebServerBase::_tcbSendWsData. interface station IP has changed.");
    _prepareDataWs(3); // 3 for message sent in case of change in station IP
    ControlerBoxes[0].updateThisBoxProperties();
  } // if
}

void myWebServerBase::_prepareDataWs(const short int _iMessageType) {
  Serial.println("- myWebServerBase::_prepareDataWs. Starting.");
  StaticJsonDocument<256> doc;
  doc["type"] = _iMessageType;
  doc["client"] = _ws_client_id;

  if (_iMessageType == 3) { // message type 3: change in station IP
    doc["message"] = (laserControllerMesh.getStationIP()).toString();
    if (MY_DEBUG) {
      const char* __stationIp = doc["stationIp"];
      Serial.print("- myWebServerBase::_tcbSendWsData. doc[\"stationIp\"] contains ");Serial.println(__stationIp);      
    }
  } else {
    const char _messages_array[][30] = {"Hello WS Client","I got your WS text message","I got your WS binary message"};
    doc["message"] = _messages_array[_iMessageType];
  }
  Serial.println("- myWebServerBase::_prepareDataWs. About to send JSON to sender function.");
  sendDataWs(doc);
  Serial.println("- myWebServerBase::_prepareDataWs. Ending.");
}


void myWebServerBase::sendDataWs(JsonDocument& doc) {
    Serial.println("- myWebServerBase::_sendDataWs. Starting.");

    const char* __stationIp = doc["stationIp"];
    Serial.print("- myWebServerBase::_tcbSendWsData. doc[\"stationIp\"] contains ");Serial.println(__stationIp);

    size_t _len = measureJson(doc);
    Serial.printf("- myWebServerBase::_sendDataWs. _len of JSON Document [%i]\n", _len);

    AsyncWebSocketMessageBuffer * _buffer = _ws.makeBuffer(_len); //  creates a buffer (len + 1) for you.

    if (_buffer) {
        serializeJson(doc, (char *)_buffer->get(), _len + 1);

        Serial.print("- myWebServerBase::_sendDataWs: _ws_client_id = ");Serial.println(_ws_client_id);
        if (_ws_client_id) {
          AsyncWebSocketClient * _ws_client = _ws.client(_ws_client_id);
          Serial.printf("- myWebServerBase::_sendDataWs. About to send a WS message message to [%i].\n", _ws_client_id);
          _ws_client->text(_buffer);
          Serial.println("- myWebServerBase::_sendDataWs. Message sent");
        } else {
          Serial.printf("- myWebServerBase::_sendDataWs. About to send a WS message message to all.\n");
          _ws.textAll(_buffer);
          Serial.println("- myWebServerBase::_sendDataWs. Message sent");
        }
    }
    Serial.println("- myWebServerBase::_sendDataWs. Ending.");
}
