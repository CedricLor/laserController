/*
  bar.h - bars are two beats to four beats, precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.


*/
#include "Arduino.h"
#include <mns.h>
#include <note.h>

#ifndef bar_h
#define bar_h

class bar
{
  public:
    static notes globalNotes;

    /** default empty constructor */
    bar();
    bar(std::array<note, 16> __notesArray); 
    bar( const bar& __bar);
    bar& operator=(const bar& __bar);
 
    // objects array initializer
    static void initBars(); // initializer of the array of bars

    /** setters */
    void setActive(const int16_t __i16_active_bar_id=-1);

    /** getters */
    uint16_t const ui16GetNotesCountInBar() const;
    uint16_t const ui16GetBaseNotesCountInBar() const;
    uint32_t const ui32GetBarDuration() const;
    const std::array<note, 16> & getNotesArray() const;

    /** player */
    bool playBarStandAlone(beat const & __beat=beat::_activeBeat, const int16_t __i16_active_bar_id=-1);
    bool playBarInSequence(const int16_t __i16_active_bar_id); // <-- TODO: we need to pass the active_bar id if we play a bar coming from the array, mainly for debug purposes 
    static Task tPlayBar;

  private:
    friend class sequence;
    friend class bars;

    // static properties
    static bar _emptyBar;
    static bar &_activeBar;
    static int16_t _i16ActiveBarId;
    static std::array<bar, 7> _barsArray;
    static std::array<note, 16> _emptyNotesArray;
    static notes _notes;

    /** player callbacks */
    static void _tcbPlayBar();
    static bool _oetcbPlayBar();

    // private instance properties
    std::array<note, 16> _notesArray;
};

#endif



#ifndef bars_h
#define bars_h
class bars
{
  public:
    /** default constructor 
     * 
     *  The bars' properties (_emptyBar, _activeBar, _i16ActiveBarId, _notes,
     *  _emptyNotesArray) are defined in the constructor, by default, without
     *  parameters.
     * 
     *  The only parameters, _sendCurrentBar, is an optional callback. */
    bars(
      void (*_sendCurrentBar)(const int16_t __i16_current_bar_id)=nullptr
    );

    /** sender to mesh */
    void (*sendCurrentBar)(const int16_t __i16_current_bar_id);

    /** setters */
    void setActive(const bar & __activeBar);
    // TODO: define the following methods re. see what's in notes class
    // void resetTPlayBarToPlayBarsInSequence();
    // void setTPlayBar(beat const & __beat);

    /** getters */
    int16_t const i16GetCurrentBarId() const; // returns _i16ActiveBarId
    bar & getCurrentBar() const;
    bar & getBarFromBarArray(const uint16_t __ui16_bar_id);
    notes & getNotes();

    /** player */
    bool playBarStandAlone(const int16_t __i16_active_bar_id=-1, beat const & __beat=beat::_activeBeat);
    bool playBarInSequence(const int16_t __i16_active_bar_id); // <-- TODO: we need to pass the active_bar id if we play a bar coming from the array, mainly for debug purposes 
    Task tPlayBar;

  private:
    friend class test;
    friend class sequence;

    // properties
    bar _emptyBar; // <-- check: probably not needed any more;
    bar & _activeBar;
    int16_t _i16ActiveBarId;
    notes _notes;
    std::array<bar, 7> _barsArray;
    std::array<note, 16> _emptyNotesArray;

    /** player callbacks */
    void _tcbPlayBar();
    bool _oetcbPlayBar();
};
#endif
