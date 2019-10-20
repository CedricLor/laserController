#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPAsyncWebServer.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)
#include <SPIFFS.h>
#define FORMAT_SPIFFS_IF_FAILED true

extern constexpr short    UI8_BOXES_COUNT                     = 10;

#include <globalVars.h>
#include <mySavedPrefs.h>
#include <myOta.h>
#include <controllerBoxesCollection.h>
#include <myMesh.h>
#include <myMeshStarter.h>
#include <pirController.h>
#include <myWebServerBase.h>
// #include <test.h>



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

painlessMesh              laserControllerMesh;
ControlerBox              ControlerBoxes[UI8_BOXES_COUNT];
ControlerBox &thisBox =   ControlerBoxes[0];
signal                    _signal;
boxStateCollection        bxStateColl;
// controllerBoxesCollection cntrllerBoxesCollection;

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

  globalVariables.scheduler.init();

  // TRUE IS READONLY and FALSE IS RW!!!
  mySavedPrefs _myPrefsRef;
  _myPrefsRef.loadPrefsWrapper();

  // The ESP was restarted with an OTA request saved in mySavedPrefs
  if (gi8OTAReboot) {
    myOta::OTAConfig();
    return;
  }

  globalVariables.scheduler.addTask(ControlerBox::tReboot);
  globalVariables.scheduler.addTask(myMeshStarter::tRestart);

  globalVariables.scheduler.addTask(myMesh::tChangedConnection);
  globalVariables.scheduler.addTask(myMesh::tIamAloneTimeOut);
  globalVariables.scheduler.addTask(myMesh::tPrintMeshTopo);
  globalVariables.scheduler.addTask(myMesh::tUpdateCBArrayOnChangedConnections);
  globalVariables.scheduler.addTask(myMesh::tSaveNodeMap);

  if (isInterface) {
    globalVariables.scheduler.addTask(myWSSender::tSendWSDataIfChangeStationIp);
    globalVariables.scheduler.addTask(myWSSender::tSendWSDataIfChangeBoxState);
  }

  thisBox.ui16MasterBoxName = gui8DefaultMasterNodeName;
  thisBox.sBoxDefaultState = gi16BoxDefaultState;

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

  if (isInterface) {
    myWebServerBase _myWebServer;
  }

  if ((isInterface == false) || (isRoot == false)) {
    globalVariables.scheduler.addTask(ControlerBox::tSetBoxState);
    globalVariables.scheduler.addTask(pirController::tSetPirTimeStampAndBrdcstMsg);
    globalVariables.scheduler.addTask(pirController::tSpeedBumper);
    globalVariables.scheduler.addTask(stepColl.tPreloadNextStep);
    globalVariables.scheduler.addTask(bxStateColl.tPlayBoxState);
    // _test.beforeSequenceStacks();
    laserInterface::init();
    // _test.sequenceStack();
  }

  enableTasks();

  // Serial.printf("setup. laserControllerMesh.subConnectionJson() = %s\n",laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("Box number: %i\n", gui16NodeName);
  Serial.printf("Version: %i\n", VERSION);
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
  __mySpiffs.listDir("/", 0);
}







void loop() {
  if (gi8OTAReboot) {
    ArduinoOTA.handle();
    return;
  }

  globalVariables.scheduler.execute();
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
    _signal.startup();
    // cntrllerBoxesCollection.signalHandlers.startup();
  } else {
    myWSSender::tSendWSDataIfChangeBoxState.enable();
  }
}
