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

  if (MY_DG_WS) {
    Serial.println("myWSReceiver::_decodeWSMessage. Starting.");
  }

  // create a StaticJsonDocument entitled doc
  StaticJsonDocument<256> doc;
  if (MY_DG_WS) {
    Serial.print("myWSReceiver::_decodeWSMessage(): jsonDocument created\n");
  }

  // deserialize the message msg received from the mesh into the StaticJsonDocument doc
  DeserializationError err = deserializeJson(doc, data);
  if (MY_DG_WS) {
    Serial.print("myWSReceiver::_decodeWSMessage(): message msg deserialized into JsonDocument doc\n");
    Serial.print("myWSReceiver::_decodeWSMessage(): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");
  }

  // read the type of message (0 for handshake, 3 for confirmation that change IP adress has been received, 4 for change boxState)
  const int8_t __i8MessageType = doc["type"]; // correspondings to root[action] in meshController
  if (MY_DG_WS) { Serial.printf("myWSReceiver::_decodeWSMessage(): The message __i8MessageType is %i \n", __i8MessageType); }


  // choose the type of reaction depending on the message type

  // if type 0, handshake -> compare the number of boxRow in DOM vs the number of connected boxes
  if (__i8MessageType == 0) {           // 0 for hand shake message
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_decodeWSMessage(): __i8MessageType = %i - new WS: going to check whether the DOM needs to be updated. \n", __i8MessageType);
    }
    _onHandshakeCheckWhetherDOMNeedsUpdate(__i8MessageType, doc);
    return;
  }

  if (__i8MessageType == 3) {           // 3 for confirmation that change IP adress has been received
    if (MY_DG_WS) {
      Serial.println("myWSReceiver::_decodeWSMessage. Ending on __i8MessageType == 3 (received confirmation that new station IP has been received).");
    }
    // disable the task sending the station IP
    myWSSender::tSendWSDataIfChangeStationIp.disable();
    return;
  }

  if (__i8MessageType == 4) {           // 4 for change boxState
    // send a mesh request to the relevant laser box

    _requestActiveStateChange(doc);
    return;
  }

  if (__i8MessageType == 8) {             // 8 for change master
    // send a mesh request to the relevant laser box

    _requestMasterChange(__i8MessageType, doc);
    return;
  }

  if (__i8MessageType == 9) {             // 9 for change default state
    // send a mesh request to the relevant laser box

    _requestDefaultStateChange(doc);
    return;
  }
}



void myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(const int8_t _i8MessageType, JsonDocument& doc) {

  // Declare and define a JSONObject
  JsonObject _obj = doc["message"].as<JsonObject>();
  if (MY_DG_WS) { Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): _i8MessageType = %i - JSON Object _obj available containing the boxState of each boxRow in the DOM \n", _i8MessageType); }



  if (_obj.size() == 0) {
    // there are no boxRows in the DOM
    if (ControlerBox::connectedBoxesCount == 1) {
      // there are no boxes connected to the mesh (and no boxes in the DOM), just return
      // if (MY_DG_WS) {
      //   Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): _i8MessageType = %i, JSON Object _obj.size: %i. There are currently no boxRow in the DOM.\n", _i8MessageType, (_obj.size() == 0));
      //   Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): _i8MessageType = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently no boxes connected to the mesh.\n", _i8MessageType, (ControlerBox::connectedBoxesCount == 1));
      //   Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending on message type [%i], because there are no boxRow, nor connectedBoxes.\n", _i8MessageType);
      // }
      return;
    }
    else // re. if (ControlerBox::connectedBoxesCount == 1)
    // there are boxes connected to the mesh (and no boxes in the DOM), look for the missing boxes
    {
      _lookForDOMMissingRows(_i8MessageType, _obj);
      return;
    }
  }

  else // re. (_obj.size() != 0)
  // there are boxRows in DOM
  {
    if (ControlerBox::connectedBoxesCount == 1) {
      // there are no connected boxes (and boxes in the DOM):
      // -> send instruction to delete all the boxRows from the DOM
      _obj["lb"] = "a"; // "a" means delete all the boxes
      myWSSender _myWSSender;
      _myWSSender.prepareWSData(7, _obj);
      return;
    }
    else // re. if (ControlerBox::connectedBoxesCount == 1)
    // there are boxes connected to the mesh (and boxes in the DOM):
    // -> check consistency between the DOM and ControlerBoxes[]
    {
      _checkConsistancyDOMDB(_i8MessageType, _obj);
    } // end else
  } // end if (_obj.size() != 0)

}





