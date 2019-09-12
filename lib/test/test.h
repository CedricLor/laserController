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
