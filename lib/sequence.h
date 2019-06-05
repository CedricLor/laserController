/*
  sequence.h - sequences corresponding to pre-determined blinking patterns of lasers
  Created by Cedric Lor, June 4, 2019.
*/
#ifndef sequence_h
#define sequence_h

#include "Arduino.h"

class sequence
{
  public:
    sequence(); // default constructor
    sequence(const char cName[7], const short int iTempo, const short int iLaserPinStatusAtEachBeat[][4]); // initialiser of each sequence
    static void initSequences(); // initializer of the array of sequences


  private:
    char _cName[7];  // array of character to hold the name of each sequences
    short int _iTempo; // tempo at which the task executing the sequence will
                       // update the state of each laser pin, in milliseconds
    short int _iNumberOfBeatsInSequence; // number of tempos required to execute
                                          // one full sequence
    short int _iLaserPinStatusAtEachBeat[4][PIN_COUNT];  // bi-dimensional array containing
                                                // the state of each laser at each tempo
    static const short int _SEQUENCES_COUNT;

    void _setCName(const char* _cname);
};

#endif
