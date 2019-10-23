#ifndef UNIT_TEST

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
#include <signal.h>
#include <myMesh.h>
#include <myMeshStarter.h>
#include <myWebServerBase.h>
// #include <test.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

controllerBoxesCollection     cntrllerBoxesCollection;
ControlerBox &thisBox =       cntrllerBoxesCollection.controllerBoxesArray.at(0);
signal                        _signal;
boxStateCollection            bxStateColl;
controllerBoxThis             thisControllerBox;

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
  // test _test;

  globalBaseVariables.scheduler.init();

  // TRUE IS READONLY and FALSE IS RW!!!
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.loadPrefsWrapper();

  // The ESP was restarted with an OTA request saved in mySavedPrefs
  if (globalBaseVariables.gi8OTAReboot) {
    myOta::OTAConfig();
    return;
  }

  globalBaseVariables.scheduler.addTask(thisControllerBox.tReboot);
  globalBaseVariables.scheduler.addTask(ControlerBox::tReboot);
  globalBaseVariables.scheduler.addTask(myMeshStarter::tRestart);

  globalBaseVariables.scheduler.addTask(myMesh::tChangedConnection);
  globalBaseVariables.scheduler.addTask(myMesh::tIamAloneTimeOut);
  globalBaseVariables.scheduler.addTask(myMesh::tPrintMeshTopo);
  globalBaseVariables.scheduler.addTask(myMesh::tUpdateCBArrayOnChangedConnections);
  globalBaseVariables.scheduler.addTask(myMesh::tSaveNodeMap);

  if (globalBaseVariables.isInterface) {
    globalBaseVariables.scheduler.addTask(myWSSender::tSendWSDataIfChangeStationIp);
    globalBaseVariables.scheduler.addTask(myWSSender::tSendWSDataIfChangeBoxState);
  }

  thisBox.ui16MasterBoxName = globalBaseVariables.gui8DefaultMasterNodeName;
  thisBox.sBoxDefaultState = globalBaseVariables.gi16BoxDefaultState;

  myMesh::start();

  Serial.println("\nStarting SPIFFS");
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  mySpiffs __mySpiffs;
  __mySpiffs.listDir("/", 0);
  __mySpiffs.convertJsonFilePrettyToUgly("/pretty-sessions.json", thisBox.ui16NodeName);
  Serial.println("\n");

  if (globalBaseVariables.isInterface) {
    myWebServerBase _myWebServer;
  }

  if ((globalBaseVariables.isInterface == false) || (globalBaseVariables.isRoot == false)) {
    globalBaseVariables.scheduler.addTask(ControlerBox::tSetBoxState);
    globalBaseVariables.scheduler.addTask(pirController::tSetPirTimeStampAndBrdcstMsg);
    globalBaseVariables.scheduler.addTask(pirController::tSpeedBumper);
    globalBaseVariables.scheduler.addTask(stepColl.tPreloadNextStep);
    globalBaseVariables.scheduler.addTask(bxStateColl.tPlayBoxState);
    // _test.beforeSequenceStacks();
    laserInterface::init();
    // _test.sequenceStack();
  }

  enableTasks();

  // Serial.printf("setup. globalBaseVariables.laserControllerMesh.subConnectionJson() = %s\n",globalBaseVariables.laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("Box number: %i\n", globalBaseVariables.gui16NodeName);
  Serial.printf("Version: %i\n", globalBaseVariables.VERSION);
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
  __mySpiffs.listDir("/", 0);
}







void loop() {
  if (globalBaseVariables.gi8OTAReboot) {
    ArduinoOTA.handle();
    return;
  }

  globalBaseVariables.scheduler.execute();
  globalBaseVariables.laserControllerMesh.update();
  if ((globalBaseVariables.isInterface == false) || (globalBaseVariables.isRoot == false)) {
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
  if ( (globalBaseVariables.isInterface == false) || (globalBaseVariables.isRoot == false) ) {
    _signal.startup();
    // cntrllerBoxesCollection.signalHandlers.startup();
  } else {
    myWSSender::tSendWSDataIfChangeBoxState.enable();
  }
}

#endif