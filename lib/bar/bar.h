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
    static void (*sendCurrentBar)(const int16_t __i16_current_bar_id);

    /** default empty constructor */
    bar();
    bar(std::array<note, 16> & __notesArray); 
    bar( const bar& __bar);
    bar& operator=(const bar& __bar);
 
    // objects array initializer
    static void initBars(); // initializer of the array of bars

    /** setters */
    void setActive(const int16_t __i16_active_bar_id=-1);

    /** getters */
    static int16_t const i16GetCurrentBarId(); // returns _i16ActiveBarId
    static bar & getCurrentBar();
    static bar & getBarFromBarArray(const uint16_t __ui16_bar_id);
    static notes & getNotes();
    uint16_t const ui16GetNotesCountInBar() const;
    uint16_t const ui16GetBaseNotesCountInBar() const;
    uint32_t const ui32GetBarDuration() const;
    const std::array<note, 16> & getNotesArray() const;

    /** player */
    bool playBarStandAlone(beat const & __beat=beat::_activeBeat, const int16_t __i16_active_bar_id=-1);
    bool playBarInSequence(/*const int16_t __i16_active_bar_id=-1*/); // <-- TODO: we need to pass the active_bar id if we play a bar coming from the array, mainly for debug purposes 
    static Task tPlayBar;

  private:
    friend class sequence;

    // static properties
    static bar _emptyBar;
    static bar &_activeBar;
    static int16_t _i16ActiveBarId;
    static std::array<bar, 7> _bars;
    static std::array<note, 16> _emptyNotesArray;
    static notes _notes;

    /** player callbacks */
    static void _tcbPlayBar();
    static bool _oetcbPlayBar();

    // static setter
    void _setTPlayBar(beat const & __beat);

    // private instance properties
    std::array<note, 16> _notesArray;
};

#endif
