
#ifndef globalBasementVars_h
#include <painlessMesh.h>
#define globalBasementVars_h

class globalBasementVars
{
  public:
    globalBasementVars();


    /** I. In all the configurations */
    Scheduler               scheduler;
    const uint8_t           UI8_NODE_NAME; //   = 201;                          // BOX BY BOX
    const bool              IS_INTERFACE;  //   = false;                        // BOX BY BOX
    const bool              IS_ROOT;       //   = false;                        // BOX BY BOX
    uint16_t                gui16NodeName;
    bool                    isInterface;
    bool                    isRoot;

    const uint8_t           UI8_CONTROLLER_BOX_PREFIX; // 200
    uint16_t                gui16ControllerBoxPrefix;
    const uint8_t           UI8_DEFAULT_INTERFACE_NODE_NAME; // 200
    uint16_t                gui16InterfaceNodeName;

    uint16_t                gui16BoxesCount;

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
     *  This variable is actually here to provide the root node id to the web
     *  browser (in myWSSender). However, the process for setting it is located 
     *  in myMeshStarter and myMesh. Accordingly, I leave in the general section.
     *  */
    uint32_t                ui32RootNodeId;

    int16_t                 gi16BoxDefaultState;

    int8_t                  gi8OTAReboot;
    int8_t                  gi8RequestedOTAReboots;
    int8_t                  gi8OTA1Success;
    int8_t                  gi8OTA2Success;

    char                    gcHostnamePrefix[];

    uint16_t                uiMeshSize;

    // painlessMesh           laserControllerMesh;

    /** I. For laserBoxes only (hasLasers): */
    /** -> relayPins (hasLasers): 
     * 
     *  defines the pins connnected to the lasers
     * uint16_t relayPins[]  = { 5, 17, 16, 4 }; // SS, ??, EXTERNAL_NUM_INTERRUPTS || NUM_ANALOG_INPUTS, A10
     * uint16_t relayPins[]  = { 22, 21, 19, 18, 5, 17, 16, 4 }; SCL, SDA, MISO, SCK
     * */  
    std::array<uint16_t, 4> relayPins;

    /** gui8DefaultMasterNodeName (hasLasers):
     * 
     * All the ControlerBoxes() are initialized with default master at 254.
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


    /** For debug purposes only */
    const bool         MY_DEBUG;
    const bool         MY_DG_LASER;
    const bool         MY_DG_WS;
    const bool         MY_DG_WEB;
    const bool         MY_DG_MESH;
    const bool         MY_DEEP_DG_MESH;

    /** For software development purposes only */
    const short        VERSION;

  private:

};

extern globalBasementVars globalBaseVariables;

#endif