void myWSReceiver::_checkConsistancyDOMDB(const int8_t _i8MessageType, JsonObject& _obj) {
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageType = %i, JSON Object _obj.size: %i. There are currently boxRow(s) in the DOM.\n", _i8MessageType, _obj.size());
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageType = %i, JSON Object ControlerBox::connectedBoxesCount =  %i. There are currently boxes connected to the mesh.\n", _i8MessageType, ControlerBox::connectedBoxesCount);
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageType = %i, about to iterate over the boxRows, looking for the existing boxRow and boxState in DOM\n", _i8MessageType);
  }

  for (JsonPair _p : _obj) { // for each pair boxIndex:boxState in the DOM,

    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageType = %i, new iteration.\n", _i8MessageType);
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageType = %i, about to use the key of the current pair of the JSON object to check whether the ControlerBox corresponding to the boxRow in the DOM really exists in ControlerBoxes.\n", _i8MessageType);
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): _i8MessageType = %i, testing p.key().c_str(): %s\n", _i8MessageType, _p.key().c_str());
    }

    // DISCONNECTED BOXES CHECKER
    _lookForDisconnectedBoxes(_i8MessageType, _p);

    // BOXSTATE CHECKER
    _checkBoxStateConsistancy(_i8MessageType, _p);

  } // end for (JsonPair _p : _obj)

  // MISSING BOXES CHECKER
  // look for missing boxes in the DOM and ask for an update
  _lookForDOMMissingRows(_i8MessageType, _obj);

  if (MY_DG_WS) {
    Serial.println("myWSReceiver::_checkConsistancyDOMDB. Ending on type 0 (received handshake message with list of boxRows in DOM).");
  }
  return;
}





void myWSReceiver::_lookForDisconnectedBoxes(const int8_t _i8MessageType, JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);

  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageType = %i, using this value to select a ControlerBoxes[]\n", _i8MessageType);
    Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageType = %i, ControlerBoxes[_iBoxIndex].nodeId == 0 is equal to %i\n", _i8MessageType, (ControlerBoxes[_iBoxIndex].nodeId == 0));
  }

  // check if it still is connected; if not, request an update of the DOM
  if (ControlerBoxes[_iBoxIndex].nodeId == 0) {
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageType = %i, the ControlerBox corresponding to the current boxRow has a nodeId of: %i. It is no longer connected to the mesh. Delete from the DOM.", _i8MessageType, ControlerBoxes[_iBoxIndex].nodeId);
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageType = %i, about to turn [boxDeletionHasBeenSignaled] of ControlerBoxes[%i] to false.\n", _i8MessageType, _iBoxIndex);
    }
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    ControlerBoxes[_iBoxIndex].boxDeletionHasBeenSignaled = false;
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageType = %i, ControlerBoxes[%i].boxDeletionHasBeenSignaled turned to %i.\n", _i8MessageType, _iBoxIndex, ControlerBoxes[_iBoxIndex].boxDeletionHasBeenSignaled);
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): _i8MessageType = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _i8MessageType);
    }
  } // if
}






