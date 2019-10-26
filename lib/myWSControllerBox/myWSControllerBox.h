/*
  myWSControllerBox.h - Library wrapping around the ControlerBox class, to keep track of changes
  requests and confirmations exchanged between the ControlerBoxes, the Interface and the Browser.
  Created by Cedric Lor, October 26, 2019.
*/
#ifndef myWSControllerBox_h
#define myWSControllerBox_h

#include "Arduino.h"
#include <globalBasementVars.h>
#include <ControlerBox.h>

class myWSControllerBox
{

  public:
    /** -------- constructors -------- */
    myWSControllerBox(ControlerBox & _controllerBox);


    /** -------- Public Instance Variables -----------------
     * */
    ControlerBox & _controllerBox;

    bool isNewBoxHasBeenSignaled;
    /** bool boxDeletionHasBeenSignaled: 
     * 
     * - read and set by myWSSender, myWSReceiver (in the consistency check between
     * browser data and ControlerBox database) and ControlerBox::_deleteBox.
     * 
     * Usefull on the IF: allows myWSSender to know of disconnections and reboots
     * of ControlerBoxes and inform the user by sending a WS message to the
     * browser. */
    bool boxDeletionHasBeenSignaled;
    /** bool boxActiveStateHasBeenSignaled:
     * 
     * - set by the ControlerBox itself upon a changes of i16BoxActiveState 
     * triggered by the mesh, boxState events, steps or WS requests.
     * 
     * - read by myWSSender, myWSReceiver, myMeshViews and boxState.
     * 
     * Usefull on the IF: allows the myWSSender to know of changes of boxStates
     * of the ControlerBoxes and inform the user by sending a WS message to the
     * browser. */
    bool boxActiveStateHasBeenSignaled;
    /** bool isNewBoxHasBeenSignaled: allows myWSSender to detect new boxes and
     *  inform the user by sending a WS message to the browser.
     * 
     * - read and set by ControlerBox, myMeshViews, myWSReceiver and myWSSender.*/
    bool sBoxDefaultStateChangeHasBeenSignaled;



    /** -------- Public Instance Methods -----------------
     * */
    void printProperties(const uint16_t _ui16BoxIndex);
    void updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex);
    const bool setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void setBoxDefaultState(const short _sBoxDefaultState);
    void setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime);

  private:
    void _deleteBox(const uint16_t _ui16BoxIndex);
};

#endif
