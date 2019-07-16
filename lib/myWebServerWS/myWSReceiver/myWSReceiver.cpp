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




myWSReceiver::myWSReceiver(uint8_t *_data)
{

  if (MY_DG_WS) {
    Serial.println("myWSReceiver::myWSReceiver. Starting.");
  }

  // create a StaticJsonDocument entitled _doc
  const int _iCapacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<_iCapacity> _doc;
  if (MY_DG_WS) {
    Serial.print("myWSReceiver::myWSReceiver(): jsonDocument created\n");
  }
  // Convert the JSON document to a JSON object
  JsonObject _obj = _doc.to<JsonObject>();

  // deserialize the message msg received from the mesh into the StaticJsonDocument _doc
  DeserializationError err = deserializeJson(_doc, _data);
  if (MY_DG_WS) {
    Serial.print("myWSReceiver::myWSReceiver(): message msg deserialized into JsonDocument _doc\n");
    Serial.print("myWSReceiver::myWSReceiver(): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");
  }

  // read the action type of message (0 for handshake, 3 for confirmation that change IP adress has been received, 4 for change boxState)
  const int8_t __i8MessageActionType = _obj["action"]; // correspondings to root[action] in meshController
  if (MY_DG_WS) { Serial.printf("myWSReceiver::myWSReceiver(): The message __i8MessageActionType is %i \n", __i8MessageActionType); }


  // choose the type of reaction depending on the message action type
  _actionSwitch(__i8MessageActionType, _obj);
}






void myWSReceiver::_actionSwitch(const int8_t __i8MessageActionType, JsonObject& _obj) {
  // if action type 0, handshake -> compare the number of boxRow in DOM vs the number of connected boxes
  // Received JSON: {action:0, message:{1:4;2:3}}
  if (__i8MessageActionType == 0) {           // 0 for hand shake message
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::myWSReceiver(): __i8MessageActionType = %i - new WS: going to check whether the DOM needs to be updated. \n", __i8MessageActionType);
    }
    _onHandshakeCheckWhetherDOMNeedsUpdate(__i8MessageActionType, _obj); // _obj = {action:0, message:{1:4;2:3}}
    return;
  }

  // 3 for confirmation that change IP adress has been received
  if (__i8MessageActionType == 3) {
    if (MY_DG_WS) {
      Serial.println("myWSReceiver::myWSReceiver. Ending on __i8MessageActionType == 3 (received confirmation that new station IP has been received).");
    }
    // disable the task sending the station IP
    myWSSender::tSendWSDataIfChangeStationIp.disable();
    return;
  }

  // 4 for change boxState
  if (__i8MessageActionType == 4) {
    // _obj = {action: 4; lb: 1; "boxState": 3}
    _requestBoxChange(_obj, (const char&)"boxState", __i8MessageActionType);
    // _requestActiveStateChange(_obj);
    return;
  }

  // 8 for change master
  if (__i8MessageActionType == 8) {
    // send a mesh request to the relevant laser box
    // _obj = {action: 8, lb: 1, "masterbox": 4}
    _requestBoxChange(_obj, (const char&)"masterbox", __i8MessageActionType);
    // _requestMasterChange(__i8MessageActionType, _obj);
    return;
  }

  // 9 for change default state
  if (__i8MessageActionType == 9) {
    // send a mesh request to the relevant laser box
    // _obj = {action: 9; lb: 1; "boxDefstate": 3}
    _requestBoxChange(_obj, (const char&)"boxDefstate", __i8MessageActionType);
    // _requestDefaultStateChange(_obj);
    return;
  }
}






void myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(const int8_t _i8MessageActionType, JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/) {

  // Declare and define a JSONObject to read
  JsonObject __joBoxesStatesInDOM = _obj["boxesStatesInDOM"].as<JsonObject>(); // __joBoxesStatesInDOM = {1:4;2:3}
  if (MY_DG_WS) { Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): _i8MessageActionType = %i - JSON Object _obj available containing the boxState of each boxRow in the DOM \n", _i8MessageActionType); }



  if (__joBoxesStatesInDOM.size() == 0) {
    // there are no boxRows in the DOM
    if (ControlerBox::connectedBoxesCount == 1) {
      // there are no boxes connected to the mesh (and no boxes in the DOM), just return
      // if (MY_DG_WS) {
      //   Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): _i8MessageActionType = %i, JSON Object __joBoxesStatesInDOM.size: %i. There are currently no boxRow in the DOM.\n", _i8MessageActionType, (__joBoxesStatesInDOM.size() == 0));
      //   Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): _i8MessageActionType = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently no boxes connected to the mesh.\n", _i8MessageActionType, (ControlerBox::connectedBoxesCount == 1));
      //   Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending on message action type [%i], because there are no boxRow, nor connectedBoxes.\n", _i8MessageActionType);
      // }
      return;
    }
    else // re. if (ControlerBox::connectedBoxesCount == 1)
    // there are boxes connected to the mesh (and no boxes in the DOM), look for the missing boxes
    {
      _lookForDOMMissingRows(_i8MessageActionType, __joBoxesStatesInDOM);
      return;
    }
  }

  else // re. (__joBoxesStatesInDOM.size() != 0)
  // there are boxRows in DOM
  {
    if (ControlerBox::connectedBoxesCount == 1) {
      // there are no connected boxes (and boxes in the DOM):
      // -> send instruction to delete all the boxRows from the DOM
      __joBoxesStatesInDOM["lb"] = "a"; // "a" means delete all the boxes
      myWSSender _myWSSender;
      _myWSSender.prepareWSData(7, __joBoxesStatesInDOM);
      return;
    }
    else // re. if (ControlerBox::connectedBoxesCount == 1)
    // there are boxes connected to the mesh (and boxes in the DOM):
    // -> check consistency between the DOM and ControlerBoxes[]
    {
      _checkConsistancyDOMDB(_i8MessageActionType, __joBoxesStatesInDOM);
    } // end else
  } // end if (__joBoxesStatesInDOM.size() != 0)

}





void myWSReceiver::_checkConsistancyDOMDB(const int8_t _i8MessageActionType, JsonObject& _joBoxState) {
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageActionType = %i, JSON Object _joBoxState.size: %i. There are currently boxRow(s) in the DOM.\n", _i8MessageActionType, _joBoxState.size());
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageActionType = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently boxes connected to the mesh.\n", _i8MessageActionType, ControlerBox::connectedBoxesCount);
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageActionType = %i, about to iterate over the boxRows, looking for the existing boxRow and boxState in DOM\n", _i8MessageActionType);
  }

  for (JsonPair _p : _joBoxState) { // for each pair boxIndex:boxState in the DOM,
    // {1:4;2:3;etc.}

    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageActionType = %i, new iteration.\n", _i8MessageActionType);
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageActionType = %i, about to use the key of the current pair of the JSON object to check whether the ControlerBox corresponding to the boxRow in the DOM really exists in ControlerBoxes.\n", _i8MessageActionType);
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageActionType = %i, testing p.key().c_str(): %s\n", _i8MessageActionType, _p.key().c_str());
    }

    // DISCONNECTED BOXES CHECKER
    _lookForDisconnectedBoxes(_i8MessageActionType, _p);

    // BOXSTATE CHECKER
    _checkBoxStateConsistancy(_i8MessageActionType, _p);

  } // end for (JsonPair _p : _joBoxState)

  // MISSING BOXES CHECKER
  // look for missing boxes in the DOM and ask for an update
  _lookForDOMMissingRows(_i8MessageActionType, _joBoxState);

  if (MY_DG_WS) {
    Serial.println("myWSReceiver::_checkConsistancyDOMDB. Ending on action type 0 (received handshake message with list of boxRows in DOM).");
  }
  return;
}





void myWSReceiver::_lookForDisconnectedBoxes(const int8_t _i8MessageActionType, JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);

  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageActionType = %i, using this value to select a ControlerBoxes[]\n", _i8MessageActionType);
    Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageActionType = %i, ControlerBoxes[_iBoxIndex].nodeId == 0 is equal to %i\n", _i8MessageActionType, (ControlerBoxes[_iBoxIndex].nodeId == 0));
  }

  // check if it still is connected; if not, request an update of the DOM
  if (ControlerBoxes[_iBoxIndex].nodeId == 0) {
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageActionType = %i, the ControlerBox corresponding to the current boxRow has a nodeId of: %i. It is no longer connected to the mesh. Delete from the DOM.", _i8MessageActionType, ControlerBoxes[_iBoxIndex].nodeId);
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageActionType = %i, about to turn [boxDeletionHasBeenSignaled] of ControlerBoxes[%i] to false.\n", _i8MessageActionType, _iBoxIndex);
    }
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    ControlerBoxes[_iBoxIndex].boxDeletionHasBeenSignaled = false;
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageActionType = %i, ControlerBoxes[%i].boxDeletionHasBeenSignaled turned to %i.\n", _i8MessageActionType, _iBoxIndex, ControlerBoxes[_iBoxIndex].boxDeletionHasBeenSignaled);
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageActionType = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _i8MessageActionType);
    }
  } // if
}






