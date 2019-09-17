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
    bar(std::array<note, 16> __notesArray); 
    bar( const bar& __bar);
    bar& operator=(const bar& __bar);
 
    /** getters */
    uint16_t const ui16GetNotesCountInBar() const;
    uint16_t const ui16GetBaseNotesCountInBar() const;
    uint32_t const ui32GetBarDuration() const;
    const std::array<note, 16> & getNotesArray() const;

  private:
    // private instance properties
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
     *  The bars' properties (_activeBar, _i16ActiveBarId, _notes) 
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
    void setActive(const bar & __activeBar);
    void disableAndResetTPlayBar();
    void resetTPlayBar();

    /** getters */
    int16_t const i16GetCurrentBarId() const; // returns _i16ActiveBarId
    bar & getCurrentBar() const;
    bar & getBarFromBarArray(const uint16_t __ui16_bar_id);
    notes & getNotes();

    /** player */
    bool playBarStandAlone(const bar & __target_bar, beat const & __beat=beat::_activeBeat);
    bool playBarInSequence(const bar & __target_bar); // <-- TODO: we need to pass the active_bar id if we play a bar coming from the array, mainly for debug purposes 
    Task tPlayBar;
    Task & tPlayNote;

  private:
    // properties
    // TODO: Check whether we really need all these variables
    bar & _activeBar;
    int16_t _i16ActiveBarId;
    notes _notes;
    std::array<bar, 7> _barsArray;

    /** player callbacks */
    void _tcbPlayBar();
    bool _oetcbPlayBar();
};
#endif
