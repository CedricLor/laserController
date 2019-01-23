/*
  myMeshViews.h - Library to handle functions related to the JSON messages sent by the box over the mesh.
  Created by Cedric Lor, January 22, 2019.
*/

#ifndef myMeshViews_h
#define myMeshViews_h

#include "Arduino.h"
#include "ControlerBox.h"

class myMeshViews
{
  public:
    myMeshViews(const char* state);

  private:
    // Views
    void _manualSwitchMsg(const short targetOnOffState);
    void _inclusionIRMsg(const short targetIrState);
    void _blinkingIntervalMsg(const unsigned long targetBlinkingInterval);
    void _changeMasterBoxMsg(const int newMasterNodeId, const char* newReaction);
    void _statusMsg(const char* myStatus);
    void _pinPairingMsg(const char* myStatus);
    void _dataRequestMsg(const char* myStatus);
    void _dataRequestResponse(const char* myStatus);

    // Helpers
    void _broadcastMsg(JsonObject& msg);
    JsonObject& _createJsonobject(const char action);
    static char _nodeNameBuf[4];
    char* _nodeNameBuilder();
};

#endif
