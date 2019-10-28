/*
  myWSReceiverReconcile.h - Library to handle reconciliation of data relating to 
  the various controller boxes between the data available in the browser and 
  and the data that the IF knows about the state of the controller boxes connected
  to the mesh.

  Created by Cedric Lor, July 9, 2019.
*/

#ifndef myWSReceiverReconcile_h
#define myWSReceiverReconcile_h

#include "Arduino.h"
#include <controllerBoxThis.h>
#include <myWSSender.h>


class myWSReceiverReconcile
{
  public:
    myWSReceiverReconcile(JsonObject& _obj);

  private:

    void _onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj);
    void _checkConsistancyDOMDB(JsonObject& _joBoxState);
    void _lookForDisconnectedBoxes(JsonPair& _p);
    void _checkBoxStateConsistancy(JsonPair& _p);
    void _lookForDOMMissingRows(JsonObject& _joBoxState);

};
#endif

