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






void test::laserToneStack() {
  Serial.printf("\n\ntest::laserToneStack: starting\n");

  Serial.printf("test::laserToneStack: testing laserTone constructor: laserTone _laserTone{{HIGH, HIGH, LOW, LOW}}.\n");
  laserPins _laserPins;
  std::array<bool, 4> const _aSecondPairOn = {HIGH, HIGH, LOW, LOW};
  laserTone _laserTone{_aSecondPairOn};

  Serial.printf("test::laserToneStack: _laserTone._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", 
    _laserTone._laserPinsStatesArr.at(0));
  Serial.printf("test::laserToneStack: _laserTone._laserPinsStatesArr.at(3) shall be 0. Is [%u]\n", 
    _laserTone._laserPinsStatesArr.at(3));
  Serial.printf("test::laserToneStack: _laserTone.i16IndexNumber shall be -1. Is [%i]\n", 
    _laserTone.i16IndexNumber);
  Serial.printf("test::laserToneStack: calling _laserTone._playLaserTone(_laserPins); shall be -1. Is [%i]\n", 
    _laserTone._playLaserTone(_laserPins));

  Serial.printf("test::laserToneStack: over. \n\n");
}




void test::rawLaserTonesStackLaserTonesArray(const char * _methodName, uint16_t _laserToneIndexNumber) {
  laserPins _laserPins;
  laserTones _laserTones{};

  Serial.printf("%s _laserTones.at(%u)._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", _methodName, _laserToneIndexNumber,
    _laserTones.at(_laserToneIndexNumber)._laserPinsStatesArr.at(0));
  Serial.printf("%s _laserTones.at(%u)._laserPinsStatesArr.at(3) shall be 1. Is [%u]\n", _methodName, _laserToneIndexNumber,
    _laserTones.at(_laserToneIndexNumber)._laserPinsStatesArr.at(3));
    
  Serial.printf("%s _laserTones.at(%u).i16IndexNumber is [%i]\n", _methodName, _laserToneIndexNumber,
    _laserTones.at(_laserToneIndexNumber).i16IndexNumber);
  Serial.printf("%s _laserTones.at(%u)._playLaserTone(_laserPins) is [%i]\n", _methodName, _laserToneIndexNumber,
    _laserTones.at(_laserToneIndexNumber)._playLaserTone(_laserPins));
  Serial.printf("%s ( _laserTones.at(%u).i16IndexNumber == _laserTones.at(%u)._playLaserTone(_laserPins)) shall be 1. Is [%i]\n", _methodName, _laserToneIndexNumber, _laserToneIndexNumber,
    _laserTones.at(_laserToneIndexNumber).i16IndexNumber == _laserTones.at(_laserToneIndexNumber)._playLaserTone(_laserPins));
  Serial.printf("%s _laserTones._playLaserTone(%u) shall be [%i]\n", _methodName, _laserToneIndexNumber,
    _laserTones._playLaserTone(_laserToneIndexNumber));
  Serial.printf("%s (_laserTones.at(%u)._playLaserTone(_laserPins)) == _laserTones._playLaserTone(%u) shall be 1. Is [%i]\n\n", _methodName, _laserToneIndexNumber, _laserToneIndexNumber,
    _laserTones.at(_laserToneIndexNumber)._playLaserTone(_laserPins) == _laserTones._playLaserTone(_laserToneIndexNumber));
}




