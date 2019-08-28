/*
  pirController.cpp - Library to store infrared sensor related functions.
  Created by Cedric Lor, January 18, 2019.
*/

#include "Arduino.h"
#include "pirController.h"

const uint8_t INPUT_PIN = 12;               // choose the input pin (for PIR sensor) //
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

pirController myPirController(INPUT_PIN);

pirController::pirController(const uint8_t INPUT_PIN):_inputPin(INPUT_PIN)
{
  Serial.print("SETUP: pirController::pirController(): starting\n");
  pinMode(_inputPin, INPUT);                                // declare sensor as input
  Serial.printf("SETUP: pirController::pirController(): pin number %u set as INPUT pin\n", _inputPin);
}


void pirController::check() {
  // ui16touchSensorValue = touchRead(T3);
  // if (ui16touchSensorValue < 50) {
  //   Serial.println( ui16touchSensorValue);
  //   Serial.println("pirController::check()");
  // }
  if (digitalRead(_inputPin)) {
    Serial.println("pirController::check(): ---------- PIR Mouvement Detected ----------");
    ControlerBox::bValFromPir                 = HIGH;
    ControlerBox::ui32SettingTimeOfValFromPir = laserControllerMesh.getNodeTime();
  }
}
