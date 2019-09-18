/*
  beat.cpp - beat handles the beat timing at which notes, bars and sequences shall be played
  Created by Cedric Lor, September 6, 2019.


Traductions en anglais:
- mesure = bar
- partition = score
- morceau = tune or composition
*/

#include "Arduino.h"
#include "beat.h"

// std::array<beat, 7> beat::_beats;

beat activeBeat;




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
beat::beat() :
  sendCurrentBeat(nullptr),
  _ui16BaseBeatInBpm(0), 
  _ui16BaseNoteForBeat(0)
{
}

// parameterized
beat::beat(
  const uint16_t __ui16_base_beat_in_bpm, 
  const uint16_t __ui16_base_note_for_beat,
  void (*_sendCurrentBeat)(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat)
):
  sendCurrentBeat(_sendCurrentBeat),
  _ui16BaseBeatInBpm(__ui16_base_beat_in_bpm), 
  _ui16BaseNoteForBeat(__ui16_base_note_for_beat)
{
}

// copy constructor
beat::beat( const beat& __beat ):
  sendCurrentBeat(__beat.sendCurrentBeat),
  _ui16BaseBeatInBpm(__beat._ui16BaseBeatInBpm), 
  _ui16BaseNoteForBeat(__beat._ui16BaseNoteForBeat)  
{
}

// assignement operator
beat& beat::operator=(const beat& __beat)
{
  if (&__beat != this) {
    sendCurrentBeat = __beat.sendCurrentBeat;
    _ui16BaseBeatInBpm = __beat._ui16BaseBeatInBpm;
    _ui16BaseNoteForBeat = __beat._ui16BaseNoteForBeat;
  }
  return *this;
}





///////////////////////////////////
// Initialisers
///////////////////////////////////
// void beat::initBeats() {
//   Serial.println("void beat::initBeats(). Starting.");
//   // define an array containing predefined beats

//   /** Signature of a beat:
//    *  a. the duration of a beat in bpm (i.e. the tempo);
//    *  b. the base note for each beat (full, half, quarter, etc.) (the 4 in 2/4, for instance) */

//   /** 2 per minute
//    * duration of a beat in bpm: 2
//    * base note: 1 (a full) */
//   _beats[0] = { 2, 1 };
//   // Serial.println("bar::initBeats(). _barsArray[0]._ui16BaseBeatInBpm: ");Serial.println(_barsArray[0]._ui16BaseBeatInBpm);
//   // Serial.println("bar::initBeats(). _barsArray[0]._iLaserPinStatusAtEachBeat[0][1]");Serial.println(_barsArray[0]._iLaserPinStatusAtEachBeat[0][1]);

//   /** 120 per minute
//    * duration of a beat in bpm: 120
//    * base note: 1 (a full) */
//   _beats[1] = { 120, 1 };

//   Serial.println("void bar::initBeats(). Ending.");
// }







///////////////////////////////////
// Getters
///////////////////////////////////
/** uint16_t beat::getBaseBeatInBpm(): public instance getter method
 * 
 *  Returns the base beat in bpm of a beat instance. */
uint16_t const beat::getBaseBeatInBpm() const {
  return _ui16BaseBeatInBpm;
}


/** uint16_t beat::getBaseNoteForBeat(): public instance getter method
 *  
 *  Returns the base note (the base note corresponding to one beat) 
 *  of a beat instance. */
uint16_t const beat::getBaseNoteForBeat() const {
  return _ui16BaseNoteForBeat;
}


/** uint16_t beat::ui16GetBaseNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration, in ms, of the base note of a beat instance. */
uint16_t const beat::ui16GetBaseNoteDurationInMs() const {
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). Starting.");
  // Serial.println(F("------------- DEBUG --------- beat --------- DEBUG -------------"));
  // Serial.printf("beat::ui16GetBaseNoteDurationInMs(). _ui16BaseBeatInBpm = %u\n", _ui16BaseBeatInBpm);

  if (_ui16BaseBeatInBpm == 0) {
    // Serial.println("beat::ui16GetBaseNoteDurationInMs(). Returning 0.");
    return 0;
  }
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). Passed the 0 test.");
  const uint16_t __ui16DurationInMs = (uint16_t)ceil(60000 / _ui16BaseBeatInBpm);
  // Serial.printf("beat::ui16GetBaseNoteDurationInMs(). __ui16DurationInMs = %u\n", __ui16DurationInMs);
  if (__ui16DurationInMs > 30000) {
    // Serial.println("beat::ui16GetBaseNoteDurationInMs(). __ui16DurationInMs is > 30000.");
    return 30000;
  }
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). Ending.");
  return __ui16DurationInMs;
}
