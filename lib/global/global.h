/*
  global.h - Library for storing global variables.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef global_h
#define global_h

#include "Arduino.h"
#include <painlessMesh.h>
#include <ESPAsyncWebServer.h>

// BOX BY BOX
extern int16_t            gi16BoxDefaultState;

extern int8_t             gi8OTAReboot;
extern int8_t             gi8RequestedOTAReboots;
extern int8_t             gi8OTA1Success;
extern int8_t             gi8OTA2Success;

extern char               gcHostnamePrefix[];

extern uint16_t           uiMeshSize;

extern painlessMesh       laserControllerMesh;
#endif





