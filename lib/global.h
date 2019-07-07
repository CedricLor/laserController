/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"
#include "ControlerBox.h"
#include "LaserPin.h"


extern const short PIN_COUNT;                                                     // BOX BY BOX
extern short relayPins[];  // an array of pin numbers to which relays are attached// BOX BY BOX

extern const bool MY_DEBUG;
extern const bool MY_DG_LASER;

extern const short VERSION;

extern const byte B_DEFAULT_MASTER_NODE_NAME;                                    // BOX BY BOX

extern const uint8_t MESH_CHANNEL;

extern byte gbNodeName;
extern bool isMeshRoot;
extern bool isInterface;

extern const byte B_DEFAULT_INTERFACE_NODE_NAME;
extern int iInterfaceNodeId;

extern const short MESH_REQUEST_CAPACITY;            // Used for sizing of my custom JSONDocuments received and sent via the mesh and is used in MeshViews and MeshController

extern const short I_DEFAULT_SLAVE_ON_OFF_REACTION;

extern const byte B_MASTER_NODE_PREFIX;                                          // NETWORK BY NETWORK
extern const byte B_CONTROLLER_BOX_PREFIX;
extern const byte MY_INDEX_IN_CB_ARRAY;

extern short iSlaveOnOffReaction;
extern byte bInterfaceNodeName;

extern const char* slaveReactionHtml[4];

extern const short BOXES_COUNT;                                                   // NETWORK BY NETWORK
extern ControlerBox ControlerBoxes[];
extern short sBoxesCount;                           // NETWORK BY NETWORK
// declare and size an array to contain the LaserPins class instances as a global variable
extern LaserPin LaserPins[];

extern short sBoxDefaultState;

extern painlessMesh laserControllerMesh;
extern Scheduler    userScheduler;             // to control your personal task

#endif
