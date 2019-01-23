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
    myMeshViews();
    // Views
    static void manualSwitchMsg(const short targetOnOffState);
    static void inclusionIRMsg(const short targetIrState);
    static void blinkingIntervalMsg(const unsigned long targetBlinkingInterval);
    static void changeMasterBoxMsg(const int newMasterNodeId, const char* newReaction);
    static void statusMsg(const char* myStatus);
    static void pinPairingMsg(const short sTargetPairingType);
    static void dataRequestMsg();
    static void dataRequestResponse();

  private:

    // Helpers
    static void _broadcastMsg(JsonObject& msg);
    static JsonObject& _createJsonobject(const char action);
    static char _nodeNameBuf[4];
    static char* _nodeNameBuilder();
};

#endif
