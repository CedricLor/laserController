#include <ESPmDNS.h>            //lib to the network communication
#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)

#include "../lib/global.h"
#include "../lib/global.cpp"
#include "../lib/Myota.h"
#include "../lib/Myota.cpp"
#include "../lib/ControlerBox.h"
#include "../lib/ControlerBox.cpp"
#include "../lib/mySavedPrefs.h"
#include "../lib/mySavedPrefs.cpp"

#include "../lib/MasterSlaveBox.h"
#include "../lib/MasterSlaveBox.cpp"

#include "../lib/LaserPin.h"
#include "../lib/LaserPin.cpp"
#include "../lib/LaserPinsArray.h"
#include "../lib/LaserPinsArray.cpp"

#include "../lib/myMesh.h"
#include "../lib/myMesh.cpp"

#include "../lib/pirStartupController.h"
#include "../lib/pirStartupController.cpp"
#include "../lib/pirController.h"
#include "../lib/pirController.cpp"

#include "../lib/myWebServerBase.h"
#include "../lib/myWebServerBase.cpp"

#include "../lib/laserSafetyLoop.h"
#include "../lib/laserSafetyLoop.cpp"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void enableTasks();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NETWORK variables //////////////////////////////////////////////////////////////////////////////////////////////
//#define   STATION_SSID     "Livebox-CF01"                                                                                   // NETWORK BY NETWORK
//#define   STATION_PASSWORD "BencNiels!"                                                                                     // NETWORK BY NETWORK


//const int SECOND_BYTE_LOCAL_NETWORK = 1;                                                                                    // NETWORK BY NETWORK
//const IPAddress MY_STA_IP(192, 168, SECOND_BYTE_LOCAL_NETWORK, I_NODE_NAME); // the desired IP Address for the station      // NETWORK BY NETWORK

//const IPAddress MY_AP_IP(10, 0, 0, I_NODE_NAME);
//const IPAddress MY_GATEWAY_IP(10, 0, 0, I_NODE_NAME);
//const IPAddress MY_N_MASK(255, 0, 0, 0);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();
  mySavedPrefs::loadPreferences();
  LaserPinsArray::initLaserPins(LaserPins);
  pirController::initPir();
  myMesh::meshSetup();
  myWebServerBase::startAsyncServer();
  Myota::OTAConfig();
  enableTasks();
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
}

void loop() {
  ArduinoOTA.handle();
  userScheduler.execute();   // it will run mesh scheduler as well
  laserControllerMesh.update();
  laserSafetyLoop::loop(LaserPins);
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
  pirStartupController::tPirStartUpDelayBlinkLaser.enable();
}
