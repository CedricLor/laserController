/*
  myMeshController.h - Library to handle mesh controller related functions.
  Created by Cedric Lor, January 22, 2019.
*/

#ifndef myMeshController_h
#define myMeshController_h

#include "Arduino.h"

class myMeshController
{
  public:
    myMeshController(JsonObject& root);

    void autoSwitchRelays(const char* senderStatus);

  private:
    static const bool _B_SLAVE_ON_OFF_REACTIONS[4][2];

    char _action[];
};

#endif
