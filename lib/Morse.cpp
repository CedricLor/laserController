/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Morse.h"

Morse::Morse()
{
  // pinMode(pin, OUTPUT);
}

void Morse::dot()
{
}

void Morse::dash()
{
}

void Morse::startota()
{
  Serial.print("Start updating filesystem\n");
}

void Morse::endota()
{
  Serial.print("\nEnd\n");
}

void Morse::progressota(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void Morse::errorota(ota_error_t error)
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
