/*
  myMeshSenderMessage.h - Library to provide a common way for other libraries to 
  pass messages to myMeshViews, which will be in charge of sending them over the mesh.
  Created by Cedric Lor, Octobre 25, 2019.
*/

#ifndef myMeshSenderMessage_h
#define myMeshSenderMessage_h

#include "Arduino.h"
#include "ArduinoJson.h"

constexpr short    MESH_REQUEST_CAPACITY               = 20;
constexpr int      _capacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);

class myMeshSenderMessage
{

  public:
    myMeshSenderMessage();

    StaticJsonDocument<_capacity> _jDoc;
    JsonObject _joMsg = _jDoc.to<JsonObject>();

  private:
};

#endif
