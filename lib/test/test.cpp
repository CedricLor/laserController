/*
  test.cpp - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#include "Arduino.h"
#include "test.h"



test::test()
{ }




void test::laserPinStack() {
  const char * _methodName = "test::laserPinStack:";
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

  Serial.printf("test::rawLaserPinsStack: _laserPins.at(0)._i16IndexNumber shall be 0. Is [%i]\n", 
    _laserPins.at(0)._i16IndexNumber);
  Serial.printf("test::rawLaserPinsStack: _laserPins.at(0)._ui16PhysicalPinNumber shall be 5. Is [%u]\n", 
    _laserPins.at(0)._ui16PhysicalPinNumber);
  Serial.printf("test::rawLaserPinsStack: _laserPins.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", 
    _laserPins.at(0)._switchPin(LOW));
  Serial.printf("test::rawLaserPinsStack: _laserPins.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n", 
    _laserPins.at(0)._switchPin(HIGH));

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
  Serial.printf("test::toneStack: calling _tone._playTone(_laserPins); shall be -1. Is [%i]\n", 
    _tone._playTone(_laserPins));

  Serial.printf("test::toneStack: over. \n\n");
}




void test::rawTonesStackTonesArray(const char * _methodName, uint16_t _toneIndexNumber) {
  laserPins _laserPins;
  tones _tones{};

  Serial.printf("%s _tones.at(%u)._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", _methodName, _toneIndexNumber,
    _tones.at(_toneIndexNumber)._laserPinsStatesArr.at(0));
  Serial.printf("%s _tones.at(%u)._laserPinsStatesArr.at(3) shall be 1. Is [%u]\n", _methodName, _toneIndexNumber,
    _tones.at(_toneIndexNumber)._laserPinsStatesArr.at(3));
    
  Serial.printf("%s _tones.at(%u).i16IndexNumber is [%i]\n", _methodName, _toneIndexNumber,
    _tones.at(_toneIndexNumber).i16IndexNumber);
  Serial.printf("%s _tones.at(%u)._playTone(_laserPins) is [%i]\n", _methodName, _toneIndexNumber,
    _tones.at(_toneIndexNumber)._playTone(_laserPins));
  Serial.printf("%s ( _tones.at(%u).i16IndexNumber == _tones.at(%u)._playTone(_laserPins)) shall be 1. Is [%i]\n", _methodName, _toneIndexNumber, _toneIndexNumber,
    _tones.at(_toneIndexNumber).i16IndexNumber == _tones.at(_toneIndexNumber)._playTone(_laserPins));
  Serial.printf("%s _tones._playTone(%u) shall be [%i]\n", _methodName, _toneIndexNumber,
    _tones._playTone(_toneIndexNumber));
  Serial.printf("%s (_tones.at(%u)._playTone(_laserPins)) == _tones._playTone(%u) shall be 1. Is [%i]\n\n", _methodName, _toneIndexNumber, _toneIndexNumber,
    _tones.at(_toneIndexNumber)._playTone(_laserPins) == _tones._playTone(_toneIndexNumber));
}




void test::rawTonesStack() {
  const char * _methodName = "test::rawTonesStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  Serial.printf("%s testing tones constructor: tones _tones{}.\n", _methodName);
  laserPins _laserPins;
  tones _tones{};

  rawTonesStackTonesArray(_methodName, 0);
  rawTonesStackTonesArray(_methodName, 15);

  Serial.printf("%s _tones._laserPins.at(0)._i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _tones._laserPins.at(0)._i16IndexNumber);
  Serial.printf("%s _tones._laserPins.at(0)._ui16PhysicalPinNumber shall be 5. Is [%i]\n", _methodName, 
    _tones._laserPins.at(0)._ui16PhysicalPinNumber);
  Serial.printf("%s _tones._laserPins.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", _methodName, 
    _tones._laserPins.at(0)._switchPin(LOW));
  Serial.printf("%s _tones._laserPins.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n", _methodName, 
    _tones._laserPins.at(0)._switchPin(HIGH));

  Serial.printf("%s over\n\n", _methodName);
}








void test::noteStackParamConstructor(const char * _methodName) {
  Serial.printf("%s testing constructor: note _note{1,1}.\n", _methodName);
  note _note{1, 1};
  tone _tone(tones{}.at(3));

  Serial.printf("%s _note.getToneNumber() shall be 1. Is [%u]\n", _methodName, 
    _note.getToneNumber());
  Serial.printf("%s _note.getNote() shall be 1. Is [%u]\n", _methodName, 
    _note.getNote());
  Serial.printf("%s _note.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _note.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(_note.getToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    note::globalTones.at(_note.getToneNumber()).i16IndexNumber);
  Serial.printf("%s calling _note._setTone(_tone) with _tone(tones{}.at(3)).\n", _methodName);
  Serial.printf("%s _note._setTone(_tone) shall be 3. Is [%i]\n", _methodName, 
    _note._setTone(_tone));
  Serial.printf("%s note::globalTones.at(_note.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    note::globalTones.at(_note.getToneNumber()).i16IndexNumber);
  Serial.printf("%s _note.getToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _note.getToneNumber());
  Serial.printf("%s _note._playTone() shall be 3. Is [%u]\n\n", _methodName, 
    _note._playTone());
}




void test::noteStackDefaultConstructor(const char * _methodName) {
  Serial.printf("%s testing default constructor: note _noteDef.\n", _methodName);
  note _noteDef;
  tone _tone2(tones{}.at(2));

  Serial.printf("%s _noteDef.getToneNumber() shall be 0. Is [%u]\n", _methodName, 
    _noteDef.getToneNumber());
  Serial.printf("%s _noteDef.getNote() shall be 0. Is [%u]\n", _methodName, 
    _noteDef.getNote());
  Serial.printf("%s _noteDef.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _noteDef.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(_noteDef.getToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    note::globalTones.at(_noteDef.getToneNumber()).i16IndexNumber);
  Serial.printf("%s calling _noteDef._setTone(_tone2) with _tone2(tones{}.at(2)).\n", _methodName);
  Serial.printf("%s _noteDef._setTone(_tone2) shall be 2. Is [%i]\n", _methodName, 
    _noteDef._setTone(_tone2));
  Serial.printf("%s note::globalTones.at(_noteDef.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    note::globalTones.at(_noteDef.getToneNumber()).i16IndexNumber);
  Serial.printf("%s _noteDef.getToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _noteDef.getToneNumber());
  Serial.printf("%s _noteDef._playTone() shall be 2. Is [%u]\n\n", _methodName, 
    _noteDef._playTone());
}




void test::noteStackStraightCopyConstructor(const char * _methodName) {
  Serial.printf("%s testing straight copy constructor: note{1,1}.\n", _methodName);
  
  Serial.printf("%s note{1,1}.getToneNumber() shall be 1. Is [%u]\n", _methodName, 
    note{1,1}.getToneNumber());
  Serial.printf("%s note{1,1}.getNote() shall be 1. Is [%u]\n", _methodName, 
    note{1,1}.getNote());
  Serial.printf("%s note{1,1}.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    note{1,1}.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(note{1,1}.getToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    note::globalTones.at(note{1,1}.getToneNumber()).i16IndexNumber);

  Serial.printf("%s calling note{1,1}._setTone(_tone2) with _tone2(tones{}.at(2)).\n", _methodName);
  tone _tone2(tones{}.at(2));
  Serial.printf("%s note{1,1}._setTone(_tone2) shall be 2. Is [%i]\n", _methodName, 
    note{1,1}._setTone(_tone2));
  Serial.printf("%s note::globalTones.at(note{1,1}.getToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    note::globalTones.at(note{1,1}.getToneNumber()).i16IndexNumber);
  Serial.printf("%s note{1,1}.getToneNumber() shall be 1. Is [%u]\n", _methodName, 
    note{1,1}.getToneNumber());
  Serial.printf("%s note{1,1}._playTone() shall be 1. Is [%u]\n\n", _methodName, 
    note{1,1}._playTone());
}




void test::noteStackCopyConstructorInitList(const char * _methodName) {
  Serial.printf("%s testing copy constructor: note _noteCpy(_note);\n", _methodName);
  note _note{1, 1};
  tone _tone(tones{}.at(3));
  _note._setTone(_tone);

  note _noteCpy(_note);
  tone _tone2(tones{}.at(2));

  Serial.printf("%s _noteCpy.getToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _noteCpy.getToneNumber());
  Serial.printf("%s _noteCpy.getNote() shall be 1. Is [%u]\n", _methodName, 
    _noteCpy.getNote());
  Serial.printf("%s _noteCpy.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _noteCpy.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(_noteCpy.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    note::globalTones.at(_noteCpy.getToneNumber()).i16IndexNumber);

  Serial.printf("%s calling _noteCpy._setTone(_tone2) with _tone(tones{}.at(2)).\n", _methodName);
  Serial.printf("%s _noteCpy._setTone(_tone2) shall be 2. Is [%i]\n", _methodName, 
    _noteCpy._setTone(_tone2));
  Serial.printf("%s note::globalTones.at(_noteCpy.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    note::globalTones.at(_noteCpy.getToneNumber()).i16IndexNumber);
  Serial.printf("%s _noteCpy.getToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _noteCpy.getToneNumber());
  Serial.printf("%s _noteCpy._playTone() shall be 2. Is [%u]\n\n", _methodName, 
    _noteCpy._playTone());
}




void test::noteStackCopyConstructorEqualSign(const char * _methodName) {
  Serial.printf("%s testing copy constructor: note _noteCpy2 = _note;\n", _methodName);
  note _note{1, 1};
  tone _tone(tones{}.at(3));
  _note._setTone(_tone);

  note _noteCpy2 = _note;
  tone _tone2(tones{}.at(2));

  Serial.printf("%s _noteCpy2.getToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _noteCpy2.getToneNumber());
  Serial.printf("%s _noteCpy2.getNote() shall be 1. Is [%u]\n", _methodName, 
    _noteCpy2.getNote());
  Serial.printf("%s _noteCpy2.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _noteCpy2.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(_noteCpy2.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    note::globalTones.at(_noteCpy2.getToneNumber()).i16IndexNumber);

  Serial.printf("%s calling _noteCpy2._setTone(_tone2) with _tone(tones{}.at(2)).\n", _methodName);
  Serial.printf("%s _noteCpy2._setTone(_tone2) shall be 2. Is [%i]\n", _methodName, 
    _noteCpy2._setTone(_tone2));
  Serial.printf("%s note::globalTones.at(_noteCpy2.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    note::globalTones.at(_noteCpy2.getToneNumber()).i16IndexNumber);
  Serial.printf("%s _noteCpy2.getToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _noteCpy2.getToneNumber());
  Serial.printf("%s _noteCpy2._playTone() shall be 2. Is [%u]\n\n", _methodName, 
    _noteCpy2._playTone());
}




void test::noteStackCopyAssignmentOp(const char * _methodName) {
  Serial.printf("%s testing assignment operator: note _note2; then _note2 = _note;\n", _methodName);
  note _note{1, 1};
  tone _tone(tones{}.at(3));
  _note._setTone(_tone);

  note _note2;
  _note2 = _note;
  tone _tone2(tones{}.at(2));

  Serial.printf("%s _note2.getToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _note2.getToneNumber());
  Serial.printf("%s _note2.getNote() shall be 1. Is [%u]\n", _methodName, 
    _note2.getNote());
  Serial.printf("%s _note2.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _note2.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(_note2.getToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    note::globalTones.at(_note2.getToneNumber()).i16IndexNumber);
    
  Serial.printf("%s calling _note2._setTone(_tone2) with _tone(tones{}.at(2)).\n", _methodName);
  Serial.printf("%s _note2._setTone(_tone2) shall be 2. Is [%i]\n", _methodName, 
    _note2._setTone(_tone2));
  Serial.printf("%s note::globalTones.at(_note2.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    note::globalTones.at(_note2.getToneNumber()).i16IndexNumber);
  Serial.printf("%s _note2.getToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _note2.getToneNumber());
  Serial.printf("%s _note2._playTone() shall be 2. Is [%u]\n\n", _methodName, 
    _note2._playTone());
}





void test::noteStackCopyConstructorAndCopyAssignment(const char * _methodName) {
  Serial.printf("%s testing straight copy constructor + assignt op: _note4 = note{1,1}.\n", _methodName);
  note _note4;
  _note4 = note{1,1};
  tone _tone2(tones{}.at(2));

  Serial.printf("%s _note4.getToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _note4.getToneNumber());
  Serial.printf("%s _note4.getNote() shall be 1. Is [%u]\n", _methodName, 
    _note4.getNote());
  Serial.printf("%s _note4.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _note4.ui16GetNoteDurationInMs());
  Serial.printf("%s note::globalTones.at(_note4.getToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    note::globalTones.at(_note4.getToneNumber()).i16IndexNumber);

  Serial.printf("%s calling _note4._setTone(_tone2) with _tone(tones{}.at(2)).\n", _methodName);
  Serial.printf("%s _note4._setTone(_tone2) shall be 1. Is [%i]\n", _methodName, 
    _note4._setTone(_tone2));
  Serial.printf("%s note::globalTones.at(_note4.getToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    note::globalTones.at(_note4.getToneNumber()).i16IndexNumber);
  Serial.printf("%s _note4.getToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _note4.getToneNumber());
  Serial.printf("%s _note4._playTone() shall be 2. Is [%u]\n\n", _methodName, 
    _note4._playTone());
}





void test::noteStackValidNote(const char * _methodName) {
  Serial.printf("%s testing _validNote()\n", _methodName);
  Serial.printf("%s testing _validNote() in main constructor: note _note3{1, 5}\n", _methodName);
  note _note3{1, 5};

  Serial.printf("%s _note3.getNote() shall be 6. Is [%u]\n", _methodName, 
    _note3.getNote());
  Serial.printf("%s testing _validNote() in straight copy constructor: note{1,7}\n", _methodName);
  Serial.printf("%s note{1,7}.getNote() shall be 8. Is [%u]\n", _methodName, 
    note{1,7}.getNote());
  Serial.printf("%s testing _validNote() in assignment op: _note2 = {2,11};\n", _methodName);
  Serial.printf("%s note2.getNote() shall be 16. Is [%u]\n\n", _methodName, 
    note{2,11}.getNote());
}




void test::noteStack() {
  const char * _methodName = "test::noteStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  noteStackParamConstructor(_methodName);
  noteStackDefaultConstructor(_methodName);
  noteStackStraightCopyConstructor(_methodName);
  noteStackCopyConstructorInitList(_methodName);
  noteStackCopyConstructorEqualSign(_methodName);
  noteStackCopyAssignmentOp(_methodName);
  noteStackCopyConstructorAndCopyAssignment(_methodName);
  noteStackValidNote(_methodName);

  Serial.printf("%s over\n", _methodName);
}




void test::notesArrayStackEmptyArray(const char * _methodName) {
  Serial.printf("%s testing _emptyNotesArray\n", _methodName);
  Serial.printf("%s building std::array<note, 16> _emptyNotesArray\n", _methodName);
  std::array<note, 16> _emptyNotesArray;

  Serial.printf("%s _emptyNotesArray.at(0)\n", _methodName);
  Serial.printf("%s _emptyNotesArray.at(0).getToneNumber() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(0).getToneNumber());
  Serial.printf("%s _emptyNotesArray.at(15).getToneNumber() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(15).getToneNumber());
  Serial.printf("%s _emptyNotesArray.at(0).getNote() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(0).getNote());
  Serial.printf("%s _emptyNotesArray.at(15).getNote() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(15).getNote());
}



void test::notesArrayStackToneSelection(const char * _methodName) {
  std::array<note, 16> _emptyNotesArray;

  Serial.printf("%s note::globalTones.at(_emptyNotesArray.at(0).getToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    note::globalTones.at(_emptyNotesArray.at(0).getToneNumber()).i16IndexNumber);
  Serial.printf("%s note::globalTones.at(_emptyNotesArray.at(15).getToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    note::globalTones.at(_emptyNotesArray.at(15).getToneNumber()).i16IndexNumber);
  Serial.printf("%s note::globalTones.at(_emptyNotesArray.at(0).getToneNumber())._laserPinsStatesArr.at(0) shall be 1. Is [%i]\n", _methodName, 
    note::globalTones.at(_emptyNotesArray.at(0).getToneNumber())._laserPinsStatesArr.at(0));
  Serial.printf("%s note::globalTones.at(_emptyNotesArray.at(15).getToneNumber())._laserPinsStatesArr.at(3) shall be 1. Is [%i]\n", _methodName, 
    note::globalTones.at(_emptyNotesArray.at(15).getToneNumber())._laserPinsStatesArr.at(3));
}




void test::notesArrayStack() {
  const char * _methodName = "test::notesArrayStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  notesArrayStackEmptyArray(_methodName);
  notesArrayStackToneSelection(_methodName);

  Serial.printf("%s over\n\n", _methodName);
}




void test::rawNotesStackSender(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note) {
    /** TODO: draft whatever  */
}




