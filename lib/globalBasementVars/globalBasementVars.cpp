

#include <globalBasementVars.h>

globalBasementVars                globalBaseVariables;

globalBasementVars::globalBasementVars():
  scheduler{},
  UI8_NODE_NAME{201},
  IS_INTERFACE{false},
  IS_ROOT{false},
  relayPins({5, 17, 16, 4}),
  gui8DefaultMasterNodeName(254),
  UI8_CONTROLLER_BOX_PREFIX{200},
  UI8_DEFAULT_INTERFACE_NODE_NAME{200},
  MY_DEBUG{true},
  MY_DG_LASER{false},
  MY_DG_WS{true},
  MY_DG_WEB{true},
  MY_DG_MESH{true},
  MY_DEEP_DG_MESH{false},
  VERSION{2}
{  }

