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

myWebServerControler provides a main decodeRequest function to myWebServerBase.
decodeRequest is being transmitted the requests received by the webServer as a parameter.
It screens the request through a chain of "if" conditions.

Upon meeting these conditions, it transmits orders to other parts of the box :
- historically:
  - LaserPins: setting on/off status; setting IR status, blinking interval, for each pin;
  - LaserPinsArray; idem but for all pins
  - MasterSlaveBox: setting the name of the master box;
- then:
  - LaserGroupedUnits: setting on/off status; setting IR status, blinking interval, for each LaserGroupedUnit;
  - LaserGroupedUnitArray: idem but for all LaserGroupedUnits;
  - MasterSlaveBox: setting the name of the master box;
- in the future:
  - tones;
  - sequences;
  - boxState.
  - MasterSlaveBox: setting the name of the master box.
*/

#include "Arduino.h"
#include "myWebServerControler.h"

myWebServerControler::myWebServerControler()
{
}

// void myWebServerControler::_webSwitchRelays(AsyncWebParameter* _p2, bool _bTargetState) {
//   if(_p2->value() == "a"){
//     // LaserGroupedUnitsArray::setTargetStateOfLaserGroupUnits(_bTargetState);
//     // LaserGroupedUnitsArray::setTargetState(4);
//   } else {
//     // int _val = _p2->value().toInt();
//     // LaserGroupedUnits[_val].setOnOffTargetState(_bTargetState);
//   }
// }
//
// void myWebServerControler::_webInclExclInPir(AsyncWebParameter* _p2, bool _bTargetState) {
//   if(_p2->value() == "a"){
//     // LaserGroupedUnitsArray::setTargetPirState(_bTargetState);
//   } else {
//     // int _val = _p2->value().toInt();
//     // LaserGroupedUnits[_val].setTargetPirState(_bTargetState);
//   }
// }
//
// void myWebServerControler::_webChangeBlinkingInterval(AsyncWebParameter* _p1, AsyncWebParameter* _p2) {
//   // Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): laser number for change in blinkingInterval %s\n", _p2->value().c_str());
//   if (_p2->value() == "0") {
//     // Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): %s\n", _p2->value().c_str());
//     // unsigned long _ulTargetBlinkingInterval = _p1->value().toInt();
//     // LaserGroupedUnitsArray::setTargetBlinkingInterval(_ulTargetBlinkingInterval);
//   }
//   else {
//     // int _iLaserGroupedUnitIndexNumber = _p2->value().toInt();
//     // int _iTargetBlinkingInterval = _p1->value().toInt();
//     // LaserGroupedUnits[_iLaserGroupedUnitIndexNumber].setTargetBlinkingInterval(_iTargetBlinkingInterval);
//   }
// }

void myWebServerControler::decodeRequest(AsyncWebServerRequest *request) {
  if (MY_DG_WEB) {
    Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): DECODING WEB REQUEST >>>>>>>>>>>>>>>>\n");
  }
  // if request is change status
  // expected request: http://192.168.43.84/?status=[some boxTargetState index number]&lb=[sboxname]
  // if(request->hasParam("boxTargetState")) {
  //   if (MY_DG_WEB) {
  //     Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"boxTargetState\")");
  //   }
  //   AsyncWebParameter* _boxTargetState = request->getParam("boxTargetState");
  //   AsyncWebParameter* _relatedLaserBoxNodeName = request->getParam("lb");
  //   if (MY_DG_WEB) {
  //     Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): param \"boxTargetState\" = %c", *(_boxTargetState->value().c_str()));
  //     Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): param \"lb\" = %c", *(_relatedLaserBoxNodeName->value().c_str()));
  //   }
  //   myMeshViews __myMeshViews;
  //   __myMeshViews.changeBoxTargetState((short)(_boxTargetState->value().c_str()), (short)(_relatedLaserBoxNodeName->value().c_str()));
  //   // send mesh request to change boxState
  //   return;
  // }

  // if(request->hasParam("manualStatus")) {
  //   Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"manualStatus\")");
  //   AsyncWebParameter* _p1 = request->getParam("manualStatus");
  //   AsyncWebParameter* _p2 = request->getParam("laser");
  //   if(_p1->value() == "on"){
  //     // _webSwitchRelays(_p2, LOW);
  //   } else {
  //     // _webSwitchRelays(_p2, HIGH);
  //   }
  //   return;
  // }
  //
  // if(request->hasParam("statusIr")) {
  //   Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"statusIr\")");
  //   AsyncWebParameter* _p1 = request->getParam("statusIr");
  //   AsyncWebParameter* _p2 = request->getParam("laser");
  //   if(_p1->value() == "on"){
  //     // _webInclExclInPir(_p2, HIGH);
  //   } else {
  //     // _webInclExclInPir(_p2, LOW);
  //   }
  //   return;
  // }
  //
  // if(request->hasParam("blinkingInterval")) {
  //   Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"statusIr\")");
  //   AsyncWebParameter* _p1 = request->getParam("blinkingInterval");
  //   AsyncWebParameter* _p2 = request->getParam("laser");
  //   // _webChangeBlinkingInterval(_p1, _p2);
  //   return;
  // }
  //
  // if(request->hasParam("masterBox")) {
  //   Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"masterBox\")");
  //   AsyncWebParameter* _p1 = request->getParam("masterBox");
  //   AsyncWebParameter* _p2 = request->getParam("reactionToMaster");
  //   // void MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action)
  //   // MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(_p1->value().toInt(), _p2->value().c_str());
  //   return;
  // }
  //
  // if(request->hasParam("pinPairing")) {
  //   Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): request->hasParam(\"pinPairing\")");
  //   // AsyncWebParameter* _p = request->getParam("pinPairing");
  //   // LaserGroupedUnitsArray::pairUnpairAllPins(_p->value().toInt());
  //   return;
  // }
}