/*
  bar.h - bars are precoded sequences of laserNotes
  Created by Cedric Lor, June 4, 2019.


*/
#include "Arduino.h"
#include <mySpiffs.h>
#include <laserNote.h>

#ifndef bar_h
#define bar_h

class bar
{
  friend class bars;
  friend class test;

  public:
    /** default empty constructor */
    bar();
    bar(std::array<laserNote, 16> __laserNotesArray, const int16_t __i16IndexNumber=-2); 
    bar( const bar& __bar);
    bar& operator=(const bar& __bar);
 
    /** getters */
    uint16_t const ui16GetNotesCountInBar() const;
    uint16_t const ui16GetBaseNotesCountInBar(const beat & _beat) const;
    uint32_t const ui32GetBarDuration(const beat & _beat) const;
    const std::array<laserNote, 16> & getNotesArray() const;

    /** public instance properties */
    int16_t i16IndexNumber;

  private:
    /** private instance properties */
    std::array<laserNote, 16> _laserNotesArray;
};

class bars
{
  friend class laserSequence;
  friend class laserSequences;
  friend class test;
  
  public:
    /** default constructor 
     * 
     *  The bars' properties (_activeBar, _laserNotes) 
     *  are defined in bars constructor, calling their default constructors without
     *  parameters.
     * 
     *  The only parameter, _sendCurrentBar, is an optional callback. */
    bars(
      myMeshViews & __thisMeshViews
      // void (*_sendCurrentBar)(const int16_t __i16_current_bar_id)=nullptr
    );


    /** Variables to read bars from SPIFSS */
    uint16_t ui16IxNumbOfBarToPreload;
    bar nextBar;
    char barFileName[20];

    /** setters */
    uint16_t const setActive(const bar & __activeBar);
    void disableAndResetTPlayBar();

    /** getters */
    int16_t const i16GetActiveBarId() const; // returns the current bar's i16IndexNumber
    laserNotes & getNotes();

    /** player */
    uint16_t const playBar(beat const & __beat);
    Task tPlayBar;

    /** Task - preload next bar from SPIFFS */
    Task tPreloadNextBar;

    void preloadNextBarStraight(uint16_t _ui16IxNumbOfBarToPreload);
    void preloadNextBarThroughTask(uint16_t _ui16IxNumbOfBarToPreload);

  private:
    /** interface to mesh */
    myMeshViews & _thisMeshViews;
    // void (*sendCurrentBar)(const int16_t __i16_current_bar_id);

    // properties
    laserNotes _laserNotes;
    bar _defaultBar;
    bar & _activeBar = _defaultBar;

    /** Task - preload next bar from SPIFFS */
    void _preloadNextBar(uint16_t _ui16IxNumbOfBarToPreload);
    void _tcbPreloadNextBar();
    void _preloadNextBarFromJSON(const JsonObject& _joBar);
    std::array<laserNote, 16> const _parseJsonNotesArray(const JsonArray& _JsonNotesArray);

    /** player callbacks */
    void _tcbPlayBar(beat const & __beat);
    bool _oetcbPlayBar();
};
#endif

