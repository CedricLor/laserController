/*
  Myota.cpp - Library to handle ArduinoOTA library.
  Created by Cedric Lor, January 2, 2019.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Myota.h"

Myota::Myota()
{
}

void Myota::OTAConfig()
{
  Serial.print("\nSETUP: OTAConfig(): starting\n");
  ArduinoOTA.onStart( startOTA ); //startOTA is a function created to simplificate the code
  ArduinoOTA.onEnd( endOTA ); //endOTA is a function created to simplificate the code
  ArduinoOTA.onProgress( progressOTA ); //progressOTA is a function created to simplificate the code
  ArduinoOTA.onError( errorOTA );//errorOTA is a function created to simplificate the code
  ArduinoOTA.begin();

  delay(3000);

  //prints the ip address used by ESP
  Serial.printf("SETUP: OTAConfig(): ready\n");
  Serial.print("SETUP: OTAConfig(): IP address: ");Serial.println(WiFi.localIP());
}

void Myota::startOTA()
{
  Serial.printf("Start updating filesystem\n");
}

void Myota::endOTA()
{
  Serial.printf("\nEnd\n");
}

void Myota::progressOTA(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
}

void Myota::errorOTA(ota_error_t error)
{
  Serial.printf("Error[%u]: \n", error);
  if (error == OTA_AUTH_ERROR)
    Serial.printf("Auth Failed\n");
  else
  if (error == OTA_BEGIN_ERROR)
    Serial.printf("Begin Failed\n");
  else
  if (error == OTA_CONNECT_ERROR)
    Serial.printf("Connect Failed\n");
  else
  if (error == OTA_RECEIVE_ERROR)
    Serial.printf("Receive Failed\n");
  else
  if (error == OTA_END_ERROR)
    Serial.printf("End Failed\n");
}