void test::rawNotesStackConstructors(const char * _methodName) {  
  Serial.printf("%s testing notes constructors.\n", _methodName);

  Serial.printf("%s testing notes default constructor: notes _notes{}.\n", _methodName);
  notes _notes{};
  Serial.printf("%s testing notes default constructor with params: notes _notes2{nullptr}.\n", _methodName);
  notes _notes2{nullptr};
  Serial.printf("%s testing notes default constructor with params: notes _notes3{rawNotesStackSender}.\n", _methodName);
  notes _notes3{rawNotesStackSender};

  Serial.printf("%s testing notes straigth copy constructor: notes{}.\n", _methodName);
  notes{};
  Serial.printf("%s testing notes copy constructor with initializer: notes _noteCpy(_notes).\n", _methodName);
  notes _notesCpy(_notes);
  Serial.printf("%s testing notes copy constructor with equal sign: notes _noteCpy(_notes).\n", _methodName);
  notes _notesCpy2 = _notes;
  Serial.printf("%s testing notes assignment op: _notes = _notes3.\n", _methodName);
  _notes = _notes3;

  Serial.printf("%s testing constructor: over.\n\n", _methodName);
}




void test::rawNotesStackDefaultResetters(const char * _methodName, laserPins & _laserPins, notes & _notes) {
  // Resetters
  Serial.printf("%s testing Task tPlayNote resetters \n", _methodName);
  Serial.printf("%s calling _notes.resetTPlayNote()\n", _methodName);
  _notes.resetTPlayNote();
  Serial.printf("%s calling _notes.disableAndResetTPlayNote()\n\n", _methodName);
  _notes.disableAndResetTPlayNote();  
}




