/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"
#include "ControlerBox.h"
#include "LaserPin.h"


extern unsigned long const DEFAULT_PIN_BLINKING_INTERVAL;
extern const short PIN_COUNT;                                                     // BOX BY BOX
extern short relayPins[];  // an array of pin numbers to which relays are attached// BOX BY BOX

extern const bool MY_DEBUG;

extern const byte B_NODE_NAME;                                                   // BOX BY BOX
extern const short I_DEFAULT_MASTER_NODE_NAME;                                    // BOX BY BOX
extern const bool MESH_ROOT;                                                      // BOX BY BOX

extern const bool IS_INTERFACE;                                                  // BOX BY BOX
extern const bool IS_STATION_MANUAL;                                             // BOX BY BOX

extern const uint8_t MESH_CHANNEL;

extern const short I_DEFAULT_INTERFACE_NODE_NAME;
extern const short MESH_REQUEST_CAPACITY;            // Used for sizing of my custom JSONDocuments received and sent via the mesh and is used in MeshViews and MeshController

extern const short I_DEFAULT_SLAVE_ON_OFF_REACTION;

extern const short BOXES_COUNT;                                                   // NETWORK BY NETWORK

extern const short I_MASTER_NODE_PREFIX;                                          // NETWORK BY NETWORK
extern const short I_CONTROLLER_BOX_PREFIX;
extern const short MY_INDEX_IN_CB_ARRAY;

extern short iSlaveOnOffReaction;
extern short iMasterNodeName;
extern short iInterfaceNodeName;

extern unsigned long pinBlinkingInterval;

extern const char* slaveReactionHtml[4];

extern ControlerBox ControlerBoxes[];
// declare and size an array to contain the LaserPins class instances as a global variable
extern LaserPin LaserPins[];

extern painlessMesh laserControllerMesh;
extern Scheduler    userScheduler;             // to control your personal task

#endif
