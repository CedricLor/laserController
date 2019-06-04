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
    myMeshController(JsonDocument root);


  private:
    static const bool _B_SLAVE_ON_OFF_REACTIONS[4][2];

    char _action;

    void _manualSwitch(JsonDocument root);
    void _changeInclusionIR(JsonDocument root);
    void _changeBlinkingInterval(JsonDocument root);
    void _changeMasterBox(JsonDocument root);
    void _slaveBoxSwitch(JsonDocument root);
    void _pinPairing(JsonDocument root);
    void _dataRequest();
};

#endif
