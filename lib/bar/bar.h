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
  friend class bars;
  friend class test;

  public:
    /** default empty constructor */
    bar();
    bar(std::array<note, 16> __notesArray, const int16_t __i16IndexNumber=-2); 
    bar( const bar& __bar);
    bar& operator=(const bar& __bar);
 
    /** getters */
    uint16_t const ui16GetNotesCountInBar() const;
    uint16_t const ui16GetBaseNotesCountInBar() const;
    uint32_t const ui32GetBarDuration() const;
    const std::array<note, 16> & getNotesArray() const;

    /** public instance properties */
    int16_t i16IndexNumber;

  private:
    /** private instance properties */
    std::array<note, 16> _notesArray;
};

#endif



#ifndef bars_h
#define bars_h
class bars
{
  friend class sequence;
  friend class test;
  
  public:
    /** default constructor 
     * 
     *  The bars' properties (_activeBar, _notes) 
     *  are defined in the constructor, by default, without
     *  parameters.
     * 
     *  The only parameters, _sendCurrentBar, is an optional callback. */
    bars(
      void (*_sendCurrentBar)(const int16_t __i16_current_bar_id)=nullptr
    );

    /** sender to mesh */
    void (*sendCurrentBar)(const int16_t __i16_current_bar_id);

    /** setters */
    uint16_t const setActive(const bar & __activeBar);
    void disableAndResetTPlayBar();
    void resetTPlayBar();

    /** getters */
    int16_t const i16GetCurrentBarId() const; // returns _i16ActiveBarId
    bar & getCurrentBar() const;
    bar & getBarFromBarArray(const uint16_t __ui16_bar_id);
    notes & getNotes();

    /** player */
    uint16_t const playBarStandAlone(const bar & __target_bar, beat const & __beat=beat());
    bool playBarInSequence(const bar & __target_bar);
    Task tPlayBar;

  private:
    // properties
    bar & _activeBar;
    notes _notes;
    std::array<bar, 7> _barsArray;

    /** player callbacks */
    void _tcbPlayBar();
    bool _oetcbPlayBar();
};
#endif
