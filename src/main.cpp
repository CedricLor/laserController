#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPAsyncWebServer.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

extern constexpr short    UI8_BOXES_COUNT                     = 10;

#include "mns.h"
#include <mySavedPrefs.h>
#include <myOta.h>
#include <ControlerBox.h>
#include <myMesh.h>
#include <myMeshStarter.h>
#include <boxState.h>
#include <signal.h>
#include <laserInterface.h>
#include <pirController.h>
#include <myWebServerBase.h>
#include <test.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

painlessMesh            laserControllerMesh;
ControlerBox            ControlerBoxes[UI8_BOXES_COUNT];
ControlerBox &thisBox = ControlerBoxes[0];
signal                  _signal;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void enableTasks();


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup //////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();
  test _test;

  mns::myScheduler.init();
  
  // TRUE IS READONLY and FALSE IS RW!!!
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.loadPrefsWrapper();

  // The ESP was restarted with an OTA request saved in mySavedPrefs
  if (gi8OTAReboot) {
    myOta::OTAConfig();
    return;
  }

  mns::myScheduler.addTask(ControlerBox::tReboot);
  mns::myScheduler.addTask(myMeshStarter::tRestart);
  
  mns::myScheduler.addTask(myMesh::tChangedConnection);
  mns::myScheduler.addTask(myMesh::tIamAloneTimeOut);
  mns::myScheduler.addTask(myMesh::tPrintMeshTopo);
  mns::myScheduler.addTask(myMesh::tUpdateCBArrayOnChangedConnections);
  mns::myScheduler.addTask(myMesh::tSaveNodeMap);

  if (isInterface) {
    mns::myScheduler.addTask(myWSSender::tSendWSDataIfChangeStationIp);
    mns::myScheduler.addTask(myWSSender::tSendWSDataIfChangeBoxState);
  }
  if ((isInterface == false) || (isRoot == false)) {
    mns::myScheduler.addTask(ControlerBox::tSetBoxState);
    mns::myScheduler.addTask(pirController::tSetPirTimeStampAndBrdcstMsg);
    mns::myScheduler.addTask(pirController::tSpeedBumper);
    mns::myScheduler.addTask(step::tPreloadNextStep);
    mns::myScheduler.addTask(boxState::tPlayBoxState);
    mns::myScheduler.addTask(sequence::tPlaySequenceInLoop);
    mns::myScheduler.addTask(sequence::tPlaySequence);
    mns::myScheduler.addTask(bar::tPlayBar);
    mns::myScheduler.addTask(note::tPlayNote);
  }


  thisBox.ui16MasterBoxName = gui8DefaultMasterNodeName;
  thisBox.sBoxDefaultState = gi16BoxDefaultState;
  
  myMesh::start();

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  mySpiffs _mySpiffs;
  _mySpiffs.convertJsonFilePrettyToUgly("/pretty-sessions.json", thisBox.ui16NodeName);

  if (isInterface) {
    myWebServerBase _myWebServer;
  }

  if ((isInterface == false) || (isRoot == false)) {
    _test.laserPinStack();
    _test.rawLaserPinsStack();
    _test.toneStack();
    _test.rawTonesStack();
    _test.implementedTonesStack();
    _test.noteStack();
    _test.rawNotesStack();
    bar::initBars();
    _test.barStack();
    sequence::initSequences();
    _test.sequenceStack();
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
  
  mns::myScheduler.execute();
  laserControllerMesh.update();
  if ((isInterface == false) || (isRoot == false)) {
    myPirController.check();
  }
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
  if ( (isInterface == false) || (isRoot == false) ) {
    signal::startup();
  } else {
    myWSSender::tSendWSDataIfChangeBoxState.enable();
  }
}
