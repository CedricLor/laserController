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
    

    uint16_t _onHandshakeCheckWhetherDOMNeedsUpdate();

    uint16_t _handleCaseNoBoxesInDom();
    uint16_t _markAllBoxesAsUnsignaledNewBox();

    uint16_t _handleCaseBoxesInDom(JsonObject& __joBoxesStatesInDOM);

    void _handleCaseBoxesInDomAndNoBoxesInCBArray(JsonObject& __joBoxesStatesInDOM);

    uint16_t _checkConsistancyDOMDB(JsonObject& __joBoxesStatesInDOM);
    void _lookForDisconnectedBoxes(JsonPair& _p);
    void _checkBoxStateConsistancy(JsonPair& _p);
    void _lookForDOMMissingRows(JsonObject& __joBoxesStatesInDOM);

    template <typename F>
    uint16_t _iterateOverCBArrayAndExecuteLambdaInLoop(F&& _lambda);
};
#endif


