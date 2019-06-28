/*
  myWebServerViews.cpp - Library to serve the views of the laser controller web server.
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
  |  |  |  |--boxState.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--boxState.h
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
  |  |  |  |--//LaserPin.cpp

myWebServerViews contains the webPage being displayed by the webServer.

*/

#include "Arduino.h"
#include "myWebServerViews.h"

// const char* myWebServerViews::_slave_Reaction[4] = {"synchronous: on - on & off - off", "opposed: on - off & off - on", "always on: off - on & on - on", "always off: on - off & off - off"};
// const char* myWebServerViews::_pairing_Type[3] = {"unpaired", "twin", "cooperative"};

myWebServerViews::myWebServerViews()
{
  strcpy(cBoxArray, "");
  _loadBoxArray();
}

void myWebServerViews::_loadBoxArray() {
  // Serial.print("myWebServerViews::loadBoxArray(): STARTING\n");
  for (short int __i = 1; __i < BOXES_COUNT; __i++) {
    // Serial.printf("myWebServerViews::loadBoxArray(): BOX LOOP STARTING: iter %i\n", __i);
    strcat(cBoxArray, "<div>Laser Box ");
    char __cNodeName[4];         // the ASCII of the integer will be stored in this char array
    //   Serial.printf("myWebServerViews::loadBoxArray(): BOX LOOP: before itoa %i\n", __i);
    itoa(ControlerBoxes[__i].bNodeName, __cNodeName, 10); //(integer, yourBuffer, base)
    strcat(cBoxArray, __cNodeName);
    strcat(cBoxArray, " - Station IP: ");
    //   Serial.printf("myWebServerViews::loadBoxArray(): BOX LOOP: before stationIp %i\n", __i);
    strcat(cBoxArray, ControlerBoxes[__i].stationIP.toString().c_str());
    strcat(cBoxArray, " - AP IP: ");
    //   Serial.printf("myWebServerViews::loadBoxArray(): BOX LOOP: before APIP %i\n", __i);
    strcat(cBoxArray, ControlerBoxes[__i].APIP.toString().c_str());
    //   Serial.printf("myWebServerViews::loadBoxArray(): BOX LOOP: after APIP %i\n", __i);

    for (short int __j = 0; __j < boxState::BOX_STATES_COUNT; __j++) {
      // Serial.printf("myWebServerViews::loadBoxArray(): BOX STATE LOOP: starting, iter %i\n", __j);
      strcat(cBoxArray, "<a href='?boxTargetState=");
      char __boxStateIndex[2];
      itoa(__j, __boxStateIndex, 10);
      strcat(cBoxArray, __boxStateIndex);
      strcat(cBoxArray, "&lb=");
      strcat(cBoxArray, __cNodeName);
      strcat(cBoxArray, "'><button>");
      strcat(cBoxArray, boxState::boxStates[__j].cName);
      strcat(cBoxArray, "</button></a>&nbsp;");
      // Serial.printf("myWebServerViews::loadBoxArray(): BOX STATE LOOP: ending, iter %i\n", __j);
    }

    strcat(cBoxArray, "</div>"); // Laser Box div
    // Serial.printf("myWebServerViews::loadBoxArray(): LOOP ENDING: iter %i\n", __i);
  }
  // Serial.print("myWebServerViews::loadBoxArray(): ENDING\n");
}