void myWSReceiver::_checkBoxStateConsistancy(const int8_t _i8MessageType, JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageType = %i, ControlerBoxes[_iBoxIndex].boxActiveState = %i\n", _i8MessageType, ControlerBoxes[_iBoxIndex].boxActiveState);
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageType = %i, (int)(_p.value().as<char*>()) = %i\n.", _i8MessageType, (int)(_p.value().as<char*>()));
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageType = %i, comparison between the two: %i\n.", _i8MessageType, (ControlerBoxes[_iBoxIndex].boxActiveState == (int)(_p.value().as<char*>())));
  }
  // check if it has the correct boxState; if not, ask for an update
  if (ControlerBoxes[_iBoxIndex].boxActiveState != (int)(_p.value().as<char*>())) {
    if (MY_DG_WS) { Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageType = %i, the state of the ControlerBox corresponding to the current boxRow is different than its boxState in the DOM. Update it in the DOM.\n", _i8MessageType);}
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled = false;
    if (MY_DG_WS) {
      Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageType = %i, ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled = %i.\n", _i8MessageType, ControlerBoxes[_iBoxIndex].boxActiveStateHasBeenSignaled);
      Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): _i8MessageType = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _i8MessageType);
    }
  } // end if
}





void myWSReceiver::_lookForDOMMissingRows(const int8_t _i8MessageType, JsonObject& _obj) {
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageType = %i, about to iterate over the ControlerBoxes to look if any is missing from the JSON object containing the boxRows from the DOM.\n", _i8MessageType);
  }

  for (short _i = 1; _i < sBoxesCount; _i++) {
    char _c[3];
    itoa(_i, _c, 10);
    const char* _keyInJson = _obj[_c];
    if ((ControlerBoxes[_i].nodeId != 0) && _keyInJson == nullptr) {
      if (MY_DG_WS) {Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageType = %i, there is a missing box in the DOM. Add it.\n", _i8MessageType);}
      // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
      ControlerBoxes[_i].isNewBoxHasBeenSignaled = false;

      if (MY_DG_WS) {
        Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageType = %i, ControlerBoxes[_i].isNewBoxHasBeenSignaled = %i\n", _i8MessageType, ControlerBoxes[_i].isNewBoxHasBeenSignaled);
        Serial.printf("myWSReceiver::_lookForDOMMissingRows(): _i8MessageType = %i, this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n", _i8MessageType);
      }
    } // if
  } // for
}






// void myWSReceiver::_requestBoxChange(JsonDocument& doc, const char& _cChangeKey, void (*_meshViewCallback)(int8_t, int8_t), const int8_t _i8WebMessagCode) {
//   const int8_t __i8NodeName = doc["lb"];
//   if (MY_DG_WS) {
//     Serial.printf("myWSReceiver::_requestBoxChange(): (from JSON) __i8NodeName = %i \n", __i8NodeName);
//   }
//   const int8_t __i8RequestedChange = doc[_cChangeKey];
//   if (MY_DG_WS) {
//     Serial.printf("myWSReceiver::_requestActiveStateChange(): %c = %i \n", _cChangeKey, __i8RequestedChange);
//   }
//   // instantiate a mesh view
//   myMeshViews __myMeshViews;
//   if (MY_DG_WS) {
//     Serial.printf("myWSReceiver::_requestActiveStateChange(): about to call __myMeshViews.changeBoxTargetState().\n");
//   }
//   // __myMeshViews.changeBoxTargetState(__i8BoxState, (short)(__i8NodeName + bControllerBoxPrefix));
//   // __myMeshViews.changeMasterBoxMsg(__i8MasterBox, __i8NodeName);
//   // __myMeshViews.changeBoxDefaultState(__i8BoxDefaultState, (short)(__i8NodeName + bControllerBoxPrefix));
//   __myMeshViews._meshViewCallback(__i8RequestedChange, (short)(__i8NodeName + bControllerBoxPrefix));
//
//   // send a response telling the instruction is in course of being executed
//   StaticJsonDocument<64> _sub_doc;
//   JsonObject _sub_obj = _sub_doc.to<JsonObject>();
//   _sub_obj["lb"] = __i8NodeName;
//   _sub_obj[&_cChangeKey] = __i8RequestedChange;
//
//   myWSSender _myWSSender;
//   _myWSSender.prepareWSData(_i8WebMessagCode, _sub_obj);
// }





