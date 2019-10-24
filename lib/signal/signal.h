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
  friend class myMeshController;
  friend class controllerBoxThis;
  public:
    signal();

    void startup();

    /** web signal */
    int16_t i16boxStateRequestedFromWeb;

    /** void setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb)
     * 
     *  -> _tcbSetBoxStateFromWeb()*/
    void setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb);
    
    const bool checkImpactOfChangeInActiveStateOfOtherBox(const uint16_t __ui16BoxIndex);
    /** const bool setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime)
     * 
     *  -> _tcbIfMeshTriggered()
     *  TODO: The same-name method in controlerBox needs to be kept but stripped of 
     *  the part that calls the Task. */
    const bool setBoxActiveState(const int16_t _i16boxActiveState, const uint32_t _ui32BoxActiveStateStartTime);

    const bool checkImpactOfUpstreamInformationOfOtherBox(const uint16_t __ui16BoxIndex);
    const bool checkImpactOfThisBoxsIRHigh();
    /** void setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime)
     * 
     *  -> _tcbIfIRTriggered()
     *  TODO: Analyse whether the same-name method in controlerBox needs to be scrapped 
     *  and replaced by this one. */
    void setBoxIRTimes(const uint32_t _ui32lastRecPirHighTime);

    /** boxState setter Task */
    Task tSetBoxState;

    controllerBoxesCollection     ctlBxColl;
    stepCollection                stepColl;
    boxStateCollection            bxStateColl;

  private:
    void _tcbSetBoxStateFromWeb();

    void _tcbIfMeshTriggered(const ControlerBox & _callingBox);
    bool _testIfMeshisHigh(const boxState & _currentBoxState, const ControlerBox & _callingBox);

    void _tcbIfIRTriggered(const ControlerBox & _callingBox);
    bool _testIfIRisHigh(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _testIfIRisHighIsMine(const ControlerBox & _callingBox);

    bool _isCallerThisBox(const ControlerBox & _callingBox);

    bool _testIfIRHighIsAmongMasters(const ControlerBox & _callingBox, const boxState & _currentBoxState);

    bool _isCallerMonitored(const ControlerBox & _callingBox, const uint16_t _ui16MonitoredNodeName);
    bool _isCallerInMonitoredArray(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _isCallerStateInMonitoredStates(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _isSignalFresherThanBoxStateStamp(const uint32_t _ui32SignalTime);
};

#endif
