/*
  myWebServerViews.cpp - Library to serve the views of the laser controller web server.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "myWebServerViews.h"

myWebServerViews::myWebServerViews()
{
}

String myWebServerViews::returnTheResponse() {
  String myResponse = "<!DOCTYPE HTML><html>";
  myResponse += "<body>";
  myResponse += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>";
  myResponse += "<h1>";
  myResponse += String(I_NODE_NAME);
  myResponse += " - ";
  myResponse += (ControlerBoxes[0].APIP).toString();
  myResponse += " - ";
  myResponse += (laserControllerMesh.getStationIP()).toString();
  myResponse += "</h1>";
  myResponse += printAllLasersCntrl();
  myResponse += printIndivLaserCntrls();
  myResponse += printLinksToBoxes();
  myResponse += "</body></html>";
  Serial.println(myResponse);
  return myResponse;
}

String myWebServerViews::printLinksToBoxes() {
  String linksToBoxes = "<div class=\"box_links_wrapper\">";
  // IPAddress testIp(0,0,0,0);
  for (short i = 0; i < BOXES_COUNT; i++) {
    if (ControlerBoxes[i].iNodeName) {
      linksToBoxes += "<div class=\"box_link_wrapper\">Box Number: ";
      linksToBoxes += String((ControlerBoxes[i].iNodeName));
      linksToBoxes += " - Station IP: ";
      linksToBoxes += "<a href=\"http://";
      linksToBoxes += (ControlerBoxes[i].stationIP).toString();
      linksToBoxes +=  "/\">";
      linksToBoxes += (ControlerBoxes[i].stationIP).toString();
      linksToBoxes += " - APIP: </a>";
      linksToBoxes += "<a href=\"http://";
      linksToBoxes += (ControlerBoxes[i].APIP).toString();
      linksToBoxes +=  "/\">Access Point IP: ";
      linksToBoxes += (ControlerBoxes[i].APIP).toString();
      linksToBoxes += "</a>";
      linksToBoxes += "</div>";
    }
  }
  linksToBoxes += "</div>";
  return linksToBoxes;
}

String myWebServerViews::printAllLasersCntrl() {
  String laserCntrls = "<div>All Lasers <a href=\"?manualStatus=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=a\"><button>OFF</button></a>";
  laserCntrls += " IR <a href=\"?statusIr=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?statusIr=of&laser=a\"><button>OFF</button></a>";
  laserCntrls += printBlinkingIntervalWebCntrl(-1);
  laserCntrls += printGlobalPinPairingWebCntrl();
  laserCntrls += "</div>";
  laserCntrls += "<div>";
  laserCntrls += printMasterCntrl();
  laserCntrls += "</div>";
  laserCntrls += "<hr>";
  return laserCntrls;
}

String myWebServerViews::printGlobalPinPairingWebCntrl() {
  String globalPinPairingWebCntrl = "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  globalPinPairingWebCntrl += printLabel("Pairing: ", "pairing-select");
  globalPinPairingWebCntrl += printGlobalPinPairingSelect();

  globalPinPairingWebCntrl += "<button type=\"submit\">Submit</button>";
  globalPinPairingWebCntrl += "</form>";

  return globalPinPairingWebCntrl;
}

String myWebServerViews::printGlobalPinPairingSelect() {
  String globalPinPairingSelect = "<select id=\"pin-pairing-select\" name=\"pinPairing\">";
  for (short i = -1; i < 2; i++) {
    String selected = "";
    // if (/*condition to be defined*/) {
    //   selected += "selected";
    // };
    // if (/*condition to be defined*/) {
      globalPinPairingSelect += printOption(String(i), _pairingType[i + 1], selected);
    // }
  }
  globalPinPairingSelect += "</select>";
  return globalPinPairingSelect;
}

String myWebServerViews::printMasterCntrl() {
  String masterCntrl = "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  masterCntrl += printLabel("Master box: ", "master-select");
  masterCntrl += printMasterSelect();

  masterCntrl += printLabel(" Reaction: ", "reaction-select");
  masterCntrl += printSlaveReactionSelect();

  masterCntrl += "<button type=\"submit\">Submit</button>";
  masterCntrl += "</form>";

  return masterCntrl;
}

String myWebServerViews::printMasterSelect() {
  String masterSelect = "<select id=\"master-select\" name=\"masterBox\">";
  for (short i = 1; i < 11; i++) {
    String selected = "";
    if (i + I_MASTER_NODE_PREFIX == iMasterNodeName) {
      selected += "selected";
    };
    if (!(i + I_MASTER_NODE_PREFIX == I_NODE_NAME)) {
      masterSelect += printOption(String(i), String(i), selected);
    }
  }
  masterSelect += "</select>";
  return masterSelect;
}

String myWebServerViews::printSlaveReactionSelect() {
  String slaveReactionSelect = "<select id=\"reaction-select\" name=\"reactionToMaster\">";
  for (short i = 0; i < 4; i++) {
    String selected = "";
    if (i == iSlaveOnOffReaction) {
      selected += "selected";
     };
    slaveReactionSelect += printOption(slaveReactionHtml[i], _slaveReaction[i], selected);
  }
  slaveReactionSelect += "</select>";
  return slaveReactionSelect;
}

String myWebServerViews::printLabel(const String labelText, const String labelFor) {
  String labelCntrl = "<label for=\"";
  labelCntrl += labelFor;
  labelCntrl += "\">";
  labelCntrl += labelText;
  labelCntrl += "</label>";
  return labelCntrl;
}

