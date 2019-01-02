/*
  myWebServer.cpp - Library to handle laser controller web server.
  Created by Cedric Lor, January 2, 2019.
  Released into the public domain.
*/

#include "Arduino.h"
#include "myWebServer.h"

myWebServer::myWebServer(LaserPin *LaserPins, unsigned long pinBlinkingInterval, const short PIN_COUNT, short iSlaveOnOffReaction, short iMasterNodeName, const short I_MASTER_NODE_PREFIX, const short I_NODE_NAME)
{
  _pinBlinkingInterval = pinBlinkingInterval;
  *_LaserPins = *LaserPins;
  _PIN_COUNT = PIN_COUNT;
  _iSlaveOnOffReaction = iSlaveOnOffReaction;

  _iMasterNodeName = iMasterNodeName;
  _I_MASTER_NODE_PREFIX = I_MASTER_NODE_PREFIX;
  _I_NODE_NAME = I_NODE_NAME;
}

String myWebServer::printAllLasersCntrl() {
  String laserCntrls = "<div>All Lasers <a href=\"?manualStatus=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=a\"><button>OFF</button></a>";
  laserCntrls += " IR <a href=\"?statusIr=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?statusIr=of&laser=a\"><button>OFF</button></a>";
  laserCntrls += printBlinkingDelayWebCntrl(9);
  laserCntrls += "</div>";
  laserCntrls += "<div>";
  laserCntrls += printMasterCntrl();
  laserCntrls += "</div>";
  laserCntrls += "<hr>";
  return laserCntrls;
}

String myWebServer::printMasterCntrl() {
  String masterCntrl = "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  masterCntrl += printLabel("Master box: ", "master-select");
  masterCntrl += printMasterSelect();

  masterCntrl += printLabel(" Reaction: ", "reaction-select");
  masterCntrl += printSlaveReactionSelect();

  masterCntrl += "<button type=\"submit\">Submit</button>";
  masterCntrl += "</form>";

  return masterCntrl;
}

String myWebServer::printMasterSelect() {
  String masterSelect = "<select id=\"master-select\" name=\"masterBox\">";
  for (short i = 1; i < 11; i++) {
    String selected = "";
    if (i + _I_MASTER_NODE_PREFIX == _iMasterNodeName) {
      selected += "selected";
    };
    if (!(i + _I_MASTER_NODE_PREFIX == _I_NODE_NAME)) {
      masterSelect += printOption(String(i), String(i), selected);
    }
  }
  masterSelect += "</select>";
  return masterSelect;
}

String myWebServer::printSlaveReactionSelect() {
  String slaveReactionSelect = "<select id=\"reaction-select\" name=\"reactionToMaster\">";
  for (short i = 0; i < 4; i++) {
    String selected = "";
    if (i == _iSlaveOnOffReaction) {
      selected += "selected";
     };
    slaveReactionSelect += printOption(_slaveReactionHtml[i], _slaveReaction[i], selected);
  }
  slaveReactionSelect += "</select>";
  return slaveReactionSelect;
}

String myWebServer::printLabel(const String labelText, const String labelFor) {
  String labelCntrl = "<label for=\"";
  labelCntrl += labelFor;
  labelCntrl += "\">";
  labelCntrl += labelText;
  labelCntrl += "</label>";
  return labelCntrl;
}

String myWebServer::printOption(const String optionValue, const String optionText, const String selected) {
  String optionCntrl = "<option value=\"";
  optionCntrl += optionValue;
  optionCntrl += "\" ";
  optionCntrl += selected;
  optionCntrl += ">";
  optionCntrl += optionText;
  optionCntrl += "</option>";
  return optionCntrl;
}

String myWebServer::printIndivLaserCntrls() {
  String laserCntrl;
  for (short thisPin = 0; thisPin < _PIN_COUNT; thisPin++) {
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

    // blinking delay control
    laserCntrl += printBlinkingDelayWebCntrl(thisPin);

    laserCntrl += "</div>";
  }
  return laserCntrl;
}

