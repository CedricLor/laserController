/*
  myWebServerViews.cpp - Library to serve the views of the laser controller web server.
  Created by Cedric Lor, January 2, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
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
  |  |  |  |--boxState.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--boxState.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
  |  |  |  |--//LaserPin.cpp

myWebServerViews contains the webPage being displayed by the webServer.

*/

#include "Arduino.h"
#include "myWebServerViews.h"

// const char* myWebServerViews::_slave_Reaction[4] = {"synchronous: on - on & off - off", "opposed: on - off & off - on", "always on: off - on & on - on", "always off: on - off & off - off"};
// const char* myWebServerViews::_pairing_Type[3] = {"unpaired", "twin", "cooperative"};

myWebServerViews::myWebServerViews(AsyncWebServerRequest *request)
{
  // strcpy(cBoxArray, "");
  // _loadBoxArray();

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

}


// Template processor
// Looks for placeholders in template
// If it meets a placeholder, replace it with a given value
String myWebServerViews::_processor(const String& var) {

  if(var == "UI8_NODE_NAME") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"UI8_NODE_NAME\")\n");
    char _cNodeName[4];         // the ASCII of the integer will be stored in this char array
    itoa(ControlerBoxes[gui16MyIndexInCBArray].ui16NodeName,_cNodeName,10); //(integer, yourBuffer, base)
    return F(_cNodeName);
  }
  if(var == "STATION_IP") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"STATION_IP\")\n");
    return F(ControlerBoxes[gui16MyIndexInCBArray].stationIP.toString().c_str());
  }
  if(var == "AP_IP") {
    // Serial.print("myWebServerBase::_processor(): if(var == \"AP_IP\")\n");
    return F(ControlerBoxes[gui16MyIndexInCBArray].APIP.toString().c_str());
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
    return F(ControlerBoxes[gui16MyIndexInCBArray].APIP.toString().c_str());
  }
  if (MY_DG_WEB) {
    Serial.print("myWebServerBase::_processor(): no condition met. Returning String()\n");
  }
  return String();
}
