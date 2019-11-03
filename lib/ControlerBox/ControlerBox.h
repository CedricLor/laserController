/*
  ControlerBox.h - Library to handle the ControlerBox attributes.
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef ControlerBox_h
#define ControlerBox_h

#include "Arduino.h"
#include <globalBasementVars.h>
#include <controllerBoxNetworkInfo.h>

class ControlerBox
{
  friend class myWSControllerBox;
  friend class controllerBoxesCollection;
  friend class controllerBoxThis;
  friend class pirController;
  friend class myMeshController;
  friend class boxStateCollection;

  public:
    /** -------- constructors -------- */
    ControlerBox(uint16_t __ui16NodeName=254);


    /** -------- Public Instance Variables -----------------
     * */
    controllerBoxNetworkInfo networkData;

    /** uint16_t ui16NodeName:
     * 
     * set by me, in global (201, 202, etc.) */
    uint16_t ui16NodeName;

    /** int16_t i16BoxActiveState:
     * - set by boxState events, by steps or manually by WS requests.
     * - read by myWSSender, myMeshViews, myWSReceiver and boxState. */
    int16_t i16BoxActiveState;
    /** uint32_t ui32BoxActiveStateStartTime: registers the starting time of a new boxState,
     *  by a call to meshNodeTime.
     * Set via ControlerBox::setBoxActiveState. */
    uint32_t ui32BoxActiveStateStartTime;
    /** uint32_t ui32lastRecPirHighTime records the last time the relevant ControlerBox 
     *  sent a PIR high signal. */
    uint32_t ui32lastRecPirHighTime;
    short int sBoxDefaultState;

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
    /** bool isNewBoxHasBeenSignaled: allows myWSSender to detect new boxes and
     *  inform the user by sending a WS message to the browser.
     * 
     * - read and set by ControlerBox, myMeshViews, myWSReceiver and myWSSender.*/
    bool sBoxDefaultStateChangeHasBeenSignaled;


  private:
    void _printProperties(const uint16_t _ui16BoxIndex);
    const bool _setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void _setBoxDefaultState(const short _sBoxDefaultState);
    void _setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime);
    void _updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex);
    void _deleteBox(const uint16_t _ui16BoxIndex);
};

#endif
