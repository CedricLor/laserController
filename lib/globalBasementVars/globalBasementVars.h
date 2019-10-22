/*
  globalBasementVars.cpp - Library for storing global variables.
  Created by Cedric Lor, October 20, 2019.
*/

#ifndef globalBasementVars_h
#define globalBasementVars_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <ESPAsyncWebServer.h>

class globalBasementVars
{
  public:
    globalBasementVars();

    /*****************************************************************/
    /** I. In all the configurations: Scheduler and mesh parameters */
    Scheduler               scheduler;
    const uint8_t           UI8_NODE_NAME; //   = 201;                          // BOX BY BOX
    const bool              IS_INTERFACE;  //   = false;                        // BOX BY BOX
    const bool              IS_ROOT;       //   = false;                        // BOX BY BOX
    uint16_t                gui16NodeName;
    bool                    isInterface;
    bool                    isRoot;

    // Used in myOta and myMesh for the mDNS service.
    // Rebooting twice may be usefull if you have to first upload a 
    // sketch and then a SPIFFS file system.
    char                    gcHostnamePrefix[10];
    uint16_t                uiMeshSize;

    const uint8_t           UI8_CONTROLLER_BOX_PREFIX; // 200
    uint16_t                gui16ControllerBoxPrefix;
    const uint8_t           UI8_DEFAULT_INTERFACE_NODE_NAME; // 200
    uint16_t                gui16InterfaceNodeName;

    /*****************************************************************/
    /** II. In all the configurations: to get identification of the root node 
     *      (see the comment below)
    */
    /** uint32_t ui32RootNodeId
     *  
     *  Set to 0 at startup.
     * 
     *  Shall be equal to one of the nodes IDs if the IF is:
     *    (i) not set as root and 
     *    (ii) served on the SoftAP of one of the nodes.
     *  (In the officially recommended painlessmesh configuration, the 
     *  IF is served on AP Interface of the ESP.)
     * 
     *  --> comment here: This variable is actually here to provide the root node id to the web
     *  browser (in myWSSender). However, the process for setting it is located 
     *  in myMeshStarter and myMesh. Accordingly, I leave in the general section.
     *  */
    uint32_t                ui32RootNodeId;

    /**********************************************************************/
    /** III. In all the configurations: for the ControllerBoxes collection*/
    /**
     * Used in mySavedPrefs, to reboot one or two times in OTA mode.
     * Rebooting twice may be usefull if you have to first upload a 
     * sketch and then a SPIFFS file system.
    */
    int8_t                  gi8OTAReboot;
    int8_t                  gi8RequestedOTAReboots;
    int8_t                  gi8OTA1Success;
    int8_t                  gi8OTA2Success;

    /**********************************************************************/
    /** IV. In the configuration: hasLasers*/
    uint16_t                gui16BoxesCount;

    /** -> relayPins: 
     * 
     *  defines the pins connnected to the lasers
     * uint16_t relayPins[]  = { 5, 17, 16, 4 }; // SS, ??, EXTERNAL_NUM_INTERRUPTS || NUM_ANALOG_INPUTS, A10
     * uint16_t relayPins[]  = { 22, 21, 19, 18, 5, 17, 16, 4 }; SCL, SDA, MISO, SCK
     * */  
    std::array<uint16_t, 4> relayPins;

    /**********************************************************************/
    /** V. In the configurations: hasLasers or hasIr */
    /** gui8DefaultMasterNodeName (hasLasers):
     * 
     * All the ControlerBoxes are initialized with default master at 254.
     * 254 shall mean no master. This value may be changed here (at compile time).
     * This value sets the ui16MasterBoxName property of this ControlerBox:
     *
     *     thisBox.ui16MasterBoxName
     *
     * If the NVS has a different value saved under key 
     *
     *     bMasterNName,
     *
     * the saved value will prevail over this one at start up.
     * 
     * !! infinite loop potential: do not set a box to be its own master!!
     * gui8DefaultMasterNodeName shall never be equal to UI8_NODE_NAME
     * Todo: draft a security 
     * */
    uint8_t                 gui8DefaultMasterNodeName;
    int16_t                 gi16BoxDefaultState;


    /** For debug purposes only */
    const bool         MY_DEBUG;
    const bool         MY_DG_LASER;
    const bool         MY_DG_WS;
    const bool         MY_DG_WEB;
    const bool         MY_DG_MESH;
    const bool         MY_DEEP_DG_MESH;

    /** For software development purposes only */
    const short        VERSION;

    /** I. In all the configurations: Scheduler and mesh parameters */
    painlessMesh       laserControllerMesh;
  private:

};

extern globalBasementVars globalBaseVariables;

#endif