void test::rawNotesStackDefaultMembers(const char * _methodName, notes & _notes) {
  // Members
  Serial.printf("%s testing access to members of _notes \n", _methodName);
  Serial.printf("%s testing access to _notes._tones \n", _methodName);
  Serial.printf("%s testing access to and values of the _laserPinsStatesArr embedded in \n", _methodName);
  Serial.printf("   tone instances extracted from the \"hard-coded\" tones array embedded in the tones class\n");
  Serial.printf("   from the default _notes instance.\n");
  Serial.printf("%s _notes._tones.at(0)._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", _methodName, 
    _notes._tones.at(0)._laserPinsStatesArr.at(0));
  Serial.printf("%s _notes._tones.at(0)._laserPinsStatesArr.at(3) shall be 1. Is [%u]\n", _methodName, 
    _notes._tones.at(0)._laserPinsStatesArr.at(3));
  Serial.printf("%s _notes._tones.at(15)._laserPinsStatesArr.at(0) shall be 0. Is [%u]\n", _methodName, 
    _notes._tones.at(15)._laserPinsStatesArr.at(0));
  Serial.printf("%s _notes._tones.at(15)._laserPinsStatesArr.at(3) shall be 0. Is [%u]\n\n", _methodName, 
    _notes._tones.at(15)._laserPinsStatesArr.at(3));
}




