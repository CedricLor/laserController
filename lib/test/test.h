/*
  test.h - Library to handle tests on the software.
  Created by Cedric Lor, September 12, 2019.
*/

#ifndef test_h
#define test_h

#include "Arduino.h"
#include "laserSequence.h"

class test
{
  public:
    test();

    void laserPinStack();
    void rawLaserPinsStack();

    void laserToneStack();
    void rawLaserTonesStackLaserTonesArray(const char * _methodName, uint16_t _toneIndexNumber);
    void rawLaserTonesStack();

    void noteStack();
    void noteStackParamConstructor(const char * _methodName);
    void noteStackDefaultConstructor(const char * _methodName);
    void noteStackStraightCopyConstructor(const char * _methodName);
    void noteStackCopyConstructorInitList(const char * _methodName);
    void noteStackCopyConstructorEqualSign(const char * _methodName);
    void noteStackCopyAssignmentOp(const char * _methodName);
    void noteStackCopyConstructorAndCopyAssignment(const char * _methodName);
    void noteStackValidNote(const char * _methodName);

    void laserNotesArrayStackEmptyArray(const char * _methodName);
    void laserNotesArrayStackToneSelection(const char * _methodName);
    void laserNotesArrayStack();

    void rawNotesStackConstructors(const char * _methodName);
    void rawNotesStackDefaultResetters(const char * _methodName, laserPins & _laserPins, laserNotes & _laserNotes);
    void rawNotesStackDefaultMembers(const char * _methodName, laserNotes & _laserNotes);
    void rawNotesStackDefaultPlayTone(const char * _methodName, laserPins & _laserPins, laserNotes & _laserNotes);
    void rawNotesStackDefaultLaserPins(const char * _methodName, laserNotes & _laserNotes);
    void rawNotesStackDefaultSettersAndGetters(const char * _methodName, laserNotes & _laserNotes);
    void rawNotesStackDefaultPlayers(const char * _methodName, laserNotes & _laserNotes);
    void rawNotesStackDefault(const char * _methodName);
    void rawNotesStack();
    static void rawNotesStackSender(const uint16_t __ui16_target_laser_tone, const uint16_t __ui16_target_note);

    void barStackGetters(const char * _methodName, bar & _bar);
    void barStackConstructors(const char * _methodName);
    void barStack();

    void barArrayStackBarGetters(const char * _methodName, std::array<bar, 7> & _barsArray);
    void barArrayStack();

    void rawBarsStackConstructors(const char * _methodName);
    void rawBarsStackInitializers(const char * _methodName);
    void rawBarsStackPlayer(const char * _methodName, bars & _bars, bar & _bar);
    void rawBarsStackNestedNotes(const char * _methodName, bars & _bars);
    void rawBarsStackTaskCallbacks(const char * _methodName, bars & _bars);
    void rawBarsStackTaskAccessFromAndToActiveBar(const char * _methodName, bars & _bars);
    void rawBarsStack();
    void implementedBarsStack();

    void beforeSequenceStacks();

    void sequenceStack();
    void rawSequencesStack();
    void implementedSequencesStack();
};

#endif
