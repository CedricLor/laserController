/*
  aThatBox.h - aThatBox integrates signal (controllerBoxesCollection + boxState) with
  pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#include "Arduino.h"
#include "aThatBox.h"

aThatBox::aThatBox():
  globBaseVars(),
  thisSignalHandler(),
  thisCtrlerBox(thisSignalHandler.ctlBxColl.controllerBoxesArray.at(0)),
  thisPirController()
{}