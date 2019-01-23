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
    // Views
    void manualSwitchMsg(const short targetOnOffState);
    void inclusionIRMsg(const short targetIrState);
    void blinkingIntervalMsg(const unsigned long targetBlinkingInterval);
    void changeMasterBoxMsg(const int newMasterNodeId, const char* newReaction);
    void statusMsg(const char* myStatus);
    void pinPairingMsg(const short sTargetPairingType);
    void dataRequestMsg();
    void dataRequestResponse();

  private:

    // Helpers
    void _broadcastMsg(JsonObject& msg);
    JsonObject& _createJsonobject(const char action);
    static char _nodeNameBuf[4];
    char* _nodeNameBuilder();
};

#endif