void myWSReceiver::_checkBoxStateConsistancy(const int8_t _i8MessageActionType, JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageActionType = %i, ControlerBoxes[_iBoxIndex].boxActiveState = %i\n", _i8MessageActionType, ControlerBoxes[_iBoxIndex].boxActiveState);
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageActionType = %i, (int)(_p.value().as<char*>()) = %i\n.", _i8MessageActionType, (int)(_p.value().as<char*>()));
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageActionType = %i, comparison between the two: %i\n.", _i8MessageActionType, (ControlerBoxes[_iBoxIndex].boxActiveState == (int)(_p.value().as<char*>())));
  }
  // check if it has the correct boxState; if not, ask for an update
  if (ControlerBoxes[_iBoxIndex].boxActiveState != (int)(_p.value().as<char*>())) {
    if (MY_DG_WS) { Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageActionType = %i, the state of the ControlerBox corresponding to the current boxRow is different than its boxState in the DOM. Update it in the DOM.\n", _i8MessageActionType);}
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled = false;
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageActionType = %i, ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled = %i.\n", _i8MessageActionType, ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled);
      Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageActionType = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _i8MessageActionType);
    }
  } // end if
}





void myWSReceiver::_lookForDOMMissingRows(const int8_t _i8MessageActionType, JsonObject& _joBoxState) {
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageActionType = %i, about to iterate over the ControlerBoxes to look if any is missing from the JSON object containing the boxRows from the DOM.\n", _i8MessageActionType);
  }

  for (short _i = 1; _i < sBoxesCount; _i++) {
    char _c[3];
    itoa(_i, _c, 10);
    const char* _keyInJson = _joBoxState[_c];
    if ((ControlerBoxes[_i].nodeId != 0) && _keyInJson == nullptr) {
      if (MY_DG_WS) {Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageActionType = %i, there is a missing box in the DOM. Add it.\n", _i8MessageActionType);}
      // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
      ControlerBoxes[_i].isNewBoxHasBeenSignaled = false;

      if (MY_DG_WS) {
        Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageActionType = %i, ControlerBoxes[_i].isNewBoxHasBeenSignaled = %i\n", _i8MessageActionType, ControlerBoxes[_i].isNewBoxHasBeenSignaled);
        Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageActionType = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _i8MessageActionType);
      }
    } // if
  } // for
}






void myWSReceiver::_requestBoxChange(JsonObject& _obj, const char& _cChangeKey, const int8_t _i8MessageActionType) {
  // _obj = {action: 4; lb: 1; "boxState": 3}
  // _obj = {action: 8, lb: 1, "masterbox": 4}
  // _obj = {action: 9; lb: 1; "boxDefstate": 3}
  // (const char&)"boxState"
  // (const char&)"masterbox"
  // (const char&)"boxDefstate"

  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestBoxChange(): Starting with _cChangeKey = %s, _i8MessageActionType = %i \n", &_cChangeKey, _i8MessageActionType);
  }

  // get destination box number
  const int8_t __i8BoxIndexInCB = _obj["lb"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestBoxChange(): (from JSON) __i8BoxIndexInCB = %i \n", __i8BoxIndexInCB);
  }

  // read target state
  const int8_t __i8RequestedChange = _obj[&_cChangeKey];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestBoxChange(): %s = %i \n", &_cChangeKey, __i8RequestedChange);
  }

  // instantiate a mesh view
  myMeshViews __myMeshViews;
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestBoxChange(): about to call __myMeshViews.changeBoxRequest().\n");
  }
  // __myMeshViews.changeBoxRequest(_obj);
  __myMeshViews.changeBoxRequest(__i8RequestedChange, _cChangeKey, __i8BoxIndexInCB);

  // send a response telling the instruction is in course of being executed
  StaticJsonDocument<64> _sub_doc;
  JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  _sub_obj["lb"] = __i8BoxIndexInCB;
  _sub_obj[&_cChangeKey] = __i8RequestedChange;

  // // send a response telling the instruction is in course of being executed
  // StaticJsonDocument<64> _sub_doc;
  // JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  // _sub_obj["lb"] = __i8BoxIndexInCB;
  // _sub_obj["boxState"] = __i8BoxTargetState;

  // // send a response to the browser telling the instruction is in course of being executed
  // StaticJsonDocument<64> _sub_doc;
  // JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  // _sub_obj["lb"] = __i8BoxIndexInCB;
  // _sub_obj["ms"] = __i8MasterBox;
  // _sub_obj["st"] = 1; // "st" for status, 1 for sent to laser controller; waiting execution

  // // send a response telling the instruction is in course of being executed
  // StaticJsonDocument<64> _sub_doc;
  // JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  // _sub_obj["lb"] = __i8BoxIndexInCB;
  // _sub_obj["boxDefstate"] = __i8BoxDefaultState;

  myWSSender _myWSSender;
  _myWSSender.prepareWSData(_i8MessageActionType, _sub_obj);
}
