/*
  myWSReceiver.h - Web controller.
  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWSReceiver_h
#define myWSReceiver_h

#include "Arduino.h"
#include <mySavedPrefs.h>
#include <controllerBoxThis.h>
#include <myWSSender.h>
#include <myWSReceiverReconcile.h>
#include <myMeshViews.h>
#include <myMeshStarter.h>

class myWSReceiver
{
  public:
    myWSReceiver(uint8_t *_data);

  private:

    void _actionSwitch(JsonObject& _obj);

    void _requestBoxChange(JsonObject& _obj, bool _bBroadcast=false);
    void _requestIFChange(JsonObject& _obj);
    
    void _rebootIF(JsonObject& _obj);
    void _saveIF(JsonObject& _obj);

    void _savegi8RequestedOTAReboots(JsonObject& _obj);
    void _requestNetChange(JsonObject& _obj);

};
#endif


