/*
  aThatBox.h - aThatBox integrates signal (controllerBoxesCollection + boxState) with
  pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#ifndef aThatBox_h

#include "Arduino.h"
#include <signal.h>
#include <pirController.h>

#define aThatBox_h

class aThatBox
{
  public:
    /** default empty constructor */
    aThatBox();
    /** copy constructor */
    aThatBox(const aThatBox & __aThatBox);
    /** assignement operator */
    aThatBox& operator=(const aThatBox & __aThatBox);
    /** move constructor */
    aThatBox(aThatBox&& __aThatBox);
    /** move assignement operator */
    aThatBox & operator=(aThatBox&& __aThatBox);

    /** members */
    globalBasementVars          globBaseVars;
    controllerBoxesCollection   ctlBxColl;
    signal                      thisSignalHandler;
    ControlerBox &              thisLaserBox;
    pirController               thisPirController;

    /** getters */

  private:

    /** private instance properties */
};

#endif
