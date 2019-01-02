/*
  myWebServerViews.h - Library to serve the views of the laser controller web server.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myWebServerViews_h
#define myWebServerViews_h

#include "Arduino.h"
#include "LaserPin.h"
#include "ControlerBox.h"

class myWebServerViews
{
  public:
    myWebServerViews(LaserPin *LaserPins, unsigned long pinBlinkingInterval, const short PIN_COUNT, short iSlaveOnOffReaction, short iMasterNodeName, const short I_MASTER_NODE_PREFIX, const short I_NODE_NAME, ControlerBox *ControlerBoxes, const short BOXES_I_PREFIX, const char** slaveReactionHtml);
    String returnTheResponse();
  private:
    unsigned long _pinBlinkingInterval;
    LaserPin *_LaserPins;
    short _PIN_COUNT;

    short _iSlaveOnOffReaction;
    const char* _slaveReaction[4] = {"synchronous: on - on & off - off", "opposed: on - off & off - on", "always on: off - on & on - on", "always off: on - off & off - off"};
    const char** _slaveReactionHtml;

    short _iMasterNodeName;
    short _I_MASTER_NODE_PREFIX;
    short _I_NODE_NAME;
    short _BOXES_I_PREFIX;

    ControlerBox *_ControlerBoxes;

    String printLinksToBoxes();
    String printAllLasersCntrl();
    String printMasterCntrl();
    String printMasterSelect();
    String printSlaveReactionSelect();
    String printLabel(const String labelText, const String labelFor);
    String printOption(const String optionValue, const String optionText, const String selected);
    String printIndivLaserCntrls();
    String printCurrentStatus(const short thisPin);
    String printOnOffControl(const short thisPin);
    String printPirStatusCntrl(const short thisPin);
    String printBlinkingDelayWebCntrl(const short thisPin);
    String printPairingCntrl(const short thisPin);
    String printDelaySelect(const short thisPin);
    String printHiddenLaserNumb(const short thisPin);
};

#endif
