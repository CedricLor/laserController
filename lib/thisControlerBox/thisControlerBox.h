/*
  ControlerBox.h - Library to replace box_type struct - handles the ControlerBox attributes.
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef thisControlerBox_h
#define thisControlerBox_h

#include "Arduino.h"
#include <mns.h>
#include <global.h>
#include <signal.h>

class thisControlerBox
{
  public:
    // default constructor
    thisControlerBox();

    // boxState Setters Tasks
    Task tSetBoxState;

    /** -------- Public Variables */
    /** - Signal catchers ----------------- */
    int16_t i16boxStateRequestedFromWeb;
    /** - Signal instance ----------------- */
    signal signalHandlers;
    ControlerBox & thisBox;

    /** setters */
    void setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb);

  private:
    /**reset Task tSetBoxState*/
    void resetTSetBoxState();
};

extern thisControlerBox thisCntrlerBox;

#endif
