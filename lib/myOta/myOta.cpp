/*
  myOta.cpp - Library to handle ArduinoOTA library.
  Created by Cedric Lor, January 2, 2019.
*/

#include "Arduino.h"
#include "myota.h"


int8_t myOta::_i8OTASuccessErrorCode = 10; // undefined


myOta::myOta()
{
}



void myOta::OTAConfig()
{
  delay(4000);
  Serial.print("\nSETUP: OTAConfig(): starting\n");

  Serial.print("SETUP: OTAConfig(): about to connect to Wifi\n");
  WiFi.mode(WIFI_STA);
  WiFi.config(IPAddress(192, 168, 43, gui16NodeName),  IPAddress(192, 168, 43, 0), IPAddress(255, 255, 255, 0));
  WiFi.begin(ssid, pass);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Set the OTA port
  ArduinoOTA.setPort(3232);
  _i8OTASuccessErrorCode = 9;

  // Set the hostname
  snprintf(gcHostnamePrefix, 10, "%s%u", gcHostnamePrefix, (uint32_t)gui16NodeName); // LaserBox_201, etc.
  ArduinoOTA.setHostname(gcHostnamePrefix);
  _i8OTASuccessErrorCode = 8;

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
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
  _i8OTASuccessErrorCode = 7;

  mySavedPrefs _myPrefsRef;
  _myPrefsRef.saveBoxSpecificPrefsWrapper(_saveOTASuccess);

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
  _i8OTASuccessErrorCode = 6;
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.saveBoxSpecificPrefsWrapper(_saveOTASuccess);
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
  _i8OTASuccessErrorCode = error;
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.saveBoxSpecificPrefsWrapper(_saveOTASuccess);
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




void myOta::_saveOTASuccess() {
  // choose the location where to save the error code for this OTA update
  Serial.println("myOta::_saveOTASuccess: starting.");
  char __cOTASuccessErrorNVSKey[9] = "OTASucc";
  snprintf(__cOTASuccessErrorNVSKey, 9, "%s%u", __cOTASuccessErrorNVSKey, (uint32_t)(gi8RequestedOTAReboots + 1));
  
  // save the success code in the relevant NVS location
  mySavedPrefs _myPrefsRef;
  size_t __i8OTASuccessErrorCodeRet = _myPrefsRef._prefLib.putUChar(__cOTASuccessErrorNVSKey, _i8OTASuccessErrorCode);
  Serial.printf("%s OTA update numb. %u success code == %i %s\"%s\"\n", _myPrefsRef.debugSaveMsgStart, (gi8RequestedOTAReboots + 1), _i8OTASuccessErrorCode, (__i8OTASuccessErrorCodeRet)?(_myPrefsRef.debugSaveMsgEndSuccess):(_myPrefsRef.debugSaveMsgEndFail), __cOTASuccessErrorNVSKey);
}
