/*
  laserNote.cpp - laserNotes are laserTones played for a given length (expressed in base beat)
  Created by Cedric Lor, June 28, 2019.

  * 1. ronde                   : 16 : 16 / 1
  * 2. blanche                 : 8  : 16 / 2
  * 3. noire + croche          : 6  : 16 / 4 + (1/2)*(16 / 4) : 16 modulo 3 = 1
  * 4. noire                   : 4  : 16 / 4
  * 6. croche + double croche  : 3  : 16 / 8 + (1/2)*(16 / 8) : 16 modulo 6 = 4
  * 8. croche                  : 2  : 16 / 8
  * 16. double croche          : 1  : 16 / 16

*/

#include "Arduino.h"
#include "laserNote.h"



//****************************************************************//
// laserNote //********************************************************//
//****************************************************************//

///////////////////////////////////
// Constructors
///////////////////////////////////
// default
laserNote::laserNote() :
  _ui16LaserTone(0),
  _ui16Note(0)
{}

// parameterized
laserNote::laserNote(
  const uint16_t __ui16_laser_tone,
  const uint16_t __ui16_note
) :
  _ui16LaserTone(__ui16_laser_tone),
  _ui16Note(__ui16_note)
{ _ui16ValidNote(); }

// copy constructor
laserNote::laserNote(const laserNote& __laserNote) :   
  _ui16LaserTone(__laserNote._ui16LaserTone),
  _ui16Note(__laserNote._ui16Note)
{ _ui16ValidNote(); }

// assignement operator
laserNote& laserNote::operator=(const laserNote& __laserNote)
{
  if (&__laserNote != this) {
    _ui16LaserTone = __laserNote._ui16LaserTone;
    _ui16Note = __laserNote._ui16Note;
    _ui16ValidNote();
  }
  return *this;
}

laserNote::laserNote(laserNote&& __laserNote):
  _ui16LaserTone(0),
  _ui16Note(0)
{
  *this = std::move(__laserNote);
}

laserNote & laserNote::operator=(laserNote&& __laserNote) {
  if (this != &__laserNote) {
    _ui16LaserTone = 0;
    _ui16Note = 0;

    _ui16LaserTone = __laserNote._ui16LaserTone;
    _ui16Note = __laserNote._ui16Note;

    __laserNote._ui16LaserTone = 0;
    __laserNote._ui16Note = 0;
  }
  return *this;
}





///////////////////////////////////
// Setters
///////////////////////////////////
/** uint16_t laserNote::_setTone(const laserTone & _target_laser_tone): private instance setter method
 * 
 *  sets the instance reference member _laserTone (laserTone & _laserTone) to the passed-in reference-to 
 *  const laserTone. */
uint16_t laserNote::_setLaserTone(const laserTone & _target_laser_tone) {
  _ui16LaserTone = _target_laser_tone.i16IndexNumber;
  return _ui16LaserTone;
}



/** laserNote::_validNote(): private instance setter method
 * 
 *  changes passed-in laserNotes to the valid laserNotes. */
uint16_t laserNote::_ui16ValidNote() {
  if (_ui16Note == 5) {
    _ui16Note = 6;
  }
  if ((_ui16Note > 6) && (_ui16Note < 11)) {
    _ui16Note = 8;
  }
  if (_ui16Note > 10) {
    _ui16Note = 16;
  }
  return _ui16Note;
}









///////////////////////////////////
// Getters
///////////////////////////////////
/** uint16_t laserNote::getLaserToneNumber(): public instance getter method
 * 
 *  Returns the laserTone number of a laserNote instance. */
uint16_t const laserNote::getLaserToneNumber() const {
  return _ui16LaserTone;
}


/** uint16_t laserNote::getNote(): public instance getter method
 *  
 *  Returns the laserNote of a laserNote instance. */
uint16_t const laserNote::getNote() const {
  return _ui16Note;
}


/** uint16_t laserNote::ui16GetNoteDurationInMs(): public instance getter method
 *  
 *  Returns the duration of a laserNote instance in ms. */
