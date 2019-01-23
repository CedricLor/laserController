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

unsigned long const DEFAULT_PIN_BLINKING_INTERVAL = 10000UL;
const short PIN_COUNT = 8;
short relayPins[] = { 22, 21, 19, 18, 5, 17, 16, 4 };

const short I_DEFAULT_MASTER_NODE_NAME = 211;
const short I_DEFAULT_INTERFACE_NODE_NAME = 001;
const short I_NODE_NAME = 201;                          // BOX BY BOX
const short IS_INTERFACE = true;                        // BOX BY BOX -- false or true

const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;

const short BOXES_COUNT = 10;                           // NETWORK BY NETWORK
const bool MESH_ROOT = true;                            // BOX BY BOX

const short I_MASTER_NODE_PREFIX = 200;                 // Used in MaserSlaveBox class and myWebServerViews to set the name of a new masterBox from website informations (where the boxes are numbered from 1 to 10)

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;
short iMasterNodeName = I_DEFAULT_MASTER_NODE_NAME;
short iInterfaceNodeName = I_DEFAULT_INTERFACE_NODE_NAME;

unsigned long pinBlinkingInterval = DEFAULT_PIN_BLINKING_INTERVAL;

const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};

ControlerBox ControlerBoxes[BOXES_COUNT];
LaserPin LaserPins[PIN_COUNT];

painlessMesh laserControllerMesh;
Scheduler    userScheduler;             // to control your personal task
