/*
  myWebServerViews.h - Library to serve the views of the laser controller web server.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myWebServerViews_h
#define myWebServerViews_h

#include "Arduino.h"

class myWebServerViews
{
  public:
    myWebServerViews();
    String returnTheResponse();

  private:
    const char* _slaveReaction[4] = {"synchronous: on - on & off - off", "opposed: on - off & off - on", "always on: off - on & on - on", "always off: on - off & off - off"};
    const char* _pairingType[3] = {"unpaired", "twin", "cooperative"};

    String printLinksToBoxes();
    String printAllLasersCntrl();
    String printGlobalPinPairingWebCntrl();
    String printGlobalPinPairingSelect();
    String printMasterCntrl();
    String printMasterSelect();
    String printSlaveReactionSelect();
    String printLabel(const String labelText, const String labelFor);
    String printOption(const String optionValue, const String optionText, const String selected);
    String printIndivLaserCntrls();
    String printCurrentStatus(const short thisPin);
    String printOnOffControl(const short thisPin);
    String printPirStatusCntrl(const short thisPin);
    String printBlinkingIntervalWebCntrl(const short thisPin);
    String printPairingCntrl(const short thisPin);
    String printIntervalSelect(const short thisPin);
    String printHiddenLaserNumb(const short thisPin);
};

#endif
