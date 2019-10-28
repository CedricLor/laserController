/*
  controllerBoxThis.h - controllerBoxThis integrates signal (controllerBoxesCollection + boxState) with
  pirController
  Created by Cedric Lor, Octobre 23, 2019.
*/

#ifndef controllerBoxThis_h

#include "Arduino.h"
#include <signal.h>
#include <pirController.h>

#define controllerBoxThis_h

class controllerBoxThis
{
  public:
    /** default empty constructor */
    controllerBoxThis();

    /** members */
    globalBasementVars &        globBaseVars;
    ControlerBox                thisCtrlerBox;
    myMeshViews                 thisMeshViews;
    signal                      thisSignalHandler;
    pirController               thisPirController;

    /** void updateThisBoxProperties() */
    void updateThisBoxProperties();

    void addLaserTasks();

    /** Reboot Task */
    Task tReboot;

    /** IR related Tasks */
    Task tSetPirTimeStampAndBrdcstMsg;
    Task tPirSpeedBumper;

  private:
    void _reboot();
};

extern controllerBoxThis             thisControllerBox;;

#endif
