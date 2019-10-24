/*
  pirController.cpp - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirController.h"


// ESP32 Pin Layout
// LEFT COLUMN
//                    3,3 V           <-- not fit
//                    EN              <-- not fit
// 36 = A0 =          SP;             --> touch
// 39 = A3 =          SM;             --> touch
// 34 = A6 =          G34;            --> touch
// 35 = A7 =          G35;            --> touch
// 32 = A4  = T9 =    G32;            --> touch
// 33 = A5  = T8 =    G33;            --> touch
// 25 = A18 = DAC1 =  G25;            --> touch
// 26 = A19 = DAC2 =  G26;            --> touch
// 27 = A17 = T7 =    G27;            --> touch
// 14 = A16 = T6 =    G14;            --> touch
// 12 = A15 = T5 =    G12;            --> touch
//                    GND;            <-- not fit
// 13 = A14 = T4 =    G13;            --> touch
// = =                SD2;            <-- not found
// = =                SD3;            <-- not found

// RIGHT COLUMN
// fit: --> at least, touch
// = = CLK
// = = SD0
// = = SD1
// 15 = A13 = T3 =    G15;             --> touch
// [2 = A12 = T2 =]   G2;             [<-- no apparent reaction to touch]
// [0 = A11 =]        G0;             [<-- always high]
// 4 = A10 = T0 =     G4;              <-- Test irrelevant; laser output
// 16 = =             G16;             <-- untested; laser output
// 17 = =             G17;             <-- untested; laser output
// 5 = SS =           G5;              <-- untested; laser output
// 18 = SCK =         G18;             <-- untested; laser output
// 19 = MISO =        G19;             --> touch; not very reliable because laser output
//                    GND;             <-- not fit
// 21 = SDA =         G21;             <-- untested; laser output
// 1 = TX =           RXD;             <-- not fit; always high
// 3 = RX =           TXD;             <-- not fit; always high
// 22 = SDL =         G22;             <-- untested; laser output
// 23 = MOSI =        G23              --> touch
//                    GND;             <-- not fit
// 

pirController::pirController(signal & __signal, Task & __tSetPirTimeStampAndBrdcstMsg, Task & __tPirSpeedBumper, myMeshViews & __thisMeshView, const uint8_t _INPUT_PIN):
  _signal(__signal),
  _inputPin(_INPUT_PIN),
  _tSetPirTimeStampAndBrdcstMsg(__tSetPirTimeStampAndBrdcstMsg),
  _tPirSpeedBumper(__tPirSpeedBumper),
  _thisMeshView(__thisMeshView)
{
  pinMode(_inputPin, INPUT);                                // declare sensor as input
  _tSetPirTimeStampAndBrdcstMsg.set(0, TASK_ONCE, [&](){_tcbSetPirTimeStampAndBrdcstMsg();}, [&](){return _oetcbSetPirTimeStampAndBrdcstMsg();}, NULL);
  _tPirSpeedBumper.set(0, TASK_ONCE, NULL, NULL, NULL);
}




/** void pirController::check()
 * 
 *  called at each pass of the main loop, to check whether the IR has detected any move
 * 
 *  If so, restarts the Task _tSetPirTimeStampAndBrdcstMsg
 * */
void pirController::check() {
  if (digitalRead(_inputPin)) {
    Serial.println("pirController::check(): digitalRead(_inputPin) is HIGH");
    _tSetPirTimeStampAndBrdcstMsg.restart();
  }
}




/** void pirController::_tcbSetPirTimeStampAndBrdcstMsg()
 * 
 *  main callback for Task tSetPirTimeStampAndBrdcstMsg
 * 
 *  Sets this boxes IR High time.
 *  Starts the Task 
 *  Broadcasts a message to the mesh with its IR High time.
 * */
void pirController::_tcbSetPirTimeStampAndBrdcstMsg() {
  Serial.println("pirController::_tcbSetPirTimeStampAndBrdcstMsg(): ---------- PIR Mouvement Detected ----------");
  _signal.ctlBxColl.controllerBoxesArray.at(0).setBoxIRTimes(globalBaseVariables.laserControllerMesh.getNodeTime());
  _signal.checkImpactOfThisBoxsIRHigh();
  _thisMeshView._IRHighMsg();
}




/** pirController::_oetcbSetPirTimeStampAndBrdcstMsg()
 * 
 *  onEnable callback for Task tSetPirTimeStampAndBrdcstMsg
 * 
 *  Test whether tSpeedBumper.isEnabled().
 *  If it is, return false and disable this Task.
 *  If it is not, restart tSpeedBumper for 3 seconds and return true
 *  to execute the main callback. 
 * */
bool pirController::_oetcbSetPirTimeStampAndBrdcstMsg() {
    // Serial.printf("pirController::tSetPirTimeStampAndBrdcstMsg(): onEnable callback: is tSpeedBumper.isEnabled()? %i\n", tSpeedBumper.isEnabled());
    if (_tPirSpeedBumper.isEnabled()) { return false; }
    _tPirSpeedBumper.restartDelayed(3000);
    // Serial.printf("pirController::tSetPirTimeStampAndBrdcstMsg(): onEnable callback: tSpeedBumper.isEnabled() was not enabled. now, is tSpeedBumper.isEnabled()? %i\n", tSpeedBumper.isEnabled());
    return true;
}
