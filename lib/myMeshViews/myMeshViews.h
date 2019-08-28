/*
  myMeshViews.h - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.

  |--main.cpp
  |  |
  |  |--myMesh.cpp
  |  |  |--myMesh.h
  |  |  |
  |  |  |--myMeshControler.cpp (private - called only from my Mesh)
  |  |  |  |--myMeshControler.h
  |  |  |  |--MasterSlaveBox.cpp
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
  |  |  |  |--ControlerBox.cpp
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
*/

#ifndef myMeshViews_h
#define myMeshViews_h

#include "Arduino.h"
#include <ControlerBox.h>

constexpr short    MESH_REQUEST_CAPACITY               = 20;

class myMeshViews
{
  public:
    myMeshViews();
    // Views
    void statusMsg(uint32_t destNodeId=0);

  private:
    friend class myWSReceiver;
    friend class myMeshController;
    friend class myMesh;
    friend class pirController;

    void _changedBoxConfirmation(JsonObject& obj);
    void _changeBoxRequest(JsonObject& _obj, bool _bBroadcast=false);
    void _droppedNodeNotif(uint16_t _ui16droppedNodeIndexInCB);
    void _IRHighMsg();

    // Helpers
    void _addIps(JsonObject& _obj);
    void _sendMsg(JsonObject& msg, uint32_t destNodeId=0);
    JsonObject _createJsonobject();
};

#endif
