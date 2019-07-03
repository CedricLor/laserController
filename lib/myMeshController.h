/*
  myMeshController.h - Library to handle mesh controller related functions.
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

#ifndef myMeshController_h
#define myMeshController_h

#include "Arduino.h"

class myMeshController
{
  public:
    myMeshController(uint32_t senderNodeId, JsonDocument& root);

  private:
    static const bool _B_SLAVE_ON_OFF_REACTIONS[4][2];

    static Task _tSendBoxStateToNewBox;
    static void _odtcbSendBoxStateToNewBox();

    char _action;

    // void _manualSwitch(JsonDocument& root);
    // void _changeInclusionIR(JsonDocument& root);
    // void _changeBlinkingInterval(JsonDocument& root);
    // void _changeMasterBox(JsonDocument& root);
    // void _slaveBoxSwitch(JsonDocument& root);
    // void _pinPairing(JsonDocument& root);
    // void _dataRequest();
};

#endif