// String myWebServerViews::returnTheResponse() {
//   String __myResponse = "<!DOCTYPE HTML><html>";
//   __myResponse += "<body>";
//   __myResponse += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>";
//   __myResponse += "<h1>";
//   __myResponse += String(B_NODE_NAME); // B_NODE_NAME defined and declared in global
//   __myResponse += " - Station IP: ";
//   __myResponse += (ControlerBoxes[MY_INDEX_IN_CB_ARRAY].stationIP).toString(); // dependancy
//   __myResponse += " - AP IP: ";
//   __myResponse += (ControlerBoxes[MY_INDEX_IN_CB_ARRAY].APIP).toString();  // dependancy
//   __myResponse += "</h1>";
//   __myResponse += _printAllLasersCntrl();
//   __myResponse += _printIndivLaserCntrls();
//   __myResponse += _printLinksToBoxes();
//   __myResponse += "</body></html>";
//   if (MY_DEBUG == true) {Serial.println(__myResponse);}; // MY_DEBUG defined and declared in global
//   return __myResponse;
// }
//
// String myWebServerViews::_printLinksToBoxes() {
//   String __linksToBoxes = "<div class=\"box_links_wrapper\">";
//   // IPAddress testIp(0,0,0,0);
//   for (short i = 0; i < BOXES_COUNT; i++) {
//     if (ControlerBoxes[i].bNodeName) {                          // dependancy
//       __linksToBoxes += "<div class=\"box_link_wrapper\">Box Number: ";
//       __linksToBoxes += String((ControlerBoxes[i].bNodeName));   // dependancy
//       __linksToBoxes += " - Station IP: ";
//       __linksToBoxes += "<a href=\"http://";
//       __linksToBoxes += (ControlerBoxes[i].stationIP).toString();   // dependancy
//       __linksToBoxes +=  "/\">";
//       __linksToBoxes += (ControlerBoxes[i].stationIP).toString();  // dependancy
//       __linksToBoxes += "</a> - Access Point IP: ";
//       __linksToBoxes += "<a href=\"http://";
//       __linksToBoxes += (ControlerBoxes[i].APIP).toString();   // dependancy
//       __linksToBoxes +=  "/\">";
//       __linksToBoxes += (ControlerBoxes[i].APIP).toString();   // dependancy
//       __linksToBoxes += "</a>";
//       __linksToBoxes += "</div>";
//     }
//   }
//   __linksToBoxes += "</div>";
//   return __linksToBoxes;
// }
//
// String myWebServerViews::_printAllLasersCntrl() {
//   String __laserCntrls = "<div>All Lasers <a href=\"?manualStatus=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=a\"><button>OFF</button></a>";
//   __laserCntrls += " IR <a href=\"?statusIr=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?statusIr=of&laser=a\"><button>OFF</button></a>";
//   __laserCntrls += _printBlinkingIntervalWebCntrl(-1);
//   __laserCntrls += _printGlobalPinPairingWebCntrl();
//   __laserCntrls += "</div>";
//   __laserCntrls += "<div>";
//   __laserCntrls += _printMasterCntrl();
//   __laserCntrls += "</div>";
//   __laserCntrls += "<hr>";
//   return __laserCntrls;
// }
//
// String myWebServerViews::_printGlobalPinPairingWebCntrl() {
//   String __globalPinPairingWebCntrl = "<form style=\"display: inline;\" method=\"get\" action=\"\">";
//   __globalPinPairingWebCntrl += _printLabel("Pairing: ", "pairing-select");
//   __globalPinPairingWebCntrl += _printGlobalPinPairingSelect();
//
//   __globalPinPairingWebCntrl += "<button type=\"submit\">Submit</button>";
//   __globalPinPairingWebCntrl += "</form>";
//
//   return __globalPinPairingWebCntrl;
// }
//
// String myWebServerViews::_printGlobalPinPairingSelect() {
//   String __globalPinPairingSelect = "<select id=\"pin-pairing-select\" name=\"pinPairing\">";
//   for (short __i = -1; __i < 2; __i++) {
//     String __selected = "";
//     // if (/*condition to be defined*/) {
//     //   selected += "selected";
//     // };
//     // if (/*condition to be defined*/) {
//       __globalPinPairingSelect += _printOption(String(__i), _pairing_Type[__i + 1], __selected);
//     // }
//   }
//   __globalPinPairingSelect += "</select>";
//   return __globalPinPairingSelect;
// }
//
// String myWebServerViews::_printMasterCntrl() {
//   String __masterCntrl = "<form style=\"display: inline;\" method=\"get\" action=\"\">";
//   __masterCntrl += _printLabel("Master box: ", "master-select");
//   __masterCntrl += _printMasterSelect();
//
//   __masterCntrl += _printLabel(" Reaction: ", "reaction-select");
//   __masterCntrl += _printSlaveReactionSelect();
//
//   __masterCntrl += "<button type=\"submit\">Submit</button>";
//   __masterCntrl += "</form>";
//
//   return __masterCntrl;
// }
//
// String myWebServerViews::_printMasterSelect() {
//   String __masterSelect = "<select id=\"master-select\" name=\"masterBox\">";
//   for (short __i = 1; __i < 11; __i++) {
//     String __selected = "";
//     if (__i + I_MASTER_NODE_PREFIX == bMasterNodeName) { // I_MASTER_NODE_PREFIX and bMasterNodeName defined and declared in global
//       __selected += "selected";
//     };
//     if (!(__i + I_MASTER_NODE_PREFIX == B_NODE_NAME)) { // I_MASTER_NODE_PREFIX and B_NODE_NAME defined and declared in global
//       __masterSelect += _printOption(String(__i), String(__i), __selected);
//     }
//   }
//   __masterSelect += "</select>";
//   return __masterSelect;
// }
//
// String myWebServerViews::_printSlaveReactionSelect() {
//   String __slaveReactionSelect = "<select id=\"reaction-select\" name=\"reactionToMaster\">";
//   for (short __i = 0; __i < 4; __i++) {
//     String __selected = "";
//     if (__i == iSlaveOnOffReaction) { // iSlaveOnOffReaction defined and declared in global
//       __selected += "selected";
//      };
//     __slaveReactionSelect += _printOption(slaveReactionHtml[__i], _slave_Reaction[__i], __selected);
//     // slaveReactionHtml is defined and declared in global
//   }
//   __slaveReactionSelect += "</select>";
//   return __slaveReactionSelect;
// }
//
// String myWebServerViews::_printLabel(const String labelText, const String labelFor) {
//   String __labelCntrl = "<label for=\"";
//   __labelCntrl += labelFor;
//   __labelCntrl += "\">";
//   __labelCntrl += labelText;
//   __labelCntrl += "</label>";
//   return __labelCntrl;
// }
//
// String myWebServerViews::_printOption(const String optionValue, const String optionText, const String selected) {
//   String __optionCntrl = "<option value=\"";
//   __optionCntrl += optionValue;
//   __optionCntrl += "\" ";
//   __optionCntrl += selected;
//   __optionCntrl += ">";
//   __optionCntrl += optionText;
//   __optionCntrl += "</option>";
//   return __optionCntrl;
// }
//
// String myWebServerViews::_printIndivLaserCntrls() {
//   String __laserCntrl;
//   for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) { // PIN_COUNT declared and defined in global
//     __laserCntrl += "<div>Laser # ";
//     __laserCntrl += __thisPin + 1;
//
//     // on/off control
//     __laserCntrl += _printCurrentStatus(__thisPin);
//
//     // on/off control
//     __laserCntrl += _printOnOffControl(__thisPin);
//
//     // PIR status control
//     __laserCntrl += _printPirStatusCntrl(__thisPin);
//
//     // pairing control
//     __laserCntrl += _printPairingCntrl(__thisPin);
//
//     // blinking interval control
//     __laserCntrl += _printBlinkingIntervalWebCntrl(__thisPin);
//
//     __laserCntrl += "</div>";
//   }
//   return __laserCntrl;
// }
//
// String myWebServerViews::_printCurrentStatus(const short thisPin) {
//   String _currentStatus;
//   // if (LaserPins[thisPin].IamBlinking) {
//   //   currentStatus += " ON ";
//   // } else {
//   //   currentStatus += " OFF ";
//   // }
//   // if (LaserPins[thisPin].pirState() == HIGH) {
//   //   currentStatus += " in IR mode ";
//   // } else {
//   //   currentStatus += " in manual mode ";
//   // }
//   return _currentStatus;
// }
//
// String myWebServerViews::_printOnOffControl(const short thisPin) {
//   String __onOffCntrl;
//   __onOffCntrl += "<a href=\"?manualStatus=on&laser=";
//   __onOffCntrl += thisPin + 1;
//   __onOffCntrl += "\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=";
//   __onOffCntrl += thisPin + 1;
//   __onOffCntrl += "\"><button>OFF</button></a>";
//   return __onOffCntrl;
// }
//
// String myWebServerViews::_printPirStatusCntrl(const short thisPin) {
//   String __pirStatusCntrl;
//   // if (LaserPins[thisPin].pirState() == LOW) {
//   //   pirStatusCntrl += "<a href=\"?statusIr=on&laser=";
//   //   pirStatusCntrl += thisPin + 1;
//   //   pirStatusCntrl += "\"><button>IR ON</button></a>&nbsp;";
//   // }
//   // else {
//   //   pirStatusCntrl += "<a href=\"?statusIr=of&laser=";
//   //   pirStatusCntrl += thisPin + 1;
//   //   pirStatusCntrl += "\"><button>IR OFF</button></a>";
//   // }
//   return __pirStatusCntrl;
// }
//
// String myWebServerViews::_printBlinkingIntervalWebCntrl(const short thisPin) {
//   String __blinkingIntervalWebCntrl;
//   __blinkingIntervalWebCntrl += "Blinking interval: ";
//   __blinkingIntervalWebCntrl += "<form style=\"display: inline;\" method=\"get\" action=\"\">";
//   __blinkingIntervalWebCntrl += _printIntervalSelect(thisPin);
//   __blinkingIntervalWebCntrl += _printHiddenLaserNumb(thisPin);
//   __blinkingIntervalWebCntrl += "<button type=\"submit\">Submit</button>";
//   __blinkingIntervalWebCntrl += "</form>";
//   return __blinkingIntervalWebCntrl;
// }
//
// String myWebServerViews::_printPairingCntrl(const short thisPin) {
//   String __pairingWebCntrl;
//   // if (LaserPins[thisPin].pairedWith() == -1) {
//   //   __pairingWebCntrl += " Unpaired ";
//   //   __pairingWebCntrl += "<a href=\"pairingState=pa&laser=";
//   //   __pairingWebCntrl += thisPin + 1;
//   //   __pairingWebCntrl += "\"><button>PAIR</button></a>&nbsp;";
//   // } else if (thisPin % 2 == 0) {
//   //   __pairingWebCntrl += " Master ";
//   //   __pairingWebCntrl += "<a href=\"pairingState=un&laser=";
//   //   __pairingWebCntrl += thisPin + 1;
//   //   __pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
//   // } else {
//   //   __pairingWebCntrl += " Slave ";
//   //   __pairingWebCntrl += "<a href=\"pairingState=un&laser=";
//   //   __pairingWebCntrl += thisPin + 1;
//   //   __pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
//   // }
//   return __pairingWebCntrl;
// }
//
// String myWebServerViews::_printIntervalSelect(const short thisPin) {
//   String __intervalSelect;
//   __intervalSelect += "<select name=\"blinkingInterval\">";
//   for (unsigned long __intervalValue = 5000UL; __intervalValue < 35000UL; __intervalValue = __intervalValue + 5000UL) {
//     __intervalSelect += "<option value=\"";
//     __intervalSelect += __intervalValue;
//     __intervalSelect += "\"";
//     if (!(thisPin == -1)) {
//       /*
//           if the blinkingInterval select we are printing is related to a pin
//           and not to the global blinkingInterval select,
//           the value of thisPin will be different than -1
//       */
//       // if (intervalValue == LaserPins[thisPin].blinkingInterval()) {
//       //   __intervalSelect += "selected";
//       // }
//     } else if (__intervalValue == pinBlinkingInterval) { // declared and defined in global
//       __intervalSelect += "selected";
//     }
//     __intervalSelect += ">";
//     __intervalSelect += __intervalValue / 1000;
//     __intervalSelect += " s.</option>";
//   }
//   __intervalSelect += "</select>";
//   return __intervalSelect;
// }
//
//
// String myWebServerViews::_printHiddenLaserNumb(const short thisPin)
// {
//   String __hiddenLaserCntrl;
//   __hiddenLaserCntrl += "<input type=\"hidden\" name=\"laser\" value=\"";
//   __hiddenLaserCntrl += thisPin + 1;
//   __hiddenLaserCntrl += "\">";
//   return __hiddenLaserCntrl;
// }
