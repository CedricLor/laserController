/*
  Myota.cpp - Library to handle ArduinoOTA library.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "Myota.h"

Myota::Myota()
{
}

void Myota::OTAConfig()
{
  delay(4000);
  Serial.print("\nSETUP: OTAConfig(): starting\n");

  Serial.print("SETUP: OTAConfig(): about to connect to Wifi\n");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname("ESP32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart( startOTA ); //startOTA is a function created to simplify the code
  ArduinoOTA.onEnd( endOTA ); //endOTA is a function created to simplify the code
  ArduinoOTA.onProgress( progressOTA ); //progressOTA is a function created to simplify the code
  ArduinoOTA.onError( errorOTA );//errorOTA is a function created to simplify the code
  ArduinoOTA.begin();

  delay(3000);

  //prints the ip address used by ESP
  Serial.printf("SETUP: OTAConfig(): ready\n");
  Serial.print("SETUP: OTAConfig(): IP address: ");Serial.println(WiFi.localIP());
}

void Myota::startOTA()
{
  String type;
  if (ArduinoOTA.getCommand() == U_FLASH) {
    type = "sketch";
  }
  else { // U_SPIFFS
    type = "filesystem";
    SPIFFS.end(); // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  }
  Serial.println("OTA Start updating " + type);
}

void Myota::endOTA()
{
  Serial.print("OTA Update End: switching i8RequestedOTAReboots to true and resetStartupTypePreferences\n");
  i8RequestedOTAReboots = false;
  mySavedPrefs::saveBoxSpecificPrefsWrapper(mySavedPrefs::saveBoxStartupTypePreferences);
  Serial.printf("\nOTA Update End\n");
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
