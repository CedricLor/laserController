/*
  global.cpp - Library for storing global variables.
  Created by Cedric Lor, January 2, 2007.
*/

#include "Arduino.h"
#include "global.h"

unsigned long const DEFAULT_PIN_BLINKING_INTERVAL = 10000UL;
const short PIN_COUNT = 8;
short relayPins[] = { 22, 21, 19, 18, 5, 17, 16, 4 };

const short I_DEFAULT_MASTER_NODE_NAME = 211;
const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;
const short I_NODE_NAME = 201;                                                                                                // BOX BY BOX

const short BOXES_COUNT = 10;                                                                                                 // NETWORK BY NETWORK
const bool MESH_ROOT = true;                                                                                                // BOX BY BOX

const short I_MASTER_NODE_PREFIX = 200;

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;
short iMasterNodeName = I_DEFAULT_MASTER_NODE_NAME;
unsigned long pinBlinkingInterval = DEFAULT_PIN_BLINKING_INTERVAL;

const char* slaveReactionHtml[4] = {"syn", "opp", "aon", "aof"};
