/*
  myMesh.h - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.
*/

#ifndef myMesh_h
#define myMesh_h

#include "Arduino.h"
#include "ControlerBox.h"

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

    static void receivedCallback( uint32_t from, String &msg);
    static void newConnectionCallback(uint32_t nodeId);
    static void changedConnectionCallback();
    static void nodeTimeAdjustedCallback(int32_t offset);
    static void delayReceivedCallback(uint32_t from, int32_t delay);

    static void _decodeRequest(uint32_t senderNodeId, String &msg);

    static char* _apSsidBuilder(char _apSsidBuf[8]);

    static short _jsonToInt(JsonObject& root, String rootKey);

};

#endif
