#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPAsyncWebServer.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

#include "../lib/global.h"
#include "../lib/global.cpp"

#include "../lib/secret.h"

// #include "../lib/Myota.h"
// #include "../lib/Myota.cpp"
#include "../lib/ControlerBox.h"
#include "../lib/ControlerBox.cpp"
#include "../lib/mySavedPrefs.h"
#include "../lib/mySavedPrefs.cpp"

#include "../lib/MasterSlaveBox.h"
#include "../lib/MasterSlaveBox.cpp"

#include "../lib/myMeshViews.h"
#include "../lib/myMeshViews.cpp"

// #include "../lib/LaserGroupedUnit.h"
// #include "../lib/LaserGroupedUnit.cpp"
// #include "../lib/LaserGroupedUnitsArray.h"
// #include "../lib/LaserGroupedUnitsArray.cpp"
#include "../lib/LaserPinsArray.h"
#include "../lib/LaserPinsArray.cpp"
#include "../lib/LaserPin.h"
#include "../lib/LaserPin.cpp"

#include "../lib/myMesh.h"
#include "../lib/myMesh.cpp"

#include "../lib/tone.h"
#include "../lib/tone.cpp"
#include "../lib/note.h"
#include "../lib/note.cpp"
#include "../lib/bar.h"
#include "../lib/bar.cpp"
#include "../lib/sequence.h"
#include "../lib/sequence.cpp"
#include "../lib/boxState.h"
#include "../lib/boxState.cpp"

#include "../lib/pirController.h"
#include "../lib/pirController.cpp"

#include "../lib/myWebServerBase.h"
#include "../lib/myWebServerBase.cpp"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void enableTasks();


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();
  mySavedPrefs::loadPreferences();
  if (isInterface == false) {
    LaserPinsArray::initLaserPins();
    pirController::initPir();
  }
  myMesh::meshSetup();

  if (isInterface == true) {
    myWebServerBase::startAsyncServer();
  }
  // Myota::OTAConfig();

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // commented out to check whether this has any impact
  // ControlerBoxes[myIndexInCBArray].boxActiveState = 0;

  if (isInterface == false) {
    tone::initTones();
    bar::initBars();
    sequence::initSequences();
  }
  boxState::initBoxStates();

  enableTasks();

  // Serial.printf("setup. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("Box number: %i\n", gbNodeName);
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
    myWebServerBase::_tSendWSDataIfChangeBoxState.enable();
  }
}
