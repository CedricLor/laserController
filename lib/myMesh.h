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

class myMesh
{
  public:
    myMesh();

    static void meshSetup();

  private:

    static const char* _ap_ssid;
    static const char* _ap_password;

    static void receivedCallback( uint32_t from, String &msg);

    static void _tcbSendStatusOnNewConnection();
    static bool _oetcbSendStatusOnNewConnection();
    static void newConnectionCallback(uint32_t nodeId);

    static void _tcbSendNotifOnDroppedConnection(uint16_t _ui16droppedIndex);
    static bool _oetcbSendNotifOnDroppedConnection(uint32_t nodeId);
    static void droppedConnectionCallback(uint32_t nodeId);

    static Task _tChangedConnection;
    static void _odtcbChangedConnection();
    static void changedConnectionCallback();
    static bool IamAlone();

    static void nodeTimeAdjustedCallback(int32_t offset);
    static void delayReceivedCallback(uint32_t from, int32_t delay);

    static void _updateNodeListSizeAndPrintNodeList();
    static void _updateNodeListSize();
    static void _printNodeListAndTopology();

    static void _decodeRequest(uint32_t _ui32SenderNodeId, String &_msg);

    static void _updateConnectedBoxCount();
};

#endif
