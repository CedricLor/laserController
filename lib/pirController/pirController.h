/*
  pirController.h - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#ifndef pirController_h
#define pirController_h

#include "Arduino.h"
#include <laserSignal.h>
#include <myMeshViews.h>

class pirController
{
  public:
    pirController(ControlerBox & __thisCtrlerBox, laserSignal & __thisLaserSignalHandler, Task & __tSetPirTimeStampAndBrdcstMsg, Task & __tPirSpeedBumper, myMeshViews & __thisMeshView, const uint8_t _INPUT_PIN=12);
    
    void check();

  private:
    ControlerBox & _thisCtrlerBox;
    laserSignal & _thisLaserSignalHandler;
    const uint8_t _inputPin;

    Task & _tSetPirTimeStampAndBrdcstMsg;
    void _tcbSetPirTimeStampAndBrdcstMsg();
    bool _oetcbSetPirTimeStampAndBrdcstMsg();

    uint16_t _speedBumper;
    Task & _tPirSpeedBumper;

    myMeshViews & _thisMeshView;
};

#endif
