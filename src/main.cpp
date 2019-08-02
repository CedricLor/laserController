#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPAsyncWebServer.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

#include <global.cpp>
#include <secret.h>

painlessMesh laserControllerMesh;
Scheduler    userScheduler;

#include <../lib/ControlerBox.cpp>

#include "../lib/mySavedPrefs/mySavedPrefs.cpp"

#include <../lib/myOta/myota.cpp>

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
ControlerBox ControlerBoxes[UI8_BOXES_COUNT];


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();

  mySavedPrefs::loadPrefsWrapper();

  // The ESP was restarted with an OTA request saved in mySavedPrefs
  if (gi8OTAReboot) {
    myOta::OTAConfig();
    return;
  }

  // The ESP was restarted normally

  myMesh::meshSetup();

  if (isInterface == false) {
    pirController::initPir(); // depends on ControlerBox and Mesh classes
  }

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  mySpiffs _mySpiffs;
  _mySpiffs.convertJsonFilePrettyToUgly("/pretty-sessions.json", ControlerBoxes[gui16MyIndexInCBArray].ui16NodeName);

  if (isInterface) {
    myWebServerBase::startAsyncServer();
  }


  if (isInterface == false) {
    tone::initTones(); // inits also laserPin class; note does not need init
    bar::initBars();
    sequence::initSequences();
  }
  boxState::initBoxStates();

  enableTasks();

  // Serial.printf("setup. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("Box number: %i\n", gui16NodeName);
  Serial.printf("Version: %i\n", VERSION);
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
  // for (short __thisPin = 0; __thisPin < PIN_COUNT; __thisPin++) {
  //    digitalWrite(relayPins[__thisPin], LOW);
  //    Serial.print("\n------ PIN number: ");Serial.print(relayPins[__thisPin]);Serial.print("\n");
  // }

}







void loop() {
  if (gi8OTAReboot) {
    ArduinoOTA.handle();
    return;
  }
  
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
