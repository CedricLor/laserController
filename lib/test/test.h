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
    void rawTonesStackTonesArray(const char * _methodName, uint16_t _toneIndexNumber);
    void rawTonesStack();

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

    void rawNotesStackConstructors(const char * _methodName);
    void rawNotesStackDefaultResetters(const char * _methodName, laserPins & _laserPins, notes & _notes);
    void rawNotesStackDefaultMembers(const char * _methodName, notes & _notes);
    void rawNotesStackDefaultPlayTone(const char * _methodName, laserPins & _laserPins, notes & _notes);
    void rawNotesStackDefaultLaserPins(const char * _methodName, notes & _notes);
    void rawNotesStackDefaultSettersAndGetters(const char * _methodName, notes & _notes);
    void rawNotesStackDefaultPlayers(const char * _methodName, notes & _notes);
    void rawNotesStackDefault(const char * _methodName);
    void rawNotesStack();
    static void rawNotesStackSender(const uint16_t __ui16_target_tone, const uint16_t __ui16_target_note);

    void barStackGetters(const char * _methodName);
    void barStackConstructors(const char * _methodName);
    void barStack();
    void rawBarsStackConstructors(const char * _methodName);
    void rawBarsStackInitializers(const char * _methodName);
    void rawBarsStackPlayers(const char * _methodName);
    void rawBarsStackNestedNotes(const char * _methodName);
    void rawBarsStackTaskCallbacks(const char * _methodName);
    void rawBarsStackTaskAccessFromActiveBar(const char * _methodName);
    void rawBarsStack();
    void implementedBarsStack();

    void sequenceStack();
    void rawSequencesStack();
    void implementedSequencesStack();
};

#endif
