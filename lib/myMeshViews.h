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
  |  |  |  |--boxState.cpp
  |  |  |  |  |--boxState.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
*/

#ifndef myMeshViews_h
#define myMeshViews_h

#include "Arduino.h"

class myMeshViews
{
  public:
    myMeshViews();
    // Views
    // static void manualSwitchMsg(const short targetOnOffState);
    // static void inclusionIRMsg(const short targetIrState);
    // static void blinkingIntervalMsg(const unsigned long targetBlinkingInterval);
    // static void changeMasterBoxMsg(const int newMasterNodeName, const char* newReaction);
    static void statusMsg(const short int myStatus);
    // static void pinPairingMsg(const short sTargetPairingType);
    // static void dataRequestMsg();
    // static void dataRequestResponse();

  private:

    // Helpers
    static void _broadcastMsg(JsonObject& msg);
    static JsonObject _createJsonobject();
};

#endif
