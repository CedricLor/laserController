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
    String _createManualSwitchMsg(const char* myStatus);
    String _createInclusionIRMsg(const char* myStatus);
    String _createBlinkingIntervalMsg(const char* myStatus);
    String _createChangeMasterBoxMsg(const char* myStatus);
    String _createStatusMsg(const char* myStatus);
    String _createPinPairingMsg(const char* myStatus);
    String _createDataRequestMsg(const char* myStatus);
    String _createDataRequestResponse(const char* myStatus);

    // Helpers
    void _broadcastMsg();
    String _createReturnString(JsonObject& msg);
    JsonObject& _createJsonobject();
    static char _nodeNameBuf[4];
    char* _nodeNameBuilder();
};

#endif
