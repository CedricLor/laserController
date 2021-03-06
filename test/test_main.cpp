#ifdef UNIT_TEST
#include <Arduino.h>

#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPAsyncWebServer.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

extern constexpr short    UI8_BOXES_COUNT                     = 10;

#include <globalBasementVars.h>
#include <mySavedPrefs.h>
#include <myOta.h>
#include <controllerBoxThis.h>
#include <laserSignal.h>
#include <myMesh.h>
#include <myMeshStarter.h>
#include <myWebServerBase.h>

#include <unity.h>

controllerBoxThis thisControllerBox;

void setUp(void) {
  // set stuff up here
  Serial.println("\nStarting SPIFFS");
  if(!SPIFFS.begin(true)){
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }
}

void tearDown(void) {
// clean stuff up here
}

void test_instantiate_myWebServerBase(void) {
  myWebServerBase _myWebServer;
  TEST_ASSERT_EQUAL_INT(_myWebServer.getStatus(), 0);
}

void test_instantiate_WSReceiverReconcile(void) {
  const char * _msg = "{action:\"handshake\",boxStateInDOM:{1:4;2:3}}";
  const int _capacity = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(1);
  StaticJsonDocument<_capacity> _jdoc;
  deserializeJson(_jdoc, _msg);
  JsonObject _jobj = _jdoc.as<JsonObject>();
  myWSReceiverReconcile _myWSReceiverReconcile(
    _jobj, 
    thisControllerBox.thisLaserSignalHandler.ctlBxColl.controllerBoxesArray, 
    thisControllerBox.thisLaserSignalHandler.ctlBxColl.ui16connectedBoxesCount, 
    thisControllerBox.globBaseVars.gui16BoxesCount
  );
}

void setup() {
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(test_instantiate_myWebServerBase);
  RUN_TEST(test_instantiate_WSReceiverReconcile);
}

void loop() {
  UNITY_END();
}

#endif