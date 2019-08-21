/*
  myOta.h - Library to handle ArduinoOTA library.
  Created by Cedric Lor, January 2, 2019.
*/

#ifndef myOta_h
#define myOta_h

#include "Arduino.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#include <global.h>
#include <secret.h>
#include <mySavedPrefs.h>

class myOta
{
  public:
    myOta();
    static void OTAConfig();

  private:
    static int8_t _i8OTASuccessErrorCode;

    static void _saveOTASuccess(mySavedPrefs * _myPrefsRef);
    static void _startOTA();
    static void _endOTA();
    static void _progressOTA(unsigned int progress, unsigned int total);
    static void _errorOTA(ota_error_t error);
};

#endif
