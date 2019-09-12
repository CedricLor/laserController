/*
  test.cpp - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#include "Arduino.h"
#include "test.h"



test::test() { }

void test::laserPinStack() {
  Serial.println("\ntest::laserPinStack: starting");

  Serial.println("test::laserPinStack: testing laserPin constructor: laserPin _laserPin{-1, 21}.");
  laserPin _laserPin{1, 21};

  Serial.printf("test::laserPinStack: _laserPin._sIndexNumber should be equal to 1. Is equal to [%u]\n", 
    _laserPin._sIndexNumber);
  Serial.printf("test::laserPinStack: _laserPin._sPhysicalPinNumber should be equal to 21. Is equal to [%u]\n", 
    _laserPin._sPhysicalPinNumber);
  Serial.printf("test::laserPinStack: _laserPin._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _laserPin._switchPin(LOW));
  Serial.printf("test::laserPinStack: _laserPin._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _laserPin._switchPin(HIGH));

  Serial.println("test::laserPinStack: testing laserPin destructor: _laserPin.~laserPin().");
  _laserPin.~laserPin();

  Serial.println("test::laserPinStack: over. \n");
}


void test::laserPinsStack() {
  Serial.println("\n\ntest::laserPinsStack: starting");

  Serial.println("test::laserPinsStack: laserPins test.");
  // Serial.println("test::laserPinsStack: destroying tone::_laserPins.~laserPins().");
  // tone::_laserPins.~laserPins();
  Serial.println("test::laserPinsStack: testing laserPins constructor: laserPins _laserPins{}.");
  laserPins _laserPins{};
  Serial.println("test::laserPinsStack: testing laserPins constructor: laserPins _laserPins{}.");

  Serial.printf("test::laserPinsStack: _laserPins._array.at(0)._sIndexNumber should be equal to 0. Is equal to [%u]\n", 
    _laserPins._array.at(0)._sIndexNumber);
  Serial.printf("test::laserPinsStack: _laserPins._array.at(0)._sPhysicalPinNumber should be equal to 5. Is equal to [%u]\n", 
    _laserPins._array.at(0)._sPhysicalPinNumber);
  Serial.printf("test::laserPinsStack: _laserPins._array.at(0)._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::laserPinsStack: _laserPins._array.at(0)._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _laserPins._array.at(0)._switchPin(HIGH));

  Serial.println("test::laserPinsStack: testing laserPins destructor: _laserPins.~laserPins().");
  _laserPins.~laserPins();
  // Serial.println("test::laserPinsStack: reinitializing tone::_laserPin: _laserPins.~laserPins().");
  // tone::_laserPins = ;
  Serial.println("test::laserPinsStack: laserPins test over.");

  Serial.println("test::laserPinsStack: over\n");
}

void test::sequenceStack() { 
  Serial.println("\ntest::sequenceStack: starting");
  beat _beat{sequence::sequences[0].getAssociatedBeat()};
  _beat.setActive();

  Serial.println("\ntest::sequenceStack: testing sequence::sequences[0]");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getAssociatedBeat().getBaseNoteForBeat() should be equal to 1. Is equal to [%u]\n", 
    sequence::sequences[0].getAssociatedBeat().getBaseNoteForBeat());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getAssociatedBeat().getBaseBeatInBpm() should be equal to 2. Is equal to [%u]\n", 
    sequence::sequences[0].getAssociatedBeat().getBaseBeatInBpm());

  Serial.println("\ntest::sequenceStack: testing notes in existing bar");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote() shall be equal to 1. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getTone() shall be equal to 7. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getTone());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).ui16GetNoteDurationInMs() shall be equal to 3000. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).ui16GetNoteDurationInMs());

  Serial.println("\ntest::sequenceStack: testing methods of existing bar");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).ui16GetNotesCountInBar() shall be equal to 2. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui16GetNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar() shall be equal to 2. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).ui32GetBarDuration() shall be equal to 60000. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui32GetBarDuration());

  Serial.println("\ntest::sequenceStack: testing notes in \"beyond bound\" bar");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getNote() shall be equal to 0. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getNote());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getTone() shall be equal to 0. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getTone());

  Serial.println("\ntest::sequenceStack: testing methods of \"beyond bound\" bar");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).ui16GetNotesCountInBar() shall be equal to 0. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).ui16GetNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).ui16GetBaseNotesCountInBar() shall be equal to 2. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).ui32GetBarDuration() shall be equal to 0. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).ui32GetBarDuration());

  Serial.println("\ntest::sequenceStack: testing sequence methods");
  Serial.printf("test::sequenceStack: sequence::sequences[0].i16GetBarCountInSequence() shall be equal to 1. Is equal to [%u]\n", 
    sequence::sequences[0].ui16GetBarCountInSequence());
  Serial.printf("test::sequenceStack: sequence::sequences[0].ui32GetSequenceDuration() shall be equal to 60000. Is equal to [%u]\n", 
    sequence::sequences[0].ui32GetSequenceDuration());

  beat _defaultBeat{};
  _defaultBeat.setActive();
  Serial.println("\ntest::sequenceStack: over\n\n");
}
