/*
  bar.h - bars are two beats to four beats, precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.


*/
#ifndef bar_h
#define bar_h

#include "Arduino.h"
#include <mns.h>
#include <note.h>

class bar
{
  public:
    /** sender to mesh */
    static void (*sendCurrentBar)(const uint16_t __ui16_active_bar);

    /** default empty constructor */
    bar();
    /** main parameterized constructor */
    bar(
      const uint16_t __ui16_base_notes_count_in_bar,
      const uint16_t __ui16_notes_count_in_bar,
      std::array<note, 16> & __notesArray
    ); 
    // copy constructor
    bar( const bar& __bar);
    // assignement operator
    bar& operator=(const bar& __bar);
 
    // objects array initializer
    static void initBars(); // initializer of the array of bars

    /** setters */
    void setActive();

    /** getters */
    static int16_t const i16GetCurrentBarId(); // returns _ui16ActiveBar
    static bar & getCurrentBar();
    static bar & getBarFromBarArray(const uint16_t __ui16_active_bar);

    /** player */
    void playBarStandAlone(beat const & __beat=beat::_activeBeat);
    void playBarInSequence();
    void playBar(beat const & __beat=beat::_activeBeat);
    static Task tPlayBar;

  private:
    friend class sequence;

    // static properties
    static bar _emptyBar;
    static bar &_activeBar;
    static uint16_t _ui16ActiveBar;
    static std::array<bar, 7> _bars;
    static std::array<note, 16> _emptyNotesArray;

    /** player callbacks */
    static void _tcbPlayBar();
    static bool _oetcbPlayBar();

    // static setter
    void _setTPlayBar(beat const & __beat);

    // private instance properties
    uint16_t _ui16BaseNotesCountInBar;
    uint16_t _ui16NotesCountInBar; // number of actual notes in the bar
    std::array<note, 16> & _notesArray;
};

#endif