void test::rawNotesStackDefaultPlayTone(const char * _methodName, laserPins & _laserPins, notes & _notes) {
  // _notes->_playTone
  Serial.printf("%s _notes._tones.at(0)._playTone(_laserPins) shall be 0. Is [%i]\n", _methodName, 
    _notes._tones.at(0)._playTone(_laserPins));
  Serial.printf("%s _notes._tones.at(15)._playTone(_laserPins) shall be 15. Is [%i]\n\n", _methodName, 
    _notes._tones.at(15)._playTone(_laserPins));
}




void test::rawNotesStackDefaultLaserPins(const char * _methodName, notes & _notes) {
  // _notes._tones_.laserPins
  Serial.printf("%s testing access to _notes._tones._laserPins \n", _methodName);
  Serial.printf("%s _notes._tones._laserPins.at(0)._i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _notes._tones._laserPins.at(0)._i16IndexNumber);
  Serial.printf("%s _notes._tones._laserPins.at(0)._ui16PhysicalPinNumber shall be 5. Is [%i]\n", _methodName, 
    _notes._tones._laserPins.at(0)._ui16PhysicalPinNumber);
  Serial.printf("%s _notes._tones._laserPins.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", _methodName, 
    _notes._tones._laserPins.at(0)._switchPin(LOW));
  Serial.printf("%s _notes._tones._laserPins.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n\n", _methodName, 
    _notes._tones._laserPins.at(0)._switchPin(HIGH));
}




void test::rawNotesStackDefaultSettersAndGetters(const char * _methodName, notes & _notes) {
  // testing _notes.setActive, _notes._activeNote.getNote and _notes._activeNote.getToneNumber
  Serial.printf("%s testing setActive, getNote and getToneNumber\n", _methodName);
  _notes.disableAndResetTPlayNote();
  Serial.printf("%s calling _notes.setActive(note{4,8}).\n", _methodName);
  _notes.setActive(note{4,8});
  Serial.printf("%s _notes._activeNote.getNote() shall be 8. Is [%u]\n", _methodName, 
    _notes._activeNote.getNote());
  Serial.printf("%s _notes._activeNote.getToneNumber() shall be 4. Is [%u]\n\n", _methodName, 
    _notes._activeNote.getToneNumber());
}




void test::rawNotesStackDefaultPlayers(const char * _methodName, notes & _notes) {
  // testing _notes.playNoteStandAlone
  _notes.disableAndResetTPlayNote();
  Serial.printf("%s calling _notes.playNoteStandAlone(note{4, 1}, beat(5, 1).\n", _methodName);
  _notes.playNoteStandAlone(note{4, 1}, beat(5, 1));
  note _note{1, 1};
  _notes.disableAndResetTPlayNote();
  Serial.printf("%s calling _notes.playNoteStandAlone(_note, beat(5, 1)) with note _note{1, 1}.\n\n", _methodName);
  _notes.playNoteStandAlone(_note, beat(5, 1));
  _notes.disableAndResetTPlayNote();

  // testing _notes.playNoteInBar
  Serial.printf("%s testing _notes.playNoteInBar\n", _methodName);
  Serial.printf("%s setting active beat to beat(5, 1)\n", _methodName);
  activeBeat = beat(5, 1);

  _notes.disableAndResetTPlayNote();
  Serial.printf("%s calling _notes.playNoteInBar(note{4, 1}.\n", _methodName);
  _notes.playNoteInBar(note{4, 1});
  _notes.disableAndResetTPlayNote();
  Serial.printf("%s calling _notes.playNoteStandAlone(_note).\n\n", _methodName);
  _notes.playNoteInBar(_note);
  _notes.disableAndResetTPlayNote();
}




void test::rawNotesStackDefault(const char * _methodName) {
  laserPins _laserPins;
  notes _notes{};

  rawNotesStackDefaultResetters(_methodName, _laserPins, _notes);
  rawNotesStackDefaultMembers(_methodName, _notes);
  rawNotesStackDefaultPlayTone(_methodName, _laserPins, _notes);
  rawNotesStackDefaultLaserPins(_methodName, _notes);
  rawNotesStackDefaultSettersAndGetters(_methodName, _notes);
  rawNotesStackDefaultPlayers(_methodName, _notes);

}




void test::rawNotesStack() {
  const char * _methodName = "test::rawNotesStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  rawNotesStackConstructors(_methodName);
  rawNotesStackDefault(_methodName);

  Serial.printf("%s over\n", _methodName);
}





void test::barStackConstructors(const char * _methodName) {
  activeBeat = beat(5, 1);

  Serial.printf("%s testing bar default constructor: bar _bar;\n", _methodName);
  bar _emptyBar;
  barStackGetters(_methodName, _emptyBar);
  Serial.printf("%s testing bar default constructor: everything above should be at 0;\n", _methodName);

  Serial.printf("%s testing bar constructor: bar _bar{std::array<note, 16>{note(4,8), note(3,8),\n", _methodName);
  Serial.printf("                            note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)}\n");
  bar _bar{std::array<note, 16>{note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)}};
  barStackGetters(_methodName, _bar);

  Serial.printf("%s testing bar copy constructor: bar _barCpy(_bar)\n", _methodName);
  bar _barCpy(_bar);
  barStackGetters(_methodName, _barCpy);

  Serial.printf("%s testing bar copy assignment: _bar = _barCpy\n", _methodName);
  _bar = _barCpy;
  barStackGetters(_methodName, _bar);

  Serial.printf("%s testing bar copy constructor + assignment: _bar = bar(_barCpy)\n", _methodName);
  barStackGetters(_methodName, _bar);
}





