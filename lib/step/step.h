/*
  step.h - interface handling the sequences of boxstate in given session
  Created by Cedric Lor, July 23, 2019.


*/

#ifndef step_h
#define step_h

#include "Arduino.h"
#include <painlessMesh.h>

class step
{
  public:
    step(); // default constructor

    static void initSteps(); // initializer of the array of steps

    static Task tPlaySteps;
    static const short int STEPS_COUNT;

    static step steps[];

    char cName[25];  // array of character to hold the name of each step
    char cHtmlTag[4];  // array of character to hold the html tag of each step

  private:

    static const short int _NAME_CHAR_COUNT;
    static const short int _HTML_TAG_CHAR_COUNT;

    static void _tcbPlaySteps();
    static bool _oetcbPlaySteps();

    static Task _tPlayStep;
    static bool _oetcbPlayStep();
    static void _odtcbPlayStep();

    unsigned long _ulDuration; // duration for which the status shall stay active before automatically returning to default
    uint16_t _ui16AssociatedSequence;  // sequence associated to a given state
    uint16_t _ui16IRTrigger; // in this state, does signals from IR trigger a restart or interrupt
    uint16_t _ui16MeshTrigger; // in this state, does signals from IR trigger a restart or interrupt

    void _initStep(const char __cpName[], const char __cpHtmlTag[], const unsigned long __ulDuration, const uint16_t __ui16AssociatedSequence, const uint16_t __ui16associatedBoxState, const uint16_t __ui16onIRTrigger, const uint16_t __ui16onMeshTrigger, const uint16_t __ui16onExpire);
};

#endif
