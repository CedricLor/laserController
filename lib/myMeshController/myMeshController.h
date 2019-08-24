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
#include <mySavedPrefs.h>
#include <myMeshViews.h>
#include <./myWSSender/myWSSender.h>

class myMeshController
{
  public:
    myMeshController(uint32_t _ui32SenderNodeId, String &_msg);

  private:
    static constexpr int _capacity = JSON_OBJECT_SIZE(30);
    StaticJsonDocument<_capacity> _nsdoc;
    JsonObject _nsobj;
    uint32_t _ui32SenderNodeId;

    void _main();

    void _statusMessage();

    void _changeBox();

    void _changeBoxRequest();

    void _changedBoxConfirmation();

    // helpers to _changeBoxRequest
    void _updateMyValFromWeb();
    void _updateMyMasterBoxName();
    void _updateMyDefaultState();
    
    void _rebootEsp();

    void _save();
    void _specificSave();
    void _savegi8RequestedOTAReboots();

    // static Task _tChangeBoxSendConfirmationMsg;
    void _changeBoxSendConfirmationMsg();


    // void _manualSwitch(JsonDocument& root);
    // void _changeInclusionIR(JsonDocument& root);
    // void _changeBlinkingInterval(JsonDocument& root);
    // void _changeMasterBox(JsonDocument& root);
    // void _slaveBoxSwitch(JsonDocument& root);
    // void _pinPairing(JsonDocument& root);
    // void _dataRequest();
};

#endif
