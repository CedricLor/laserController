/*
  laserSafetyLoop.cpp - Library to handle the transfer of update to the instances of the LaserPin class into HIGH/LOW states of the pins.
  Created by Cedric Lor, January 4, 2007.
*/

#include "Arduino.h"
#include "laserSafetyLoop.h"

laserSafetyLoop::laserSafetyLoop(/*int pin*/)
{
  // pinMode(pin, OUTPUT);
  // _pin = pin;
}

void laserSafetyLoop::dot()
{
  // digitalWrite(_pin, HIGH);
  // delay(250);
  // digitalWrite(_pin, LOW);
  // delay(250);
}

void laserSafetyLoop::dash()
{
  // digitalWrite(_pin, HIGH);
  // delay(1000);
  // digitalWrite(_pin, LOW);
  // delay(250);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LASER SAFETY TIMER -- EXECUTED AT THE END OF EACH LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void laserSafetyLoop::laserSafetyLoop() {
  // Loop around each struct representing a pin connected to a laser before restarting a global loop and
  // make any update that may be required. For instance:
  // - safety time elapsed of lasers in blinking cycle (blinking every 10 to 30 s., to avoid burning the lasers);
  // - update the paired laser or its pair if the lasers are paired;
  // and then, execute the updates.
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    blinkLaserIfBlinking(thisPin);                          // check if laser is in blinking cycle and check whether the blinking interval has elapsed
    ifMasterPairedThenUpdateOnOffOfSlave(thisPin);          // update the on/off status of slave
    executeUpdates(thisPin);                                // transform the update to the struct to analogical updates in the status of the related pin
    LaserPin::pinParityWitness = (LaserPin::pinParityWitness == 0) ? 1 : 0;
  }
  LaserPin::pinParityWitness = 0;
}

void laserSafetyLoop::blinkLaserIfBlinking(const short thisPin) {
  /*
     Called for each pin by laserSafetyLoop()
     If a laser is in blinking mode and is either (i) non-paired or (ii) master in a pair,
     if so, switch its on/off state
  */
  if (LaserPins[thisPin].blinking == true && (LaserPins[thisPin].paired == 8 || LaserPin::pinParityWitness == 0)) {
    blinkLaserIfTimeIsDue(thisPin);
  }
}

void laserSafetyLoop::blinkLaserIfTimeIsDue(const short thisPin) {
  /*
     Called when a laser is in blinking mode and is either (i) non-paired or (ii) master in a pair
     Checks if the blinking interval of this laser has elapsed
     If so, switches the pin on/off target variable to the contrary of the current pin on/off
     TO ANALYSE: this may overwrite other changes that have been requested at other stages
  */
  const unsigned long currentTime = millis();
  if (currentTime - LaserPins[thisPin].previous_time > LaserPins[thisPin].blinking_interval) {
      LaserPins[thisPin].on_off_target = !LaserPins[thisPin].on_off;
  }
}

void laserSafetyLoop::ifMasterPairedThenUpdateOnOffOfSlave(const short thisPin) {
  /*
      Called from within the laser safety loop for each pin
      Test if the laser is paired and if it is a master in a pair
      If so, calls evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave
  */
  if (!(LaserPins[thisPin].paired == 8) && (LaserPin::pinParityWitness == 0)) {
    evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(thisPin);
  }
}

void laserSafetyLoop::evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(const short thisPin) {
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
    updatePairedSlave(thisPin, HIGH);
    return;
  }
  updatePairedSlave(thisPin, !LaserPins[thisPin].on_off_target);
}

void laserSafetyLoop::updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget) {
  LaserPins[thisPin + 1].on_off_target = nextPinOnOffTarget;                          // update the on_off target of the paired slave laser depending on the received instruction
  LaserPins[thisPin + 1].blinking = LaserPins[thisPin].blinking;                            // align the blinking state of the paired slave laser
  LaserPins[thisPin + 1].pir_state = LaserPins[thisPin].pir_state;                          // align the IR state of the paired slave laser
  // LaserPins[thisPin + 1].paired = thisPin;                                            // align the paired pin of the slave to this pin
}

void laserSafetyLoop::executeUpdates(const short thisPin) {
  /*
      Called from within the laser safety loop for each pin
      Checks whether the current on_off_target state is different than the current on_off state
      If so:
      1. turn on or off the laser as requested in the on_off_target_state
      2. align the on_off state with the on_off_target state
      3. reset the safety blinking timer of this pin
      // TO ANALYSE: I have the feeling that the condition to be tested shall be different
      // in the case a) a laser is in a blinking cycle and in the case b) a laser is not in
      // a blinking cycle and cooling off
  */
  if (LaserPins[thisPin].on_off != LaserPins[thisPin].on_off_target) {
    digitalWrite(LaserPins[thisPin].number, LaserPins[thisPin].on_off_target);
    LaserPins[thisPin].on_off = LaserPins[thisPin].on_off_target;
    LaserPins[thisPin].previous_time = millis();
  }
}
