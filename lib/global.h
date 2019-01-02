/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2007.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"

extern const short I_DEFAULT_MASTER_NODE_NAME;                                                                                 // BOX BY BOX
extern const short I_DEFAULT_SLAVE_ON_OFF_REACTION;
extern unsigned long const DEFAULT_PIN_BLINKING_INTERVAL;

extern short iSlaveOnOffReaction;
extern short iMasterNodeName;
extern unsigned long pinBlinkingInterval;

#endif
