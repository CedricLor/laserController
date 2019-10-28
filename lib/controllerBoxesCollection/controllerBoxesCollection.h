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
  friend class myMeshController;
  public:
    // default constructor
    controllerBoxesCollection();

    // Class utilities (finders, etc.)
    uint16_t findIndexByNodeId(uint32_t _ui32nodeId);
    uint16_t findIndexByNodeName(uint16_t _ui16NodeName);
    void deleteBoxByNodeId(uint32_t _ui32nodeId);
    void deleteBoxByBoxIndex(uint16_t _ui16index);

    // Boxes counter
    uint16_t const ui16updateConnectedBoxCount(uint16_t _ui16newConnectedBoxesCount);
    uint16_t ui16connectedBoxesCount;
    uint16_t ui16previousConnectedBoxesCount;

    // Boxes array
    std::array<ControlerBox, 10> controllerBoxesArray;

  private:
    uint16_t _updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj);

    /** helpers */
    void printSearchResults(uint16_t _index, uint32_t _ui32saughtTerm, const char * _saughtIdentifier);
};

#endif
