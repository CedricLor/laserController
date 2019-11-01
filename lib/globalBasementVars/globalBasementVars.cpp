/*
  globalBasementVars.cpp - Library for storing global variables.
  Created by Cedric Lor, October 20, 2019.
*/


#include <globalBasementVars.h>

globalBasementVars                globalBaseVariables;

globalBasementVars::globalBasementVars():
  scheduler{},

  UI8_NODE_NAME{201},
  HAS_INTERFACE{false},
  IS_ROOT{false},

  // UI8_NODE_NAME{202},
  // HAS_INTERFACE{false},
  // IS_ROOT{false},

  // UI8_NODE_NAME{200},
  // HAS_INTERFACE{true},
  // IS_ROOT{true},
  
  gui16NodeName{UI8_NODE_NAME},
  hasInterface{HAS_INTERFACE},
  isRoot{IS_ROOT},

  gcHostnamePrefix{"LaserBox_"},
  uiMeshSize{0},
  
  UI8_CONTROLLER_BOX_PREFIX{200},
  gui16ControllerBoxPrefix{UI8_CONTROLLER_BOX_PREFIX},
  UI8_DEFAULT_INTERFACE_NODE_NAME{200},
  gui16InterfaceNodeName(UI8_DEFAULT_INTERFACE_NODE_NAME),

  ui32RootNodeId{0},
  gi8OTAReboot{0},
  gi8RequestedOTAReboots{0},

  gui16BoxesCount{10},

  relayPins({5, 17, 16, 4}),
  gui8DefaultMasterNodeName(254),
  gi16BoxDefaultState{5},
  
  MY_DEBUG{true},
  MY_DG_LASER{false},
  MY_DG_WS{true},
  MY_DG_WEB{true},
  MY_DG_MESH{true},
  MY_DEEP_DG_MESH{false},
  VERSION{2},
  laserControllerMesh{}
{  
}

