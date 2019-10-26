/*
  myWSReceiver.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include <myWSReceiver.h>




myWSReceiver::myWSReceiver(uint8_t *_data)
{

  if (globalBaseVariables.MY_DG_WS) {
    Serial.println("myWSReceiver::myWSReceiver. starting.");
  }

  // create a StaticJsonDocument entitled _doc
  constexpr int _iCapacity = JSON_OBJECT_SIZE(MESH_REQUEST_CAPACITY);
  StaticJsonDocument<_iCapacity> _doc;
  if (globalBaseVariables.MY_DG_WS) {
    Serial.print("myWSReceiver::myWSReceiver(): jsonDocument created\n");
  }
  // Convert the JSON document to a JSON object
  JsonObject _obj = _doc.to<JsonObject>();

  // deserialize the message msg received from the mesh into the StaticJsonDocument _doc
  DeserializationError err = deserializeJson(_doc, _data);
  if (globalBaseVariables.MY_DG_WS) {
    Serial.print("myWSReceiver::myWSReceiver(): message msg deserialized into JsonDocument _doc\n");
    Serial.print("myWSReceiver::myWSReceiver(): DeserializationError = ");Serial.print(err.c_str());Serial.print("\n");
    serializeJson(_doc, Serial);Serial.println();
  }
  
  // choose the type of reaction depending on the message action type
  _actionSwitch(_obj);
}






void myWSReceiver::_actionSwitch(JsonObject& _obj) {
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiver::_actionSwitch. starting.\n");
  }

  // if action type 0, handshake -> compare the number of boxRow in DOM vs the number of connected boxes
  // Received JSON: {action:0, message:{1:4;2:3}}
  if (_obj["action"] == "handshake") {           // 0 for hand shake message
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_actionSwitch(): new WS: checking whether the DOM needs update. \n");
    }
    _onHandshakeCheckWhetherDOMNeedsUpdate(_obj); // _obj = {action:0, message:{1:4;2:3}}
    return;
  }

  // 3 for confirmation that change IP adress has been received
  // disable the task sending the IP by WS to the browser
  if (_obj["action"] == "ReceivedIP") {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.println("myWSReceiver::_actionSwitch(): Ending on ReceivedIP (confirmation that new station IP has been received).");
      Serial.println("myWSReceiver::_actionSwitch(): tSendWSDataIfChangeStationIp.disable()");
    }
    // disable the task sending the station IP
    myWSSender::tSendWSDataIfChangeStationIp.disable();
    return;
  }

  if ((_obj["action"] == "changeBox")  && (_obj["lb"] == 0)) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.println("myWSReceiver::_actionSwitch(): Received a message for the IF.");
    }
    _requestIFChange(_obj);
    return;
  }

  // Former 4 (change boxState) and 9 (change default state)
  // added also key "reboot" and "save"
  if (_obj["action"] == "changeBox") {
    // send a mesh request to the relevant laser box
    // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
    // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9
    // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 0} // reboot without saving
    // _obj = {action: "changeBox"; key: "reboot"; lb: 1, save: 1} // reboot and save
    // _obj = {action: "changeBox"; key: "save"; lb: 1, val: "all"} // save all the values
    _requestBoxChange(_obj);
    return;
  }

  // reboot all or part of the laser boxes
  if (_obj["action"] == "changeNet") {
    // send a mesh request to the existing laser boxes
    // {action: "changeNet", key: "reboot", save: 0, lb: "LBs"}
    // {action: "changeNet", key: "reboot", save: 0, lb: "all"}
    // {action: "changeNet", key: "save", val: "all", lb: "LBs"}
    // {action: "changeNet", key: "save", val: "all", lb: "all"}
    // {"action":"changeNet","key":"save","dataset":{"mssid":"laser_boxes","mpass":"somethingSneaky","mch":"6","mport":"5555","mhi":"0","mmc":"10"},"lb":"all","val":"mesh"}
    // {"action":"changeNet","key":"save","dataset":{"sIP":"192.168.5.1","sssid":"ESP32-Access-Point","spass":"123456789","sgw":"192.168.5.1","snm":"255.255.255.0","shi":"0","smc":"10"},"lb":"all","val":"softAP"}
    _requestNetChange(_obj);
    return;
  }
}





void myWSReceiver::_requestIFChange(JsonObject& _obj) {
  // reboot and/or save the IF
  if (_obj["key"] == "reboot") {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.println("myWSReceiver::_requestIFChange(): This is a REBOOT message.");
    }
    // {action:"changeBox", key:"reboot", save: 0, lb:0} // reboot without saving
    // {action:"changeBox", key:"reboot", save: 1, lb:0} // reboot and save
    _rebootIF(_obj);
    return;
  }

  // save the IF
  if ((_obj["key"] == "save") && (_obj["val"] == "all")) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.println("myWSReceiver::_requestIFChange(): This is a SAVE everything message.");
    }
    // {action:"changeBox", key:"save", val: "all", lb:0} // reboot and save
    _saveIF(_obj);
    return;
  }

  //  If this is a save or apply message for Wifi, softAP, mesh, globalBaseVariables.isRoot or globalBaseVariables.isInterface settings
  if (( (_obj["key"] == "save") || (_obj["key"] == "apply") ) && ((_obj["val"] == "wifi") || (_obj["val"] == "softAP") || (_obj["val"] == "mesh") || (_obj["val"] == "RoSet") || (_obj["val"] == "IFSet") )) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_requestIFChange(): This is a SAVE %s settings message.\n", _obj["val"].as<const char*>());
    }
    // {"action":"changeBox","key":"save","val":"wifi","dataset":{"ssid":"LTVu_dG9ydG9y","pass":"totototo","gatewayIP":"192.168.43.1","ui16GatewayPort":"0","fixedIP":"192.168.43.50","fixedNetmaskIP":"255.255.255.0","ui8WifiChannel":"6"},"lb":0}
    mySavedPrefs::saveFromNetRequest(_obj);

    if (_obj["key"] == "apply") {
      myMeshStarter::tRestart.restartDelayed();
    }
    return;
  }

  // save gi8RequestedOTAReboots for next reboot
  if ((_obj["key"] == "save") && (_obj["val"] == "gi8RequestedOTAReboots")) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.println("myWSReceiver::_requestIFChange(): This is a SAVE gi8RequestedOTAReboots message.");
    }
    // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 0}
    _savegi8RequestedOTAReboots(_obj);
    return;
  }
}




void myWSReceiver::_rebootIF(JsonObject& _obj) {
  // {action:"changeBox", key:"reboot", save: 0, lb:0} // reboot without saving
  // {action:"changeBox", key:"reboot", save: 1, lb:0} // reboot and save
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_rebootIF(): About to reboot.\n"); }

  // inform the browser and the other boxes that IF is going to reboot
  _requestBoxChange(_obj, true /*_bBroadcast*/);

  // reboot with a little delay
  thisControllerBox.tReboot.restartDelayed();
}





