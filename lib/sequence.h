/*
  sequence.h - sequences are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.
*/
#ifndef sequence_h
#define sequence_h

#include "Arduino.h"
#include "global.h"

class sequence
{
  public:
    sequence(); // default constructor
    sequence(const char cName[7], const unsigned long ulTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[]); // constructor and initialiser
    void initSequence(const char cName[7], const unsigned long ulTempo, const short int iNumberOfBeatsInSequence, const short int iLaserPinStatusAtEachBeat[]);

    static const short int SEQUENCE_COUNT;
    static sequence sequences[];
    static void initSequences(); // initializer of the array of sequences

    static void setActiveSequence(const short int activeSequence);

    static Task tPlaySequenceInLoop;
    static void tcbPlaySequenceInLoop();
    static bool oetcbPlaySequenceInLoop();
    static void odtcbPlaySequenceInLoop();

    static Task tEndSequence;
    static void _tcbTEndSequence();

    static void playSequence();
    static Task _tPlaySequence;
    static void _tcbPlaySequence();

  private:
    char _cName[7];  // array of character to hold the name of each sequences
    unsigned long  _ulTempo; // tempo at which the task executing the sequence will
                       // update the state of each laser pin, in milliseconds
    short int _iNumberOfBeatsInSequence; // number of tempos required to execute
                                          // one full sequence
    short int _iLaserPinStatusAtEachBeat[4];  // bi-dimensional array containing
                                                // the state of each laser at each tempo

    static short int _activeSequence;

    static unsigned long _ulSequenceGroupDurationSetter();
    static unsigned long _ulSequenceGroupDuration;
};

#endif
