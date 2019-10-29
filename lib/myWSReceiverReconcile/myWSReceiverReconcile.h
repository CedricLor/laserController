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
    myWSReceiverReconcile(JsonObject& _obj, std::array<ControlerBox, 10U> & __controllerBoxesArray, uint16_t __ui16connectedBoxesCount, uint16_t __ui16PotentialBoxesCount);

  private:
    JsonObject& _joMsg;
    std::array<ControlerBox, 10U> & _controllerBoxesArray;
    uint16_t _ui16connectedBoxesCount;
    uint16_t _ui16PotentialBoxesCount;
    

    int16_t _onHandshakeCheckWhetherDOMNeedsUpdate();

    void _handleCaseNoBoxesInDom();
    void _markAllBoxesAsUnsignaledNewBox();

    void _handleCaseBoxesInDom(JsonObject& __joBoxesStatesInDOM);

    void _handleCaseBoxesInDomAndNoBoxesInCBArray(JsonObject& __joBoxesStatesInDOM);

    void _checkConsistancyDOMDB(JsonObject& __joBoxesStatesInDOM);
    void _lookForDisconnectedBoxes(JsonPair& _p);
    void _checkBoxStateConsistancy(JsonPair& _p);
    void _lookForDOMMissingRows(JsonObject& __joBoxesStatesInDOM);

    template <typename F>
    void _parseCBArrayAndMarkUnsignaledCBs(F&& _lambda);
};
#endif


