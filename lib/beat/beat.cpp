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

beat _beat;
beat &beat::_activeBeat = _beat;
std::array<beat, 7> beat::_beats;



// pointer to functions to produce an interface for beat
void (*beat::sendCurrentBeat)(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat) = nullptr;




///////////////////////////////////
// Constructors
///////////////////////////////////
// default
beat::beat() :
  _ui16BaseBeatInBpm(0), 
  _ui16BaseNoteForBeat(0)
{
}

// parameterized
beat::beat(
  const uint16_t __ui16_base_beat_in_bpm, 
  const uint16_t __ui16_base_note_for_beat
):
  _ui16BaseBeatInBpm(__ui16_base_beat_in_bpm), 
  _ui16BaseNoteForBeat(__ui16_base_note_for_beat)
{
}

// // copy constructor
// beat::beat( const beat& )
// {
// }

// // assignement operator
// bar& bar::operator=(const bar& )
// {
//    return *this;
// }





///////////////////////////////////
// Initialisers
///////////////////////////////////
void beat::initBeats() {
  Serial.println("void beat::_initBars(). Starting.");
  // define an array containing predefined beats

  /** Signature of a beat:
   *  a. the duration of a beat in bpm (i.e. the tempo);
   *  b. the base note for each beat (full, half, quarter, etc.) (the 4 in 2/4, for instance) */

  /** 2 per minute
   * duration of a beat in bpm: 2
   * base note: 1 (a full) */
  _beats[0] = { 2, 1 };
  // Serial.println("bar::_initBars(). _bars[0]._ui16BaseBeatInBpm: ");Serial.println(_bars[0]._ui16BaseBeatInBpm);
  // Serial.println("bar::_initBars(). _bars[0]._iLaserPinStatusAtEachBeat[0][1]");Serial.println(_bars[0]._iLaserPinStatusAtEachBeat[0][1]);

  /** 120 per minute
   * duration of a beat in bpm: 120
   * base note: 1 (a full) */
  _beats[1] = { 120, 1 };

  Serial.println("void bar::_initBars(). Ending.");
}







///////////////////////////////////
// Setters
///////////////////////////////////
void beat::setActiveBeat(const uint16_t __ui16_base_beat_in_bpm, const uint16_t __ui16_base_note_for_beat) {
  // Serial.println("beat::setActiveBeat(). Starting.");
  // if (MY_DG_LASER) {
  //   Serial.print("beat::setActiveBeat(). (before setting) _activeBeat._ui16BaseBeatInBpm = ");Serial.println(_activeBeat._ui16BaseBeatInBpm);
  // }
  _activeBeat._ui16BaseBeatInBpm = __ui16_base_beat_in_bpm;
  // if (MY_DG_LASER) {
  //   Serial.print("beat::setActiveBeat(). (after setting) _activeBeat._ui16BaseBeatInBpm = ");Serial.println(_activeBeat._ui16BaseBeatInBpm);
  // }
  // if (MY_DG_LASER) {
  //   Serial.print("beat::setActiveBeat(). (before setting) _activeBeat._ui16BaseNoteForBeat = ");Serial.println(_activeBeat._ui16BaseNoteForBeat);
  // }
  _activeBeat._ui16BaseNoteForBeat = __ui16_base_note_for_beat;
  // if (MY_DG_LASER) {
  //   Serial.print("beat::setActiveBeat(). (after setting) _activeBeat._ui16BaseNoteForBeat = ");Serial.println(_activeBeat._ui16BaseNoteForBeat);
  // }
  // Serial.println("beat::setActiveBeat(). Ending.");
};


/** note::setActiveNoteFromNote(): public static setter method
 * 
 *  sets the parameters of the static variable &beat::_activeBeat
 *  from a passed in beat reference. */
void beat::setActiveBeatFromBeat(const beat & __target_beat) {
  _activeBeat = __target_beat;
}




///////////////////////////////////
// Getters
///////////////////////////////////
/** const beat &beat::getCurrentBeat(): public static getter method
 *  
 *  returns a reference to the beat instance that is currently active. */
const beat &beat::getCurrentBeat() {
  return _activeBeat;
}


/** uint16_t beat::getBaseBeatInBpm(): public instance getter method
 * 
 *  Returns the base beat in bpm of a beat instance. */
uint16_t beat::getBaseBeatInBpm() const {
  return _ui16BaseBeatInBpm;
}


/** uint16_t beat::getBaseNoteForBeat(): public instance getter method
 *  
 *  Returns the base note (the base note corresponding to one beat) 
 *  of a beat instance. */
uint16_t beat::getBaseNoteForBeat() const {
  return _ui16BaseNoteForBeat;
}


/** uint16_t beat::ui16GetBaseNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration, in ms, of the base note of a beat instance. */
uint16_t beat::ui16GetBaseNoteDurationInMs() const {
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). Starting.");
  // Serial.println(F("------------- DEBUG --------- note --------- DEBUG -------------"));
  // Serial.printf("beat::ui16GetBaseNoteDurationInMs(). _ui16BaseBeatInBpm = %u\n, _ui16BaseBeatInBpm);

  if (_ui16BaseBeatInBpm == 0) {
    return 0;
  }
  // see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
  uint16_t __ui16DurationInMs = ((60 * 1000) + _ui16BaseBeatInBpm - 1) / _ui16BaseBeatInBpm;
  // Serial.print("beat::ui16GetBaseNoteDurationInMs(). __ui16DurationInMs = ");Serial.println(__ui16DurationInMs);
  if (__ui16DurationInMs > 30000) {
    return 30000;
  }
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). Ending.");
  return __ui16DurationInMs;
}