void test::rawLaserTonesStack() {
  const char * _methodName = "test::rawLaserTonesStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  Serial.printf("%s testing laserTones constructor: laserTones _laserTones{}.\n", _methodName);
  laserPins _laserPins;
  laserTones _laserTones{};

  rawLaserTonesStackLaserTonesArray(_methodName, 0);
  rawLaserTonesStackLaserTonesArray(_methodName, 15);

  Serial.printf("%s _laserTones._laserPins.at(0)._i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _laserTones._laserPins.at(0)._i16IndexNumber);
  Serial.printf("%s _laserTones._laserPins.at(0)._ui16PhysicalPinNumber shall be 5. Is [%i]\n", _methodName, 
    _laserTones._laserPins.at(0)._ui16PhysicalPinNumber);
  Serial.printf("%s _laserTones._laserPins.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", _methodName, 
    _laserTones._laserPins.at(0)._switchPin(LOW));
  Serial.printf("%s _laserTones._laserPins.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n", _methodName, 
    _laserTones._laserPins.at(0)._switchPin(HIGH));

  Serial.printf("%s over\n\n", _methodName);
}








void test::noteStackParamConstructor(const char * _methodName) {
  Serial.printf("%s testing constructor: laserNote _note{1,1}.\n", _methodName);
  laserTones _laserTones;
  laserNote _note{1, 1};
  laserTone _laser_tone(_laserTones.at(3));

  Serial.printf("%s _note.getLaserToneNumber() shall be 1. Is [%u]\n", _methodName, 
    _note.getLaserToneNumber());
  Serial.printf("%s _note.getNote() shall be 1. Is [%u]\n", _methodName, 
    _note.getNote());
  
  beat _beat;
  Serial.printf("%s _note.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _note.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(_note.getLaserToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    _laserTones.at(_note.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s calling _note._setLaserTone(_laser_tone) with _laser_tone(_laserTones.at(3)).\n", _methodName);
  Serial.printf("%s _note._setLaserTone(_laser_tone) shall be 3. Is [%i]\n", _methodName, 
    _note._setLaserTone(_laser_tone));
  Serial.printf("%s _laserTones.at(_note.getLaserToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    _laserTones.at(_note.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _note.getLaserToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _note.getLaserToneNumber());
  Serial.printf("%s _note._playLaserTone() shall be 3. Is [%u]\n\n", _methodName, 
    _note._playLaserTone(_laserTones));
}




void test::noteStackDefaultConstructor(const char * _methodName) {
  Serial.printf("%s testing default constructor: laserNote _noteDef.\n", _methodName);
  laserTones _laserTones;
  laserNote _noteDef;
  laserTone _laserTone2(_laserTones.at(2));

  Serial.printf("%s _noteDef.getLaserToneNumber() shall be 0. Is [%u]\n", _methodName, 
    _noteDef.getLaserToneNumber());
  Serial.printf("%s _noteDef.getNote() shall be 0. Is [%u]\n", _methodName, 
    _noteDef.getNote());

  beat _beat;

  Serial.printf("%s _noteDef.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _noteDef.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(_noteDef.getLaserToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _laserTones.at(_noteDef.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s calling _noteDef._setLaserTone(_laserTone2) with _laserTone2(_laserTones.at(2)).\n", _methodName);
  Serial.printf("%s _noteDef._setLaserTone(_laserTone2) shall be 2. Is [%i]\n", _methodName, 
    _noteDef._setLaserTone(_laserTone2));
  Serial.printf("%s _laserTones.at(_noteDef.getLaserToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    _laserTones.at(_noteDef.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _noteDef.getLaserToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _noteDef.getLaserToneNumber());
  Serial.printf("%s _noteDef._playLaserTone() shall be 2. Is [%u]\n\n", _methodName, 
    _noteDef._playLaserTone(_laserTones));
}




void test::noteStackStraightCopyConstructor(const char * _methodName) {
  Serial.printf("%s testing straight copy constructor: laserNote{1,1}.\n", _methodName);

  laserTones _laserTones;


  Serial.printf("%s laserNote{1,1}.getLaserToneNumber() shall be 1. Is [%u]\n", _methodName, 
    laserNote{1,1}.getLaserToneNumber());
  Serial.printf("%s laserNote{1,1}.getNote() shall be 1. Is [%u]\n", _methodName, 
    laserNote{1,1}.getNote());

  beat _beat;

  Serial.printf("%s laserNote{1,1}.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    laserNote{1,1}.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(laserNote{1,1}.getLaserToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    _laserTones.at(laserNote{1,1}.getLaserToneNumber()).i16IndexNumber);

  Serial.printf("%s calling laserNote{1,1}._setLaserTone(_laser_tone2) with _laser_tone2(laserTones{}.at(2)).\n", _methodName);
  laserTone _laser_tone2(laserTones{}.at(2));
  Serial.printf("%s laserNote{1,1}._setLaserTone(_laser_tone2) shall be 2. Is [%i]\n", _methodName, 
    laserNote{1,1}._setLaserTone(_laser_tone2));
  Serial.printf("%s _laserTones.at(laserNote{1,1}.getLaserToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    _laserTones.at(laserNote{1,1}.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s laserNote{1,1}.getLaserToneNumber() shall be 1. Is [%u]\n", _methodName, 
    laserNote{1,1}.getLaserToneNumber());
  Serial.printf("%s laserNote{1,1}._playLaserTone() shall be 1. Is [%u]\n\n", _methodName, 
    laserNote{1,1}._playLaserTone(laserTones{}));
}




void test::noteStackCopyConstructorInitList(const char * _methodName) {
  Serial.printf("%s testing copy constructor: laserNote _noteCpy(_note);\n", _methodName);
  laserTones _laserTones;

  laserNote _note{1, 1};
  laserTone _laserTone(_laserTones.at(3));
  _note._setLaserTone(_laserTone);

  laserNote _noteCpy(_note);
  laserTone _laserTone2(_laserTones.at(2));

  Serial.printf("%s _noteCpy.getLaserToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _noteCpy.getLaserToneNumber());
  Serial.printf("%s _noteCpy.getNote() shall be 1. Is [%u]\n", _methodName, 
    _noteCpy.getNote());

  beat _beat;

  Serial.printf("%s _noteCpy.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _noteCpy.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(_noteCpy.getLaserToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    _laserTones.at(_noteCpy.getLaserToneNumber()).i16IndexNumber);

  Serial.printf("%s calling _noteCpy._setLaserTone(_laserTone2) with _laserTone(_laserTones.at(2)).\n", _methodName);
  Serial.printf("%s _noteCpy._setLaserTone(_laserTone2) shall be 2. Is [%i]\n", _methodName, 
    _noteCpy._setLaserTone(_laserTone2));
  Serial.printf("%s _laserTones.at(_noteCpy.getLaserToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    _laserTones.at(_noteCpy.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _noteCpy.getLaserToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _noteCpy.getLaserToneNumber());
  Serial.printf("%s _noteCpy._playLaserTone() shall be 2. Is [%u]\n\n", _methodName, 
    _noteCpy._playLaserTone(_laserTones));
}




void test::noteStackCopyConstructorEqualSign(const char * _methodName) {
  Serial.printf("%s testing copy constructor: laserNote _noteCpy2 = _note;\n", _methodName);
  laserTones _laserTones;

  laserNote _note{1, 1};
  laserTone _laserTone(_laserTones.at(3));
  _note._setLaserTone(_laserTone);

  laserNote _noteCpy2 = _note;
  laserTone _laserTone2(_laserTones.at(2));

  Serial.printf("%s _noteCpy2.getLaserToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _noteCpy2.getLaserToneNumber());
  Serial.printf("%s _noteCpy2.getNote() shall be 1. Is [%u]\n", _methodName, 
    _noteCpy2.getNote());

  beat _beat;

  Serial.printf("%s _noteCpy2.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _noteCpy2.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(_noteCpy2.getLaserToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    _laserTones.at(_noteCpy2.getLaserToneNumber()).i16IndexNumber);

  Serial.printf("%s calling _noteCpy2._setLaserTone(_laserTone2) with _laserTone(_laserTones.at(2)).\n", _methodName);
  Serial.printf("%s _noteCpy2._setLaserTone(_laserTone2) shall be 2. Is [%i]\n", _methodName, 
    _noteCpy2._setLaserTone(_laserTone2));
  Serial.printf("%s _laserTones.at(_noteCpy2.getLaserToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    _laserTones.at(_noteCpy2.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _noteCpy2.getLaserToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _noteCpy2.getLaserToneNumber());
  Serial.printf("%s _noteCpy2._playLaserTone() shall be 2. Is [%u]\n\n", _methodName, 
    _noteCpy2._playLaserTone(_laserTones));
}




void test::noteStackCopyAssignmentOp(const char * _methodName) {
  Serial.printf("%s testing assignment operator: laserNote _note2; then _note2 = _note;\n", _methodName);
  laserTones _laserTones;

  laserNote _note{1, 1};
  laserTone _laserTone(_laserTones.at(3));
  _note._setLaserTone(_laserTone);

  laserNote _note2;
  _note2 = _note;
  laserTone _laserTone2(_laserTones.at(2));

  Serial.printf("%s _note2.getLaserToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _note2.getLaserToneNumber());
  Serial.printf("%s _note2.getNote() shall be 1. Is [%u]\n", _methodName, 
    _note2.getNote());

  beat _beat;

  Serial.printf("%s _note2.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _note2.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(_note2.getLaserToneNumber()).i16IndexNumber shall be 3. Is [%i]\n", _methodName, 
    _laserTones.at(_note2.getLaserToneNumber()).i16IndexNumber);
    
  Serial.printf("%s calling _note2._setLaserTone(_laserTone2) with _laserTone(_laserTones.at(2)).\n", _methodName);
  Serial.printf("%s _note2._setLaserTone(_laserTone2) shall be 2. Is [%i]\n", _methodName, 
    _note2._setLaserTone(_laserTone2));
  Serial.printf("%s _laserTones.at(_note2.getLaserToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    _laserTones.at(_note2.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _note2.getLaserToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _note2.getLaserToneNumber());
  Serial.printf("%s _note2._playLaserTone() shall be 2. Is [%u]\n\n", _methodName, 
    _note2._playLaserTone(_laserTones));
}





void test::noteStackCopyConstructorAndCopyAssignment(const char * _methodName) {
  Serial.printf("%s testing straight copy constructor + assignt op: _note4 = laserNote{1,1}.\n", _methodName);
  laserTones _laserTones;

  laserNote _note4;
  _note4 = laserNote{1,1};
  laserTone _laserTone2(_laserTones.at(2));

  Serial.printf("%s _note4.getLaserToneNumber() shall be 3. Is [%u]\n", _methodName, 
    _note4.getLaserToneNumber());
  Serial.printf("%s _note4.getNote() shall be 1. Is [%u]\n", _methodName, 
    _note4.getNote());

  beat _beat;

  Serial.printf("%s _note4.ui16GetNoteDurationInMs() shall be 0 (because beat is set to its defaults). Is [%u]\n", _methodName, 
    _note4.ui16GetNoteDurationInMs(_beat));
  Serial.printf("%s _laserTones.at(_note4.getLaserToneNumber()).i16IndexNumber shall be 1. Is [%i]\n", _methodName, 
    _laserTones.at(_note4.getLaserToneNumber()).i16IndexNumber);

  Serial.printf("%s calling _note4._setLaserTone(_laserTone2) with _laser_tone(_laserTones.at(2)).\n", _methodName);
  Serial.printf("%s _note4._setLaserTone(_laserTone2) shall be 1. Is [%i]\n", _methodName, 
    _note4._setLaserTone(_laserTone2));
  Serial.printf("%s _laserTones.at(_note4.getLaserToneNumber()).i16IndexNumber shall be 2. Is [%i]\n", _methodName, 
    _laserTones.at(_note4.getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _note4.getLaserToneNumber() shall be 2. Is [%u]\n", _methodName, 
    _note4.getLaserToneNumber());
  Serial.printf("%s _note4._playLaserTone() shall be 2. Is [%u]\n\n", _methodName, 
    _note4._playLaserTone(_laserTones));
}





void test::noteStackValidNote(const char * _methodName) {
  Serial.printf("%s testing _validNote()\n", _methodName);
  Serial.printf("%s testing _validNote() in main constructor: laserNote _note3{1, 5}\n", _methodName);
  laserNote _note3{1, 5};

  Serial.printf("%s _note3.getNote() shall be 6. Is [%u]\n", _methodName, 
    _note3.getNote());
  Serial.printf("%s testing _validNote() in straight copy constructor: laserNote{1,7}\n", _methodName);
  Serial.printf("%s laserNote{1,7}.getNote() shall be 8. Is [%u]\n", _methodName, 
    laserNote{1,7}.getNote());
  Serial.printf("%s testing _validNote() in assignment op: _note2 = {2,11};\n", _methodName);
  Serial.printf("%s note2.getNote() shall be 16. Is [%u]\n\n", _methodName, 
    laserNote{2,11}.getNote());
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




void test::laserNotesArrayStackEmptyArray(const char * _methodName) {
  Serial.printf("%s testing _emptyNotesArray\n", _methodName);
  Serial.printf("%s building std::array<laserNote, 16> _emptyNotesArray\n", _methodName);
  std::array<laserNote, 16> _emptyNotesArray;

  Serial.printf("%s _emptyNotesArray.at(0)\n", _methodName);
  Serial.printf("%s _emptyNotesArray.at(0).getLaserToneNumber() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(0).getLaserToneNumber());
  Serial.printf("%s _emptyNotesArray.at(15).getLaserToneNumber() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(15).getLaserToneNumber());
  Serial.printf("%s _emptyNotesArray.at(0).getNote() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(0).getNote());
  Serial.printf("%s _emptyNotesArray.at(15).getNote() shall be 0. Is [%u]\n", _methodName, 
    _emptyNotesArray.at(15).getNote());
}



void test::laserNotesArrayStackToneSelection(const char * _methodName) {
  laserTones _laserTones;

  std::array<laserNote, 16> _emptyNotesArray;

  Serial.printf("%s _laserTones.at(_emptyNotesArray.at(0).getLaserToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _laserTones.at(_emptyNotesArray.at(0).getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _laserTones.at(_emptyNotesArray.at(15).getLaserToneNumber()).i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _laserTones.at(_emptyNotesArray.at(15).getLaserToneNumber()).i16IndexNumber);
  Serial.printf("%s _laserTones.at(_emptyNotesArray.at(0).getLaserToneNumber())._laserPinsStatesArr.at(0) shall be 1. Is [%i]\n", _methodName, 
    _laserTones.at(_emptyNotesArray.at(0).getLaserToneNumber())._laserPinsStatesArr.at(0));
  Serial.printf("%s _laserTones.at(_emptyNotesArray.at(15).getLaserToneNumber())._laserPinsStatesArr.at(3) shall be 1. Is [%i]\n", _methodName, 
    _laserTones.at(_emptyNotesArray.at(15).getLaserToneNumber())._laserPinsStatesArr.at(3));
}




void test::laserNotesArrayStack() {
  const char * _methodName = "test::laserNotesArrayStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  laserNotesArrayStackEmptyArray(_methodName);
  laserNotesArrayStackToneSelection(_methodName);

  Serial.printf("%s over\n\n", _methodName);
}




void test::rawNotesStackSender(const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note) {
    /** TODO: draft whatever  */
}




void test::rawNotesStackConstructors(const char * _methodName) {  
  Serial.printf("%s testing laserNotes constructors.\n", _methodName);

  Serial.printf("%s testing laserNotes default constructor: laserNotes _laserNotes{}.\n", _methodName);
  laserNotes _laserNotes{};
  Serial.printf("%s testing laserNotes default constructor with params: laserNotes _laserNotes2{nullptr}.\n", _methodName);
  laserNotes _laserNotes2{nullptr};
  Serial.printf("%s testing laserNotes default constructor with params: laserNotes _laserNotes3{rawNotesStackSender}.\n", _methodName);
  laserNotes _laserNotes3{rawNotesStackSender};

  Serial.printf("%s testing laserNotes straigth copy constructor: laserNotes{}.\n", _methodName);
  laserNotes{};
  Serial.printf("%s testing laserNotes copy constructor with initializer: laserNotes _noteCpy(_laserNotes).\n", _methodName);
  laserNotes _laserNotesCpy(_laserNotes);
  Serial.printf("%s testing laserNotes copy constructor with equal sign: laserNotes _noteCpy(_laserNotes).\n", _methodName);
  laserNotes _laserNotesCpy2 = _laserNotes;
  Serial.printf("%s testing laserNotes assignment op: _laserNotes = _laserNotes3.\n", _methodName);
  _laserNotes = _laserNotes3;

  Serial.printf("%s testing constructor: over.\n\n", _methodName);
}




void test::rawNotesStackDefaultResetters(const char * _methodName, laserPins & _laserPins, laserNotes & _laserNotes) {
  // Resetters
  Serial.printf("%s testing Task tPlayNote resetters \n", _methodName);
  Serial.printf("%s calling _laserNotes._disableAndResetTPlayNote()\n\n", _methodName);
  _laserNotes._disableAndResetTPlayNote();  
}




void test::rawNotesStackDefaultMembers(const char * _methodName, laserNotes & _laserNotes) {
  // Members
  Serial.printf("%s testing access to members of _laserNotes \n", _methodName);
  Serial.printf("%s testing access to _laserNotes._laserTones \n", _methodName);
  Serial.printf("%s testing access to and values of the _laserPinsStatesArr embedded in \n", _methodName);
  Serial.printf("   laserTone instances extracted from the \"hard-coded\" laserTones array embedded in the laserTones class\n");
  Serial.printf("   from the default _laserNotes instance.\n");
  Serial.printf("%s _laserNotes._laserTones.at(0)._laserPinsStatesArr.at(0) shall be 1. Is [%u]\n", _methodName, 
    _laserNotes._laserTones.at(0)._laserPinsStatesArr.at(0));
  Serial.printf("%s _laserNotes._laserTones.at(0)._laserPinsStatesArr.at(3) shall be 1. Is [%u]\n", _methodName, 
    _laserNotes._laserTones.at(0)._laserPinsStatesArr.at(3));
  Serial.printf("%s _laserNotes._laserTones.at(15)._laserPinsStatesArr.at(0) shall be 0. Is [%u]\n", _methodName, 
    _laserNotes._laserTones.at(15)._laserPinsStatesArr.at(0));
  Serial.printf("%s _laserNotes._laserTones.at(15)._laserPinsStatesArr.at(3) shall be 0. Is [%u]\n\n", _methodName, 
    _laserNotes._laserTones.at(15)._laserPinsStatesArr.at(3));
}




void test::rawNotesStackDefaultPlayTone(const char * _methodName, laserPins & _laserPins, laserNotes & _laserNotes) {
  // _laserNotes->_playLaserTone
  Serial.printf("%s _laserNotes._laserTones.at(0)._playLaserTone(_laserPins) shall be 0. Is [%i]\n", _methodName, 
    _laserNotes._laserTones.at(0)._playLaserTone(_laserPins));
  Serial.printf("%s _laserNotes._laserTones.at(15)._playLaserTone(_laserPins) shall be 15. Is [%i]\n\n", _methodName, 
    _laserNotes._laserTones.at(15)._playLaserTone(_laserPins));
}




void test::rawNotesStackDefaultLaserPins(const char * _methodName, laserNotes & _laserNotes) {
  // _laserNotes._laserTones.laserPins
  Serial.printf("%s testing access to _laserNotes._laserTones._laserPins \n", _methodName);
  Serial.printf("%s _laserNotes._laserTones._laserPins.at(0)._i16IndexNumber shall be 0. Is [%i]\n", _methodName, 
    _laserNotes._laserTones._laserPins.at(0)._i16IndexNumber);
  Serial.printf("%s _laserNotes._laserTones._laserPins.at(0)._ui16PhysicalPinNumber shall be 5. Is [%i]\n", _methodName, 
    _laserNotes._laserTones._laserPins.at(0)._ui16PhysicalPinNumber);
  Serial.printf("%s _laserNotes._laserTones._laserPins.at(0)._switchPin(LOW) shall be 0. Is [%i]\n", _methodName, 
    _laserNotes._laserTones._laserPins.at(0)._switchPin(LOW));
  Serial.printf("%s _laserNotes._laserTones._laserPins.at(0)._switchPin(HIGH) shall be 1. Is [%i]\n\n", _methodName, 
    _laserNotes._laserTones._laserPins.at(0)._switchPin(HIGH));
}




void test::rawNotesStackDefaultSettersAndGetters(const char * _methodName, laserNotes & _laserNotes) {
  // testing _laserNotes.setActive, _laserNotes._activeLaserNote.getNote and _laserNotes._activeLaserNote.getLaserToneNumber
  Serial.printf("%s testing setActive, getNote and getLaserToneNumber\n", _methodName);
  _laserNotes._disableAndResetTPlayNote();
  Serial.printf("%s calling _laserNotes.setActive(laserNote{4,8}).\n", _methodName);
  _laserNotes.setActive(laserNote{4,8});
  Serial.printf("%s _laserNotes._activeLaserNote.getNote() shall be 8. Is [%u]\n", _methodName, 
    _laserNotes._activeLaserNote.getNote());
  Serial.printf("%s _laserNotes._activeLaserNote.getLaserToneNumber() shall be 4. Is [%u]\n\n", _methodName, 
    _laserNotes._activeLaserNote.getLaserToneNumber());
}




void test::rawNotesStackDefaultPlayers(const char * _methodName, laserNotes & _laserNotes) {
  // testing _laserNotes.playNote
  _laserNotes._disableAndResetTPlayNote();

  beat _beat51(5, 1);

  Serial.printf("%s calling _laserNotes.playNote(laserNote{4, 1}, _beat51).\n", _methodName);
  _laserNotes.playNote(laserNote{4, 1}, _beat51);
  laserNote _note{1, 1};
  _laserNotes._disableAndResetTPlayNote();
  Serial.printf("%s calling _laserNotes.playNote(_note, _beat51) with laserNote _note{1, 1}.\n\n", _methodName);
  _laserNotes.playNote(_note, _beat51);
  _laserNotes._disableAndResetTPlayNote();

  // testing _laserNotes.playNote
  Serial.printf("%s testing _laserNotes.playNote\n", _methodName);
  Serial.printf("%s setting active beat to beat(5, 1)\n", _methodName);

  _laserNotes._disableAndResetTPlayNote();
  Serial.printf("%s calling _laserNotes.playNote(laserNote{4, 1}.\n", _methodName);
  _laserNotes.playNote(laserNote{4, 1}, _beat51);
  _laserNotes._disableAndResetTPlayNote();
  Serial.printf("%s calling _laserNotes.playNote(_note).\n\n", _methodName);
  _laserNotes.playNote(_note, _beat51);
  _laserNotes._disableAndResetTPlayNote();
}




void test::rawNotesStackDefault(const char * _methodName) {
  laserPins _laserPins;
  laserNotes _laserNotes{};

  rawNotesStackDefaultResetters(_methodName, _laserPins, _laserNotes);
  rawNotesStackDefaultMembers(_methodName, _laserNotes);
  rawNotesStackDefaultPlayTone(_methodName, _laserPins, _laserNotes);
  rawNotesStackDefaultLaserPins(_methodName, _laserNotes);
  rawNotesStackDefaultSettersAndGetters(_methodName, _laserNotes);
  rawNotesStackDefaultPlayers(_methodName, _laserNotes);

}




void test::rawNotesStack() {
  const char * _methodName = "test::rawNotesStack:";
  Serial.printf("\n\n%s starting\n", _methodName);

  rawNotesStackConstructors(_methodName);
  rawNotesStackDefault(_methodName);

  Serial.printf("%s over\n", _methodName);
}





void test::barStackConstructors(const char * _methodName) {

  Serial.printf("%s testing bar default constructor: bar _bar;\n", _methodName);
  bar _emptyBar;
  barStackGetters(_methodName, _emptyBar);
  Serial.printf("%s testing bar default constructor: everything above should be at 0;\n", _methodName);

  Serial.printf("%s testing bar constructor: bar _bar{std::array<laserNote, 16>{laserNote(4,8), laserNote(3,8),\n", _methodName);
  Serial.printf("                            laserNote(2,8), laserNote(1,8), laserNote(2,8), laserNote(3,8), laserNote(4,8), laserNote(0,8)}\n");
  bar _bar{std::array<laserNote, 16>{laserNote(4,8), laserNote(3,8), laserNote(2,8), laserNote(1,8), laserNote(2,8), laserNote(3,8), laserNote(4,8), laserNote(0,8)}};
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
  beat _beat51(5, 1);
  // getters
  Serial.printf("%s _bar.ui16GetNotesCountInBar() shall be 8. Is [%u]\n", _methodName, 
    _bar.ui16GetNotesCountInBar());
  Serial.printf("%s _bar.ui16GetBaseNotesCountInBar() shall be 1. Is [%u]\n", _methodName, 
    _bar.ui16GetBaseNotesCountInBar(_beat51));
  const uint32_t _ui32ExpectGetBarDuration = 12000;
  Serial.printf("%s _bar.ui32GetBarDuration() shall be %u. Is [%u]. ---> %s\n", _methodName, _ui32ExpectGetBarDuration,
    _bar.ui32GetBarDuration(_beat51), ((_bar.ui32GetBarDuration(_beat51) == _ui32ExpectGetBarDuration) ? "PASSED" : "FAILED"));
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

  beat _beat51(5, 1);

  Serial.printf("%s testing bar instance's creation and assignment as in bars default (and only) constructor\n", _methodName);

  Serial.printf("%s creating std::array<bar, 7> _barsArray (empty bars array)\n", _methodName);
  std::array<bar, 7> _barsArray;
  Serial.printf("%s creating std::array<bar, 7> _laserNotesArray {laserNote(4,8), laserNote(3,8), ...\n", _methodName);
  std::array<laserNote, 16> _laserNotesArray {laserNote(4,8), laserNote(3,8), laserNote(2,8), laserNote(1,8), laserNote(2,8), laserNote(3,8), laserNote(4,8), laserNote(0,8)};
  Serial.printf("%s bar copy and assignemnt: _barsArray[0] = {_laserNotesArray}\n", _methodName);
  _barsArray[0] = {_laserNotesArray};
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
  bar _bar{std::array<laserNote, 16>{laserNote(4,8), laserNote(3,8), laserNote(2,8), laserNote(1,8), laserNote(2,8), laserNote(3,8), laserNote(4,8), laserNote(0,8)}};
  Serial.printf("%s testing _bars.setActive(_bar) with bar _bar{std::array<laserNote, 16>{laserNote(4,8), laserNote(3,8)...} \n", _methodName);
  rawBarsStackPlayer(_methodName, _bars, _bar);
  _bars.setActive(_bar);
  rawBarsStackNestedNotes(_methodName, _bars);
  rawBarsStackTaskCallbacks(_methodName, _bars);
  rawBarsStackTaskAccessFromAndToActiveBar(_methodName, _bars);
}




void test::rawBarsStackPlayer(const char * _methodName, bars & _bars, bar & _bar) {

  Serial.printf("%s testing players\n", _methodName);

  Serial.printf("%s calling _bars.disableAndResetTPlayBar()\n", _methodName);
  _bars.disableAndResetTPlayBar();

  _bars.setActive(_bar);
  Serial.printf("%s _bars.playBar(beat(5, 1)) shall be 2. Is [%i]\n", _methodName,
    _bars.playBar(beat(5, 1)));

  Serial.printf("%s _bar.playBar(beat(0, 0)) shall fail because beat is set to base beat == 0 bpm and base note == 0). Shall be 0. Is [%i]\n", _methodName,
    _bars.playBar(beat(0, 0)));
  _bars.disableAndResetTPlayBar();
}




void test::rawBarsStackNestedNotes(const char * _methodName, bars & _bars) {
  Serial.printf("%s testing nested laserNotes \n", _methodName);
  // nested laserNotes class (including laserNotes array)
  Serial.printf("%s calling _bars.getNotes().setActive(laserNote{4,8}).\n", _methodName);
  _bars.getNotes().setActive(laserNote{4,8});
  Serial.printf("%s _bars.getNotes().getCurrentNote().getNote() shall be 8. Is [%u]\n", _methodName, 
    _bars.getNotes().getCurrentNote().getNote());
  Serial.printf("%s _bars.getNotes()._activeLaserNote.getLaserToneNumber() shall be 4. Is [%u]\n", _methodName, 
    _bars.getNotes().getCurrentNote().getLaserToneNumber());
}




void test::rawBarsStackTaskCallbacks(const char * _methodName, bars & _bars) {

  Serial.printf("%s testing Task callbacks \n", _methodName);
  // Task TPlayBar callbacks
  Serial.printf("%s calling _bars._tcbPlayBar().\n", _methodName);
  _bars._tcbPlayBar(beat{});
  Serial.printf("%s _bars._oetcbPlayBar() shall be 1. Is [%i]\n", _methodName, _bars._oetcbPlayBar());
  _bars.disableAndResetTPlayBar();
}




void test::rawBarsStackTaskAccessFromAndToActiveBar(const char * _methodName, bars & _bars) {

  Serial.printf("%s _bars._activeBar.ui16GetNotesCountInBar() is [%u]\n", _methodName, 
    _bars._activeBar.ui16GetNotesCountInBar());

  beat _beat;

  Serial.printf("%s _bars._activeBar.ui16GetBaseNotesCountInBar(_beat) shall be 0 (because _beat has been set to default). Is [%u]\n", _methodName, 
    _bars._activeBar.ui16GetBaseNotesCountInBar(_beat));
  Serial.printf("%s _bars._activeBar.ui32GetBarDuration(_beat) shall be 0 (because beat has been set to default). Is [%u]\n", _methodName, 
    _bars._activeBar.ui32GetBarDuration(_beat));

  Serial.printf("%s _bars._activeBar.getNotesArray().at(0).getNote() is [%u]\n", _methodName,   
  _bars._activeBar.getNotesArray().at(0).getNote());
  Serial.printf("%s _bars._activeBar.getNotesArray().at(0).getLaserToneNumber() is [%u]\n", _methodName,   
  _bars._activeBar.getNotesArray().at(0).getLaserToneNumber());

  beat _beat51(5, 1);

  Serial.printf("%s _bars._activeBar.ui16GetBaseNotesCountInBar(_beat51) is [%u]\n", _methodName, 
    _bars._activeBar.ui16GetBaseNotesCountInBar(_beat51));
  Serial.printf("%s _bars._activeBar.ui32GetBarDuration() is [%u]\n", _methodName, 
    _bars._activeBar.ui32GetBarDuration(_beat51));
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

  laserSequences _laserSequences;

  rawBarsStackNestedNotes(_methodName, _laserSequences._bars);
  rawBarsStackTaskCallbacks(_methodName, _laserSequences._bars);
  rawBarsStackTaskAccessFromAndToActiveBar(_methodName, _laserSequences._bars);

  Serial.printf("%s over.\n\n", _methodName);
}




void test::beforeSequenceStacks() {
  laserPinStack();
  rawLaserPinsStack();
  laserToneStack();
  rawLaserTonesStack();
  noteStack();
  laserNotesArrayStack();
  rawNotesStack();
  barStack();
  rawBarsStack();
  implementedBarsStack();
}




void test::sequenceStack() {
  const char * _methodName = "test::sequenceStack:";
  laserSequences _laserSequences;
  Serial.printf("\n%s starting\n", _methodName);
  Serial.printf("\n%s testing _laserSequences.setActive(0)\n", _methodName);
  _laserSequences.setActive(1);
  Serial.printf("%s _laserSequences._activeSequence.i16IndexNumber shall be 1. Is [%i]\n", _methodName, _laserSequences._activeSequence.i16IndexNumber);

  Serial.printf("\n%s alloting _laserSequences._activeSequence to local variable laserSequence & _activeSequence\n", _methodName);  
  laserSequence & _activeSequence = _laserSequences._activeSequence;

  Serial.printf("\n%s testing _activeSequence.getAssociatedBeat() submethods\n", _methodName);
  Serial.printf("%s _activeSequence.getAssociatedBeat().getBaseNoteForBeat() shall be 1. Is [%u]\n", _methodName, 
    _activeSequence.getAssociatedBeat().getBaseNoteForBeat());
  Serial.printf("%s _activeSequence.getAssociatedBeat().getBaseBeatInBpm() shall be 2. Is [%u]\n", _methodName, 
    _activeSequence.getAssociatedBeat().getBaseBeatInBpm());

  Serial.printf("\n%s testing access to the methods of _activeSequence\n", _methodName);
  Serial.printf("%s _activeSequence.ui16GetBarCountInSequence() shall be equal to 1. Is [%u]\n", _methodName, 
    _activeSequence.ui16GetBarCountInSequence());
  Serial.printf("%s _activeSequence.i16GetFirstBarIndexNumber() shall be equal to 1. Is [%i]\n", _methodName, 
    _activeSequence.i16GetFirstBarIndexNumber());
  Serial.printf("%s _activeSequence.i16GetBarIndexNumber(0) shall be equal to 1. Is [%i]\n", _methodName, 
    _activeSequence.i16GetBarIndexNumber(0));

  Serial.printf("\n%s testing access to the members of _laserSequences.\n", _methodName);
  Serial.printf("%s _laserSequences.ui16IxNumbOfSequenceToPreload shall be equal to 1. Is [%u]\n", _methodName, 
    _laserSequences.ui16IxNumbOfSequenceToPreload);
  Serial.printf("%s _laserSequences.nextSequence.i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences.nextSequence.i16IndexNumber);
  Serial.printf("%s _laserSequences._defaultSequence.sequenceFileName shall be equal to \"/laserSequences.json\". Is [%s]\n", _methodName, 
    _laserSequences.sequenceFileName);
  Serial.printf("%s _laserSequences.getActiveSequence().i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences.getActiveSequence().i16IndexNumber);
  Serial.printf("%s _laserSequences._activeSequence.i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences._activeSequence.i16IndexNumber);
  Serial.printf("%s _laserSequences._defaultSequence.i16IndexNumber shall be equal to -2. Is [%i]\n", _methodName, 
    _laserSequences._defaultSequence.i16IndexNumber);
  
  Serial.printf("\n%s testing access _laserSequences._bars members\n", _methodName);
  Serial.printf("%s _laserSequences._bars.ui16IxNumbOfBarToPreload shall be equal to 1. Is [%u]\n", _methodName, 
    _laserSequences._bars.ui16IxNumbOfBarToPreload);
  Serial.printf("%s _laserSequences._bars.nextBar.i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences._bars.nextBar.i16IndexNumber);
  Serial.printf("%s _laserSequences._bars.barFileName shall be equal to \"/laserSequences.json\". Is [%s]\n", _methodName, 
    _laserSequences._bars.barFileName);
  Serial.printf("%s _laserSequences._bars.i16GetActiveBarId() shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences._bars.i16GetActiveBarId());
  Serial.printf("%s _laserSequences._bars._defaultBar.i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences._bars._defaultBar.i16IndexNumber);
  Serial.printf("%s _laserSequences._bars._defaultBar.i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences._bars._defaultBar.i16IndexNumber);
  Serial.printf("%s _laserSequences._bars._activeBar.i16IndexNumber shall be equal to 1. Is [%i]\n", _methodName, 
    _laserSequences._bars._activeBar.i16IndexNumber);

  Serial.printf("\n%s alloting _activeSequence.getAssociatedBeat() to a private _beat instance\n", _methodName);  
  beat _beat = _activeSequence.getAssociatedBeat();

  Serial.printf("\n%s testing access to laserNotes of _laserSequences._bars._activeBar\n", _methodName);
  Serial.printf("%s _laserSequences._bars._activeBar.getNotesArray().at(0).getNote() shall be equal to 1. Is [%u]\n", _methodName, 
    _laserSequences._bars._activeBar.getNotesArray().at(0).getNote());
  Serial.printf("%s _laserSequences._bars._activeBar.getNotesArray().at(0).getLaserToneNumber() shall be equal to 5. Is [%u]\n", _methodName, 
    _laserSequences._bars._activeBar.getNotesArray().at(0).getLaserToneNumber());
  Serial.printf("%s _laserSequences._bars._activeBar.getNotesArray().at(0).ui16GetNoteDurationInMs() shall be equal to 3000. Is [%u]\n", _methodName, 
    _laserSequences._bars._activeBar.getNotesArray().at(0).ui16GetNoteDurationInMs(_beat));

  Serial.printf("\n%s testing members and methods of _laserSequences._bars._activeBar\n", _methodName);
  Serial.printf("%s _laserSequences._bars._activeBar.ui16GetNotesCountInBar() shall be equal to 2. Is [%u]\n", _methodName, 
    _laserSequences._bars._activeBar.ui16GetNotesCountInBar());
  Serial.printf("%s _laserSequences._bars._activeBar.ui16GetBaseNotesCountInBar(_beat) shall be equal to 2. Is [%u]\n", _methodName, 
    _laserSequences._bars._activeBar.ui16GetBaseNotesCountInBar(_beat));
  Serial.printf("%s _laserSequences._bars._activeBar.ui32GetBarDuration(_beat) shall be equal to 60000. Is [%u]\n", _methodName, 
    _laserSequences._bars._activeBar.ui32GetBarDuration(_beat));

  Serial.printf("\n%s testing preloading a \"beyond bound\" laserSequence\n", _methodName);
  Serial.printf("%s calling _laserSequences._preloadNextSequence(10), 10 being out of bound\n", _methodName);
  _laserSequences._preloadNextSequence(10);
  Serial.printf("%s _laserSequences.nextSequence.i16IndexNumber is [%i]\n", _methodName, 
    _laserSequences.nextSequence.i16IndexNumber);
  Serial.printf("%s _laserSequences.nextSequence.ui16GetBarCountInSequence() is [%u]\n", _methodName, 
    _laserSequences.nextSequence.ui16GetBarCountInSequence());
  Serial.printf("%s _laserSequences.nextSequence.getAssociatedBeat().getBaseBeatInBpm shall be 0. Is [%u]\n", _methodName, 
    _laserSequences.nextSequence.getAssociatedBeat().getBaseBeatInBpm());
  Serial.printf("%s _laserSequences.nextSequence.getAssociatedBeat().getBaseNoteForBeat shall be 0. Is [%u]\n", _methodName, 
    _laserSequences.nextSequence.getAssociatedBeat().getBaseNoteForBeat());
  Serial.printf("%s _laserSequences.nextSequence.i16GetFirstBarIndexNumber() is [%i]\n", _methodName, 
    _laserSequences.nextSequence.i16GetFirstBarIndexNumber());
  Serial.printf("%s _laserSequences.nextSequence._i16barsArraySize is [%i]\n", _methodName, 
    _laserSequences.nextSequence._i16barsArraySize);

  Serial.printf("\n%s testing setting the \"beyond bound\" nextBar as _activeBar\n", _methodName);
  // TO BE DRAFTED

  Serial.printf("\n%s over\n\n", _methodName);
}