uint16_t const laserNote::ui16GetNoteDurationInMs(const beat & _beat) const {
  // Serial.println("laserNote::ui16GetNoteDurationInMs(). starting");
  // Serial.println(F("------------- DEBUG --------- laserNote --------- DEBUG -------------"));
  // Serial.printf("laserNote::ui16GetNoteDurationInMs(). _ui16Note == %u\n", _ui16Note);
  // Serial.printf("laserNote::ui16GetNoteDurationInMs(). _beat.getBaseNoteForBeat() == %u\n", _beat.getBaseNoteForBeat());

  if ((_ui16Note == 0) || (_beat.getBaseNoteForBeat() == 0)) {
    return 0;
  }

  // const uint16_t __ui16baseNoteDuration = _beat.ui16GetBaseNoteDurationInMs();
  // Serial.printf("laserNote::ui16GetNoteDurationInMs(). __ui16baseNoteDuration == %u\n", __ui16baseNoteDuration);

  // const uint16_t __ui16BaseNoteForBeat = _beat.getBaseNoteForBeat();
  // Serial.printf("laserNote::ui16GetNoteDurationInMs(). __ui16BaseNoteForBeat == %u\n", __ui16BaseNoteForBeat);

  // const uint16_t __num = __ui16baseNoteDuration * __ui16BaseNoteForBeat;
  // Serial.printf("laserNote::ui16GetNoteDurationInMs(). __num == %u\n", __num);

  // see https://stackoverflow.com/questions/17005364/dividing-two-integers-and-rounding-up-the-result-without-using-floating-point
  uint16_t __ui16DurationInMs =
    (uint16_t)ceil(
      ((_beat.getBaseNoteForBeat() * _beat.ui16GetBaseNoteDurationInMs()))
      / _ui16Note
    );
  // Serial.printf("laserNote::ui16GetNoteDurationInMs(). __ulDurationInM == %u\n", __ui16DurationInMs);

  if (__ui16DurationInMs > 30000) {
    return 30000;
  }
  // Serial.println("laserNote::ui16GetNoteDurationInMs(). over");
  return __ui16DurationInMs;
}









///////////////////////////////////
// Players
///////////////////////////////////
/** const int16_t laserNote::_playTone() const */
const int16_t laserNote::_playLaserTone(laserTones const & __laser_tones) const {
  return __laser_tones._playLaserTone(getLaserToneNumber());
}





//****************************************************************//
// Notes //*******************************************************//
//****************************************************************//

laserNotes::laserNotes(
  myMeshViews & __thisMeshViews
):
  _thisMeshViews(__thisMeshViews),
  _laserTones(_thisMeshViews)
{
  _disableAndResetTPlayNote();
}
// // copy constructor
// laserNotes::laserNotes(const laserNotes & __laserNotes):
//   sendCurrentNote(__laserNotes.sendCurrentNote),
//   _activeLaserNote(__laserNotes._activeLaserNote),
//   _laserTones(__laserNotes._laserTones)
// {
//   _disableAndResetTPlayNote();
// }
// // copy assignment operator
// laserNotes & laserNotes::operator=(const laserNotes & __laserNotes) {
//   if (&__laserNotes != this) {
//     sendCurrentNote = __laserNotes.sendCurrentNote;
//     _activeLaserNote = __laserNotes._activeLaserNote;
//     _laserTones = __laserNotes._laserTones;
//     _disableAndResetTPlayNote();
//   }
//   return *this;
// }
// // move constructor
// laserNotes::laserNotes(laserNotes&& __laserNotes):
//   sendCurrentNote(nullptr),
//   _laserTones({})
// {
//   *this = std::move(__laserNotes);
// }
// // move assignment op
// laserNotes & laserNotes::operator=(laserNotes&& __laserNotes) {
//   if (this != &__laserNotes) {
//     sendCurrentNote = nullptr;
//     _activeLaserNote = laserNote{};
//     _laserTones = {};

//     sendCurrentNote = __laserNotes.sendCurrentNote;
//     _activeLaserNote = __laserNotes._activeLaserNote;
//     _laserTones = __laserNotes._laserTones;

//     __laserNotes.sendCurrentNote = nullptr;
//     __laserNotes._activeLaserNote = laserNote{};
//     __laserNotes._laserTones = {};

//     _disableAndResetTPlayNote();
//   }
//   return *this;
// }



///////////////////////////////////
// Setters
///////////////////////////////////
/** laserNotes::setActive(const laserNote & __activeLaserNote): public instance setter method
 * 
 *  sets the instance variable _activeLaserNote from a passed in laserNote reference.
 *  
 *  The _activeLaserNote is then used: 
 *  (i)  in this class (laserNotes), to provide 
 *       (a) the interval param to Task tPlayNote (ie. the laserNote duration);
 *       (b) to call _playLaserTone (ie. the laserNote laserTone). <-- TODO: correct erroneous comments
 *  (ii) in laserInterface, to retrieve the params (_ui16Note and _ui16Tone) of
 *       the currently active laserNote and send them to the mesh. */
void laserNotes::setActive(const laserNote & __activeLaserNote) {
  // Serial.println("laserNotes::setActive: starting");
  _disableAndResetTPlayNote();
  // Serial.println("laserNotes::setActive: tPlayNote disabled");
  // Serial.printf("laserNotes::setActive: __activeLaserNote._ui16Tone: %u\n", __activeLaserNote._ui16Tone);
  // Serial.printf("laserNotes::setActive: __activeLaserNote._ui16Note: %u\n", __activeLaserNote._ui16Note);
  // Serial.printf("laserNotes::setActive: _activeLaserNote._ui16Tone: %u\n", _activeLaserNote._ui16Tone);
  // Serial.printf("laserNotes::setActive: _activeLaserNote._ui16Note: %u\n", _activeLaserNote._ui16Note);
  _activeLaserNote = __activeLaserNote;
  // Serial.println("laserNotes::setActive: over");
}


