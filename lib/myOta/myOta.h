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
#include <SPIFFS.h>
#include <mySavedPrefs.h>

class myOta
{
  public:
    myOta();
    static void OTAConfig();



  private:

    static void _startOTA();
    static void _endOTA();
    static void _progressOTA(unsigned int progress, unsigned int total);
    static void _errorOTA(ota_error_t error);
};

#endif
