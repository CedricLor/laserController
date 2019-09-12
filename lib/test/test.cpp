/*
  test.cpp - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#include "Arduino.h"
#include "test.h"



test::test() { }

void test::laserPins() {
  Serial.println("\n\ntest::laserPins: starting\n");

  Serial.println("\n\ntest::laserPins: ending\n\n");
}

void test::sequenceStack() { 
  Serial.println("\n\ntest::sequenceStack: starting\n");
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
  Serial.println("\ntest::sequenceStack: ending\n\n");
}