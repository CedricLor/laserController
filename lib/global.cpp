/*
  global.cpp - Library for storing global variables.
  Created by Cedric Lor, January 2, 2007.
*/

#include "Arduino.h"
#include "global.h"

const short I_DEFAULT_MASTER_NODE_NAME = 211;
const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;
unsigned long const DEFAULT_PIN_BLINKING_INTERVAL = 10000UL;

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;
short iMasterNodeName = I_DEFAULT_MASTER_NODE_NAME;
unsigned long pinBlinkingInterval = DEFAULT_PIN_BLINKING_INTERVAL;