void myWSReceiver::_saveIF(JsonObject& _obj) {
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_saveIF(): About to save IF preferences.\n"); }
  // save preferences
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.savePrefsWrapper();
}





void myWSReceiver::_savegi8RequestedOTAReboots(JsonObject& _obj) {
  // {action: "changeBox", key: "save", val: "gi8RequestedOTAReboots", lb: 0}
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_savegi8RequestedOTAReboots(): About to save gi8RequestedOTAReboots in mySavedPrefs on IF.\n"); }
  // save preferences
  mySavedPrefs::saveFromNetRequest(_obj);
  // reboot
  _rebootIF(_obj);
}





void myWSReceiver::_requestBoxChange(JsonObject& _obj, bool _bBroadcast) {
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3} // boxState // ancient 4
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3} // boxDefstate // ancient 9

  _obj["st"] = 1; // add a "st" field, for "execution status" of the request. 1 is "forwarded to the box"; 2 is "executed"

  // broadcast or send the message
  if (_bBroadcast) {
    thisControllerBox.thisMeshViews._sendMsg(_obj);
  } else {
    // get the destination nodeId
    uint32_t _destNodeId = thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_obj["lb"].as<uint8_t>()).networkData.nodeId;
    thisControllerBox.thisMeshViews._sendMsg(_obj, _destNodeId);
  }
  // _obj = {action: "changeBox"; key: "boxState"; lb: 1; val: 3, st: 1} // boxState // ancient 4
  // _obj = {action: "changeBox"; key: "boxDefstate"; lb: 1; val: 3, st: 1} // boxDefstate // ancient 9

  // send a response to the browser telling the instruction is in course of being executed
  myWSSender _myWSSender;
  _myWSSender.sendWSData(_obj);
}





