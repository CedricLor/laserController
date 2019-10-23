/*
  controllerBoxThis.h - controllerBoxThis integrates signal (controllerBoxesCollection + boxState) with
  pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#include "Arduino.h"
#include "controllerBoxThis.h"

controllerBoxThis::controllerBoxThis():
  globBaseVars(),
  thisSignalHandler(),
  thisCtrlerBox(thisSignalHandler.ctlBxColl.controllerBoxesArray.at(0)),
  thisPirController()
{}