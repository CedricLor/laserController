/*
  laserSafetyLoop.cpp - Library to handle the transfer of update to the instances of the LaserPin class into HIGH/LOW states of the pins.
  Created by Cedric Lor, January 4, 2019.
*/

#include "Arduino.h"
#include "laserSafetyLoop.h"

laserSafetyLoop::laserSafetyLoop()
{
  // pinMode(pin, OUTPUT);
  // _pin = pin;
}

// LASER SAFETY TIMER -- EXECUTED AT THE END OF EACH LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void laserSafetyLoop::loop() {
  // Loop around each struct representing a pin connected to a laser before restarting a global loop and
  // make any update that may be required. For instance:
  // - safety time elapsed of lasers in blinking cycle (blinking every 10 to 30 s., to avoid burning the lasers);
  // - update the paired laser or its pair if the lasers are paired;
  // and then, execute the updates.
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    LaserPins[thisPin].executePinStateChange();
    LaserPins[thisPin].blinkLaserInBlinkingCycle();
    // _blinkLaserIfBlinking(LaserPins[thisPin]);                          // check if laser is in blinking cycle and check whether the blinking interval has elapsed
    // _ifMasterPairedThenUpdateOnOffOfSlave(LaserPins, thisPin);          // update the on/off status of slave
    LaserPins[thisPin].laserProtectionSwitch();
  }
  LaserPinsArray::pinParityWitness = 0;
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
  blinkLaserIfBlinking:
  - this function seem equivocal:
    1.  why should it be limited to non-paired or master pin?
    2.  why does it update the on_off_target without checking first whether the on_off_target
        has not been updated otherwise? Certes, it updates on_off_target only if blinking is
        true and blinking_interval has elapsed, but still, this seems a second error in the logic of the thing
*/

void laserSafetyLoop::_blinkLaserIfBlinking(LaserPin &LaserPin) {
  /*
     Called for each pin by laserSafetyLoop()
     If a laser is in blinking mode and is either (i) non-paired or (ii) master in a pair,
     if so, switch its on/off state
  */
  if ((LaserPin.blinking == true) && ((LaserPin.paired_with == -1) || (LaserPinsArray::pinParityWitness == 0))) {
    _blinkLaserIfTimeIsDue(LaserPin);
  }
  /*
    Test: simplify => why should this apply only to a master or a non-paired?
  */
  /*
  if (LaserPin.blinking == true) {
    blinkLaserIfTimeIsDue(LaserPin);
  }
  */
}

void laserSafetyLoop::_blinkLaserIfTimeIsDue(LaserPin &LaserPin) {
  /*
     Called when a laser is in blinking mode and is either (i) non-paired or (ii) master in a pair
     Checks if the blinking interval of this laser has elapsed
     If so, switches the pin on/off target variable to the contrary of the current pin on/off
     TO ANALYSE: this may overwrite other changes that have been requested at other stages
  */
  const unsigned long currentTime = millis();
  if (currentTime - LaserPin.previous_time > LaserPin.blinking_interval) {
      LaserPin.on_off_target = !LaserPin.on_off;
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void laserSafetyLoop::_ifMasterPairedThenUpdateOnOffOfSlave(const int thisPin) {
  /*
      Called from within the laser safety loop for each pin
      Test if the laser is paired and if it is a master in a pair
      If so, calls evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave
  */
  if (!(LaserPins[thisPin].paired_with == -1) && (LaserPinsArray::pinParityWitness == 0)) {
    _updatePairedSlaveWrapper(thisPin);
  }
}

void laserSafetyLoop::_updatePairedSlaveWrapper(const int thisPin) {
  /*
      Called by ifMasterPairedThenUpdateOnOffOfSlave() for pins which are master in a pair.
      A. If the pin is NOT in blinking mode AND its on_off_target state is to turn off,
      it means that the master pin has been turned off (off and off blinking mode) => The slave pin should then be turned off.
      Comment: raisonnement bancal:
        Le fait que le on_off_target_state soit sur off quand un laser est "not in blinking mode" ne veut pas nécessairement dire qu'il vient d'être turned off.
        En tout état de cause, ceci permet bien sur de tourner l'interrupteur du slave sur off (mais ceci le fait même si l'interrupteur était déjà éteint).
      B. Else:
        either the master pin is in blinking mode and its on_off_target is to turn off (A = false and B = true),
                                                      or on_off_target is to turn on (A = false and B = false),
                  which means that the master pin, in blinking mode, has received a status change.
        or the master pin is not in blinking mode and its on_off_target is to turn on (A = true and B = false).
                  which means that the master pin has received the instruction to start a blinking cycle.
        In each case, the slave will receive the instruction to put its on_off_target to the opposite of that of its master.
  */
  if ((LaserPins[thisPin].blinking == false) && (LaserPins[thisPin].on_off_target == HIGH)) {
    _updatePairedSlave(thisPin, HIGH);
    return;
  }
  _updatePairedSlave(thisPin, !LaserPins[thisPin].on_off_target);
}

void laserSafetyLoop::_updatePairedSlave(const int thisPin, const bool nextPinOnOffTarget) {
  LaserPins[LaserPins[thisPin].paired_with].on_off_target = nextPinOnOffTarget;                          // update the on_off target of the paired slave laser depending on the received instruction
  LaserPins[LaserPins[thisPin].paired_with].blinking = LaserPins[thisPin].blinking;                            // align the blinking state of the paired slave laser
  LaserPins[LaserPins[thisPin].paired_with].pir_state = LaserPins[thisPin].pir_state;                          // align the IR state of the paired slave laser
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
