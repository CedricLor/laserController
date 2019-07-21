/*
  myOta.cpp - Library to handle ArduinoOTA library.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "myota.h"


int8_t myOta::OTASuccessErrorCode = 10; // undefined


myOta::myOta()
{
}



void myOta::OTAConfig()
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
  OTASuccessErrorCode = 9;
  ArduinoOTA.setHostname("ESP32");
  OTASuccessErrorCode = 8;

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



void myOta::startOTA()
{
  OTASuccessErrorCode = 7;
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



void myOta::endOTA()
{
  Serial.print("OTA Update End: switching i8RequestedOTAReboots to true and resetStartupTypePreferences\n");
  // if 1, this means that there will be another OTA reboot after this one
  // else, it is 0, this means that there will be not be another OTA reboot after this one
  OTASuccessErrorCode = 6;
  mySavedPrefs::saveBoxSpecificPrefsWrapper(saveOTASuccess);
  Serial.printf("\nOTA Update End\n");
}



void myOta::progressOTA(unsigned int progress, unsigned int total)
{
  Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
}




void myOta::errorOTA(ota_error_t error)
{
  // if 1, this means that there will be another OTA reboot after this one
  // else, it is 0, this means that there will be not be another OTA reboot after this one
  OTASuccessErrorCode = error;
  mySavedPrefs::saveBoxSpecificPrefsWrapper(saveOTASuccess);
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




void myOta::saveOTASuccess(Preferences& _preferences) {
  // choose the location where to save the error code for this OTA update
  char __cOTASuccessErrorNVSKey[9] = "OTASucc";
  strcat(__cOTASuccessErrorNVSKey, (char*)(i8RequestedOTAReboots + 1));
  // save the success code in the relevant NVS location
  size_t _OTASuccessErrorCodeRet = _preferences.putChar(__cOTASuccessErrorNVSKey, OTASuccessErrorCode);
  Serial.printf("%s OTA update numb. %i success code == %i %s\"%s\"\n", mySavedPrefs::debugSaveMsgStart, (i8RequestedOTAReboots + 1), OTASuccessErrorCode, (_OTASuccessErrorCodeRet)?(mySavedPrefs::debugSaveMsgEndSuccess):(mySavedPrefs::debugSaveMsgEndFail), __cOTASuccessErrorNVSKey);
}
