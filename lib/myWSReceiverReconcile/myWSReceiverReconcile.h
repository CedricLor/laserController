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
    myWSReceiverReconcile(JsonObject& _obj, controllerBoxesCollection & _ctlBxColl);

  private:

    int16_t _onHandshakeCheckWhetherDOMNeedsUpdate(JsonObject& _obj, controllerBoxesCollection & _ctlBxColl);

    void _handleCaseNoBoxesInDom(controllerBoxesCollection & _ctlBxColl);
    void _markAllBoxesAsUnsignaledNewBox(std::array<ControlerBox, 10U> & _controllerBoxesArray);

    void _handleCaseBoxesInDom(JsonObject& __joBoxesStatesInDOM, controllerBoxesCollection & _ctlBxColl);

    void _handleCaseBoxesInDomAndNoBoxesInCBArray(JsonObject& __joBoxesStatesInDOM);

    void _checkConsistancyDOMDB(JsonObject& __joBoxesStatesInDOM, std::array<ControlerBox, 10U> & _controllerBoxesArray);
    void _lookForDisconnectedBoxes(JsonPair& _p, std::array<ControlerBox, 10U> & _controllerBoxesArray);
    void _checkBoxStateConsistancy(JsonPair& _p, std::array<ControlerBox, 10U> & _controllerBoxesArray);
    void _lookForDOMMissingRows(JsonObject& __joBoxesStatesInDOM, std::array<ControlerBox, 10U> & _controllerBoxesArray);

};
#endif


