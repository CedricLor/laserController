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

#include "Arduino.h"
#include "myWSReceiver.h"

myWSReceiver::myWSReceiver(uint8_t *data)
{

  Serial.println("myWebServerWS::_decodeWSMessage. Starting.");

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<256> doc;
  Serial.print("myWebServerWS::_decodeWSMessage(): jsonDocument created\n");

  // deserialize the message msg received from the mesh into the StaticJsonDocument doc
  DeserializationError err = deserializeJson(doc, data);
  Serial.print("myWebServerWS::_decodeWSMessage(): message msg deserialized into JsonDocument doc\n");
  Serial.print("myWebServerWS::_decodeWSMessage(): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");

  // read the type of message (0 for handshake, 3 for confirmation that change IP adress has been received, 4 for change boxState)
  const int _type = doc["type"]; // correspondings to root[action] in meshController
  Serial.printf("myWebServerWS::_decodeWSMessage(): The message _type is %i \n", _type);


  // choose the type of reaction depending on the message type
  // if type 0, handshake and comparing the number of boxRow vs the number of connected boxes
  if (_type == 0) {           // 0 for hand shake message
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i - converting doc[\"message\"] to JSON Object \n", _type);
    // Declare and define a JSONObject
    JsonObject _obj = doc["message"].as<JsonObject>();
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i - JSON Object _obj available containing the boxState of each boxRow in the DOM \n", _type);

    // if no boxRow in DOM and no boxes connected to the mesh, just return
    if (_obj.size() == 0 && ControlerBox::connectedBoxesCount == 1) {
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object _obj.size: %i. There are currently no boxRow in the DOM.\n", _type, (_obj.size() == 0));
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently no boxes connected to the mesh.\n", _type, (ControlerBox::connectedBoxesCount == 1));
      Serial.printf("myWebServerWS::_decodeWSMessage(): Ending on message type [%i], because there are no boxRow, nor connectedBoxes.\n", _type);
      return;
    }
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object _obj.size: %i. There are currently boxRow(s) in the DOM.\n", _type, _obj.size());
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently boxes connected to the mesh.\n", _type, ControlerBox::connectedBoxesCount);


    //else, there is a JSON Object of this type: {1:3,4:5,7:2}
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to iterate over the boxRows, looking for the existing boxRow and boxState in DOM\n", _type);
    for (JsonPair p : _obj) { // for each pair boxIndex:boxState in the DOM,
      Serial.printf("myWebServerWS::_decodeWSMessage(): Mew iteration.\n");
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to use the key of the current pair of the JSON object to check whether the ControlerBox corresponding to the boxRow in the DOM really exists in ControlerBoxes.\n", _type);
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, testing (int)p.key().c_str(): %i\n", _type, (int)p.key().c_str());

      // DELETED BOXES CHECKER
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, using this value to select a ControlerBoxes[]\n", _type);
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[(int)p.key().c_str()].nodeId == 0 is equal to %i\n", _type, (ControlerBoxes[(int)p.key().c_str()].nodeId == 0));
      // check if it still is connected; if not, ask for an update
      if (ControlerBoxes[(int)p.key().c_str()].nodeId == 0) {
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, the ControlerBox corresponding to the current boxRow has a nodeId of: %i. It is no longer connected to the mesh. Delete from the DOM.", _type, ControlerBoxes[(int)p.key().c_str()].nodeId);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to turn [boxDeletionHasBeenSignaled] of ControlerBoxes[%i] to false.\n", _type, (int)p.key().c_str());
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        ControlerBoxes[(int)p.key().c_str()].boxDeletionHasBeenSignaled = false;
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[%i].boxDeletionHasBeenSignaled turned to %i.\n", _type, (int)p.key().c_str(), ControlerBoxes[(int)p.key().c_str()].boxDeletionHasBeenSignaled);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _type);
      } // if


      // BOXSTATE CHECKER
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[(int)p.key().c_str()].boxActiveState = %i\n", _type, ControlerBoxes[(int)p.key().c_str()].boxActiveState);
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, (int)p.value().as<char*>() = %i\n.", _type, (int)p.value().as<char*>());
      Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, comparison between the two: %i\n.", _type, (ControlerBoxes[(int)p.key().c_str()].boxActiveState != (int)p.value().as<char*>()));
      // check if it has the correct boxState; if not, ask for an update
      if (ControlerBoxes[(int)p.key().c_str()].boxActiveState != (int)p.value().as<char*>()) {
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, the state of the ControlerBox corresponding to the current boxRow is different than its boxState in the DOM. Update it in the DOM.\n", _type);
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled = false;
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled = %i.\n", _type, ControlerBoxes[(int)p.key().c_str()].boxActiveStateHasBeenSignaled);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _type);
      } // if
    } // for

    // MISSING BOXES CHECKER
    // look for the missing items and ask for an update
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, about to iterate over the ControlerBoxes to look if any is missing from the JSON object containing the boxRows from the DOM.\n", _type);
    for (short _i = 1; _i < sBoxesCount; _i++) {
      char _c[3];
      itoa(_i, _c, 10);
      const char* _keyInJson = _obj[_c];
      if ((ControlerBoxes[_i].nodeId != 0) &&_keyInJson == nullptr) {
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, there is a missing box in the DOM. Add it.\n", _type);
        // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
        ControlerBoxes[_i].isNewBoxHasBeenSignaled = false;
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, ControlerBoxes[_i].isNewBoxHasBeenSignaled = %i\n", _type, ControlerBoxes[_i].isNewBoxHasBeenSignaled);
        Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _type);
      } // if
    } // for
    Serial.println("myWebServerWS::_decodeWSMessage. Ending on type 0 (received handshake message with list of boxRows in DOM).");
    return;
  }
  if (_type == 3) {           // 3 for confirmation that change IP adress has been received
    Serial.println("myWebServerWS::_decodeWSMessage. Ending on type 3 (received confirmation that new station IP has been received).");
    myWSSender::tSendWSDataIfChangeStationIp.disable();
    return;
  }
  if (_type == 4) {           // 4 for change boxState
    // send a mesh request to the other box
    // convert the box name to a char array box name
    int __iNodeName = doc["lb"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): (from JSON) __iNodeName = %i \n", __iNodeName);
    char _cNodeName[4];
    itoa((__iNodeName + bControllerBoxPrefix), _cNodeName, 10);
    Serial.printf("myWebServerWS::_decodeWSMessage(): _cNodeName = %s \n", _cNodeName);
    // convert the box state to a char array
    const char* _boxState = doc["boxState"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): _boxState = %s \n", _boxState);
    myMeshViews __myMeshViews;
    // instantiate a mesh view
    Serial.printf("myWebServerWS::_decodeWSMessage(): about to call __myMeshViews.changeBoxTargetState().\n");
    __myMeshViews.changeBoxTargetState(_boxState, _cNodeName);

    // send a response telling the instruction is in course of being executed
    StaticJsonDocument<64> _sub_doc;
    JsonObject _sub_obj = _sub_doc.to<JsonObject>();
    Serial.printf("---------------------- %i -------------------\n", __iNodeName);
    _sub_obj["lb"] = __iNodeName;
    _sub_obj["boxState"] = _boxState;
    myWSSender::prepareWSData(4, _sub_obj);
  }
  if (_type == 8) {             // 8 for change master
    Serial.printf("myWebServerWS::_decodeWSMessage(): _type = %i - starting \n", _type);
    // send a mesh request to the other box
    // convert the box name to a char array box name
    int __iNodeName = doc["lb"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): (from JSON) __iNodeName = %i \n", __iNodeName);
    char _cNodeName[4];
    itoa((__iNodeName + bControllerBoxPrefix), _cNodeName, 10);
    Serial.printf("myWebServerWS::_decodeWSMessage(): _cNodeName = %s \n", _cNodeName);
    // get the masterbox number
    int _iMasterBox = doc["masterbox"];
    Serial.printf("myWebServerWS::_decodeWSMessage(): _boxState = %i \n", _iMasterBox);
    myMeshViews __myMeshViews;
    // instantiate a mesh view
    Serial.printf("myWebServerWS::_decodeWSMessage(): about to call __myMeshViews.changeMasterBox().\n");
    __myMeshViews.changeMasterBoxMsg(_iMasterBox, __iNodeName);

    // send a response telling the instruction is in course of being executed
    StaticJsonDocument<64> _sub_doc;
    JsonObject _sub_obj = _sub_doc.to<JsonObject>();
    Serial.printf("---------------------- %i -------------------\n", __iNodeName);
    _sub_obj["lb"] = __iNodeName;
    _sub_obj["ms"] = _iMasterBox;
    _sub_obj["st"] = 1; // "st" for status, 1 for sent to laser controller; waiting execution
    myWSSender::prepareWSData(8, _sub_obj);
  }
  Serial.println("myWebServerWS::_decodeWSMessage. Ending.");

}
