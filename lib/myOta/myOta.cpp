/*
  myOta.cpp - Library to handle ArduinoOTA library.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "myota.h"




myOta::myOta()
{
}



void myOta::OTAConfig()
{
  delay(4000);
  Serial.print("\nSETUP: OTAConfig(): starting\n");

  Serial.print("SETUP: OTAConfig(): about to connect to Wifi\n");
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = 11;
  WiFi.mode(WIFI_STA);
  WiFi.config(IPAddress(192, 168, 43, globalBaseVariables.gui16NodeName),  IPAddress(192, 168, 43, 0), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, pass);
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = 10;
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Set the OTA port
  ArduinoOTA.setPort(3232);
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = 9;
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.actOnPrefsThroughCallback(&mySavedPrefs::saveOTASuccess, _myPrefsRef);

  // Set the hostname
  snprintf(gcHostnamePrefix, 10, "%s%u", gcHostnamePrefix, (uint32_t)globalBaseVariables.gui16NodeName); // LaserBox_201, etc.
  ArduinoOTA.setHostname(gcHostnamePrefix);
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = 8;
  _myPrefsRef.actOnPrefsThroughCallback(&mySavedPrefs::saveOTASuccess, _myPrefsRef);

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with its md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart( _startOTA ); //startOTA is a function created to simplify the code
  ArduinoOTA.onEnd( _endOTA ); //endOTA is a function created to simplify the code
  ArduinoOTA.onProgress( _progressOTA ); //progressOTA is a function created to simplify the code
  ArduinoOTA.onError( _errorOTA );//errorOTA is a function created to simplify the code
  ArduinoOTA.begin();

  delay(3000);

  //prints the ip address used by ESP
  Serial.printf("SETUP: OTAConfig(): ready\n");
  Serial.print("SETUP: OTAConfig(): IP address: ");Serial.println(WiFi.localIP());
}



void myOta::_startOTA()
{
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = 7;

  mySavedPrefs _myPrefsRef;
  _myPrefsRef.actOnPrefsThroughCallback(&mySavedPrefs::saveOTASuccess, _myPrefsRef);

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



void myOta::_endOTA()
{
  Serial.print("OTA Update End: switching gi8RequestedOTAReboots to true and resetStartupTypePreferences\n");
  // if 1, this means that there will be another OTA reboot after this one
  // else, it is 0, this means that there will be not be another OTA reboot after this one
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = 6;
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.actOnPrefsThroughCallback(&mySavedPrefs::saveOTASuccess, _myPrefsRef);
  Serial.printf("\nOTA Update End\n");
}



void myOta::_progressOTA(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
}




void myOta::_errorOTA(ota_error_t error)
{
  // if 1, this means that there will be another OTA reboot after this one
  // else, it is 0, this means that there will be not be another OTA reboot after this one
  mySavedPrefs::ui8OTASuccessErrorCodeWitness = error;
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.actOnPrefsThroughCallback(&mySavedPrefs::saveOTASuccess, _myPrefsRef);
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
  ESP.restart();
}




