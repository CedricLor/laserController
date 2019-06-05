/*
  sequence.cpp - sequences corresponding to pre-determined blinking patterns of lasers
  Created by Cedric Lor, June 4, 2019.
*/

#include "Arduino.h"
#include "sequence.h"


const short int _SEQUENCE_COUNT = PIN_COUNT;

// constructor
sequence::sequence() {

}

sequence::sequence(const char cName[7], short int iTempo, const short int iLaserPinStatusAtEachBeat[][4])
{
  strcpy(_cName, cName);
  _iTempo = iTempo;
  // for (short int i = 0; iLaserPinStatusAtEachTempos[i][0] == -1; i++) {
  //   for (short int j = 0; j < PIN_COUNT; i++) {
  //     _iLaserPinStatusAtEachBeat[i][j] = iLaserPinStatusAtEachBeat[i][j];
  //   }
  //   _iNumberOfTemposInSequence = i + 1;
  // }
}

void sequence::initSequences() {
  sequence Sequences[PIN_COUNT];
  Sequences[0]._setCName("swipe");
  Sequences[0]._iTempo = 500;
  Sequences[0]._iNumberOfBeatsInSequence = 4;
  Sequences[0]._iLaserPinStatusAtEachBeat[0][0] = LOW;
  Sequences[0]._iLaserPinStatusAtEachBeat[0][1] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[0][2] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[0][3] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[1][0] = LOW;
  Sequences[0]._iLaserPinStatusAtEachBeat[1][1] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[1][2] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[1][3] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[2][0] = LOW;
  Sequences[0]._iLaserPinStatusAtEachBeat[2][1] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[2][2] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[2][3] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[3][0] = LOW;
  Sequences[0]._iLaserPinStatusAtEachBeat[3][1] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[3][2] = HIGH;
  Sequences[0]._iLaserPinStatusAtEachBeat[3][3] = HIGH;
  Sequences[1]._setCName("relays");
  Sequences[1]._iTempo = 30000;
  Sequences[1]._iNumberOfBeatsInSequence = 2;
  Sequences[1]._iLaserPinStatusAtEachBeat[0][0] = LOW;
  Sequences[1]._iLaserPinStatusAtEachBeat[0][1] = HIGH;
  Sequences[1]._iLaserPinStatusAtEachBeat[0][2] = LOW;
  Sequences[1]._iLaserPinStatusAtEachBeat[0][3] = HIGH;
  Sequences[1]._iLaserPinStatusAtEachBeat[1][0] = HIGH;
  Sequences[1]._iLaserPinStatusAtEachBeat[1][1] = LOW;
  Sequences[1]._iLaserPinStatusAtEachBeat[1][2] = HIGH;
  Sequences[1]._iLaserPinStatusAtEachBeat[1][3] = LOW;
  Sequences[2]._setCName("twins");
  Sequences[2]._iTempo = 30000;
  Sequences[2]._iNumberOfBeatsInSequence = 2;
  Sequences[2]._iLaserPinStatusAtEachBeat[0][0] = LOW;
  Sequences[2]._iLaserPinStatusAtEachBeat[0][1] = LOW;
  Sequences[2]._iLaserPinStatusAtEachBeat[0][2] = HIGH;
  Sequences[2]._iLaserPinStatusAtEachBeat[0][3] = HIGH;
  Sequences[2]._iLaserPinStatusAtEachBeat[1][0] = HIGH;
  Sequences[2]._iLaserPinStatusAtEachBeat[1][1] = HIGH;
  Sequences[2]._iLaserPinStatusAtEachBeat[1][2] = LOW;
  Sequences[2]._iLaserPinStatusAtEachBeat[1][3] = LOW;
  Sequences[3]._setCName("all");
  Sequences[3]._iTempo = 30000;
  Sequences[3]._iNumberOfBeatsInSequence = 2;
  Sequences[3]._iLaserPinStatusAtEachBeat[0][0] = LOW;
  Sequences[3]._iLaserPinStatusAtEachBeat[0][1] = LOW;
  Sequences[3]._iLaserPinStatusAtEachBeat[0][2] = LOW;
  Sequences[3]._iLaserPinStatusAtEachBeat[0][3] = LOW;
  Sequences[3]._iLaserPinStatusAtEachBeat[1][0] = HIGH;
  Sequences[3]._iLaserPinStatusAtEachBeat[1][1] = HIGH;
  Sequences[3]._iLaserPinStatusAtEachBeat[1][2] = HIGH;
  Sequences[3]._iLaserPinStatusAtEachBeat[1][3] = HIGH;
}

void sequence::_setCName(const char* _cname) {
  strcpy(_cName, _cname);
}
