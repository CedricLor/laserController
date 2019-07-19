/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"


extern const short PIN_COUNT;                                                     // BOX BY BOX
extern short relayPins[];  // an array of pin numbers to which relays are attached// BOX BY BOX

extern const bool MY_DEBUG;
extern const bool MY_DG_LASER;

extern const short VERSION;

extern const byte B_DEFAULT_MASTER_NODE_NAME;                                    // BOX BY BOX

extern const uint8_t MESH_CHANNEL;

extern byte gbNodeName;
extern bool isInterface;

extern const byte B_DEFAULT_INTERFACE_NODE_NAME;
extern uint32_t iInterfaceNodeId;
extern byte bInterfaceIndexInCBArray;

extern const short MESH_REQUEST_CAPACITY;            // Used for sizing of my custom JSONDocuments received and sent via the mesh and is used in MeshViews and MeshController

extern const short I_DEFAULT_SLAVE_ON_OFF_REACTION;

extern const byte B_MASTER_NODE_PREFIX;                                          // NETWORK BY NETWORK
extern byte bControllerBoxPrefix;
extern byte myIndexInCBArray;

extern short iSlaveOnOffReaction;
extern byte bInterfaceNodeName;

extern const char* slaveReactionHtml[4];

extern const short BOXES_COUNT;                                                   // NETWORK BY NETWORK
// extern ControlerBox ControlerBoxes[];
extern short sBoxesCount;                           // NETWORK BY NETWORK

extern const short S_BOX_DEFAULT_STATE;

extern painlessMesh laserControllerMesh;
extern Scheduler    userScheduler;             // to control your personal task

#endif