void myWSReceiver::_requestNetChange(JsonObject& _obj) {
  // {"action":"changeNet","key":"save","dataset":{"mssid":"laser_boxes","mpass":"somethingSneaky","mch":"6","mport":"5555","mhi":"0","mmc":"10"},"lb":"all","val":"mesh"}
  // {"action":"changeNet","key":"save","dataset":{"sIP":"192.168.5.1","sssid":"ESP32-Access-Point","spass":"123456789","sgw":"192.168.5.1","snm":"255.255.255.0","shi":"0","smc":"10"},"lb":"all","val":"softAP"}
  // {"action":"changeNet","key":"save","dataset":{"roNNa":"200","IFNNA":"200"},"lb":"all","val":"RoSet"}
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_requestNetChange(): starting.\n"); }

  // If this is a reboot message
  if (_obj["key"] == "reboot") {
    // If reboot "all", IF shall be also rebooted
    if (_obj["lb"] == "all") {
      thisControllerBox.tReboot.enableDelayed();
    }
  }

  // If this is a save message
  if ((_obj["key"] == "save") && (_obj["val"] == "all")) {
    // If _obj["lb"] == "all", it shall also be saved on the IF
    if (_obj["lb"] == "all") {
      _saveIF(_obj);
    }
  }

  // If this is a save or apply message for Wifi, softAP, mesh, isRoot or isInterface settings
  if (((_obj["key"] == "save") || (_obj["key"] == "apply")) && ((_obj["val"] == "wifi") || (_obj["val"] == "softAP") || (_obj["val"] == "mesh") || (_obj["val"] == "RoSet") || (_obj["val"] == "IFSet"))) {
    // If save "all", IF shall be rebooted
    if (_obj["lb"] == "all") {
      mySavedPrefs::saveFromNetRequest(_obj);;
      if (_obj["key"] == "apply") {
        myMeshStarter::tRestart.restartDelayed();
      }
    }
  }

  // change action to "changeBox", as this "changeNet" is being dispatched
  // to each LBs
  _obj["action"] = "changeBox";

  // broadcast the _obj (including its "reboot" or "save" key)
  _requestBoxChange(_obj, true /*_bBroadcast*/);

  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_requestNetChange(): over.\n"); }
}






void myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj /*_obj = {action:0, message:{1:4;2:3}}*/) {

  // Declare and define a JSONObject to read the box numbers and box states from the nested JSON object
  JsonObject __joBoxesStatesInDOM = _obj["boxesStatesInDOM"].as<JsonObject>(); // __joBoxesStatesInDOM = {1:4;2:3}
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): JSON Object _obj available containing the boxState of each boxRow in the DOM \n"); }


  if (__joBoxesStatesInDOM.size() == 0) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): no boxRow in the DOM \n");
      Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount =  %i.\n", thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount);
    }
    // there are no boxRows in the DOM
    if (thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount == 1) {
      // there are no boxes connected to the mesh (and no boxes in the DOM), just return
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending action type \"handshake\", because there are no boxRow in DOM nor connectedBoxes.\n");
      }
      return;
    }
    else // re. if (thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount == 1)
    // there are boxes connected to the mesh (and no boxes in the DOM), look for the missing boxes
    {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): No boxRow in DOM but connectedBoxes.\n");
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Calling _lookForDOMMissingRows().\n");
      }
      _lookForDOMMissingRows(__joBoxesStatesInDOM);
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending after checking missing boxRows.\n");
      }
      return;
    }
  }

  else // re. (__joBoxesStatesInDOM.size() != 0)
  // there are boxRows in DOM
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): There are boxRows in the DOM \n");
    Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount =  %i.\n", thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount);
  }
  {
    if (thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount == 1) {
      // there are no connected boxes (and boxes in the DOM):
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): There are boxRows in DOM but no connectedBoxes.\n");
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): About to instruct to delete all boxRows in DOM.\n");
      }
      // -> send instruction to delete all the boxRows from the DOM
      _obj.clear();
      _obj["action"] = "deleteBox";
      _obj["lb"] = "a";
      myWSSender _myWSSender;
      _myWSSender.sendWSData(_obj);
      // _obj = {action: "deleteBox"; lb: "a"}
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending after instructing to delete all boxRows in DOM.\n");
      }
      return;
    }
    else // re. if (ControlerBox::connectedBoxesCount == 1)
    // there are boxes connected to the mesh (and boxes in the DOM):
    // -> check consistency between the DOM and thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray
    {
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): There are boxRows in DOM and connectedBoxes.\n");
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): About to check consistency.\n");
      }
      _checkConsistancyDOMDB(__joBoxesStatesInDOM);
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_onHandshakeCheckWhetherDOMNeedsUpdate(): Ending after consistency check done.\n");
      }
    } // end else
  } // end if (__joBoxesStatesInDOM.size() != 0)

}





