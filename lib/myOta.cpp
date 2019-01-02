/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Myota.h"

Myota::Myota()
{
}

void Myota::OTAConfig()
{
}

void Myota::startOTA()
{
  Serial.print("Start updating filesystem\n");
}

void Myota::endOTA()
{
  Serial.print("\nEnd\n");
}

void Myota::progressOTA(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void Myota::errorOTA(ota_error_t error)
{
  Serial.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR)
    Serial.print("Auth Failed\n");
  else
  if (error == OTA_BEGIN_ERROR)
    Serial.print("Begin Failed\n");
  else
  if (error == OTA_CONNECT_ERROR)
    Serial.print("Connect Failed\n");
  else
  if (error == OTA_RECEIVE_ERROR)
    Serial.print("Receive Failed\n");
  else
  if (error == OTA_END_ERROR)
    Serial.print("End Failed\n");
}
