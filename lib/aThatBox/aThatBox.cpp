/*
  aThatBox.h - aThatBox integrates signal (controllerBoxesCollection + boxState) with
  pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#include "Arduino.h"
#include "aThatBox.h"

aThatBox::aThatBox():
  globBaseVars(),
  ctlBxColl(),
  thisSignalHandler(),
  thisLaserBox(ctlBxColl.controllerBoxesArray.at(0)),
  thisPirController()
{}