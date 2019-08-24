/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <ESPAsyncWebServer.h>

/** an array of pin numbers to which relays are attached 
BOX BY BOX */
extern short              relayPins[]; 

extern const bool         MY_DEBUG;
extern const bool         MY_DG_LASER;
extern const bool         MY_DG_WS;
extern const bool         MY_DG_WEB;
extern const bool         MY_DG_MESH;
extern const bool         MY_DEEP_DG_MESH;

extern const short        VERSION;

// BOX BY BOX
extern uint8_t            gui8DefaultMasterNodeName;

extern uint16_t           gui16NodeName;
extern bool               isInterface;
extern bool               isRoot;
extern uint32_t           ui32RootNodeId;

extern const short        I_DEFAULT_SLAVE_ON_OFF_REACTION;

extern uint16_t           gui16ControllerBoxPrefix;

extern short              iSlaveOnOffReaction;
extern uint16_t           gui16InterfaceNodeName;

extern const char*        slaveReactionHtml[4];

extern uint16_t           gui16BoxesCount;

extern int16_t            gi16BoxDefaultState;

extern int8_t             gi8OTAReboot;
extern int8_t             gi8RequestedOTAReboots;
extern int8_t             gi8OTA1Success;
extern int8_t             gi8OTA2Success;

extern char               gcHostnamePrefix[];

extern uint16_t           uiMeshSize;

extern painlessMesh       laserControllerMesh;
extern Scheduler          userScheduler;
#endif





