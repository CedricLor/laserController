/*
  bar.h - bars are precoded sequences of notes
  Created by Cedric Lor, June 4, 2019.


*/
#include "Arduino.h"
#include <mySpiffs.h>
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
    uint16_t const ui16GetBaseNotesCountInBar(const beat & _beat) const;
    uint32_t const ui32GetBarDuration(const beat & _beat) const;
    const std::array<note, 16> & getNotesArray() const;

    /** public instance properties */
    int16_t i16IndexNumber;

  private:
    /** private instance properties */
    std::array<note, 16> _notesArray;
};

class bars
{
  friend class sequence;
  friend class sequences;
  friend class test;
  
  public:
    /** default constructor 
     * 
     *  The bars' properties (_activeBar, _notes) 
     *  are defined in bars constructor, calling their default constructors without
     *  parameters.
     * 
     *  The only parameter, _sendCurrentBar, is an optional callback. */
    bars(
      void (*_sendCurrentBar)(const int16_t __i16_current_bar_id)=nullptr
    );
    // bars(const bars& __bars);
    // bars& operator=(const bars& __bars);

    /** sender to mesh */
    void (*sendCurrentBar)(const int16_t __i16_current_bar_id);

    /** Variables to read bars from SPIFSS */
    uint16_t ui16IxNumbOfBarToPreload;
    bar nextBar;
    char barFileName[20];

    /** setters */
    uint16_t const setActive(const bar & __activeBar);
    void disableAndResetTPlayBar();

    /** getters */
    int16_t const i16GetCurrentBarId() const; // returns the current bar's i16IndexNumber
    bar const & getCurrentBar() const;
    bar const & getBarFromBarArray(const uint16_t __ui16_bar_id) const;
    notes & getNotes();
    bar const & at(const uint16_t __ui16_bar_id) const;

    /** player */
    uint16_t const playBar(beat const & __beat);
    Task tPlayBar;

    /** Task - preload next bar from SPIFFS */
    Task tPreloadNextBar;

    void _preloadNextBar(uint16_t _ui16IxNumbOfBarToPreload);
    void _tcbPreloadNextBar();
    void _preloadNextBarFromJSON(const JsonObject& _joBar);
    std::array<note, 16> const _parseJsonNotesArray(const JsonArray& _JsonNotesArray);

  private:
    // properties
    bar _defaultBar;
    bar & _activeBar = _defaultBar;
    notes _notes;
    std::array<bar, 7> _barsArray;

    /** player callbacks */
    void _tcbPlayBar(beat const & __beat);
    bool _oetcbPlayBar();
};
#endif