void myWSReceiver::_checkConsistancyDOMDB(JsonObject& _joBoxState) {
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): JSON Object _joBoxState.size: %i. There are currently boxRow(s) in the DOM.\n", _joBoxState.size());
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount =  %i. There are currently boxes connected to the mesh.\n", thisControllerBox.thisSignalHandler.ctlBxColl.connectedBoxesCount);
    Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): about to iterate over the boxRows, looking for the existing boxRow and boxState in DOM\n");
  }

  for (JsonPair _p : _joBoxState) { // for each pair boxIndex:boxState in the DOM,
    // {1:4;2:3;etc.}

    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): new iteration.\n");
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): about to use the key of the current pair of the JSON object to check whether the ControlerBox corresponding to the boxRow in the DOM really exists in thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.\n");
      Serial.printf("myWSReceiver::_checkConsistancyDOMDB(): testing p.key().c_str(): %s\n", _p.key().c_str());
    }

    // DISCONNECTED BOXES CHECKER
    _lookForDisconnectedBoxes(_p);

    // BOXSTATE CHECKER
    _checkBoxStateConsistancy(_p);

  } // end for (JsonPair _p : _joBoxState)

  // MISSING BOXES CHECKER
  // look for missing boxes in the DOM and ask for an update
  _lookForDOMMissingRows(_joBoxState);

  if (globalBaseVariables.MY_DG_WS) {
    Serial.println("myWSReceiver::_checkConsistancyDOMDB. Ending on action type 0 (received handshake message with list of boxRows in DOM).");
  }
  return;
}





void myWSReceiver::_lookForDisconnectedBoxes(JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);

  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): using this value to select a ControlerBox in thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray\n");
    Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0 is %s\n", 
      ((thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0) ? "true" : "false")
    );
  }

  // check if it still is connected; if not, request an update of the DOM
  if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId == 0) {
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): the ControlerBox corresponding to the current boxRow has a networkData.nodeId of: %i. It is no longer connected to the mesh. Delete from the DOM.", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).networkData.nodeId);
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): about to turn thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i).boxDeletionHasBeenSignaled to false.\n", _iBoxIndex);
    }
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxDeletionHasBeenSignaled = false;
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i).boxDeletionHasBeenSignaled turned to %i.\n", _iBoxIndex, thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxDeletionHasBeenSignaled);
      Serial.printf("myWSReceiver::_lookForDisconnectedBoxes(): this shall be caught by the task _tSendWSDataIfChangeBoxState at next pass.\n");
    }
  } // if
}






void myWSReceiver::_checkBoxStateConsistancy(JsonPair& _p) {
  const char* _ccBoxIndex = _p.key().c_str();
  short _iBoxIndex = (short)strtol(_ccBoxIndex, NULL, 10);
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState == %i\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState);
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): (int)(_p.value().as<char*>()) = %i\n.", (int)(_p.value().as<char*>()));
    Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): comparison between the two: %i\n.", (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState == (int)(_p.value().as<char*>())));
  }
  // check if it has the correct boxState; if not, ask for an update
  if (thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).i16BoxActiveState != (int)(_p.value().as<char*>())) {
    if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): the state of the ControlerBox corresponding to the current boxRow is different than its boxState in the DOM. Update it in the DOM.\n");}
    // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
    thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled = false;
    if (globalBaseVariables.MY_DG_WS) {
      Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled = %i.\n", thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_iBoxIndex).boxActiveStateHasBeenSignaled);
      Serial.printf("myWSReceiver::_checkBoxStateConsistancy(): this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n");
    }
  } // end if
}





void myWSReceiver::_lookForDOMMissingRows(JsonObject& _joBoxState) {
  if (globalBaseVariables.MY_DG_WS) {
    Serial.printf("myWSReceiver::_lookForDOMMissingRows(): about to iterate over thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray to look if any is missing from the JSON object containing the boxRows from the DOM.\n");
  }

  // iterate over all the potentially existing laser boxes (note that it starts at 1)
  for (uint16_t _i = 1; _i < globalBaseVariables.gui16BoxesCount; _i++) {
    char _c[3];  // declare an array of char of 3 characters ("   ")
    itoa(_i, _c, 10); // convert the iterator into a char (ex. "1")
    const char* _keyInJson = _joBoxState[_c]; // access the object of box-state by the iterator: _joBoxState["1"]
    if ((thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).networkData.nodeId != 0) && _keyInJson == nullptr) {
      // if the thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i) has a networkData.nodeId and the corresponding _joBoxState[_c] is a nullprt
      // there is a missing box in the DOM
      if (globalBaseVariables.MY_DG_WS) {Serial.printf("myWSReceiver::_lookForDOMMissingRows(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i) is missing box in the DOM. Add it.\n", _i);}
      // this line will trigger in the callback of task _tSendWSDataIfChangeBoxState
      thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).isNewBoxHasBeenSignaled = false;
      if (globalBaseVariables.MY_DG_WS) {
        Serial.printf("myWSReceiver::_lookForDOMMissingRows(): thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(%i).isNewBoxHasBeenSignaled = %i\n", _i, thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray.at(_i).isNewBoxHasBeenSignaled);
        Serial.printf("myWSReceiver::_lookForDOMMissingRows(): this shall be caught by the task  _tSendWSDataIfChangeBoxState at next pass.\n");
      }
    } // if
  } // for
}
