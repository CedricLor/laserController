/*
  ControlerBox.h - Library to replace box_type struct - handles the ControlerBox attributes.
  Created by Cedric Lor, January 2, 2019.
*/
#ifndef ControlerBox_h
#define ControlerBox_h

#include "Arduino.h"
#include <global.h>

class ControlerBox
{
  public:
    /** constructor */
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

    /** bool boxActiveStateHasBeenTakenIntoAccount: used by boxSate to detect
     *  changes in other boxes boxState .
     * 
     * Set via ControlerBox::setBoxActiveState(), ControlerBox::deleteBox() 
     * or boxState class.
     * 
     * Read from boxState.
     * 
     * Set to false when updating another box value (not used by the controlerBoxes
     * for their own states) and to true by boxState, once  the new state of another
     * box (a master box) has been taken into account. */
    bool boxActiveStateHasBeenTakenIntoAccount;

    /** uint32_t ui32BoxActiveStateStartTime: registers the starting time of a new boxState,
     *  by a call to meshNodeTime.
     * 
     * Set via ControlerBox::setBoxActiveState.
     * 
     * TODO: should be set for other boxes from status messages received from from the mesh. 
     * */
    uint32_t ui32BoxActiveStateStartTime;

    /** bool boxDeletionHasBeenSignaled: 
     * 
     * - read and set by myWSSender, myWSReceiver (in the consistency check between
     * browser data and ControlerBox database) and ControlerBox::deleteBox.
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
    void updateMasterBoxName(const uint16_t _ui16MasterBoxName);
    void updateMasterBoxNameFromWeb(const uint16_t _ui16MasterBoxName);
    void updateOtherBoxProperties(uint32_t _ui32SenderNodeId, JsonObject& _obj, uint16_t __ui16BoxIndex);
    void setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void setBoxDefaultState(const short _sBoxDefaultState);
    void deleteBox();





    /** -------- Public Static Variables - Signal catchers -----------------
     * */
    static bool bValFromPir;
    static uint32_t ui32SettingTimeOfValFromPir;
    static short int valFromWeb;

    // -------- Public Static Methods -----------------

    // Class utilities (finders, etc.)
    static uint16_t findIndexByNodeId(uint32_t _ui32nodeId);
    static uint16_t findIndexByNodeName(uint16_t _ui16NodeName);
    static void deleteBoxByNodeId(uint32_t _ui32nodeId);

    // Box counter
    static void updateConnectedBoxCount(short int _newConnectedBoxesCount);
    static short int connectedBoxesCount;
    static short int previousConnectedBoxesCount;

    static Task tReboot;
    static void _reboot();

  private:
};

extern ControlerBox ControlerBoxes[];

#endif
