/*
  myMeshViews.h - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.
*/

#ifndef myMeshViews_h
#define myMeshViews_h

#include "Arduino.h"
#include <controllerBoxesCollection.h>
#include <myMeshSenderMessage.h>


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
    void _droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB);
    void _IRHighMsg(uint32_t _ui32IRHighTime);

    // Helpers
    void _sendMsg(JsonObject& msg, uint32_t destNodeId=0);
};

#endif
