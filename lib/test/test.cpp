/*
  test.cpp - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#include "Arduino.h"
#include "test.h"



test::test() { }




void test::laserPinStack() {
  Serial.println("\n\ntest::laserPinStack: starting");

  Serial.println("test::laserPinStack: testing laserPin constructor: laserPin _laserPin{21, 2}.");
  laserPin _laserPin{21, 2};
  Serial.printf("test::laserPinStack: _laserPin._i16IndexNumber should be equal to 2. Is equal to [%i]\n", 
    _laserPin._i16IndexNumber);
  Serial.printf("test::laserPinStack: _laserPin._ui16PhysicalPinNumber should be equal to 21. Is equal to [%u]\n", 
    _laserPin._ui16PhysicalPinNumber);
  Serial.printf("test::laserPinStack: _laserPin._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _laserPin._switchPin(LOW));
  Serial.printf("test::laserPinStack: _laserPin._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _laserPin._switchPin(HIGH));
  Serial.println("test::laserPinStack: testing laserPin destructor: _laserPin.~laserPin().");
  _laserPin.~laserPin();

  Serial.println("test::laserPinStack: testing laserPin default constructor: laserPin _laserPin_2.");
  laserPin _laserPin_2;
  Serial.printf("test::laserPinStack: _laserPin_2._i16IndexNumber should be equal to -2. Is equal to [%i]\n", 
    _laserPin_2._i16IndexNumber);
  Serial.printf("test::laserPinStack: _laserPin._ui16PhysicalPinNumber should be equal to 0. Is equal to [%u]\n", 
    _laserPin_2._ui16PhysicalPinNumber);
  Serial.println("test::laserPinStack: testing laserPin destructor: _laserPin.~laserPin().");
  _laserPin_2.~laserPin();

  Serial.println("test::laserPinStack: testing laserPin constructor: laserPin _laserPin_3{21}.");
  laserPin _laserPin_3{21};
  Serial.printf("test::laserPinStack: _laserPin_3._i16IndexNumber should be equal to -1. Is equal to [%i]\n", 
    _laserPin_3._i16IndexNumber);
  Serial.printf("test::laserPinStack: _laserPin_3._ui16PhysicalPinNumber should be equal to 21. Is equal to [%u]\n", 
    _laserPin_3._ui16PhysicalPinNumber);
  Serial.printf("test::laserPinStack: _laserPin_3._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _laserPin_3._switchPin(LOW));
  Serial.printf("test::laserPinStack: _laserPin_3._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _laserPin_3._switchPin(HIGH));
  Serial.println("test::laserPinStack: testing laserPin destructor: _laserPin_3.~laserPin().");
  _laserPin_3.~laserPin();


  Serial.println("test::laserPinStack: over. \n");
}




void test::rawLaserPinsStack() {
  Serial.println("\n\ntest::rawLaserPinsStack: starting");

  Serial.println("test::rawLaserPinsStack: testing laserPins constructor: laserPins _laserPins{}.");
  laserPins _laserPins{};

  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._i16IndexNumber should be equal to 0. Is equal to [%i]\n", 
    _laserPins._array.at(0)._i16IndexNumber);
  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._ui16PhysicalPinNumber should be equal to 5. Is equal to [%u]\n", 
    _laserPins._array.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _laserPins._array.at(0)._switchPin(HIGH));

  Serial.println("test::rawLaserPinsStack: testing laserPins destructor: _laserPins.~laserPins().");
  _laserPins.~laserPins();

  Serial.println("test::rawLaserPinsStack: over\n");
}






void test::toneStack() {
  Serial.println("\n\ntest::toneStack: starting");

  Serial.println("test::toneStack: testing tone constructor: tone _tone{{HIGH, HIGH, LOW, LOW}}.");
  laserPins _laserPins;
  std::array<bool, 4> const _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  tone _tone{_aSecondPairOn};

  Serial.printf("test::toneStack: _tone._laserPinsStatesArr.at(0) should be equal to 1. Is equal to [%u]\n", 
    _tone._laserPinsStatesArr.at(0));
  Serial.printf("test::toneStack: _tone._laserPinsStatesArr.at(3) should be equal to 0. Is equal to [%u]\n", 
    _tone._laserPinsStatesArr.at(3));
  Serial.printf("test::toneStack: _tone.i16IndexNumber should be equal to -1. Is equal to [%i]\n", 
    _tone.i16IndexNumber);
  Serial.printf("test::toneStack: calling _tone._playTone(_laserPins); should return -1. Returns [%i]\n", 
    _tone._playTone(_laserPins));

  Serial.println("test::toneStack: over. \n");
}




void test::rawTonesStack() {
  Serial.println("\n\ntest::rawTonesStack: starting");

  Serial.println("test::rawTonesStack: testing tones constructor: tones _tones{}.");
  laserPins _laserPins;
  tones _tones{};

  Serial.printf("test::rawTonesStack: _tones._array.at(0)._laserPinsStatesArr.at(0) should be equal to 1. Is equal to [%u]\n", 
    _tones._array.at(0)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawTonesStack: _tones._array.at(0)._laserPinsStatesArr.at(3) should be equal to 1. Is equal to [%u]\n", 
    _tones._array.at(0)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawTonesStack: _tones._array.at(0).i16IndexNumber should be equal to 0. Is equal to [%i]\n", 
    _tones._array.at(0).i16IndexNumber);
  Serial.printf("test::rawTonesStack: _tones._array.at(0)._playTone(_laserPins) shall return 0. Returns [%i]\n", 
    _tones._array.at(0)._playTone(_laserPins));

  Serial.printf("test::rawTonesStack: _tones._array.at(15)._laserPinsStatesArr.at(0) should be equal to 0. Is equal to [%u]\n", 
    _tones._array.at(15)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawTonesStack: _tones._array.at(15)._laserPinsStatesArr.at(3) should be equal to 0. Is equal to [%u]\n", 
    _tones._array.at(15)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawTonesStack: _tones._array.at(15).i16IndexNumber should be equal to 15. Is equal to [%i]\n", 
    _tones._array.at(15).i16IndexNumber);
  Serial.printf("test::rawTonesStack: _tones._array.at(15)._playTone(_laserPins) shall return 15. Returns [%i]\n", 
    _tones._array.at(15)._playTone(_laserPins));

  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._i16IndexNumber should be equal to 0. Is equal to [%i]\n", 
    _tones._laserPins._array.at(0)._i16IndexNumber);
  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._ui16PhysicalPinNumber should be equal to 5. Is equal to [%i]\n", 
    _tones._laserPins._array.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _tones._laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _tones._laserPins._array.at(0)._switchPin(HIGH));

  Serial.println("test::rawTonesStack: over\n");
}




void test::implementedTonesStack() {

}




void test::noteStack() {
  Serial.println("\n\ntest::noteStack: starting");

  Serial.println("test::noteStack: testing note constructor: note _note{1,1}.");
  note _note{1, 1};
  Serial.printf("test::noteStack: _note.getToneNumber() should be equal to 1. Is equal to [%u]\n", 
    _note.getToneNumber());
  Serial.printf("test::noteStack: _note.getNote() should be equal to 1. Is equal to [%u]\n", 
    _note.getNote());
  Serial.printf("test::noteStack: _note.ui16GetNoteDurationInMs() should be equal to 0 (because beat is set to its defaults). Is equal to [%u]\n", 
    _note.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: _note._tone.i16IndexNumber should be equal 1. Is equal to [%i]\n", 
    _note._tone.i16IndexNumber);
  tone _tone(tones{}._array.at(3));
  Serial.println("test::noteStack: calling _note._setTone(_tone) with _tone(tones{}._array.at(3)).");
  Serial.printf("test::noteStack: _note._setTone(_tone) should return a tone & with i16IndexNumber equal to 3. Is equal to [%i]\n", 
    _note._setTone(_tone).i16IndexNumber);
  Serial.printf("test::noteStack: _note._tone.i16IndexNumber should be equal 3. Is equal to [%i]\n\n", 
    _note._tone.i16IndexNumber);

  Serial.println("test::noteStack: testing note default constructor: note _noteDef.");
  note _noteDef;
  Serial.printf("test::noteStack: _noteDef.getToneNumber() should be equal to 0. Is equal to [%u]\n", 
    _noteDef.getToneNumber());
  Serial.printf("test::noteStack: _noteDef.getNote() should be equal to 0. Is equal to [%u]\n", 
    _noteDef.getNote());
  Serial.printf("test::noteStack: _noteDef.ui16GetNoteDurationInMs() should be equal to 0 (because beat is set to its defaults). Is equal to [%u]\n", 
    _noteDef.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: _noteDef._tone.i16IndexNumber should be equal 0. Is equal to [%i]\n", 
    _noteDef._tone.i16IndexNumber);
  tone _tone2(tones{}._array.at(2));
  Serial.println("test::noteStack: calling _noteDef._setTone(_tone2) with _tone2(tones{}._array.at(2)).");
  Serial.printf("test::noteStack: _noteDef._setTone(_tone) should return a tone & with i16IndexNumber equal to 2. Is equal to [%i]\n", 
    _noteDef._setTone(_tone).i16IndexNumber);
  Serial.printf("test::noteStack: _noteDef._tone.i16IndexNumber should be equal 2. Is equal to [%i]\n\n", 
    _noteDef._tone.i16IndexNumber);

  Serial.println("test::noteStack: testing note straight copy constructor: note{1,1}.");
  Serial.printf("test::noteStack: note{1,1}.getToneNumber() should be equal to 1. Is equal to [%u]\n", 
    note{1,1}.getToneNumber());
  Serial.printf("test::noteStack: note{1,1}.getNote() should be equal to 1. Is equal to [%u]\n", 
    note{1,1}.getNote());
  Serial.printf("test::noteStack: note{1,1}.ui16GetNoteDurationInMs() should be equal to 0 (because beat is set to its defaults). Is equal to [%u]\n", 
    note{1,1}.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note{1,1}._tone.i16IndexNumber should be equal 3. Is equal to [%i]\n", 
    note{1,1}._tone.i16IndexNumber);
  Serial.println("test::noteStack: calling note{1,1}._setTone(_tone2) with _tone2(tones{}._array.at(2)).");
  Serial.printf("test::noteStack: note{1,1}._setTone(_tone) should return a tone & with i16IndexNumber equal to 2. Is equal to [%i]\n", 
    note{1,1}._setTone(_tone2).i16IndexNumber);
  Serial.printf("test::noteStack: note{1,1}._tone.i16IndexNumber should be equal 2. Is equal to [%i]\n\n", 
    note{1,1}._tone.i16IndexNumber);

  Serial.println("test::noteStack: testing note copy constructor: note _noteCpy(_note);");
  note _noteCpy(_note);
  Serial.printf("test::noteStack: _noteCpy.getToneNumber() should be equal to 1. Is equal to [%u]\n", 
    _noteCpy.getToneNumber());
  Serial.printf("test::noteStack: _noteCpy.getNote() should be equal to 1. Is equal to [%u]\n", 
    _noteCpy.getNote());
  Serial.printf("test::noteStack: _noteCpy.ui16GetNoteDurationInMs() should be equal to 0 (because beat is set to its defaults). Is equal to [%u]\n", 
    _noteCpy.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: _noteCpy._tone.i16IndexNumber should be equal 3. Is equal to [%i]\n", 
    _noteCpy._tone.i16IndexNumber);
  Serial.println("test::noteStack: calling _noteCpy._setTone(_tone) with _tone(tones{}._array.at(2)).");
  Serial.printf("test::noteStack: _noteCpy._setTone(_tone) should return a tone & with i16IndexNumber equal to 2. Is equal to [%i]\n", 
    _noteCpy._setTone(_tone2).i16IndexNumber);
  Serial.printf("test::noteStack: _noteCpy._tone.i16IndexNumber should be equal 2. Is equal to [%i]\n\n", 
    _noteCpy._tone.i16IndexNumber);

  Serial.println("test::noteStack: testing note copy constructor: note _noteCpy2 = _note;");
  note _noteCpy2 = _note;
  Serial.printf("test::noteStack: _noteCpy2.getToneNumber() should be equal to 1. Is equal to [%u]\n", 
    _noteCpy2.getToneNumber());
  Serial.printf("test::noteStack: _noteCpy2.getNote() should be equal to 1. Is equal to [%u]\n", 
    _noteCpy2.getNote());
  Serial.printf("test::noteStack: _noteCpy2.ui16GetNoteDurationInMs() should be equal to 0 (because beat is set to its defaults). Is equal to [%u]\n", 
    _noteCpy2.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: _noteCpy2._tone.i16IndexNumber should be equal 3. Is equal to [%i]\n", 
    _noteCpy2._tone.i16IndexNumber);
  Serial.println("test::noteStack: calling _noteCpy2._setTone(_tone) with _tone(tones{}._array.at(2)).");
  Serial.printf("test::noteStack: _noteCpy2._setTone(_tone) should return a tone & with i16IndexNumber equal to 2. Is equal to [%i]\n", 
    _noteCpy2._setTone(_tone).i16IndexNumber);
  Serial.printf("test::noteStack: _noteCpy2._tone.i16IndexNumber should be equal 2. Is equal to [%i]\n\n", 
    _noteCpy2._tone.i16IndexNumber);

  Serial.println("test::noteStack: testing note assignment operator: note _note2; then _note2 = _note;");
  note _note2;
  _note2 = _note;
  Serial.printf("test::noteStack: _note2.getToneNumber() should be equal to 1. Is equal to [%u]\n", 
    _note2.getToneNumber());
  Serial.printf("test::noteStack: _note2.getNote() should be equal to 1. Is equal to [%u]\n", 
    _note2.getNote());
  Serial.printf("test::noteStack: _note2.ui16GetNoteDurationInMs() should be equal to 0 (because beat is set to its defaults). Is equal to [%u]\n", 
    _note2.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: _note2._tone.i16IndexNumber should be equal 3. Is equal to [%i]\n", 
    _note2._tone.i16IndexNumber);
  Serial.println("test::noteStack: calling _note2._setTone(_tone) with _tone(tones{}._array.at(2)).");
  Serial.printf("test::noteStack: _note2._setTone(_tone) should return a tone & with i16IndexNumber equal to 2. Is equal to [%i]\n", 
    _note2._setTone(_tone).i16IndexNumber);
  Serial.printf("test::noteStack: _note2._tone.i16IndexNumber should be equal 2. Is equal to [%i]\n\n", 
    _note2._tone.i16IndexNumber);

  Serial.printf("test::noteStack: testing _validNote()\n");
  Serial.printf("test::noteStack: testing _validNote() in main constructor: note _note3{1, 5}\n");
  note _note3{1, 5};
  Serial.printf("test::noteStack: _note3.getNote() should be equal to 6. Is equal to [%u]\n", 
    _note3.getNote());
  Serial.printf("test::noteStack: testing _validNote() in straight copy constructor: note{1,7}\n");
  Serial.printf("test::noteStack: note{1,7}.getNote() should be equal to 8. Is equal to [%u]\n", 
    note{1,7}.getNote());
  Serial.printf("test::noteStack: testing _validNote() in assignment op: _note2 = {2,11};\n");
  Serial.printf("test::noteStack: note2.getNote() should be equal to 16. Is equal to [%u]\n", 
    note{2,11}.getNote());

  Serial.println("test::noteStack: over\n");
}




void test::rawNotesStack() {
  Serial.println("\n\ntest::rawNotesStack: starting");

  Serial.println("test::rawNotesStack: testing notes constructor: notes _notes{}.");
  laserPins _laserPins;
  notes _notes{};

  Serial.println("test::rawNotesStack: calling _notes.resetTPlayNoteToPlayNotesInBar()");
  _notes.resetTPlayNoteToPlayNotesInBar();

  Serial.printf("test::rawNotesStack: _notes._tones._array.at(0)._laserPinsStatesArr.at(0) should be equal to 1. Is equal to [%u]\n", 
    _notes._tones._array.at(0)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(0)._laserPinsStatesArr.at(3) should be equal to 1. Is equal to [%u]\n", 
    _notes._tones._array.at(0)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(0)._playTone(_laserPins) shall return 0. Returns [%i]\n", 
    _notes._tones._array.at(0)._playTone(_laserPins));

  Serial.printf("test::rawNotesStack: _notes._tones._array.at(15)._laserPinsStatesArr.at(0) should be equal to 0. Is equal to [%u]\n", 
    _notes._tones._array.at(15)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(15)._laserPinsStatesArr.at(3) should be equal to 0. Is equal to [%u]\n", 
    _notes._tones._array.at(15)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(15)._playTone(_laserPins) shall return 15. Returns [%i]\n", 
    _notes._tones._array.at(15)._playTone(_laserPins));

  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._i16IndexNumber should be equal to 0. Is equal to [%i]\n", 
    _notes._tones._laserPins._array.at(0)._i16IndexNumber);
  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._ui16PhysicalPinNumber should be equal to 5. Is equal to [%i]\n", 
    _notes._tones._laserPins._array.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._switchPin(LOW) should be equal to 0. Is equal to [%i]\n", 
    _notes._tones._laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._switchPin(HIGH) should be equal to 1. Is equal to [%i]\n", 
    _notes._tones._laserPins._array.at(0)._switchPin(HIGH));
  Serial.println("test::rawNotesStack: calling _notes.playNoteStandAlone(note{4, 1}, beat(5, 1).\n");
  _notes.playNoteStandAlone(note{4, 1}, beat(5, 1));
  note _note{1, 1};
  Serial.println("test::rawNotesStack: calling _notes.playNoteStandAlone(_note, beat(5, 1)).\n");
  _notes.playNoteStandAlone(_note, beat(5, 1));
  Serial.println("test::rawNotesStack: calling _bar.getNotes().setActive(note{4,8}).\n");
  _notes.setActive(note{4,8});
  Serial.printf("test::rawNotesStack: _notes._activeNote.getNote() should be equal to 8. Is equal to [%u]\n", 
    _notes._activeNote.getNote());
  Serial.printf("test::rawNotesStack: _notes._activeNote.getToneNumber() should be equal to 4. Is equal to [%u]\n", 
    _notes._activeNote.getToneNumber());

  Serial.println("test::rawNotesStack: over\n");
}




void test::barStack() {
  Serial.println("\n\ntest::barStack: starting");

  Serial.println("test::barStack: testing bar constructor: bar _bar(_arrayOfNotes) with _arrayOfNotes = ");
  Serial.println("                std::array<note, 16> {note(4,8), note(3,8), note(2,8), note(1,8),");
  Serial.println("                note(2,8), note(3,8), note(4,8), note(0,8)}");
  std::array<note, 16> _arrayOfNotes;
  _arrayOfNotes = {note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)};
  bar _bar(_arrayOfNotes);

  Serial.println("test::barStack: calling _bar.setActive().");
  _bar.setActive();

  Serial.printf("test::barStack: _bar.ui16GetNotesCountInBar() should be equal to 8. Is equal to [%u]\n", 
    _bar.ui16GetNotesCountInBar());

  Serial.printf("test::barStack: _bar.ui16GetBaseNotesCountInBar() should be equal to 0 (because beat has not been set). Is equal to [%u]\n", 
    _bar.ui16GetBaseNotesCountInBar());

  Serial.printf("test::barStack: _note.ui32GetBarDuration() should be equal to 0 (because beat has not been set). Is equal to [%u]\n", 
    _bar.ui32GetBarDuration());

  Serial.printf("test::barStack: _bar.getNotesArray().at(0).getNote() should be equal to 8. Is equal to [%u]\n", 
    _bar.getNotesArray().at(0).getNote());

  Serial.printf("test::barStack: _bar.playBarStandAlone(beat(5, 1)) should return 0 because the active bar id has not been set. Returns [%i]\n",
    _bar.playBarStandAlone(beat(5, 1)));

  Serial.printf("test::barStack: _bar.playBarInSequence() should fail because beat is set to default (beat == 0 and base note == 0). Returns [%i]\n",
    _bar.playBarInSequence());

  Serial.println("test::barStack: calling _bar.getNotes().setActive(note{4,8}).");
  _bar.getNotes().setActive(note{4,8});

  Serial.printf("test::barStack: _bar.getNotes()._activeNote.getNote() should be equal to 8. Is equal to [%u]\n", 
    _bar.getNotes()._activeNote.getNote());
  Serial.printf("test::barStack: _bar.getNotes()._activeNote.getToneNumber() should be equal to 4. Is equal to [%u]\n", 
    _bar.getNotes()._activeNote.getToneNumber());

  Serial.println("test::barStack: over.\n");
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
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber() shall be equal to 7. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
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
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getToneNumber() shall be equal to 0. Is equal to [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getToneNumber());

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
