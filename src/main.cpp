#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPAsyncWebServer.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

#include <global.h>
#include <global.cpp>

#include "../lib/secret.h"

// #include "../lib/myOta/Myota.cpp"

painlessMesh laserControllerMesh;
Scheduler    userScheduler;

#include "../lib/ControlerBox.cpp"

ControlerBox ControlerBoxes[BOXES_COUNT];

#include "../lib/mySavedPrefs.cpp"

#include "../lib/myMesh.cpp"

#include "../lib/boxState.cpp"

#include "../lib/pirController.cpp"

#include "../lib/myWebServerBase.cpp"




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void enableTasks();


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables //////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();
  // create the ControlerBoxes array

  mySavedPrefs::loadPrefsWrapper();

  myMesh::meshSetup();

  if (isInterface == false) {
    pirController::initPir(); // depends on ControlerBox and Mesh classes
  }

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  if (isInterface == true) {
    myWebServerBase::startAsyncServer();
  }
  // Myota::OTAConfig();

  if (isInterface == false) {
    tone::initTones(); // inits also laserPin class; note does not need init
    bar::initBars();
    sequence::initSequences();
  }
  boxState::initBoxStates();

  enableTasks();

  // Serial.printf("setup. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("Box number: %i\n", gui8NodeName);
  Serial.printf("Version: %i\n", VERSION);
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
  // for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
  //    digitalWrite(relayPins[__thisPin], LOW);
  //    Serial.print("\n------ PIN number: ");Serial.print(relayPins[__thisPin]);Serial.print("\n");
  // }
}







void loop() {
  ArduinoOTA.handle();
  userScheduler.execute();   // it will run mesh scheduler as well
  laserControllerMesh.update();
  pirController::pirCntrl();
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serialInit() {
  Serial.begin(115200);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("\nSETUP: serialInit(): done\n");
}




void enableTasks() {
  if (isInterface == false) {
    boxState::tPlayBoxStates.enable();
  } else {
    myWSSender::tSendWSDataIfChangeBoxState.enable();
  }
}
