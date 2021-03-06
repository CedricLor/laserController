/*
  laserSignal.h - Library to handle the impact of signals read from the mesh 
  on the boxStates of this box
  Created by Cedric Lor, August 28, 2019.
*/

#ifndef laserSignal_h
#define laserSignal_h

#include "Arduino.h"
#include "controllerBoxesCollection.h"
#include "boxState.h"

class laserSignal
{
  friend class myMeshController;
  friend class controllerBoxThis;
  public:
    laserSignal(ControlerBox & __thisCtrlerBox, myMeshViews & __thisMeshViews);

    void startup();

    /** USER MANUAL SIGNAL (from web through mesh) */
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
    ControlerBox &                _thisCtrlerBox; // TODO: make it private
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

    bool _testIfIRHighIsAmongMasters(const ControlerBox & _callingBox, const std::array<uint16_t, 4U> & __ui16monitoredMasterBoxesNodeNames);

    bool _isCallerMonitored(const ControlerBox & _callingBox, const uint16_t _ui16MonitoredNodeName);
    bool _isCallerInMonitoredArray(const ControlerBox & _callingBox, const std::array<uint16_t, 4U> & __ui16monitoredMasterBoxesNodeNames);
    bool _isCallerStateInMonitoredStates(const ControlerBox & _callingBox, const boxState & _currentBoxState);
    bool _isSignalFresherThanBoxStateStamp(const uint32_t _ui32SignalTime);
};

#endif
