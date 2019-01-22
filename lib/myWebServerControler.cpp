/*
  myWebServerControler.h - Library web server controller related functions.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "myWebServerControler.h"

myWebServerControler::myWebServerControler()
{
}

void myWebServerControler::webSwitchRelays(LaserPin *LaserPins, AsyncWebParameter* _p2, bool targetState) {
  if(_p2->value() == "a"){
    LaserPinsArray::manualSwitchAllRelays(LaserPins, targetState);
  } else {
    int val = _p2->value().toInt();
    LaserPins[val].manualSwitchOneRelay(targetState);
  }
}

void myWebServerControler::webInclExclRelaysInPir(LaserPin *LaserPins, AsyncWebParameter* _p2, bool targetState) {
  if(_p2->value() == "a"){
    LaserPinsArray::inclExclAllRelaysInPir(LaserPins, targetState);
  } else {
    int val = _p2->value().toInt();
    LaserPins[val].inclExclOneRelayInPir(targetState);
  }
}

void myWebServerControler::decodeRequest(LaserPin *LaserPins, AsyncWebServerRequest *request) {
  Serial.print("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): DECODING WEB REQUEST >>>>>>>>>>>>>>>>\n");

  if(request->hasParam("manualStatus")) {
    AsyncWebParameter* _p1 = request->getParam("manualStatus");
    AsyncWebParameter* _p2 = request->getParam("laser");
    if(_p1->value() == "on"){
      webSwitchRelays(LaserPins, _p2, LOW);
    } else {
      webSwitchRelays(LaserPins, _p2, HIGH);
    }
    return;
  }

  if(request->hasParam("statusIr")) {
    AsyncWebParameter* _p1 = request->getParam("statusIr");
    AsyncWebParameter* _p2 = request->getParam("laser");
    if(_p1->value() == "on"){
      webInclExclRelaysInPir(LaserPins, _p2, HIGH);
    } else {
      webInclExclRelaysInPir(LaserPins, _p2, LOW);
    }
    return;
  }

  if(request->hasParam("blinkingInterval")) {
    AsyncWebParameter* _p1 = request->getParam("blinkingInterval");
    AsyncWebParameter* _p2 = request->getParam("laser");
    // Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): laser number for change in blinkingInterval %s\n", _p2->value().c_str());
    if (_p2->value() == "10") {
      // Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): %s\n", _p2->value().c_str());
      int targetBlinkingInterval = _p1->value().toInt();
      LaserPinsArray::changeGlobalBlinkingInterval(LaserPins, targetBlinkingInterval);
    }
    else {
      int pinIndexNumber = _p2->value().toInt();
      int targetBlinkingInterval = _p1->value().toInt();
      LaserPins[pinIndexNumber].changeIndividualBlinkingInterval(targetBlinkingInterval);
    }
    return;
  }

  if(request->hasParam("masterBox")) {
    AsyncWebParameter* _p1 = request->getParam("masterBox");
    AsyncWebParameter* _p2 = request->getParam("reactionToMaster");
    MasterSlaveBox::changeGlobalMasterBoxAndSlaveReaction(_p1->value().toInt(), _p2->value().c_str());
    return;
  }
}
