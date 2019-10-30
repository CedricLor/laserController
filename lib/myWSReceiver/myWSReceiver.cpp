/*
  myWSReceiver.cpp - Library web socket related functions.
  Created by Cedric Lor, July 9, 2019.
*/

#include "Arduino.h"
#include <myWSReceiver.h>




myWSReceiver::myWSReceiver(uint8_t *_data)
{

  if (globalBaseVariables.MY_DG_WS) {
    Serial.println("myWSReceiver::myWSReceiver. starting");
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
    Serial.printf("myWSReceiver::_actionSwitch. starting\n");
  }

  // if action type 0, handshake -> compare the number of boxRow in DOM vs the number of connected boxes
  // Received JSON: {action:"handshake", boxStateInDOM:{1:4;2:3}}
  if (_obj["action"] == "handshake") {           // 0 for hand shake message
    if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_actionSwitch(): new WS: checking whether the DOM needs update. \n"); }
    myWSReceiverReconcile _myWSReceiverReconcile(
      _obj, 
      thisControllerBox.thisSignalHandler.ctlBxColl.controllerBoxesArray, 
      thisControllerBox.thisSignalHandler.ctlBxColl.ui16connectedBoxesCount, 
      thisControllerBox.globBaseVars.gui16BoxesCount
    );
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

  //  If this is a save or apply message for Wifi, softAP, mesh, globalBaseVariables.isRoot or globalBaseVariables.hasInterface settings
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
  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_requestNetChange(): starting\n"); }

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

  // If this is a save or apply message for Wifi, softAP, mesh, isRoot or hasInterface settings
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

  if (globalBaseVariables.MY_DG_WS) { Serial.printf("myWSReceiver::_requestNetChange(): over\n"); }
}