/** laserNotes::_disableAndResetTPlayNote(): public setter method
 * 
 *  disables Task tPlayNote and resets its default parameters, so as
 *  it be ready to play any laserNote.
 * 
 *  Default parameters:
 *  - an interval of 30.000 seconds;
 *  - 1 iteration;
 *  - no main callback;
 *  - an onEnable callback, that turns the lasers on in a given laserTone;
 *  - an onDisable callback, that turn the lasers off.
 * 
 * 
 *   The Task is enabled upon instanciating a laserNote in the bar class.
 *   It is disabled:
 *   - at the expiration of the interval set herein (30000 ms); or
 *   - by _tcbPlayBar, the main callback of tPlayBar, when the laserNote is supposed
 *     to finish.
 *   It does not need any mainCallback, as it does not iterate.
 * 
 *   task tPlaynote plays a given laserTone (set in laserNote::_ui16ActiveTone)
 *   for a given laserNote type (--> full, half, ..., set in the bar) at a given
 *   beat rate. 
 *  */
void laserNotes::_disableAndResetTPlayNote() {
  tPlayNote.disable();
  tPlayNote.set(30000, 1, NULL, [&](){return this->_oetcbPlayNote();}, [&](){this->_odtcbPlayNote();});
}




///////////////////////////////////
// Getters
///////////////////////////////////
/** const laserNote &laserNotes::getCurrentNote() const: public getter method
 *  
 *  returns a reference to the laserNote instance that is currently active. */
const laserNote &laserNotes::getCurrentNote() const {
  return _activeLaserNote;
}


///////////////////////////////////
// Task - Player
///////////////////////////////////
/** bool const laserNotes::playNote(const laserNote & __laserNote, const beat & __beat):
 *  
 *  Play a single laserNote for a given duration (calculated using the passed-in beat)
 *  or the maximum duration (30.000 ms) if the calculated duration exceeds 30000 ms.
 *  by enabling Task tPlayNote.
 *  
 *  Task tPlayNote may then be disabled by calls to _disableAndResetTPlayNote().
 * 
 *  {@ params} const laserNote & __laserNote: pass a constant reference to a laserNote used
 *             to calculate the laserNotes duration
 *  {@ params} const beat & beat: pass a constant reference to a beat used
 *             to calculate the laserNotes duration.
 * */
bool const laserNotes::playNote(const laserNote & __laserNote, const beat & __beat) {
  Serial.println("laserNotes::playNote: starting");
  if ((__beat.getBaseNoteForBeat() == 0) || (__beat.getBaseBeatInBpm() == 0)) {
    return false;
  }
  setActive(__laserNote);
  tPlayNote.setInterval(__laserNote.ui16GetNoteDurationInMs(__beat));
  tPlayNote.restartDelayed();
  Serial.println("laserNotes::playNote: over");
  return true;
}




/** laserNotes::_oetcbPlayNote()
 *  On enable Task _tNote, turn the lasers to a given laserTone */
bool laserNotes::_oetcbPlayNote() {
  Serial.println("laserNotes::_oetcbPlayNote(). starting");
  if (globalBaseVariables.MY_DG_LASER) {
    Serial.printf("laserNotes::_oetcbPlayNote(). Going to play laserTone number (_activeLaserNote.getLaserToneNumber()) %u\n", _activeLaserNote.getLaserToneNumber());
    Serial.printf("laserNotes::_oetcbPlayNote(). Going to play laserTone number (_laser_tones.at(_activeLaserNote.getLaserToneNumber()).i16IndexNumber) %i\n", _laserTones.at(_activeLaserNote.getLaserToneNumber()).i16IndexNumber);
    Serial.printf("laserNotes::_oetcbPlayNote(). TEST: (_activeLaserNote.getLaserToneNumber()) == (_laser_tones.at(_activeLaserNote.getLaserToneNumber()).i16IndexNumber): [%i]\n", (_activeLaserNote.getLaserToneNumber()) == (_laserTones.at(_activeLaserNote.getLaserToneNumber()).i16IndexNumber));
  }
  bool _return = false;
  if (_activeLaserNote._playLaserTone(_laserTones) >= 0) {
    _thisMeshViews.sendNote(_activeLaserNote._ui16LaserTone, _activeLaserNote._ui16Note);
    _return = true;
  }
  Serial.printf("laserNotes::_oetcbPlayNote(). returning [%s]\n", (_return == true) ? "true" : "false");
  return _return;
}


/** laserNotes::_odtcbPlayNote()
 * 
 *  On disable Task _tNote, turn off all the lasers */
void laserNotes::_odtcbPlayNote() {
  Serial.println("laserNotes::_odtcbPlayNote(). starting");
  if (globalBaseVariables.MY_DG_LASER) {
    Serial.print("laserNotes::_oetcbPlayNote(). Turning off all the lasers");
  }
  _activeLaserNote._setLaserTone(_laserTones.at(0)); // _laserTones.at(0) means turn off all the lasers
  _activeLaserNote._playLaserTone(_laserTones);
  Serial.println("laserNotes::_odtcbPlayNote(). over");
}
