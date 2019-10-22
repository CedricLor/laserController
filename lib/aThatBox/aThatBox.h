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

class aThatbox
{
  public:
    /** default empty constructor */
    aThatbox();
    /** copy constructor */
    aThatbox(const aThatbox & __aThatbox);
    /** assignement operator */
    aThatbox& operator=(const aThatbox & __aThatbox);
    /** move constructor */
    aThatbox(aThatbox&& __aThatbox);
    /** move assignement operator */
    aThatbox & operator=(aThatbox&& __aThatbox);

    /** members */

    /** getters */

  private:

    /** private instance properties */
};

#endif