void test::barStackGetters(const char * _methodName, bar & _bar) {
  // getters
  Serial.printf("%s _bar.ui16GetNotesCountInBar() shall be 8. Is [%u]\n", _methodName, 
    _bar.ui16GetNotesCountInBar());
  Serial.printf("%s _bar.ui16GetBaseNotesCountInBar() shall be 1. Is [%u]\n", _methodName, 
    _bar.ui16GetBaseNotesCountInBar());
  const uint32_t _ui32ExpectGetBarDuration = 12000;
  Serial.printf("%s _bar.ui32GetBarDuration() shall be %u. Is [%u]. ---> %s\n", _methodName, _ui32ExpectGetBarDuration,
    _bar.ui32GetBarDuration(), ((_bar.ui32GetBarDuration() == _ui32ExpectGetBarDuration) ? "PASSED" : "FAILED"));
  Serial.printf("%s _bar.getNotesArray().at(0).getNote() shall be 8. Is [%u]\n\n", _methodName, 
    _bar.getNotesArray().at(0).getNote());
}





void test::barStack() {
  const char * _methodName = "test::barStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  barStackConstructors(_methodName);

  Serial.printf("%s over.\n\n", _methodName);
}





void test::barArrayStackBarGetters(const char * _methodName, std::array<bar, 7> & _barsArray) {
  Serial.printf("%s testing access to _barsArray[0] properties\n", _methodName);
  barStackGetters(_methodName, _barsArray[0]);

  Serial.printf("%s testing access to _barsArray[1] properties\n", _methodName);
  barStackGetters(_methodName, _barsArray[1]);
  Serial.printf("%s testing access to _barsArray[1] props: everythg above should be at 0;\n", _methodName);
}





