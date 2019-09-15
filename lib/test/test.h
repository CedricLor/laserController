/*
  test.h - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#ifndef test_h
#define test_h

#include "Arduino.h"
#include "sequence.h"

class test
{
  public:
    test();

    void laserPinStack();
    void rawLaserPinsStack();

    void toneStack();
    void rawTonesStack();
    void implementedTonesStack();

    void noteStack();
    void noteStackParamConstructor(const char * _methodName);
    void noteStackDefaultConstructor(const char * _methodName);
    void noteStackStraightCopyConstructor(const char * _methodName);
    void noteStackCopyConstructorInitList(const char * _methodName);
    void noteStackCopyConstructorEqualSign(const char * _methodName);
    void noteStackCopyAssignmentOp(const char * _methodName);
    void noteStackCopyConstructorAndCopyAssignment(const char * _methodName);
    void noteStackValidNote(const char * _methodName);

    void notesArrayStack();
    void notesArrayStackEmptyArray(const char * _methodName);
    void notesArrayStackToneSelection(const char * _methodName);

    void rawNotesStack();
    void implementedNotesStack();

    void barStack();
    void rawBarsStack();
    void implementedBarsStack();

    void sequenceStack();
    void rawSequencesStack();
    void implementedSequencesStack();
};

#endif
