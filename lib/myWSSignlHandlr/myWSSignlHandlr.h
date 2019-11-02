/*
  myWSSignlHandlr.h - Library to handle the transmission of signals read from the mesh 
  to the browser via the web socket
  Created by Cedric Lor, November 1, 2019.
*/

#ifndef myWSSignlHandlr_h
#define myWSSignlHandlr_h

#include "Arduino.h"
#include <controllerBoxThis.h>
#include <myWSSender.h>

class myWSSignlHandlr
{
  public:
    myWSSignlHandlr(AsyncWebSocket & __asyncWebSocketInstance);

    Task tSendWSData;

  private:
    AsyncWebSocket & _asyncWebSocketInstance;

    void _tcbSendWSData();
};











#endif
