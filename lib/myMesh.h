/*
  myMesh.h - Library web server controller related functions.
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
    static void broadcastStatusOverMesh(const char* state);


  private:
    static const char PREFIX_AP_SSID[5];
    static char myApSsidBuf[8];
    static char nodeNameBuf[4];

    static void receivedCallback( uint32_t from, String &msg);
    static void newConnectionCallback(uint32_t nodeId);
    static void changedConnectionCallback();
    static void nodeTimeAdjustedCallback(int32_t offset);
    static void delayReceivedCallback(uint32_t from, int32_t delay);

    static void meshController(uint32_t senderNodeId, String &msg);
    static void autoSwitchAllRelaysMeshWrapper(const char* senderStatus, const short iSenderNodeName);
    static String createMeshMessage(const char* myStatus);

    static char* nodeNameBuilder(const short _I_NODE_NAME, char _nodeNameBuf[4]);
    static char* apSsidBuilder(const short _I_NODE_NAME, char _apSsidBuf[8]);

    static void autoSwitchAllRelays(const bool targetState);
    static void autoSwitchOneRelay(const short thisPin, const bool targetState);

    static short jsonToInt(JsonObject& root, String rootKey);

};

#endif
