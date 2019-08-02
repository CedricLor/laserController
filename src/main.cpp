#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <IPAddress.h>
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

#include <global.h>
#include <secret.h>

#include <ControlerBox.h>

#include <mySavedPrefs.h>
#include <myOta.h>
#include <myMesh.h>

#include <boxState.h>

#include <pirController.h>

#include <myWebServerBase.h>




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void enableTasks();


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables //////////////////////////////////////////////////////////////////////////////////////////////
// ControlerBox ControlerBoxes[BOXES_COUNT];
ControlerBox ControlerBoxes[10];

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
  
  bool taskExecuted = myTaskScheduler.execute();   // it will run mesh scheduler as well
  if (taskExecuted == false) {
    Serial.println("PROUT");
  }
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
