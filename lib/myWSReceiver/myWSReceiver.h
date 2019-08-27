/*
  myWSReceiver.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |--ControlerBox.h
  |  |  |
  |  |  |--myWebServerControler.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerControler.h
  |  |  |  |--MasterSlaveBox.cpp (called to set some values re. master/slave box reactions in global)
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |--//LaserGroupedUnits.cpp
  |  |  |  |--//MasterSlaveBox.cpp
  |  |  |
  |  |  |--myWebServerViews.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerViews.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
  |  |  |  |--//LaserPin.cpp
  |  |  |
  |  |  |--myWebServerWS
  |  |  |  |--myWebServerWS.cpp
  |  |  |  |  |--myWebServerWS.h
  |  |  |  |--myWSReceiver
  |  |  |  |  |--myWSReceiver.cpp
  |  |  |  |  |  |--myWSReceiver.H
  |  |  |  |--myWSSender
  |  |  |  |  |--myWSSender.cpp
  |  |  |  |  |  |--myWSSender.H

*/

#ifndef myWSReceiver_h
#define myWSReceiver_h

#include "Arduino.h"
#include <mySavedPrefs.h>
#include <ControlerBox.h>
#include <myWSSender.h>
#include <myMeshViews.h>
#include <myMeshStarter.h>

class myWSReceiver
{
  public:
    myWSReceiver(uint8_t *_data);

  private:

    void _actionSwitch(JsonObject& _obj);

    void _onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj);
    void _checkConsistancyDOMDB(JsonObject& _joBoxState);
    void _lookForDisconnectedBoxes(JsonPair& _p);
    void _checkBoxStateConsistancy(JsonPair& _p);
    void _lookForDOMMissingRows(JsonObject& _joBoxState);

    void _requestBoxChange(JsonObject& _obj, bool _bBroadcast=false);
    void _requestIFChange(JsonObject& _obj);
    
    void _rebootIF(JsonObject& _obj);
    void _saveIF(JsonObject& _obj);
    void _specificSaveIF(JsonObject& _obj);

    void _savegi8RequestedOTAReboots(JsonObject& _obj);
    void _requestNetChange(JsonObject& _obj);

};
#endif


