

#include <globalBasementVars.h>

globalBasementVars                globalBaseVariables;

globalBasementVars::globalBasementVars():
  scheduler{},
  relayPins({5, 17, 16, 4}),
  gui8DefaultMasterNodeName(254),
  MY_DEBUG{true},
  MY_DG_LASER{false},
  MY_DG_WS{true},
  MY_DG_WEB{true},
  MY_DG_MESH{true},
  MY_DEEP_DG_MESH{false},
  VERSION{2}
{  }

