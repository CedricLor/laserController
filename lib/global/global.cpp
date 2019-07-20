/*
  global.cpp - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

/*
 * 2752557361, 10.107.105.1 = box 201, master 201, StationIP = 10.177.49.2
 * 2752932713, 10.177.49.1 = box 202, master 201
 * 2752577349, 10.255.69.1 = box 204, master 201
 */

#include "Arduino.h"
#include "global.h"

const short PIN_COUNT = 4;
short relayPins[] = { 5, 17, 16, 4 };
// const short PIN_COUNT = 8;
// short relayPins[] = { 22, 21, 19, 18, 5, 17, 16, 4 };

const bool MY_DEBUG = true;
const bool MY_DG_LASER = false;
const bool MY_DG_WS = true;
const bool MY_DG_WEB = true;
const bool MY_DG_MESH = true;

const short VERSION = 0;

// UI8_DEFAULT_MASTER_NODE_NAME is now common to all the ControlerBoxes().
// They are initiated with default master at 254.
// 254 shall mean no master.
// Otherwise, set to which ever value.
// !! infinite loop potential:
// !! do not set a box to be its own master!!
// UI8_DEFAULT_MASTER_NODE_NAME shall never be equal to UI8_NODE_NAME
// In principle, UI8_DEFAULT_MASTER_NODE_NAME shall not be changed (this is why it is a constant).
// Todo: draft a security
// To reset
const uint8_t UI8_DEFAULT_MASTER_NODE_NAME = 254; // 254 means no one

// CONTROLLER BOX 201
// node id in the current configuration 764691274
// const byte UI8_NODE_NAME = 201;                          // BOX BY BOX
// const bool IS_INTERFACE = false;                        // BOX BY BOX -- false or true

// CONTROLLER BOX 200 - INTERFACE, ROOT NODE AND WEB SERVER
// node id in the current configuration 2760139053
const uint8_t UI8_NODE_NAME = 200;                          // BOX BY BOX
const bool IS_INTERFACE = true;                        // BOX BY BOX -- false or true

// CONTROLLER BOX 202
// const byte UI8_NODE_NAME = 202;                          // BOX BY BOX
// const bool IS_INTERFACE = false;                        // BOX BY BOX -- false or true

uint8_t gui8NodeName = UI8_NODE_NAME;
bool isInterface = IS_INTERFACE;

const uint8_t UI8_DEFAULT_INTERFACE_NODE_NAME = 200;
const uint32_t I_DEFAULT_INTERFACE_NODE_ID = 2760139053;
const short MESH_REQUEST_CAPACITY = 20;            // Used for sizing of my custom JSONDocuments received and sent via the mesh and is used in MeshViews and MeshController

const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;

const uint8_t UI8_MASTER_NODE_PREFIX = 200;                 // Used in MaserSlaveBox class and myWebServerViews to set the name of a new masterBox from website informations (where the boxes are numbered from 1 to 10)
const uint8_t UI8_CONTROLLER_BOX_PREFIX = 200;              // Used to calculate the index of this box in the ControlerBoxes array
uint8_t gui8ControllerBoxPrefix = UI8_CONTROLLER_BOX_PREFIX;
uint8_t gui8MyIndexInCBArray = gui8NodeName - gui8ControllerBoxPrefix;

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;

uint32_t gui32InterfaceNodeId = I_DEFAULT_INTERFACE_NODE_ID;
uint8_t gui8InterfaceNodeName = UI8_DEFAULT_INTERFACE_NODE_NAME;
uint8_t gui8InterfaceIndexInCBArray = gui8InterfaceNodeName - gui8ControllerBoxPrefix;

const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};

const uint8_t UI8_BOXES_COUNT = 10;
uint8_t gui8BoxesCount = UI8_BOXES_COUNT;                           // NETWORK BY NETWORK

const short S_BOX_DEFAULT_STATE = 5;

int8_t i8OTAReboot = 0;
int8_t i8RequestedOTAReboots = 0;
