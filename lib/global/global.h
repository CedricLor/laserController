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

extern const uint8_t UI8_DEFAULT_MASTER_NODE_NAME;                                    // BOX BY BOX

extern uint8_t gui8NodeName;
extern bool isInterface;

extern const uint8_t UI8_DEFAULT_INTERFACE_NODE_NAME;
extern uint32_t gui32InterfaceNodeId;
extern uint8_t gui8InterfaceIndexInCBArray;

extern const short MESH_REQUEST_CAPACITY;            // Used for sizing of my custom JSONDocuments received and sent via the mesh and is used in MeshViews and MeshController

extern const short I_DEFAULT_SLAVE_ON_OFF_REACTION;

extern const uint8_t UI8_MASTER_NODE_PREFIX;                                          // NETWORK BY NETWORK
extern uint8_t gui8ControllerBoxPrefix;
extern uint8_t gui8MyIndexInCBArray;

extern short iSlaveOnOffReaction;
extern uint8_t gui8InterfaceNodeName;

extern const char* slaveReactionHtml[4];

extern const uint8_t UI8_BOXES_COUNT;                                                   // NETWORK BY NETWORK
extern uint8_t gui8BoxesCount;                           // NETWORK BY NETWORK

extern const short S_BOX_DEFAULT_STATE;

extern bool gbSwitchToOTA;

#endif
