
#ifndef globalBasementVars_h
#include <painlessMesh.h>
#define globalBasementVars_h

class globalBasementVars
{
  public:
    globalBasementVars();

    /**members*/
    Scheduler          scheduler;
  
    uint16_t           relayPins[]; 

    const bool         MY_DEBUG;
    const bool         MY_DG_LASER;
    const bool         MY_DG_WS;
    const bool         MY_DG_WEB;
    const bool         MY_DG_MESH;
    const bool         MY_DEEP_DG_MESH;

    const short        VERSION;

    // BOX BY BOX
    uint8_t            gui8DefaultMasterNodeName;

    uint16_t           gui16NodeName;
    bool               isInterface;
    bool               isRoot;
    uint32_t           ui32RootNodeId;

    uint16_t           gui16ControllerBoxPrefix;

    uint16_t           gui16InterfaceNodeName;

    uint16_t           gui16BoxesCount;

    int16_t            gi16BoxDefaultState;

    int8_t             gi8OTAReboot;
    int8_t             gi8RequestedOTAReboots;
    int8_t             gi8OTA1Success;
    int8_t             gi8OTA2Success;

    char               gcHostnamePrefix[];

    uint16_t           uiMeshSize;

    // painlessMesh       laserControllerMesh;

  private:

};

extern globalBasementVars globalVariables;

#endif
