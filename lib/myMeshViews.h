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

class myMeshViews
{
  public:
    static Task tSendBoxStateToNewBox;

    myMeshViews();
    // Views
    void statusMsg(uint32_t destNodeId=0);

    void changeBoxRequest(JsonObject& _obj);
    // void changeBoxRequest(const int8_t __i8RequestedChange, const char& _cChangeKey, const int8_t __i8BoxIndexInCB);

    // void changeBoxTargetState(const int8_t __i8BoxTargetState, const int8_t _i8BoxIndexInCB);
    // void changeMasterBox(const int8_t _i8MasterBox, const int8_t _i8BoxIndexInCB);
    // void changeBoxDefaultState(const int8_t _sBoxDefaultState, const int8_t _i8BoxIndexInCB);

    void changedBoxConfirmation(JsonObject& obj);
    // void changedMasterBoxConfirmation(const int8_t _i8MasterBox);
    // void changedBoxDefaultStateConfirmation(const int8_t _sBoxDefaultState);


    // static void manualSwitchMsg(const short targetOnOffState);
    // static void inclusionIRMsg(const short targetIrState);
    // static void blinkingIntervalMsg(const unsigned long targetBlinkingInterval);
    // static void pinPairingMsg(const short sTargetPairingType);
    // static void dataRequestMsg();
    // static void dataRequestResponse();

  private:
    static void _odtcbSendBoxStateToNewBox();

    // Helpers
    void _sendMsg(JsonObject& msg, uint32_t destNodeId=0);
    JsonObject _createJsonobject();
};

#endif
