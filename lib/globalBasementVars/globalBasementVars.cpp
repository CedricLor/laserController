

#include <globalBasementVars.h>

globalBasementVars                globalVariables;

globalBasementVars::globalBasementVars():
  scheduler(),
  MY_DEBUG(false),
  MY_DG_LASER(false),
  MY_DG_WS(false),
  MY_DG_WEB(false),
  MY_DG_MESH(false),
  MY_DEEP_DG_MESH(false),
  VERSION(2)
{ }