void test::barArrayStack() {
  const char * _methodName = "test::barArrayStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  activeBeat = beat(5, 1);

  Serial.printf("%s testing bar instance's creation and assignment as in bars default (and only) constructor\n", _methodName);

  Serial.printf("%s creating std::array<bar, 7> _barsArray (empty bars array)\n", _methodName);
  std::array<bar, 7> _barsArray;
  Serial.printf("%s creating std::array<bar, 7> _notesArray {note(4,8), note(3,8), ...\n", _methodName);
  std::array<note, 16> _notesArray {note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)};
  Serial.printf("%s bar copy and assignemnt: _barsArray[0] = {_notesArray}\n", _methodName);
  _barsArray[0] = {_notesArray};
  Serial.printf("%s bar instance's created and assigned to _barsArray[0]\n", _methodName);

  barArrayStackBarGetters(_methodName, _barsArray);

  Serial.printf("%s over.\n\n", _methodName);
}





void test::rawBarsStackConstructors(const char * _methodName) {
  Serial.printf("%s testing default constructor bars _bars\n", _methodName);
  bars _bars;
}




void test::rawBarsStackInitializers(const char * _methodName) {
  bars _bars;
  bar _bar{std::array<note, 16>{note(4,8), note(3,8), note(2,8), note(1,8), note(2,8), note(3,8), note(4,8), note(0,8)}};
  Serial.printf("%s testing _bars.setActive(_bar) with bar _bar{std::array<note, 16>{note(4,8), note(3,8)...} \n", _methodName);
  rawBarsStackStandAlonePlayer(_methodName, _bars, _bar);
  rawBarsStackBarInSequencePlayer(_methodName, _bars);
  _bars.setActive(_bar);
  rawBarsStackNestedNotes(_methodName, _bars);
  rawBarsStackTaskCallbacks(_methodName, _bars);
  rawBarsStackTaskAccessFromAndToActiveBar(_methodName, _bars);
}




