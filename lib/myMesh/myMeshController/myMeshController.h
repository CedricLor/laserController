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
    myMeshController(uint32_t _ui32SenderNodeId, JsonObject& _obj);

  private:
    static const bool _B_SLAVE_ON_OFF_REACTIONS[4][2];

    char _action;

    void _statusMessage(JsonObject& _obj, uint32_t _ui32SenderNodeId);

    void _changeBox(JsonObject& _obj, uint32_t _ui32SenderNodeId);

    void _changeBoxRequest(JsonObject& _obj, uint32_t _ui32SenderNodeId);

    void _changedBoxConfirmation(JsonObject& _obj, uint32_t _ui32SenderNodeId);

    // helpers to _statusMessage
    int8_t _getSenderBoxIndexNumber(JsonObject& _obj);

    // helpers to _changeBoxRequest
    void _updateMyValFromWeb(JsonObject& _obj);
    void _updateMyMasterBoxName(JsonObject& _obj, uint32_t _ui32SenderNodeId);
    void _updateMyDefaultState(JsonObject& _obj, uint32_t _ui32SenderNodeId);
    void _rebootEsp(JsonObject&_obj, uint32_t _ui32SenderNodeId);
    void _save(JsonObject& _obj, uint32_t _ui32SenderNodeId);
    void _changeBoxSendConfirmationMsg(JsonObject& _obj, uint32_t _ui32SenderNodeId=0);

    // helpers to _changedBoxConfirmation (on the interface and the other boxes)
    void _updateSenderMasterBox(int8_t _i8BoxIndex, JsonObject& _obj);
    void _updateSenderDefaultState(int8_t _i8BoxIndex, JsonObject& _obj);

    // void _manualSwitch(JsonDocument& root);
    // void _changeInclusionIR(JsonDocument& root);
    // void _changeBlinkingInterval(JsonDocument& root);
    // void _changeMasterBox(JsonDocument& root);
    // void _slaveBoxSwitch(JsonDocument& root);
    // void _pinPairing(JsonDocument& root);
    // void _dataRequest();
};

#endif
