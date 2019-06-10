/*
  note.h - notes are statuses of all the lasers connected to the box at one beat
  Created by Cedric Lor, June 10, 2019.
*/
#ifndef note_h
#define note_h

#include "Arduino.h"
#include "global.h"

class note
{
  public:
    note(); // default constructor
    note(const char cName[15], const short int iTempo, const short int iLaserPinsStatus[4]); // constructor and initialiser
    void initNote(const char cName[15], const short int iTempo, const short int iLaserPinsStatus[4]);

    static const short int NOTE_COUNT;
    static note notes[];
    static void initNotes(); // initializer of the array of notes

    static void setActiveNote(const short int activeNote);

    static void testPlay();

    static void playNote();

  private:
    char _cName[15];  // array of character to hold the name of each sequences
    short int _iTempo; // tempo at which the task executing the note will
                       // update the state of each laser pin, in milliseconds
    short int _iLaserPinStatus[4];  // array containing the state of each laser at each tempo

    static short int _activeNote;
};

#endif
