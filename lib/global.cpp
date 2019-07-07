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

const short VERSION = 0;

// B_DEFAULT_MASTER_NODE_NAME is now common to all the ControlerBoxes().
// They are initiated with default master at 255.
// 255 shall mean no master.
// Otherwise, set to which ever value.
// !! infinite loop potential:
// !! do not set a box to be its own master!!
// B_DEFAULT_MASTER_NODE_NAME shall never be equal to B_NODE_NAME
// In principle, B_DEFAULT_MASTER_NODE_NAME shall not be changed (this is why it is a constant).
// Todo: draft a security
const byte B_DEFAULT_MASTER_NODE_NAME = 255;

// CONTROLLER BOX 201
// node id in the current configuration 764691274
// const byte B_NODE_NAME = 201;                          // BOX BY BOX
// const bool IS_MESH_ROOT = false;                            // BOX BY BOX
// const bool IS_INTERFACE = false;                        // BOX BY BOX -- false or true

// CONTROLLER BOX 200 - INTERFACE, ROOT NODE AND WEB SERVER
// node id in the current configuration 2760139053
const byte B_NODE_NAME = 200;                          // BOX BY BOX
const bool IS_MESH_ROOT = true;                            // BOX BY BOX
const bool IS_INTERFACE = true;                        // BOX BY BOX -- false or true

// CONTROLLER BOX 202
// const byte B_NODE_NAME = 202;                          // BOX BY BOX
// const bool IS_MESH_ROOT = false;                            // BOX BY BOX
// const bool IS_INTERFACE = false;                        // BOX BY BOX -- false or true

const uint8_t MESH_CHANNEL = 6;

const byte B_DEFAULT_INTERFACE_NODE_NAME = 200;
const int I_DEFAULT_INTERFACE_NODE_ID = 2760139053;
const short MESH_REQUEST_CAPACITY = 20;            // Used for sizing of my custom JSONDocuments received and sent via the mesh and is used in MeshViews and MeshController

const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;

const byte B_MASTER_NODE_PREFIX = 200;                 // Used in MaserSlaveBox class and myWebServerViews to set the name of a new masterBox from website informations (where the boxes are numbered from 1 to 10)
const byte B_CONTROLLER_BOX_PREFIX = 200;              // Used to calculate the index of this box in the ControlerBoxes array
const byte MY_INDEX_IN_CB_ARRAY = B_NODE_NAME - B_CONTROLLER_BOX_PREFIX;

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;
int iInterfaceNodeId = I_DEFAULT_INTERFACE_NODE_ID;
byte bInterfaceNodeName = B_DEFAULT_INTERFACE_NODE_NAME;

const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};

const short BOXES_COUNT = 10;
ControlerBox ControlerBoxes[BOXES_COUNT];
short sBoxesCount = BOXES_COUNT;                           // NETWORK BY NETWORK

LaserPin LaserPins[PIN_COUNT];
// LaserGroupedUnit LaserGroupedUnits[PIN_COUNT];
// PIN_COUNT? We define the array of LaserGroupUnits as large as the number of pins connected to the lasers.
// Potentially, the LaserGroupedUnit can composed of a single laser. And in such a case, the LaserGroupedUnitsArray shall be able to contain them all.

const short S_BOX_DEFAULT_STATE = 5;
short sBoxDefaultState = S_BOX_DEFAULT_STATE;

painlessMesh laserControllerMesh;
Scheduler    userScheduler;             // to control your personal task
