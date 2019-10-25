/*
  myMeshSenderMessage.h - Library to provide a common way for other libraries to 
  pass messages to myMeshViews, which will be in charge of sending them over the mesh.
  Created by Cedric Lor, Octobre 25, 2019.
*/

#ifndef myMeshSenderMessage_h
#define myMeshSenderMessage_h

#include "Arduino.h"
#include <controllerBoxesCollection.h>

constexpr short    MESH_REQUEST_CAPACITY               = 20;

class myMeshViews
{
  friend class pirController;
  friend class myWSReceiver;
  friend class myMeshController;
  friend class myMesh;

  public:
    myMeshViews(controllerBoxesCollection & __ctlBxColl);
    // Views
    void statusMsg(uint32_t destNodeId=0);

  private:
    controllerBoxesCollection & _ctlBxColl;

    void _changedBoxConfirmation(JsonObject& obj);
    void _changeBoxRequest(JsonObject& _obj, bool _bBroadcast=false);
    void _droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB);
    void _IRHighMsg();

    // Helpers
    void _sendMsg(JsonObject& msg, uint32_t destNodeId=0);
};

#endif
