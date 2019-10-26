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

    /** USER MANUAL SIGNAL (from web) */
    int16_t i16boxStateRequestedFromWeb;    
    void setBoxActiveStateFromWeb(const int16_t _i16boxStateRequestedFromWeb); /** --> _tcbSetBoxStateFromWeb() as main callback*/

    /** MESH SIGNAL (change of state of another box in the mesh) */
    const bool checkImpactOfChangeInActiveStateOfOtherBox(const uint16_t __ui16BoxIndex); /**  --> _tcbIfMeshTriggered() */

    /** IR SIGNAL (from PIR Controller or upstream info (usi) from other boxes in the mesh) */
    const bool checkImpactOfUpstreamInformationOfOtherBox(const uint16_t __ui16BoxIndex); /**  --> _tcbIfIRTriggered() */
    const bool checkImpactOfThisBoxsIRHigh();

    /** TASK tSetBoxState */
    Task tSetBoxState;

    /** Members */
    controllerBoxesCollection     ctlBxColl;
    boxStateCollection            thisBxStateColl;

  private:

    /** The Brain: analyze signals and triggers reactions */
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
