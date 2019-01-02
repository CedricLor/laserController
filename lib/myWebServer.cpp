/*
  myWebServer.cpp - Library to handle laser controller web server.
  Created by Cedric Lor, January 2, 2019.
  Released into the public domain.
*/

#include "Arduino.h"
#include "myWebServer.h"
#include "LaserPin.h"

myWebServer::myWebServer(LaserPin pin, unsigned long pinBlinkingInterval)
{
  _pinBlinkingInterval = pinBlinkingInterval;
  _pin = pin;
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

String myWebServer::printDelaySelect(const short thisPin) {
  String delaySelect;
  delaySelect += "<select name=\"blinkingDelay\">";
  for (unsigned long delayValue = 5000UL; delayValue < 35000UL; delayValue = delayValue + 5000UL) {
    delaySelect += "<option value=\"";
    delaySelect += delayValue;
    delaySelect += "\"";
    if (thisPin < 9) {
      if (delayValue == _pin[thisPin].blinking_interval) {
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