void myWSReceiver::_requestActiveStateChange(JsonDocument& doc) {
  const int8_t __i8NodeName = doc["lb"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestActiveStateChange(): (from JSON) __i8NodeName = %i \n", __i8NodeName);
  }
  const int8_t __i8BoxState = doc["boxState"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestActiveStateChange(): _boxState = %i \n", __i8BoxState);
  }
  // instantiate a mesh view
  myMeshViews __myMeshViews;
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestActiveStateChange(): about to call __myMeshViews.changeBoxTargetState().\n");
  }
  __myMeshViews.changeBoxTargetState(__i8BoxState, (short)(__i8NodeName + bControllerBoxPrefix));

  // send a response telling the instruction is in course of being executed
  StaticJsonDocument<64> _sub_doc;
  JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  _sub_obj["lb"] = __i8NodeName;
  _sub_obj["boxState"] = __i8BoxState;

  myWSSender _myWSSender;
  _myWSSender.prepareWSData(4, _sub_obj);
}





// we received a master change request from the Web.
// 1. sending it to the mesh
// 2. sending reply to the browser (request being processed)
void myWSReceiver::_requestMasterChange(const int8_t _i8MessageType, JsonDocument& doc) {
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestMasterChange(): _i8MessageType = %i - starting \n", _i8MessageType);
  }

  const int8_t __i8NodeName = doc["lb"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestMasterChange(): (from JSON + 200) __i8NodeName = %i \n", (__i8NodeName + bControllerBoxPrefix));
  }
  // get the masterbox number
  const int8_t __i8MasterBox = doc["masterbox"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestMasterChange(): _boxState = %i \n", __i8MasterBox);
  }

  // instantiate a mesh view and send a changeMasterBoxMsg
  myMeshViews __myMeshViews;
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestMasterChange(): about to call __myMeshViews.changeMasterBox().\n");
  }
  __myMeshViews.changeMasterBoxMsg(__i8MasterBox, __i8NodeName);

  // send a response to the browser telling the instruction is in course of being executed
  StaticJsonDocument<64> _sub_doc;
  JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  _sub_obj["lb"] = __i8NodeName;
  _sub_obj["ms"] = __i8MasterBox;
  _sub_obj["st"] = 1; // "st" for status, 1 for sent to laser controller; waiting execution

  // send an update to the browser
  myWSSender _myWSSender;
  _myWSSender.prepareWSData(8, _sub_obj);
}





void myWSReceiver::_requestDefaultStateChange(JsonDocument& doc) {
  const int8_t __i8NodeName = doc["lb"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestDefaultStateChange(): (from JSON) __i8NodeName = %i \n", __i8NodeName);
  }
  const int8_t __i8BoxDefaultState = doc["boxDefstate"];
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestDefaultStateChange(): __i8BoxDefaultState = %i \n", __i8BoxDefaultState);
  }
  // instantiate a mesh view
  myMeshViews __myMeshViews;
  if (MY_DG_WS) {
    Serial.printf("myWSReceiver::_requestDefaultStateChange(): about to call __myMeshViews.changeBoxTargetState().\n");
  }
  __myMeshViews.changeBoxDefaultState(__i8BoxDefaultState, (short)(__i8NodeName + bControllerBoxPrefix));

  // send a response telling the instruction is in course of being executed
  StaticJsonDocument<64> _sub_doc;
  JsonObject _sub_obj = _sub_doc.to<JsonObject>();
  _sub_obj["lb"] = __i8NodeName;
  _sub_obj["boxDefstate"] = __i8BoxDefaultState;

  myWSSender _myWSSender;
  _myWSSender.prepareWSData(9, _sub_obj);
}