String myWebServerViews::printOption(const String optionValue, const String optionText, const String selected) {
  String optionCntrl = "<option value=\"";
  optionCntrl += optionValue;
  optionCntrl += "\" ";
  optionCntrl += selected;
  optionCntrl += ">";
  optionCntrl += optionText;
  optionCntrl += "</option>";
  return optionCntrl;
}

String myWebServerViews::printIndivLaserCntrls() {
  String laserCntrl;
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    laserCntrl += "<div>Laser # ";
    laserCntrl += thisPin + 1;

    // on/off control
    laserCntrl += printCurrentStatus(thisPin);

    // on/off control
    laserCntrl += printOnOffControl(thisPin);

    // PIR status control
    laserCntrl += printPirStatusCntrl(thisPin);

    // pairing control
    laserCntrl += printPairingCntrl(thisPin);

    // blinking interval control
    laserCntrl += printBlinkingIntervalWebCntrl(thisPin);

    laserCntrl += "</div>";
  }
  return laserCntrl;
}

String myWebServerViews::printCurrentStatus(const short thisPin) {
  String currentStatus;
  // if (LaserPins[thisPin].IamBlinking) {
  //   currentStatus += " ON ";
  // } else {
  //   currentStatus += " OFF ";
  // }
  // if (LaserPins[thisPin].pirState() == HIGH) {
  //   currentStatus += " in IR mode ";
  // } else {
  //   currentStatus += " in manual mode ";
  // }
  return currentStatus;
}

String myWebServerViews::printOnOffControl(const short thisPin) {
  String onOffCntrl;
  onOffCntrl += "<a href=\"?manualStatus=on&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>OFF</button></a>";
  return onOffCntrl;
}

String myWebServerViews::printPirStatusCntrl(const short thisPin) {
  String pirStatusCntrl;
  // if (LaserPins[thisPin].pirState() == LOW) {
  //   pirStatusCntrl += "<a href=\"?statusIr=on&laser=";
  //   pirStatusCntrl += thisPin + 1;
  //   pirStatusCntrl += "\"><button>IR ON</button></a>&nbsp;";
  // }
  // else {
  //   pirStatusCntrl += "<a href=\"?statusIr=of&laser=";
  //   pirStatusCntrl += thisPin + 1;
  //   pirStatusCntrl += "\"><button>IR OFF</button></a>";
  // }
  return pirStatusCntrl;
}

String myWebServerViews::printBlinkingIntervalWebCntrl(const short thisPin) {
  String blinkingIntervalWebCntrl;
  blinkingIntervalWebCntrl += "Blinking interval: ";
  blinkingIntervalWebCntrl += "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  blinkingIntervalWebCntrl += printIntervalSelect(thisPin);
  blinkingIntervalWebCntrl += printHiddenLaserNumb(thisPin);
  blinkingIntervalWebCntrl += "<button type=\"submit\">Submit</button>";
  blinkingIntervalWebCntrl += "</form>";
  return blinkingIntervalWebCntrl;
}

String myWebServerViews::printPairingCntrl(const short thisPin) {
  String pairingWebCntrl;
  // if (LaserPins[thisPin].pairedWith() == -1) {
  //   pairingWebCntrl += " Unpaired ";
  //   pairingWebCntrl += "<a href=\"pairingState=pa&laser=";
  //   pairingWebCntrl += thisPin + 1;
  //   pairingWebCntrl += "\"><button>PAIR</button></a>&nbsp;";
  // } else if (thisPin % 2 == 0) {
  //   pairingWebCntrl += " Master ";
  //   pairingWebCntrl += "<a href=\"pairingState=un&laser=";
  //   pairingWebCntrl += thisPin + 1;
  //   pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
  // } else {
  //   pairingWebCntrl += " Slave ";
  //   pairingWebCntrl += "<a href=\"pairingState=un&laser=";
  //   pairingWebCntrl += thisPin + 1;
  //   pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
  // }
  return pairingWebCntrl;
}

String myWebServerViews::printIntervalSelect(const short thisPin) {
  String intervalSelect;
  intervalSelect += "<select name=\"blinkingInterval\">";
  for (unsigned long intervalValue = 5000UL; intervalValue < 35000UL; intervalValue = intervalValue + 5000UL) {
    intervalSelect += "<option value=\"";
    intervalSelect += intervalValue;
    intervalSelect += "\"";
    if (!(thisPin == -1)) {
      /*
          if the blinkingInterval select we are printing is related to a pin
          and not to the global blinkingInterval select,
          the value of thisPin will be different than -1
      */
      // if (intervalValue == LaserPins[thisPin].blinkingInterval()) {
      //   intervalSelect += "selected";
      // }
    } else if (intervalValue == pinBlinkingInterval) {
      intervalSelect += "selected";
    }
    intervalSelect += ">";
    intervalSelect += intervalValue / 1000;
    intervalSelect += " s.</option>";
  }
  intervalSelect += "</select>";
  return intervalSelect;
}


String myWebServerViews::printHiddenLaserNumb(const short thisPin)
{
  String hiddenLaserCntrl;
  hiddenLaserCntrl += "<input type=\"hidden\" name=\"laser\" value=\"";
  hiddenLaserCntrl += thisPin + 1;
  hiddenLaserCntrl += "\">";
  return hiddenLaserCntrl;
}
