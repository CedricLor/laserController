/**
 * beat.cpp - beat handles the beat timing at which laserNotes, bars and laserSequences shall be played
 * Created by Cedric Lor, September 6, 2019.
 * 
 * Traductions en anglais:
 * - mesure = bar
 * - partition = score
 * - morceau = tune or composition
 * */
#include "Arduino.h"
#include "beat.h"





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

/** parameterized constructor
 * 
 * {@} params: const uint16_t __ui16_base_beat_in_bpm
 * {@} params: const uint16_t __ui16_base_note_for_beat
 * {@} params: void (*_sendCurrentBeat)(
 *        const uint16_t __ui16_base_beat_in_bpm,
 *        const uint16_t __ui16_base_note_for_beat)
 *  */
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
  _ui16BaseBeatInBpm(__beat._ui16BaseBeatInBpm), 
  _ui16BaseNoteForBeat(__beat._ui16BaseNoteForBeat)  
{
  if (__beat.sendCurrentBeat != nullptr) {
    sendCurrentBeat = __beat.sendCurrentBeat;
  }
}

// assignement operator
beat& beat::operator=(const beat & __beat)
{
  if (&__beat != this) {
    if (__beat.sendCurrentBeat != nullptr) {
      sendCurrentBeat = __beat.sendCurrentBeat;
    }
    _ui16BaseBeatInBpm = __beat._ui16BaseBeatInBpm;
    _ui16BaseNoteForBeat = __beat._ui16BaseNoteForBeat;
  }
  return *this;
}

// move constructor
beat::beat(beat&& __beat):
  _ui16BaseBeatInBpm(0), 
  _ui16BaseNoteForBeat(0)
{
  *this = std::move(__beat);
}

// move assignment op
beat & beat::operator=(beat&& __beat) {
  if (this != &__beat) {
    _ui16BaseBeatInBpm = 0;
    _ui16BaseNoteForBeat = 0;

    if (__beat.sendCurrentBeat != nullptr) {
      sendCurrentBeat = __beat.sendCurrentBeat;
    }
    _ui16BaseBeatInBpm = __beat._ui16BaseBeatInBpm;
    _ui16BaseNoteForBeat = __beat._ui16BaseNoteForBeat;

    __beat._ui16BaseBeatInBpm = 0;
    __beat._ui16BaseNoteForBeat = 0;
  }
  return *this;
}



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
 *  Returns the base laserNote (the base laserNote corresponding to one beat) 
 *  of a beat instance. */
uint16_t const beat::getBaseNoteForBeat() const {
  return _ui16BaseNoteForBeat;
}




/** uint16_t beat::ui16GetBaseNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration, in ms, of the base laserNote of a beat instance. */
uint16_t const beat::ui16GetBaseNoteDurationInMs() const {
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). starting");
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
  // Serial.println("beat::ui16GetBaseNoteDurationInMs(). over");
  return __ui16DurationInMs;
}
