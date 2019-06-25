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
#include "myMeshController.cpp"

class myMesh
{
  public:
    myMesh();

    static void meshSetup();

  private:
    static const char _PREFIX_AP_SSID[5];
    static char _myApSsidBuf[8];

    static const char* STATION_SSID;
    static const char* STATION_PASSWORD;
    static const uint8_t* STATION_IP;

    static void receivedCallback( uint32_t from, String &msg);
    static void newConnectionCallback(uint32_t nodeId);
    static void changedConnectionCallback();
    static void nodeTimeAdjustedCallback(int32_t offset);
    static void delayReceivedCallback(uint32_t from, int32_t delay);

    static void _decodeRequest(uint32_t senderNodeId, String &msg);

    static char* _apSsidBuilder(char _apSsidBuf[8]);

    // static short _jsonToInt(JsonDocument root, String rootKey);

};

#endif
