/*
  test.cpp - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#include "Arduino.h"
#include "test.h"



test::test() { }




void test::laserPinStack() {
  char * _methodName = "test::laserPinStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  Serial.printf("%s testing laserPin constructor: laserPin _laserPin{21, 2}.\n", _methodName);
  laserPin _laserPin{21, 2};
  Serial.printf("%s _laserPin._i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    _laserPin._i16IndexNumber);
  Serial.printf("%s _laserPin._ui16PhysicalPinNumber shall be 21. Is [%u]\n", _methodName, 
    _laserPin._ui16PhysicalPinNumber);
  Serial.printf("%s _laserPin._switchPin(LOW) shall be 0. Is [%i]\n", _methodName, 
    _laserPin._switchPin(LOW));
  Serial.printf("%s _laserPin._switchPin(HIGH) shall be 1. Is [%i]\n", _methodName, 
    _laserPin._switchPin(HIGH));
  Serial.printf("%s testing laserPin destructor: _laserPin.~laserPin().\n", _methodName);
  _laserPin.~laserPin();

  Serial.printf("%s testing laserPin default constructor: laserPin _laserPin_2.\n", _methodName);
  laserPin _laserPin_2;
  Serial.printf("%s _laserPin_2._i16IndexNumber shall be -2. Is [%i]\n", _methodName, 
    _laserPin_2._i16IndexNumber);
  Serial.printf("%s _laserPin._ui16PhysicalPinNumber shall be 0. Is [%u]\n", _methodName, 
    _laserPin_2._ui16PhysicalPinNumber);
  Serial.printf("%s testing laserPin destructor: _laserPin.~laserPin().\n", _methodName);
  _laserPin_2.~laserPin();

  Serial.printf("%s testing laserPin constructor: laserPin _laserPin_3{21}.\n", _methodName);
  laserPin _laserPin_3{21};
  Serial.printf("%s _laserPin_3._i16IndexNumber shall be -1. Is [%i]\n", _methodName, 
    _laserPin_3._i16IndexNumber);
  Serial.printf("%s _laserPin_3._ui16PhysicalPinNumber shall be 21. Is [%u]\n", _methodName, 
    _laserPin_3._ui16PhysicalPinNumber);
  Serial.printf("%s _laserPin_3._switchPin(LOW) shall be 0. Is [%i]\n", _methodName, 
    _laserPin_3._switchPin(LOW));
  Serial.printf("%s _laserPin_3._switchPin(HIGH) shall be 1. Is [%i]\n", _methodName, 
    _laserPin_3._switchPin(HIGH));
  Serial.printf("%s testing laserPin destructor: _laserPin_3.~laserPin().\n", _methodName);
  _laserPin_3.~laserPin();


  Serial.printf("%s over. \n\n", _methodName);
}




void test::rawLaserPinsStack() {
  Serial.printf("\n\ntest::rawLaserPinsStack: starting\n");

  Serial.printf("test::rawLaserPinsStack: testing laserPins constructor: laserPins _laserPins{}.\n");
  laserPins _laserPins{};

  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._i16IndexNumber shall be 0. Is [%i]\n", 
    _laserPins._array.at(0)._i16IndexNumber);
  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._ui16PhysicalPinNumber shall be 5. Is [%u]\n", 
    _laserPins._array.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", 
    _laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::rawLaserPinsStack: _laserPins._array.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n", 
    _laserPins._array.at(0)._switchPin(HIGH));

  Serial.printf("test::rawLaserPinsStack: testing laserPins destructor: _laserPins.~laserPins().\n");
  _laserPins.~laserPins();

  Serial.printf("test::rawLaserPinsStack: over\n\n");
}






void test::toneStack() {
  Serial.printf("\n\ntest::toneStack: starting\n");

  Serial.printf("test::toneStack: testing tone constructor: tone _tone{{HIGH, HIGH, LOW, LOW}}.\n");
  laserPins _laserPins;
  std::array<bool, 4> const _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  tone _tone{_aSecondPairOn};

  Serial.printf("test::toneStack: _tone._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", 
    _tone._laserPinsStatesArr.at(0));
  Serial.printf("test::toneStack: _tone._laserPinsStatesArr.at(3) shall be 0. Is [%u]\n", 
    _tone._laserPinsStatesArr.at(3));
  Serial.printf("test::toneStack: _tone.i16IndexNumber shall be -1. Is [%i]\n", 
    _tone.i16IndexNumber);
  Serial.printf("test::toneStack: calling _tone._playTone(_laserPins); should return -1. Returns [%i]\n", 
    _tone._playTone(_laserPins));

  Serial.printf("test::toneStack: over. \n\n");
}




void test::rawTonesStack() {
  Serial.printf("\n\ntest::rawTonesStack: starting\n");

  Serial.printf("test::rawTonesStack: testing tones constructor: tones _tones{}.\n");
  laserPins _laserPins;
  tones _tones{};

  Serial.printf("test::rawTonesStack: _tones._array.at(0)._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", 
    _tones._array.at(0)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawTonesStack: _tones._array.at(0)._laserPinsStatesArr.at(3) shall be 1. Is [%u]\n", 
    _tones._array.at(0)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawTonesStack: _tones._array.at(0).i16IndexNumber shall be 0. Is [%i]\n", 
    _tones._array.at(0).i16IndexNumber);
  Serial.printf("test::rawTonesStack: _tones._array.at(0)._playTone(_laserPins) shall return 0. Returns [%i]\n", 
    _tones._array.at(0)._playTone(_laserPins));

  Serial.printf("test::rawTonesStack: _tones._array.at(15)._laserPinsStatesArr.at(0) shall be 0. Is [%u]\n", 
    _tones._array.at(15)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawTonesStack: _tones._array.at(15)._laserPinsStatesArr.at(3) shall be 0. Is [%u]\n", 
    _tones._array.at(15)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawTonesStack: _tones._array.at(15).i16IndexNumber shall be 15. Is [%i]\n", 
    _tones._array.at(15).i16IndexNumber);
  Serial.printf("test::rawTonesStack: _tones._array.at(15)._playTone(_laserPins) shall return 15. Returns [%i]\n", 
    _tones._array.at(15)._playTone(_laserPins));

  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._i16IndexNumber shall be 0. Is [%i]\n", 
    _tones._laserPins._array.at(0)._i16IndexNumber);
  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._ui16PhysicalPinNumber shall be 5. Is [%i]\n", 
    _tones._laserPins._array.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", 
    _tones._laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::rawTonesStack: _tones._laserPins._array.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n", 
    _tones._laserPins._array.at(0)._switchPin(HIGH));

  Serial.printf("test::rawTonesStack: over\n\n");
}




void test::implementedTonesStack() {

}




void test::noteStack() {
  Serial.printf("\n\ntest::noteStack: starting\n");

  Serial.printf("test::noteStack: testing note constructor: note _note{1,1}.\n");
  note _note{1, 1};
  tone _tone(tones{}._array.at(3));
  tone _tone2(tones{}._array.at(2));

  Serial.printf("test::noteStack: _note.getToneNumber() shall be 1. Is [%u]\n", 
    _note.getToneNumber());
  Serial.printf("test::noteStack: _note.getNote() shall be 1. Is [%u]\n", 
    _note.getNote());
  Serial.printf("test::noteStack: _note.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", 
    _note.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note::globalTones._array.at(_note.getToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", 
    note::globalTones._array.at(_note.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: calling _note._setTone(_tone) with _tone(tones{}._array.at(3)).\n");
  Serial.printf("test::noteStack: _note._setTone(_tone) should return a tone & with i16IndexNumber equal to 3. Is [%i]\n", 
    _note._setTone(_tone));
  Serial.printf("test::noteStack: note::globalTones._array.at(_note.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", 
    note::globalTones._array.at(_note.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: _note.getToneNumber() shall be 3. Is [%u]\n\n", 
    _note.getToneNumber());

  Serial.printf("test::noteStack: testing note default constructor: note _noteDef.\n");
  note _noteDef;
  Serial.printf("test::noteStack: _noteDef.getToneNumber() shall be 0. Is [%u]\n", 
    _noteDef.getToneNumber());
  Serial.printf("test::noteStack: _noteDef.getNote() shall be 0. Is [%u]\n", 
    _noteDef.getNote());
  Serial.printf("test::noteStack: _noteDef.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", 
    _noteDef.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note::globalTones._array.at(_noteDef.getToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", 
    note::globalTones._array.at(_noteDef.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: calling _noteDef._setTone(_tone2) with _tone2(tones{}._array.at(2)).\n");
  Serial.printf("test::noteStack: _noteDef._setTone(_tone2) should return a tone & with i16IndexNumber equal to 2. Is [%i]\n", 
    _noteDef._setTone(_tone2));
  Serial.printf("test::noteStack: note::globalTones._array.at(_noteDef.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", 
    note::globalTones._array.at(_noteDef.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: _noteDef.getToneNumber() shall be 2. Is [%u]\n\n", 
    _noteDef.getToneNumber());

  Serial.printf("test::noteStack: testing note straight copy constructor: note{1,1}.\n");
  Serial.printf("test::noteStack: note{1,1}.getToneNumber() shall be 1. Is [%u]\n", 
    note{1,1}.getToneNumber());
  Serial.printf("test::noteStack: note{1,1}.getNote() shall be 1. Is [%u]\n", 
    note{1,1}.getNote());
  Serial.printf("test::noteStack: note{1,1}.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", 
    note{1,1}.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note::globalTones._array.at(note{1,1}.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", 
    note::globalTones._array.at(note{1,1}.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: calling note{1,1}._setTone(_tone2) with _tone2(tones{}._array.at(2)).\n");
  Serial.printf("test::noteStack: note{1,1}._setTone(_tone2) should return a tone & with i16IndexNumber equal to 2. Is [%i]\n", 
    note{1,1}._setTone(_tone2));
  Serial.printf("test::noteStack: note::globalTones._array.at(note{1,1}.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", 
    note::globalTones._array.at(note{1,1}.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: note{1,1}.getToneNumber() shall be 2. Is [%u]\n\n", 
    note{1,1}.getToneNumber());

  Serial.printf("test::noteStack: testing note copy constructor: note _noteCpy(_note);\n");
  note _noteCpy(_note);
  Serial.printf("test::noteStack: _noteCpy.getToneNumber() shall be 3. Is [%u]\n", 
    _noteCpy.getToneNumber());
  Serial.printf("test::noteStack: _noteCpy.getNote() shall be 1. Is [%u]\n", 
    _noteCpy.getNote());
  Serial.printf("test::noteStack: _noteCpy.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", 
    _noteCpy.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note::globalTones._array.at(_noteCpy.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", 
    note::globalTones._array.at(_noteCpy.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: calling _noteCpy._setTone(_tone2) with _tone(tones{}._array.at(2)).\n");
  Serial.printf("test::noteStack: _noteCpy._setTone(_tone2) should return a tone & with i16IndexNumber equal to 2. Is [%i]\n", 
    _noteCpy._setTone(_tone2));
  Serial.printf("test::noteStack: note::globalTones._array.at(_noteCpy.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", 
    note::globalTones._array.at(_noteCpy.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: _noteCpy.getToneNumber() shall be 2. Is [%u]\n\n", 
    _noteCpy.getToneNumber());

  Serial.printf("test::noteStack: testing note copy constructor: note _noteCpy2 = _note;\n");
  note _noteCpy2 = _note;
  Serial.printf("test::noteStack: _noteCpy2.getToneNumber() shall be 3. Is [%u]\n", 
    _noteCpy2.getToneNumber());
  Serial.printf("test::noteStack: _noteCpy2.getNote() shall be 1. Is [%u]\n", 
    _noteCpy2.getNote());
  Serial.printf("test::noteStack: _noteCpy2.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", 
    _noteCpy2.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note::globalTones._array.at(_noteCpy2.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", 
    note::globalTones._array.at(_noteCpy2.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: calling _noteCpy2._setTone(_tone2) with _tone(tones{}._array.at(2)).\n");
  Serial.printf("test::noteStack: _noteCpy2._setTone(_tone2) should return a tone & with i16IndexNumber equal to 2. Is [%i]\n", 
    _noteCpy2._setTone(_tone2));
  Serial.printf("test::noteStack: note::globalTones._array.at(_noteCpy2.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", 
    note::globalTones._array.at(_noteCpy2.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: _noteCpy2.getToneNumber() shall be 2. Is [%u]\n\n", 
    _noteCpy2.getToneNumber());

  Serial.printf("test::noteStack: testing note assignment operator: note _note2; then _note2 = _note;\n");
  note _note2;
  _note2 = _note;
  Serial.printf("test::noteStack: _note2.getToneNumber() shall be 1. Is [%u]\n", 
    _note2.getToneNumber());
  Serial.printf("test::noteStack: _note2.getNote() shall be 3. Is [%u]\n", 
    _note2.getNote());
  Serial.printf("test::noteStack: _note2.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", 
    _note2.ui16GetNoteDurationInMs());
  Serial.printf("test::noteStack: note::globalTones._array.at(_note2.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", 
    note::globalTones._array.at(_note2.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: calling _note2._setTone(_tone2) with _tone(tones{}._array.at(2)).\n");
  Serial.printf("test::noteStack: _note2._setTone(_tone2) should return a tone & with i16IndexNumber equal to 2. Is [%i]\n", 
    _note2._setTone(_tone2));
  Serial.printf("test::noteStack: note::globalTones._array.at(_note2.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", 
    note::globalTones._array.at(_note2.getToneNumber()).i16IndexNumber);
  Serial.printf("test::noteStack: _note2.getToneNumber() shall be 2. Is [%u]\n\n", 
    _note2.getToneNumber());

  Serial.printf("test::noteStack: testing _validNote()\n");
  Serial.printf("test::noteStack: testing _validNote() in main constructor: note _note3{1, 5}\n");
  note _note3{1, 5};
  Serial.printf("test::noteStack: _note3.getNote() shall be 6. Is [%u]\n", 
    _note3.getNote());
  Serial.printf("test::noteStack: testing _validNote() in straight copy constructor: note{1,7}\n");
  Serial.printf("test::noteStack: note{1,7}.getNote() shall be 8. Is [%u]\n", 
    note{1,7}.getNote());
  Serial.printf("test::noteStack: testing _validNote() in assignment op: _note2 = {2,11};\n");
  Serial.printf("test::noteStack: note2.getNote() shall be 16. Is [%u]\n\n", 
    note{2,11}.getNote());

  Serial.printf("test::noteStack: over\n");
}


void test::notesArrayStack() {
  Serial.printf("\n\ntest::notesArrayStack: starting\n");

  Serial.printf("test::notesArrayStack: testing _emptyNotesArray\n");
  Serial.printf("test::notesArrayStack: building std::array<note, 16> _emptyNotesArray\n");
  std::array<note, 16> _emptyNotesArray;
  Serial.printf("test::notesArrayStack: _emptyNotesArray.at(0)\n");
  Serial.printf("test::notesArrayStack: _emptyNotesArray.at(0).getToneNumber() shall be 0. Is [%u]\n", 
    _emptyNotesArray.at(0).getToneNumber());
  Serial.printf("test::notesArrayStack: _emptyNotesArray.at(15).getToneNumber() shall be 0. Is [%u]\n", 
    _emptyNotesArray.at(15).getToneNumber());
  Serial.printf("test::notesArrayStack: _emptyNotesArray.at(0).getNote() shall be 0. Is [%u]\n", 
    _emptyNotesArray.at(0).getNote());
  Serial.printf("test::notesArrayStack: _emptyNotesArray.at(15).getNote() shall be 0. Is [%u]\n", 
    _emptyNotesArray.at(15).getNote());

  Serial.printf("test::notesArrayStack: note::globalTones._array.at(_emptyNotesArray.at(0).getToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", 
    note::globalTones._array.at(_emptyNotesArray.at(0).getToneNumber()).i16IndexNumber);
  Serial.printf("test::notesArrayStack: note::globalTones._array.at(_emptyNotesArray.at(15).getToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", 
    note::globalTones._array.at(_emptyNotesArray.at(15).getToneNumber()).i16IndexNumber);
  Serial.printf("test::notesArrayStack: note::globalTones._array.at(_emptyNotesArray.at(0).getToneNumber())._laserPinsStatesArr.at(0) shall be 1. Is [%i]\n", 
    note::globalTones._array.at(_emptyNotesArray.at(0).getToneNumber())._laserPinsStatesArr.at(0));
  Serial.printf("test::notesArrayStack: note::globalTones._array.at(_emptyNotesArray.at(15).getToneNumber())._laserPinsStatesArr.at(3) shall be 1. Is [%i]\n", 
    note::globalTones._array.at(_emptyNotesArray.at(15).getToneNumber())._laserPinsStatesArr.at(3));

  Serial.printf("test::notesArrayStack: over\n\n");
}


void test::rawNotesStack() {
  Serial.printf("\n\ntest::rawNotesStack: starting\n");

  Serial.printf("test::rawNotesStack: testing notes constructor: notes _notes{}.\n");
  laserPins _laserPins;
  notes _notes{};

  Serial.printf("test::rawNotesStack: calling _notes.resetTPlayNoteToPlayNotesInBar()\n");
  _notes.resetTPlayNoteToPlayNotesInBar();

  Serial.printf("test::rawNotesStack: _notes._tones._array.at(0)._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", 
    _notes._tones._array.at(0)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(0)._laserPinsStatesArr.at(3) shall be 1. Is [%u]\n", 
    _notes._tones._array.at(0)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(0)._playTone(_laserPins) shall return 0. Returns [%i]\n", 
    _notes._tones._array.at(0)._playTone(_laserPins));

  Serial.printf("test::rawNotesStack: _notes._tones._array.at(15)._laserPinsStatesArr.at(0) shall be 0. Is [%u]\n", 
    _notes._tones._array.at(15)._laserPinsStatesArr.at(0));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(15)._laserPinsStatesArr.at(3) shall be 0. Is [%u]\n", 
    _notes._tones._array.at(15)._laserPinsStatesArr.at(3));
  Serial.printf("test::rawNotesStack: _notes._tones._array.at(15)._playTone(_laserPins) shall return 15. Returns [%i]\n", 
    _notes._tones._array.at(15)._playTone(_laserPins));

  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._i16IndexNumber shall be 0. Is [%i]\n", 
    _notes._tones._laserPins._array.at(0)._i16IndexNumber);
  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._ui16PhysicalPinNumber shall be 5. Is [%i]\n", 
    _notes._tones._laserPins._array.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", 
    _notes._tones._laserPins._array.at(0)._switchPin(LOW));
  Serial.printf("test::rawNotesStack: _notes._tones._laserPins._array.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n", 
    _notes._tones._laserPins._array.at(0)._switchPin(HIGH));
  Serial.printf("test::rawNotesStack: calling _notes.playNoteStandAlone(note{4, 1}, beat(5, 1).\n");
  _notes.playNoteStandAlone(note{4, 1}, beat(5, 1));
  note _note{1, 1};
  Serial.printf("test::rawNotesStack: calling _notes.playNoteStandAlone(_note, beat(5, 1)).\n");
  _notes.playNoteStandAlone(_note, beat(5, 1));
  Serial.printf("test::rawNotesStack: calling _bar.getNotes().setActive(note{4,8}).\n");
  _notes.setActive(note{4,8});
  Serial.printf("test::rawNotesStack: _notes._activeNote.getNote() shall be 8. Is [%u]\n", 
    _notes._activeNote.getNote());
  Serial.printf("test::rawNotesStack: _notes._activeNote.getToneNumber() shall be 4. Is [%u]\n", 
    _notes._activeNote.getToneNumber());

  Serial.printf("test::rawNotesStack: over\n");
}




void test::barStack() {
  Serial.printf("\n\ntest::barStack: starting\n");

  Serial.printf("test::barStack: testing bar constructor: bar _bar(_arrayOfNotes) with _arrayOfNotes = \n");
  Serial.printf("                std::array<note, 16> {note(4,8), note(3,8), note(2,8), note(1,8),\n");
  Serial.printf("                note(2,8), note(3,8), note(4,8), note(0,8)}\n");
  std::array<note, 16> _arrayOfNotes;
  _arrayOfNotes = {note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)};
  bar _bar(_arrayOfNotes);

  Serial.printf("test::barStack: calling _bar.setActive().\n");
  _bar.setActive();

  Serial.printf("test::barStack: _bar.ui16GetNotesCountInBar() shall be 8. Is [%u]\n", 
    _bar.ui16GetNotesCountInBar());

  Serial.printf("test::barStack: _bar.ui16GetBaseNotesCountInBar() shall be 0 (because beat has not been set). Is [%u]\n", 
    _bar.ui16GetBaseNotesCountInBar());

  Serial.printf("test::barStack: _note.ui32GetBarDuration() shall be 0 (because beat has not been set). Is [%u]\n", 
    _bar.ui32GetBarDuration());

  Serial.printf("test::barStack: _bar.getNotesArray().at(0).getNote() shall be 8. Is [%u]\n", 
    _bar.getNotesArray().at(0).getNote());

  Serial.printf("test::barStack: _bar.playBarStandAlone(beat(5, 1)) should return 0 because the active bar id has not been set. Returns [%i]\n",
    _bar.playBarStandAlone(beat(5, 1)));

  Serial.printf("test::barStack: _bar.playBarInSequence() should fail because beat is set to default (beat == 0 and base note == 0). Returns [%i]\n",
    _bar.playBarInSequence());

  Serial.printf("test::barStack: calling _bar.getNotes().setActive(note{4,8}).\n");
  _bar.getNotes().setActive(note{4,8});

  Serial.printf("test::barStack: _bar.getNotes()._activeNote.getNote() shall be 8. Is [%u]\n", 
    _bar.getNotes()._activeNote.getNote());
  Serial.printf("test::barStack: _bar.getNotes()._activeNote.getToneNumber() shall be 4. Is [%u]\n", 
    _bar.getNotes()._activeNote.getToneNumber());

  Serial.printf("test::barStack: over.\n\n");
}




void test::sequenceStack() { 
  Serial.printf("\ntest::sequenceStack: starting\n");
  beat _beat{sequence::sequences[0].getAssociatedBeat()};
  _beat.setActive();

  Serial.printf("\ntest::sequenceStack: testing sequence::sequences[0]\n");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getAssociatedBeat().getBaseNoteForBeat() shall be 1. Is [%u]\n", 
    sequence::sequences[0].getAssociatedBeat().getBaseNoteForBeat());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getAssociatedBeat().getBaseBeatInBpm() shall be 2. Is [%u]\n", 
    sequence::sequences[0].getAssociatedBeat().getBaseBeatInBpm());

  Serial.printf("\ntest::sequenceStack: testing notes in existing bar\n");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote() shall be equal to 1. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber() shall be equal to 7. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).ui16GetNoteDurationInMs() shall be equal to 3000. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).ui16GetNoteDurationInMs());

  Serial.printf("\ntest::sequenceStack: testing methods of existing bar\n");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).ui16GetNotesCountInBar() shall be equal to 2. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui16GetNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar() shall be equal to 2. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(0).ui32GetBarDuration() shall be equal to 60000. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui32GetBarDuration());

  Serial.printf("\ntest::sequenceStack: testing notes in \"beyond bound\" bar\n");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getNote() shall be equal to 0. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getNote());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getToneNumber() shall be equal to 0. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getToneNumber());

  Serial.printf("\ntest::sequenceStack: testing methods of \"beyond bound\" bar\n");
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).ui16GetNotesCountInBar() shall be equal to 0. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).ui16GetNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).ui16GetBaseNotesCountInBar() shall be equal to 2. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar());
  Serial.printf("test::sequenceStack: sequence::sequences[0].getBarsArray().at(2).ui32GetBarDuration() shall be equal to 0. Is [%u]\n", 
    sequence::sequences[0].getBarsArray().at(2).ui32GetBarDuration());

  Serial.printf("\ntest::sequenceStack: testing sequence methods\n");
  Serial.printf("test::sequenceStack: sequence::sequences[0].i16GetBarCountInSequence() shall be equal to 1. Is [%u]\n", 
    sequence::sequences[0].ui16GetBarCountInSequence());
  Serial.printf("test::sequenceStack: sequence::sequences[0].ui32GetSequenceDuration() shall be equal to 60000. Is [%u]\n", 
    sequence::sequences[0].ui32GetSequenceDuration());

  beat _defaultBeat{};
  _defaultBeat.setActive();
  Serial.printf("\ntest::sequenceStack: over\n\n");
}
