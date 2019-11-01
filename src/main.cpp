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
controllerBoxThis                                       thisControllerBox;

myWebServerBase                                         _myWebServer;


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

  thisControllerBox.globBaseVars.scheduler.init();

  mySavedPrefs _myPrefsRef;
  _myPrefsRef.loadPrefsWrapper();

  // If the ESP was restarted with an OTA request saved in mySavedPrefs
  if (globalBaseVariables.gi8OTAReboot) {
    myOta::OTAConfig();
    return;
  }

  thisControllerBox.globBaseVars.scheduler.addTask(thisControllerBox.tReboot);
  thisControllerBox.globBaseVars.scheduler.addTask(myMeshStarter::tRestart);

  thisControllerBox.globBaseVars.scheduler.addTask(myMesh::tChangedConnection);
  thisControllerBox.globBaseVars.scheduler.addTask(myMesh::tIamAloneTimeOut);
  thisControllerBox.globBaseVars.scheduler.addTask(myMesh::tPrintMeshTopo);
  thisControllerBox.globBaseVars.scheduler.addTask(myMesh::tUpdateCBArrayOnChangedConnections);
  thisControllerBox.globBaseVars.scheduler.addTask(myMesh::tSaveNodeMap);

  if (thisControllerBox.globBaseVars.hasInterface) {
    thisControllerBox.globBaseVars.scheduler.addTask(_myWebServer.getTSendWSDataIfChangeStationIp());
    thisControllerBox.globBaseVars.scheduler.addTask(_myWebServer.getTSendWSDataIfChangeBoxState());
  }

  thisControllerBox.thisCtrlerBox.sBoxDefaultState = thisControllerBox.globBaseVars.gi16BoxDefaultState;

  myMesh::start();

  Serial.println("\nStarting SPIFFS");
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  mySpiffs __mySpiffs;
  // IMPORTANT: Uncomment the following lines if a new uploadfs was made on the device
  // __mySpiffs.listDir("/", 0);
  // __mySpiffs.convertJsonFilePrettyToUgly("/pretty-sessions.json", thisControllerBox.thisCtrlerBox.ui16NodeName);
  // Serial.println("\n");

  if (thisControllerBox.globBaseVars.hasInterface) {
    _myWebServer.begin();
  }

  thisControllerBox.addLaserTasks();

  enableTasks();

  // Serial.printf("setup. thisControllerBox.globBaseVars.laserControllerMesh.subConnectionJson() = %s\n",thisControllerBox.globBaseVars.laserControllerMesh.subConnectionJson().c_str());
  Serial.printf("Box number: %i\n", thisControllerBox.globBaseVars.gui16NodeName);
  Serial.printf("Version: %i\n", thisControllerBox.globBaseVars.VERSION);
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
  __mySpiffs.listDir("/", 0);
}







void loop() {
  if (thisControllerBox.globBaseVars.gi8OTAReboot) {
    ArduinoOTA.handle();
    return;
  }

  thisControllerBox.globBaseVars.scheduler.execute();
  thisControllerBox.globBaseVars.laserControllerMesh.update();
  if (thisControllerBox.globBaseVars.hasIRSensor) {
    thisControllerBox.thisPirController.check();
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
  if (thisControllerBox.globBaseVars.hasLasers == true) {
    thisControllerBox.thisSignalHandler.startup();
  } else {
    _myWebServer.getTSendWSDataIfChangeBoxState().enable();
  }
}

#endif