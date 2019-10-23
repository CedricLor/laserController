/*
  signal.h - Library to handle the signals received from whichever source
  Created by Cedric Lor, August 28, 2019.
*/

#ifndef signal_h
#define signal_h

#include "Arduino.h"
#include "controllerBoxesCollection.h"
#include "boxState.h"

class signal
{
  friend class aThatBox;
  public:
    signal();

    void startup();

  private:
    controllerBoxesCollection     ctlBxColl;
    stepCollection                stepColl;
    boxStateCollection            bxStateColl;

    bool _testIfMeshisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox);
    static void _tcbIfIRTriggered(const ControlerBox & _callingBox);
    bool _testIfIRisHigh(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _testIfIRisHighIsMine(const ControlerBox & _callingBox);
    bool _isCallerThisBox(const ControlerBox & _callingBox);
    bool _testIfIRHighIsAmongMasters(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _isCallerMonitored(const ControlerBox & _callingBox, const uint16_t _ui16MonitoredNodeName);
    bool _isCallerInMonitoredArray(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _isCallerStateInMonitoredStates(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _isSignalFresherThanBoxStateStamp(const uint32_t _ui32SignalTime);

};

extern signal _signal;
#endif
