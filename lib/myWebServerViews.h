/*
  myWebServerViews.h - Library to serve the views of the laser controller web server.
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
  |  |  |  |--ControlerBox.cpp (called to set some values, in particular on the other boxes in the mesh)
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--global.cpp (called to retrieve some values re. master/slave box reactions in global)
  |  |  |  |  |--global.h
  |  |  |  |--//LaserPin.cpp

myWebServerViews contains the webPage being displayed by the webServer.

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
    static const char* _slave_Reaction[];
    static const char* _pairing_Type[];

    String _printLinksToBoxes();
    String _printAllLasersCntrl();
    String _printGlobalPinPairingWebCntrl();
    String _printGlobalPinPairingSelect();
    String _printMasterCntrl();
    String _printMasterSelect();
    String _printSlaveReactionSelect();
    String _printLabel(const String labelText, const String labelFor);
    String _printOption(const String optionValue, const String optionText, const String selected);
    String _printIndivLaserCntrls();
    String _printCurrentStatus(const short thisPin);
    String _printOnOffControl(const short thisPin);
    String _printPirStatusCntrl(const short thisPin);
    String _printBlinkingIntervalWebCntrl(const short thisPin);
    String _printPairingCntrl(const short thisPin);
    String _printIntervalSelect(const short thisPin);
    String _printHiddenLaserNumb(const short thisPin);
};

#endif
