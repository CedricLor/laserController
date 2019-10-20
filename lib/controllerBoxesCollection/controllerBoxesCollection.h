/*
  controllerBoxesCollection.h - Library to handle ControlerBoxes collection.
  Created by Cedric Lor, Octobre 14, 2019.
*/
#ifndef controllerBoxesCollection_h
#define controllerBoxesCollection_h

#include "Arduino.h"
#include <global.h>
#include <signal.h>

class controllerBoxesCollection
{
  public:
    // default constructor
    controllerBoxesCollection();

    /** -------- Public Variables */
    /** - Signal catchers ----------------- */
    int16_t i16boxStateRequestedFromWeb;
    /** - Signal instance ----------------- */
    signal signalHandlers;
    /** - this ControlerBox instance reference ----------------- */
    ControlerBox & thisBox;

    /** setters */
    void setBoxActiveState(const short _sBoxActiveState, const uint32_t _ui32BoxActiveStateStartTime);
    void setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb);

    // Class utilities (finders, etc.)
    uint16_t updateOrCreate(uint32_t _ui32nodeId, JsonObject &_obj);
    uint16_t findIndexByNodeId(uint32_t _ui32nodeId);
    void printSearchResults(uint16_t _index, uint32_t _ui32saughtTerm, const char * _saughtIdentifier);
    uint16_t findIndexByNodeName(uint16_t _ui16NodeName);
    void deleteBoxByNodeId(uint32_t _ui32nodeId);

    // Box counter
    void updateConnectedBoxCount(short int _newConnectedBoxesCount);
    short int connectedBoxesCount;
    short int previousConnectedBoxesCount;

    // Tasks
    Task tReboot;

    // boxState Setters Tasks
    Task tSetBoxState;
    void (*_tcbIfMeshTriggered)(const ControlerBox & _callingBox);
    void (*_tcbIfIRTriggered)(const ControlerBox & _callingBox);
    void (*_tcbSetBoxStateFromWeb)();

  private:
    /**reset Task tSetBoxState*/
    void resetTSetBoxState();
    void _reboot();
};

extern controllerBoxesCollection cntrllerBoxesCollection;

#endif
