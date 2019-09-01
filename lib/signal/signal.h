/*
  signal.h - Library to handle the signals received from whichever source
  Created by Cedric Lor, August 28, 2019.
*/

#ifndef signal_h
#define signal_h

#include "Arduino.h"
#include "ControlerBox.h"
#include "boxState.h"

class signal
{
  public:
    signal();

  private:
    static void _tcbSetBoxStateFromWeb();
    static void _tcbIsMeshHigh(const ControlerBox & _callingBox);
    static bool _testIfMeshisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox);
    static void _tcbIsIRHigh(const ControlerBox & _callingBox);
    static bool _testIfIRisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox);
    static bool _isCallerMonitored(const ControlerBox & _callingBox, const uint16_t _ui16MonitoredNodeName);
    static bool _isSignalFresherThanBoxStateStamp(const uint32_t _ui32SignalTime);

};

#endif
