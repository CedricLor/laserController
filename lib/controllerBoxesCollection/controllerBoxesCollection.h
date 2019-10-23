/*
  controllerBoxesCollection.h - Library to handle ControlerBoxes collection.
  Created by Cedric Lor, Octobre 14, 2019.
*/
#ifndef controllerBoxesCollection_h
#define controllerBoxesCollection_h

#include "Arduino.h"
#include <globalBasementVars.h>
#include <ControlerBox.h>

class controllerBoxesCollection
{
  public:
    // default constructor
    controllerBoxesCollection();

    // Class utilities (finders, etc.)
    uint16_t updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj); // <-
    uint16_t findIndexByNodeId(uint32_t _ui32nodeId); // <-
    uint16_t findIndexByNodeName(uint16_t _ui16NodeName); // <-
    void deleteBoxByNodeId(uint32_t _ui32nodeId); // <-
    void deleteBoxByBoxIndex(uint16_t _ui16index); // <-

    // Boxes counter
    uint16_t const updateConnectedBoxCount(uint16_t _ui16newConnectedBoxesCount);
    short int connectedBoxesCount;
    short int previousConnectedBoxesCount;

    // Boxes array
    std::array<ControlerBox, 10> controllerBoxesArray;

    // this ControlerBox instance reference
    ControlerBox & thisBox;

  private:
    /** helpers */
    void printSearchResults(uint16_t _index, uint32_t _ui32saughtTerm, const char * _saughtIdentifier);
};

extern controllerBoxesCollection cntrllerBoxesCollection;

#endif
