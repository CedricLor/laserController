/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2019.

  |--main.cpp
  |  |
  |  |--myWebServerBase.cpp
  |  |  |--myWebServerBase.h
  |  |  |  |--AsyncTCP.h
  |  |  |
  |  |  |--myWebServerControler.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerControler.h
  |  |  |  |--MasterSlaveBox.cpp (called to set some values re. master/slave box reactions in global)
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp (used to be called to start and stop cycle)
  |  |  |  |--//LaserGroupedUnits.cpp (used to be called to pair and unpair)
  |  |  |
  |  |  |--myWebServerViews.cpp ("private" class: called only from myWebServerBase.cpp)
  |  |  |  |--myWebServerViews.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
*/

#include "Arduino.h"
#include "myWebServerControler.h"

myWebServerControler::myWebServerControler()
{
}

void myWebServerControler::_webSwitchRelays(AsyncWebParameter* _p2, bool _bTargetState) {
  if(_p2->value() == "a"){
    // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_bTargetState);
    // LaserGroupedUnitsArray::setTargetState(4);
  } else {
    int _val = _p2->value().toInt();
    // LaserGroupedUnits[_val].setOnOffTargetState(_bTargetState);
  }
}

void myWebServerControler::_webInclExclInPir(AsyncWebParameter* _p2, bool _bTargetState) {
  if(_p2->value() == "a"){
    // LaserGroupedUnitsArray::setTargetPirState(_bTargetState);
  } else {
    int _val = _p2->value().toInt();
    // LaserGroupedUnits[_val].setTargetPirState(_bTargetState);
  }
}

void myWebServerControler::_webChangeBlinkingInterval(AsyncWebParameter* _p1, AsyncWebParameter* _p2) {
  // Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): laser number for change in blinkingInterval %s\n", _p2->value().c_str());
  if (_p2->value() == "0") {
    Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): %s\n", _p2->value().c_str());
    unsigned long _ulTargetBlinkingInterval = _p1->value().toInt();
    // LaserGroupedUnitsArray::setTargetBlinkingInterval(_ulTargetBlinkingInterval);
  }
  else {
    int _iLaserGroupedUnitIndexNumber = _p2->value().toInt();
    int _iTargetBlinkingInterval = _p1->value().toInt();
    // LaserGroupedUnits[_iLaserGroupedUnitIndexNumber].setTargetBlinkingInterval(_iTargetBlinkingInterval);
  }
}

void myWebServerControler::decodeRequest(AsyncWebServerRequest *request) {
  Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): DECODING WEB REQUEST >>>>>>>>>>>>>>>>\n");

  if(request->hasParam("manualStatus")) {
    Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"manualStatus\")");
    AsyncWebParameter* _p1 = request->getParam("manualStatus");
    AsyncWebParameter* _p2 = request->getParam("laser");
    if(_p1->value() == "on"){
      _webSwitchRelays(_p2, LOW);
    } else {
      _webSwitchRelays(_p2, HIGH);
    }
    return;
  }

  if(request->hasParam("statusIr")) {
    Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"statusIr\")");
    AsyncWebParameter* _p1 = request->getParam("statusIr");
    AsyncWebParameter* _p2 = request->getParam("laser");
    if(_p1->value() == "on"){
      _webInclExclInPir(_p2, HIGH);
    } else {
      _webInclExclInPir(_p2, LOW);
    }
    return;
  }

  if(request->hasParam("blinkingInterval")) {
    Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"statusIr\")");
    AsyncWebParameter* _p1 = request->getParam("blinkingInterval");
    AsyncWebParameter* _p2 = request->getParam("laser");
    _webChangeBlinkingInterval(_p1, _p2);
    return;
  }

  if(request->hasParam("masterBox")) {
    Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"masterBox\")");
    AsyncWebParameter* _p1 = request->getParam("masterBox");
    AsyncWebParameter* _p2 = request->getParam("reactionToMaster");
    // void MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action)
    MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(_p1->value().toInt(), _p2->value().c_str());
    return;
  }

  if(request->hasParam("pinPairing")) {
    Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"pinPairing\")");
    AsyncWebParameter* _p = request->getParam("pinPairing");
    // LaserGroupedUnitsArray::pairUnpairAllPins(_p->value().toInt());
    return;
  }
}
