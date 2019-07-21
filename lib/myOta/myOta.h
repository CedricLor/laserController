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

class myOta
{
  public:
    myOta();
    static void OTAConfig();
    static int8_t OTASuccessErrorCode;

  private:
    static void saveOTASuccess(Preferences& _preferences);
    static void startOTA();
    static void endOTA();
    static void progressOTA(unsigned int progress, unsigned int total);
    static void errorOTA(ota_error_t error);
};

#endif
