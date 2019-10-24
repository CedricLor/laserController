/*
  controllerBoxThis.h - controllerBoxThis integrates signal (controllerBoxesCollection + boxState) with
  pirController.
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
    /** copy constructor */
    controllerBoxThis(const controllerBoxThis & __controllerBoxThis);
    /** assignement operator */
    controllerBoxThis& operator=(const controllerBoxThis & __controllerBoxThis);
    /** move constructor */
    controllerBoxThis(controllerBoxThis&& __controllerBoxThis);
    /** move assignement operator */
    controllerBoxThis & operator=(controllerBoxThis&& __controllerBoxThis);

    /** members */
    globalBasementVars          globBaseVars;
    signal                      thisSignalHandler;
    ControlerBox &              thisCtrlerBox;
    pirController               thisPirController;

    /** void updateThisBoxProperties()
     * 
     *  TODO: The same-name method in controlerBox needs to be scrapped 
     *  and replaced by this one. */
    void updateThisBoxProperties();

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
