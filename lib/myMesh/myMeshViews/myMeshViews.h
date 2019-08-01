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
#include <global.h>

class myMeshViews
{
  public:
    myMeshViews();
    void statusMsg(uint32_t destNodeId=0);

    void droppedNodeNotif(int16_t _droppedNodeName);

    void changeBoxRequest(JsonObject& _obj, bool _bBroadcast=false);

    void changedBoxConfirmation(JsonObject& obj);

  private:
    // Helpers
    void _sendMsg(JsonObject& msg, uint32_t destNodeId=0);
    JsonObject _createJsonobject();
};

#endif
