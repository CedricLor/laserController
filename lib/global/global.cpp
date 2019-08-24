/*
  global.cpp - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/


/**********************************************************************
***************************** GLOBAL **********************************
***********************************************************************/

/*
 * 2752557361, 10.107.105.1 = box 201, StationIP = 10.177.49.2
 * 2752932713, 10.177.49.1 = box 202, 
* 2752577349, 10.255.69.1 = box 204, 
*/

#include "Arduino.h"
#include "global.h"

/** short relayPins[PIN_COUNT]: array of pins connected to the lasers
 * in ControlerBoxes */ 
short relayPins[]                             = { 5, 17, 16, 4 };
// short relayPins[]                            = { 22, 21, 19, 18, 5, 17, 16, 4 };


// Debug variables: setting them to true or false enables or disables more
// Serial.prints
const bool MY_DEBUG                           = true;
const bool MY_DG_LASER                        = false;
const bool MY_DG_WS                           = true;
const bool MY_DG_WEB                          = true;
const bool MY_DG_MESH                         = true;
const bool MY_DEEP_DG_MESH                    = false;

const short VERSION                           = 0;


/** gui8DefaultMasterNodeName:
 * 
 * All the ControlerBoxes() are initialized with default master at 254.
 * 254 shall mean no master. This value may be changed here (at compile time).
 * This value sets the ui16MasterBoxName property of this ControlerBox:
 *
 *     thisBox.ui16MasterBoxName
 *
 * If the NVS has a different value saved under key 
 *
 *     bMasterNName,
 *
 * the saved value will prevail over this one at start up.
 * 
 * !! infinite loop potential: do not set a box to be its own master!!
 * gui8DefaultMasterNodeName shall never be equal to UI8_NODE_NAME
 * Todo: draft a security */
uint8_t gui8DefaultMasterNodeName                 = 254; // 254 means no one


/** CONTROLLER BOX 201 
 *  node id in the current configuration 2760608337 */
const uint8_t UI8_NODE_NAME                   = 201;                          // BOX BY BOX
const bool IS_INTERFACE                       = false;                        // BOX BY BOX
const bool IS_ROOT                            = false;                        // BOX BY BOX

/** CONTROLLER BOX 202 
 *  node id in the current configuration 2752898073 */
// const uint8_t UI8_NODE_NAME                = 202;                          // BOX BY BOX
// const bool IS_INTERFACE                    = false;                        // BOX BY BOX
// const bool IS_ROOT                         = false;                        // BOX BY BOX

/** CONTROLLER BOX 200 - INTERFACE, ROOT NODE AND WEB SERVER 
 *  node id in the current configuration 2760139053 */
// const uint8_t UI8_NODE_NAME                = 200;                          // BOX BY BOX
// const bool IS_INTERFACE                    = true;                         // BOX BY BOX
// const bool IS_ROOT                         = true;                         // BOX BY BOX


uint16_t gui16NodeName                        = UI8_NODE_NAME;
bool isInterface                              = IS_INTERFACE;
bool isRoot                                   = IS_ROOT;

const uint8_t UI8_DEFAULT_INTERFACE_NODE_NAME = 200;

/** uint32_t ui32RootNodeId
 *  
 *  Set to 0 at startup.
 * 
 *  Shall be equal to one of the nodes IDs if the IF is (IF is served on AP Interface of the ESP):
 *    (i) not set as root and 
 *    (ii) served on the SoftAP of one of the nodes.
*/
uint32_t ui32RootNodeId                       = 0;

const short I_DEFAULT_SLAVE_ON_OFF_REACTION   = 0;

const uint8_t UI8_CONTROLLER_BOX_PREFIX       = 200;              // Used to calculate the index of this box in the ControlerBoxes array
uint16_t gui16ControllerBoxPrefix             = UI8_CONTROLLER_BOX_PREFIX;

short iSlaveOnOffReaction                     = I_DEFAULT_SLAVE_ON_OFF_REACTION;

uint16_t gui16InterfaceNodeName               = UI8_DEFAULT_INTERFACE_NODE_NAME;

const char* slaveReactionHtml[4]              = {"syn", "opp", "aon", "aof"};

/** UI8_BOXES_COUNT is defined in as a constexpr in global.h */
uint16_t gui16BoxesCount                      = UI8_BOXES_COUNT;

int16_t gi16BoxDefaultState                   = 5;


// Used in mySavedPrefs, to reboot one or two times in OTA mode.
// Rebooting twice may be usefull if you have to first upload a 
// sketch and then a SPIFFS file system.
int8_t gi8OTAReboot                           = 0;
int8_t gi8RequestedOTAReboots                 = 0;




// Used in myOta and myMesh for the mDNS service.
// Rebooting twice may be usefull if you have to first upload a 
// sketch and then a SPIFFS file system.
char gcHostnamePrefix[10]                     = "LaserBox_";

uint16_t uiMeshSize                           = 0;

painlessMesh                                  laserControllerMesh;
Scheduler                                     userScheduler;