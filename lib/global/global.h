/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"
#include <painlessMesh.h>


extern const uint16_t PIN_COUNT;
extern short relayPins[];  // an array of pin numbers to which relays are attached// BOX BY BOX
extern const short BOXES_COUNT;
extern const uint16_t MESH_REQUEST_CAPACITY;


extern  bool MY_DEBUG;
extern  bool MY_DG_LASER;
extern  bool MY_DG_WS;
extern  bool MY_DG_WEB;
extern  bool MY_DG_MESH;

extern const short VERSION;

extern const uint8_t UI8_DEFAULT_MASTER_NODE_NAME;                                    // BOX BY BOX

extern uint16_t gui16NodeName;
extern bool isInterface;

extern uint32_t gui32InterfaceNodeId;
extern uint16_t gui16InterfaceIndexInCBArray;

extern const short I_DEFAULT_SLAVE_ON_OFF_REACTION;

extern uint16_t gui16ControllerBoxPrefix;
extern uint16_t gui16MyIndexInCBArray;

extern short iSlaveOnOffReaction;
extern uint16_t gui16InterfaceNodeName;

extern const char* slaveReactionHtml[4];

extern uint16_t gui16BoxesCount;                           // NETWORK BY NETWORK

extern const short S_BOX_DEFAULT_STATE;

extern int8_t gi8OTAReboot;
extern int8_t gi8RequestedOTAReboots;
extern int8_t gi8OTA1Success;
extern int8_t gi8OTA2Success;

extern char gcHostnamePrefix[];

extern uint16_t uiMeshSize;

extern painlessMesh laserControllerMesh;
extern Scheduler    userScheduler;
#endif
