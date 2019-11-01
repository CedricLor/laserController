/*
  controllerBoxThis.h - controllerBoxThis integrates laserSignal 
  (controllerBoxesCollection + boxState) with pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#ifndef controllerBoxThis_h

#include "Arduino.h"
#include <laserSignal.h>
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
    laserSignal                 thisLaserSignalHandler;
    pirController               thisPirController;

    /** properties updaters */
    void updateThisBoxProperties();

    void addLaserTasks();

    /** Reboot Task */
    Task tReboot;

    /** IR related Tasks */
    Task tSetPirTimeStampAndBrdcstMsg;
    Task tPirSpeedBumper;

  private:
    void _reboot();
    void _updateThisBoxNetWorkProperties();
    void _updateThisBoxNodeName();
};

extern controllerBoxThis             thisControllerBox;;

#endif