void test::rawBarsStackStandAlonePlayer(const char * _methodName, bars & _bars, bar & _bar) {

  Serial.printf("%s testing players\n", _methodName);

  // players
  Serial.printf("%s calling _bars.disableAndResetTPlayBar()\n", _methodName);
  _bars.disableAndResetTPlayBar();
  Serial.printf("%s _bars.playBarStandAlone(_bar, beat(5, 1)) shall be 1. Is [%i]\n", _methodName,
    _bars.playBarStandAlone(_bar, beat(5, 1)));
  _bars.disableAndResetTPlayBar();
}




void test::rawBarsStackBarInSequencePlayer(const char * _methodName, bars & _bars) {

  Serial.printf("%s testing players\n", _methodName);

  // players
  Serial.printf("%s calling _bars.disableAndResetTPlayBar()\n", _methodName);
  _bars.disableAndResetTPlayBar();
  Serial.printf("%s _bar.playBarInSequence() shall fail because beat is set to default (beat == 0 and base note == 0). Is [%i]\n", _methodName,
    _bars.playBarInSequence(_bars._barsArray.at(0)));
  _bars.disableAndResetTPlayBar();
}




void test::rawBarsStackNestedNotes(const char * _methodName, bars & _bars) {
  Serial.printf("%s testing nested notes \n", _methodName);
  // nested notes class (including notes array)
  Serial.printf("%s calling _bars.getNotes().setActive(note{4,8}).\n", _methodName);
  _bars.getNotes().setActive(note{4,8});
  Serial.printf("%s _bars.getNotes().getCurrentNote().getNote() shall be 8. Is [%u]\n", _methodName, 
    _bars.getNotes().getCurrentNote().getNote());
  Serial.printf("%s _bars.getNotes()._activeNote.getToneNumber() shall be 4. Is [%u]\n", _methodName, 
    _bars.getNotes().getCurrentNote().getToneNumber());
}




void test::rawBarsStackTaskCallbacks(const char * _methodName, bars & _bars) {

  Serial.printf("%s testing Task callbacks \n", _methodName);
  // Task TPlayBar callbacks
  Serial.printf("%s calling _bars._tcbPlayBar().\n", _methodName);
  _bars._tcbPlayBar();
  Serial.printf("%s _bars._oetcbPlayBar() shall be 1. Is [%i]\n", _methodName, _bars._oetcbPlayBar());
  _bars.disableAndResetTPlayBar();
}




