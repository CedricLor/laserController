/*
  ControlerBox.h - Library to handle the ControlerBox attributes.
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef ControlerBox_h
#define ControlerBox_h

#include "Arduino.h"
#include <globalBasementVars.h>

class ControlerBox
{
  friend class controllerBoxesCollection;

  public:
    /** -------- constructors -------- */
    ControlerBox();


    /** -------- Public Instance Variables -----------------
     * */

    /** uint32_t nodeId: calculated by painlessMesh from the ESP's mac address */
    uint32_t nodeId;

    /** bool isNewBoxHasBeenSignaled: allows myWSSender to detect new boxes and
     *  inform the user by sending a WS message to the browser.
     * 
     * - read and set by ControlerBox, myMeshViews, myWSReceiver and myWSSender.*/
    bool isNewBoxHasBeenSignaled;

    /** IPAddress stationIP: stores the stationIP (by reading painlessMesh.stationIP or wifi.stationIP).
     * 
     * The stationIP may be set automatically (by an AP to which this box connects)
     * or manually (see myMeshStarter, etc.). */
    IPAddress stationIP;

    /** IPAddress APIP: stores the APIP  (by reading painlessMesh.APIP or wifi.APIP).
     * 
     * The APIP set by painlessMesh, calculated from the ESP's mac address or set manually
     * when the IF is broadcasted on this ESP's AP interface. */
    IPAddress APIP;

    /** uint16_t ui16NodeName:
     * 
     * set by me, in global (201, 202, etc.) */
    uint16_t ui16NodeName;

    /** int16_t i16BoxActiveState:
     * 
     * - set by boxState events, by steps or manually by WS requests.
     * - read by myWSSender, myMeshViews, myWSReceiver and boxState. */
    int16_t i16BoxActiveState;

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

    /** uint32_t ui32BoxActiveStateStartTime: registers the starting time of a new boxState,
     *  by a call to meshNodeTime.
     * 
     * Set via ControlerBox::setBoxActiveState. */
    uint32_t ui32BoxActiveStateStartTime;

    /** uint32_t ui32lastRecPirHighTime records the last time the relevant ControlerBox 
     *  sent a PIR high signal. */
    uint32_t ui32lastRecPirHighTime;

    /** bool boxDeletionHasBeenSignaled: 
     * 
     * - read and set by myWSSender, myWSReceiver (in the consistency check between
     * browser data and ControlerBox database) and ControlerBox::_deleteBox.
     * 
     * Usefull on the IF: allows myWSSender to know of disconnections and reboots
     * of ControlerBoxes and inform the user by sending a WS message to the
     * browser. */
    bool boxDeletionHasBeenSignaled;

    uint16_t ui16MasterBoxName;

    bool bMasterBoxNameChangeHasBeenSignaled;

    short int sBoxDefaultState;
    
    bool sBoxDefaultStateChangeHasBeenSignaled;



    /** -------- Public Instance Methods -----------------
     * */
    void updateThisBoxProperties();
    void printProperties(const uint16_t _ui16BoxIndex);
    void updateMasterBoxNameFromWeb(const uint16_t _ui16MasterBoxName);
    uint16_t getMasterBoxNameForWeb();
    void updateBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex, ControlerBox & __thisBox);
    const bool setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void setBoxDefaultState(const short _sBoxDefaultState);
    void setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime);

  private:
    void _deleteBox();
};

extern ControlerBox & thisBox;

#endif
