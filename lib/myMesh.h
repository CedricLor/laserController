/*
  myMesh.h - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.

  |--main.cpp
  |  |
  |  |--myMesh.cpp
  |  |  |--myMesh.h
  |  |  |
  |  |  |--myMeshControler.cpp (private - called only from my Mesh)
  |  |  |  |--myMeshControler.h
  |  |  |  |--MasterSlaveBox.cpp
  |  |  |  |  |--MasterSlaveBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
  |  |  |
  |  |  |--myMeshViews.cpp
  |  |  |  |--myMeshViews.h
  |  |  |  |--ControlerBox.cpp
  |  |  |  |  |--ControlerBox.h
  |  |  |  |--//LaserGroupedUnitsArray.cpp
  |  |  |  |  |--//LaserGroupedUnitsArray.h
*/

#ifndef myMesh_h
#define myMesh_h

#include "Arduino.h"
#include <ESPmDNS.h>
#include <painlessMesh.h>
#include <global.h>
#include <secret.h>
#include <../ControlerBox.h>
#include "./myMesh/myMeshViews/myMeshViews.cpp"
#include "./myMesh/myMeshController/myMeshController.cpp"

class myMeshStarter
{
  public:
    myMeshStarter();
    
    // Variables for Interface on AP mode
    static const char* _soft_ap_ssid;    
    static const char* _soft_ap_password;

    static IPAddress _soft_ap_my_ip;
    static IPAddress _soft_ap_me_as_gateway_ip;
    static IPAddress _soft_ap_netmask;
    static IPAddress _fixed_ip;
    static IPAddress _fixed_netmask;

    void myMeshSetup();

  private:
    // setup subs
    void _initAndConfigureMesh();
    void _initMesh();
    void _initStationManual();
    void _rootTheMesh();
    void _interfaceOnAPInit();
    void _setMeshCallbacks();
    void _setupMdns();

};

class myMesh
{
  public:
    myMesh();

  private:
    friend class myMeshStarter;
    // painlessMesh callbacks
    static void receivedCallback( uint32_t from, String &msg);

    static void _tcbSendStatusOnNewConnection();  // attaches to _tChangedConnection 
    static void newConnectionCallback(uint32_t nodeId);

    static void _tcbSendNotifOnDroppedConnection(uint32_t nodeId);  // attaches to _tChangedConnection 
    static void droppedConnectionCallback(uint32_t nodeId);

    static Task _tChangedConnection;
    static void _odtcbChangedConnection();
    static void changedConnectionCallback();

    // static void nodeTimeAdjustedCallback(int32_t offset);
    // static void delayReceivedCallback(uint32_t from, int32_t delay);

    // Helpers
    // For receivedCallback (on receive messages from the mesh)
    static Task _tDecodeRequest;
    static void _tcbDecodeRequest(uint32_t _ui32SenderNodeId, String& _msg);
    static Task _tPassRequestToMeshController;
    static void _tcbPassRequestToMeshController(uint32_t _ui32SenderNodeId, JsonObject& _obj);

    // For the bug when the subnodes stop scanning after being disconnected
    static Task _tIamAloneTimeOut;
    static void _tcbIamAloneTimeOut();
    static bool IamAlone();
    static bool _IamAlone;

    // For debug purposes
    static Task _tPrintMeshTopo;
    static void _printNodeListAndTopology();

    // To update the ControlerBoxes array
    static Task _tUpdateCDOnChangedConnections;
    static bool _oetcbUpdateCBOnChangedConnections();
    static void _tcbUpdateCBOnChangedConnections();
    static void _odtcbUpdateCBOnChangedConnections();
    static void _deleteBoxesFromCBArray(std::map<uint32_t, uint16_t> &_nodeMap);
    static uint16_t _deleteBoxFromCBArray(uint32_t nodeId);

    // To keep track of the mesh layout
    static Task _tSaveNodeMap;
    static void _tcbSaveNodeMap();
    static std::map<uint32_t, uint16_t> _nodeMap;
    static void _saveNodeMap();
};

#endif