void test::rawBarsStackTaskAccessFromAndToActiveBar(const char * _methodName, bars & _bars) {

  Serial.printf("%s _bars._activeBar.ui16GetNotesCountInBar() is [%u]\n", _methodName, 
    _bars._activeBar.ui16GetNotesCountInBar());

  activeBeat = beat(0, 0);

  Serial.printf("%s _bars._activeBar.ui16GetBaseNotesCountInBar() shall be 0 (because beat has been set to default). Is [%u]\n", _methodName, 
    _bars._activeBar.ui16GetBaseNotesCountInBar());
  Serial.printf("%s _bars._activeBar.ui32GetBarDuration() shall be 0 (because beat has been set to default). Is [%u]\n", _methodName, 
    _bars._activeBar.ui32GetBarDuration());

  Serial.printf("%s _bars._activeBar.getNotesArray().at(0).getNote() is [%u]\n", _methodName,   
  _bars._activeBar.getNotesArray().at(0).getNote());
  Serial.printf("%s _bars._activeBar.getNotesArray().at(0).getToneNumber() is [%u]\n", _methodName,   
  _bars._activeBar.getNotesArray().at(0).getToneNumber());

  activeBeat = beat(5, 1);

  Serial.printf("%s _bars._activeBar.ui16GetBaseNotesCountInBar() is [%u]\n", _methodName, 
    _bars._activeBar.ui16GetBaseNotesCountInBar());
  Serial.printf("%s _bars._activeBar.ui32GetBarDuration() is [%u]\n", _methodName, 
    _bars._activeBar.ui32GetBarDuration());
}




void test::rawBarsStack() {
  const char * _methodName = "test::rawBarsStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  rawBarsStackConstructors(_methodName);
  rawBarsStackInitializers(_methodName);

  Serial.printf("%s over.\n\n", _methodName);
}




void test::implementedBarsStack() {
  const char * _methodName = "test::rawBarsStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  sequence::globalBars.setActive(sequence::globalBars._barsArray.at(1));
  rawBarsStackNestedNotes(_methodName, sequence::globalBars);
  rawBarsStackTaskCallbacks(_methodName, sequence::globalBars);
  rawBarsStackTaskAccessFromAndToActiveBar(_methodName, sequence::globalBars);

  Serial.printf("%s over.\n\n", _methodName);
}




void test::sequenceStack() {
  const char * _methodName = "test::sequenceStack:";
  Serial.printf("\n%s starting\n", _methodName);
  activeBeat = beat{sequence::sequences[0].getAssociatedBeat()};

  Serial.printf("\n%s testing sequence::sequences[0]\n", _methodName);
  Serial.printf("%s sequence::sequences[0].getAssociatedBeat().getBaseNoteForBeat() shall be 1. Is [%u]\n", _methodName, 
    sequence::sequences[0].getAssociatedBeat().getBaseNoteForBeat());
  Serial.printf("%s sequence::sequences[0].getAssociatedBeat().getBaseBeatInBpm() shall be 2. Is [%u]\n", _methodName, 
    sequence::sequences[0].getAssociatedBeat().getBaseBeatInBpm());

  Serial.printf("\n%s testing notes in existing bar\n", _methodName);
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote() shall be equal to 1. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getNote());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber() shall be equal to 7. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).getToneNumber());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).ui16GetNoteDurationInMs() shall be equal to 3000. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).getNotesArray().at(0).ui16GetNoteDurationInMs());

  Serial.printf("\n%s testing methods of existing bar\n", _methodName);
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(0).ui16GetNotesCountInBar() shall be equal to 2. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).ui16GetNotesCountInBar());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar() shall be equal to 2. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(0).ui32GetBarDuration() shall be equal to 60000. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).ui32GetBarDuration());

  Serial.printf("\n%s testing notes in \"beyond bound\" bar\n", _methodName);
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getNote() shall be equal to 0. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getNote());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getToneNumber() shall be equal to 0. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(2).getNotesArray().at(0).getToneNumber());

  Serial.printf("\n%s testing methods of \"beyond bound\" bar\n", _methodName);
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(2).ui16GetNotesCountInBar() shall be equal to 0. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(2).ui16GetNotesCountInBar());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(2).ui16GetBaseNotesCountInBar() shall be equal to 2. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(0).ui16GetBaseNotesCountInBar());
  Serial.printf("%s sequence::sequences[0].getBarsArray().at(2).ui32GetBarDuration() shall be equal to 0. Is [%u]\n", _methodName, 
    sequence::sequences[0].getBarsArray().at(2).ui32GetBarDuration());

  Serial.printf("\n%s testing sequence methods\n", _methodName);
  Serial.printf("%s sequence::sequences[0].i16GetBarCountInSequence() shall be equal to 1. Is [%u]\n", _methodName, 
    sequence::sequences[0].ui16GetBarCountInSequence());
  Serial.printf("%s sequence::sequences[0].ui32GetSequenceDuration() shall be equal to 60000. Is [%u]\n", _methodName, 
    sequence::sequences[0].ui32GetSequenceDuration());

  activeBeat = beat{};
  Serial.printf("\n%s over\n\n", _methodName);
}
