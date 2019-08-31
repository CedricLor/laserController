/*
  signal.cpp - Library to handle the signals received from whichever source
  Created by Cedric Lor, August 28, 2019.
*/

#include "Arduino.h"
#include "signal.h"


signal::signal()
{
  /** 
   *  --> NOT SURE 1. will be needed
   *  1. Define the callback of a Task located in ControlerBox, to be enabled upon
   *     receiving the registration of a new ControlerBox.
   *     This callback shall test:
   *  (a). whether the ControlerBoxes pertains to one of the monitored ControlerBoxes 
   *       (i.e. is one of the masterBoxes);
   *  (b). if so, it shall add a ref to the corresponding 
   *       ControlerBox to an array of references to monitored ControlerBoxes.
   * 
   *     Needed in ControlerBox:
   *     => Task:
   *     Task tIsNewBoxMonitored;
   *     Task ControlerBox::tIsNewBoxMonitored(0, 1, NULL, NULL, false, NULL, NULL);
   * 
   *     Needed here:
   *     => a function: 
   *     static void _tcbIsNewBoxMonitored();
   *     => a storage structure: i.e. a ControlerBox refs array?
   * 
   * 
   *  2. Define the callback of a Task located in ControlerBox, to be enabled upon
   *     receiving a change in the boxState of one of the ControlerBoxes.
   *     This callback shall test:
   *  (a). whether the ControlerBoxes pertains to one of the monitored ControlerBoxes 
   *       (i.e. is one of the masterBoxes);
   *  (b). whether the new boxState pertains to one of the monitored boxStates for this
   *       ControlerBox;
   *  (c). if both conditions are fullfiled, it shall turn a Mesh HIGH switch (as 
   *       instance property) in boxState.
   * 
   *     Needed in ControlerBox:
   *     => Task:
   *     Task tIsMeshHigh;
   *     Task ControlerBox::tIsMeshHigh(0, 1, NULL, NULL, false, NULL, NULL);
   * 
   *     Needed in boxState:
   *     => instance property:
   *     uint16_t ui16MeshHigh;
   * 
   *     Needed here:
   *     => a function: 
   *     static void _tcbIsMeshHigh();
   *  
   * 
   *  3. Define the callback of a Task located in ControlerBox, to be enabled upon
   *     receiving a change in IR of one of the ControlerBoxes (including thisBox).
   *     This callback shall test:
   *  (a). whether the ControlerBoxes pertains to one of the monitored ControlerBoxes 
   *       (i.e. is one of the masterBoxes);
   *  (b). if so, it shall turn an IR HIGH switch (as instance property) in boxState.
   * 
   *     Needed in ControlerBox:
   *     => Task:
   *     Task tIsIRHigh;
   *     Task ControlerBox::tIsIRHigh(0, 1, NULL, NULL, false, NULL, NULL);
   * 
   *     Needed in boxState:
   *     => instance property:
   *     uint16_t ui16IRHigh
   * 
   *     Needed here:
   *     => a function: 
   *     static void _tcbIsIRHigh();
   * 
   *  4. Define the callback of a Task located in boxState, to be enabled upon starting
   *     a new boxState (or before, if it can be anticipated), to pass the list of 
   *     ControlerBoxes and the corresponding boxStates to be monitored
   *     to this "signal" class.
   * 
   *     Needed in boxState:
   *     => Task:
   *     Task boxState::tPassControlerBoxListToSignal
   * 
   *     Needed here:
   *     => a function: 
   *     static void _tcbPassControlerBoxListToSignal();
   *     => a storage structure: instance of signal?
   *    */
}