String myWebServer::printCurrentStatus(const short thisPin) {
  String currentStatus;
  if (_LaserPins[thisPin].blinking == true) {
    currentStatus += " ON ";
  } else {
    currentStatus += " OFF ";
  }
  if (_LaserPins[thisPin].pir_state == HIGH) {
    currentStatus += " in IR mode ";
  } else {
    currentStatus += " in manual mode ";
  }
  return currentStatus;
}

String myWebServer::printOnOffControl(const short thisPin) {
  String onOffCntrl;
  onOffCntrl += "<a href=\"?manualStatus=on&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>OFF</button></a>";
  return onOffCntrl;
}

String myWebServer::printPirStatusCntrl(const short thisPin) {
  String pirStatusCntrl;
  if (_LaserPins[thisPin].pir_state == LOW) {
    pirStatusCntrl += "<a href=\"?statusIr=on&laser=";
    pirStatusCntrl += thisPin + 1;
    pirStatusCntrl += "\"><button>IR ON</button></a>&nbsp;";
  }
  else {
    pirStatusCntrl += "<a href=\"?statusIr=of&laser=";
    pirStatusCntrl += thisPin + 1;
    pirStatusCntrl += "\"><button>IR OFF</button></a>";
  }
  return pirStatusCntrl;
}

String myWebServer::printBlinkingDelayWebCntrl(const short thisPin) {
  String blinkingDelayWebCntrl;
  blinkingDelayWebCntrl += "Blinking delay: ";
  blinkingDelayWebCntrl += "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  blinkingDelayWebCntrl += printDelaySelect(thisPin);
  blinkingDelayWebCntrl += printHiddenLaserNumb(thisPin);
  blinkingDelayWebCntrl += "<button type=\"submit\">Submit</button>";
  blinkingDelayWebCntrl += "</form>";
  return blinkingDelayWebCntrl;
}

String myWebServer::printPairingCntrl(const short thisPin) {
  String pairingWebCntrl;
  if (_LaserPins[thisPin].paired == 8) {
    pairingWebCntrl += " Unpaired ";
    pairingWebCntrl += "<a href=\"pairingState=pa&laser=";
    pairingWebCntrl += thisPin + 1;
    pairingWebCntrl += "\"><button>PAIR</button></a>&nbsp;";
  } else if (thisPin % 2 == 0) {
    pairingWebCntrl += " Master ";
    pairingWebCntrl += "<a href=\"pairingState=un&laser=";
    pairingWebCntrl += thisPin + 1;
    pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
  } else {
    pairingWebCntrl += " Slave ";
    pairingWebCntrl += "<a href=\"pairingState=un&laser=";
    pairingWebCntrl += thisPin + 1;
    pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
  }
  return pairingWebCntrl;
}

String myWebServer::printDelaySelect(const short thisPin) {
  String delaySelect;
  delaySelect += "<select name=\"blinkingDelay\">";
  for (unsigned long delayValue = 5000UL; delayValue < 35000UL; delayValue = delayValue + 5000UL) {
    delaySelect += "<option value=\"";
    delaySelect += delayValue;
    delaySelect += "\"";
    if (thisPin < 9) {
      if (delayValue == _LaserPins[thisPin].blinking_interval) {
        delaySelect += "selected";
      }
    } else if (delayValue == _pinBlinkingInterval) {
      delaySelect += "selected";
    }
    delaySelect += ">";
    delaySelect += delayValue / 1000;
    delaySelect += " s.</option>";
  }
  delaySelect += "</select>";
  return delaySelect;
}


String myWebServer::printHiddenLaserNumb(const short thisPin)
{
  String hiddenLaserCntrl;
  hiddenLaserCntrl += "<input type=\"hidden\" name=\"laser\" value=\"";
  hiddenLaserCntrl += thisPin + 1;
  hiddenLaserCntrl += "\">";
  return hiddenLaserCntrl;
}
