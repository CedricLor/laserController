#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPmDNS.h>            //lib to the network communication
#include <FS.h>
#include <AsyncTCP.h>
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
#include "../lib/LaserPin.h"
#include "../lib/LaserPin.cpp"
#include "../lib/myWebServerViews.h"
#include "../lib/myWebServerViews.cpp"
#include "../lib/MasterSlaveBox.h"
#include "../lib/MasterSlaveBox.cpp"
#include "../lib/myWebServerControler.h"
#include "../lib/myWebServerControler.cpp"
#include "../lib/myMesh.h"
#include "../lib/myMesh.cpp"
#include "../lib/laserSafetyLoop.h"
#include "../lib/laserSafetyLoop.cpp"

// v 3.0.8

 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void initPir();
void startAsyncServer();
void enableTasks();

void switchPirRelays(const bool state);
void broadcastPirStatus(const char* state);
void stopPirCycle();
void setPirValue();
void startOrRestartPirCycleIfPirValueIsHigh();

void onRequest(AsyncWebServerRequest *request);
void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NETWORK variables //////////////////////////////////////////////////////////////////////////////////////////////
//#define   STATION_SSID     "Livebox-CF01"                                                                                   // NETWORK BY NETWORK
//#define   STATION_PASSWORD "BencNiels!"                                                                                     // NETWORK BY NETWORK


//const int SECOND_BYTE_LOCAL_NETWORK = 1;                                                                                    // NETWORK BY NETWORK
//const IPAddress MY_STA_IP(192, 168, SECOND_BYTE_LOCAL_NETWORK, I_NODE_NAME); // the desired IP Address for the station      // NETWORK BY NETWORK

//const IPAddress MY_AP_IP(10, 0, 0, I_NODE_NAME);
//const IPAddress MY_GATEWAY_IP(10, 0, 0, I_NODE_NAME);
//const IPAddress MY_N_MASK(255, 0, 0, 0);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RELAYS variables /////////////////////////////

short LaserPin::pinParityWitness = 0;  // LaserPin::pinParityWitness is a variable that can be used when looping around the pins structs array.
                             // it avoids using the modulo.
                             // by switching it to 0 and 1 at each iteration of the loop
                             // in principle, the switch takes the following footprint: LaserPin::pinParityWitness = (LaserPin::pinParityWitness == 0) ? 1 : 0;
                             // this footprint shall be inserted as the last instruction within the loop (so that it is set to the correct state for the following iteration).
                             // once the loop is over, it should be reset to 0: LaserPin::pinParityWitness = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AutoSwitch variables /////////////////////////////
short siAutoSwitchInterval = 60;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PIR variables //////////////////////////////////////////////////////////////////////////////////////////////////
short const INPUT_PIN = 23;                  // choose the input pin (for PIR sensor)
                                      // we start assuming no motion detected
bool valPir = LOW;                    // variable for reading the pin status
const int I_PIR_INTERVAL = 1000;      // interval in the PIR cycle task (runs every second)
const short SI_PIR_ITERATIONS = 60;   // iteration of the PIR cycle

// after being started, the Pir values shall not be read for the next 60 seconds, as the PIR is likely to send equivoqual values
const short SI_PIR_START_UP_DELAY_ITERATIONS = 7;  // This const stores the number of times the tPirStartUpDelay Task shall repeat and inform the user that the total delay for the PIR to startup has not expired
const long L_PIR_START_UP_DELAY = 10000UL;         // This const stores the duration of the cycles (10 seconds) of the tPirStartUpDelay Task

short LaserPin::highPinsParityDuringStartup = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Web server variables ///////////////////////////////////////////////////////////////////////////////////////////
AsyncWebServer asyncServer(80);
char linebuf[80];
short charcount=0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scheduler variables ////////////////////////////////////////////////////////////////////////////////////////////

void sendMessage();
// Task taskSendMessage( &userScheduler, TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

/////////////////////////////////
// IR STARTUP
// Tasks related to the IR startup
// Declarations

void cbtPirStartUpDelayBlinkLaser();
bool onEnablePirStartUpDelayBlinkLaser();
void onDisablePirStartUpDelayBlinkLaser();
Task tPirStartUpDelayBlinkLaser( L_PIR_START_UP_DELAY, SI_PIR_START_UP_DELAY_ITERATIONS, &cbtPirStartUpDelayBlinkLaser, &userScheduler, false, &onEnablePirStartUpDelayBlinkLaser, &onDisablePirStartUpDelayBlinkLaser );

void cbtPirStartUpDelayPrintDash();
Task tPirStartUpDelayPrintDash( 1000UL, 9, &cbtPirStartUpDelayPrintDash, &userScheduler );

void cbtLaserOff();
Task tLaserOff( 0, 1, &cbtLaserOff, &userScheduler );

void cbtLaserOn();
Task tLaserOn( 0, 1, &cbtLaserOn, &userScheduler );


/////////////////////////////////
// PIR CONTROL
// Tasks related to the PIR controller (placing the lasers under control of the PIR and waiting for a motion to be detected to turn the lasers on/off)
// Declarations
void tcbPirCntrl();
// tcbPirCntrl() reads the status of the pin connected to the PIR controller; if HIGH, it enables tPirCycle.
/* Task tPirCntrl is here defined to run every 4 seconds (TASK_SECOND * 4), indefinitely (TASK_FOREVER),
 * to run its main callback tcbPirCntrl() each time and is added to the userScheduler.
 * It is created without being enabled (false) and has no onEnable and onDisable callbacks (NULL, NULL).
 */
Task tPirCntrl ( TASK_SECOND * 4, TASK_FOREVER, &tcbPirCntrl, &userScheduler, false, NULL, NULL);

/////////////////////////////////
// PIR CYCLE
// Tasks related to the PIR cycle (on/off of the lasers upon detecting a motion)
// Declarations
bool tcbOnEnablePirCycle();
void tcbOnDisablePirCycle();
Task tPirCycle ( I_PIR_INTERVAL, SI_PIR_ITERATIONS, NULL, &userScheduler, false, &tcbOnEnablePirCycle, &tcbOnDisablePirCycle);

/////////////////////////////////
// IR STARTUP
// Callback functions of the tasks relating to the start up delay of the IR sensor

void cbtPirStartUpDelayBlinkLaser() {
  Serial.print("+");

  if (!(tPirStartUpDelayBlinkLaser.isFirstIteration())) {
    LaserPin::directPinsSwitch(LaserPins, HIGH);
    LaserPin::highPinsParityDuringStartup = (LaserPin::highPinsParityDuringStartup == 0) ? 1 : 0;
  }
  LaserPin::directPinsSwitch(LaserPins, LOW);
  tPirStartUpDelayPrintDash.restartDelayed();
  if (!(tPirStartUpDelayBlinkLaser.isLastIteration())) {
    tLaserOff.restartDelayed(1000);
    tLaserOn.restartDelayed(2000);
  }
}

bool onEnablePirStartUpDelayBlinkLaser() {
  LaserPin::pairAllPins(LaserPins, false);
  return true;
}

void onDisablePirStartUpDelayBlinkLaser() {
  LaserPin::pairAllPins(LaserPins, true);
  LaserPin::directPinsSwitch(LaserPins, HIGH);
  LaserPin::inclExclAllRelaysInPir(LaserPins, HIGH);                // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: includes all the relays in PIR mode
  tPirCntrl.enable();
}

void cbtPirStartUpDelayPrintDash() {
  Serial.print("-");
}

void cbtLaserOff() {
  LaserPin::directPinsSwitch(LaserPins, HIGH);
}

void cbtLaserOn() {
  LaserPin::directPinsSwitch(LaserPins, LOW);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN SUBS - SETUP AND LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();
  mySavedPrefs::loadPreferences();
  LaserPin::initLaserPins(LaserPins);
  initPir();
  myMesh::meshSetup();
  startAsyncServer();
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subs LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PIR Controller
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read the state of the PIR and turn the relays on or off depending on the state of the PIR

//////////////////////
// CALLBACKS FOR TASK Task tPirCycle (the Task that controls the switching on and off of the laser when the PIR has detected some movement)
bool tcbOnEnablePirCycle() {
  Serial.print("PIR: tcbStartPirCycle(): Motion detected!!!\n");
  switchPirRelays(LOW);
  broadcastPirStatus("on");                                                                        // broadcast startup of a new pir Cycle
  Serial.print("PIR: tcbStartPirCycle(): broadcastPirStatus(\"on\")");
  return true;
}

void tcbOnDisablePirCycle() {
  Serial.print("PIR: tcbStopPirCycle(): PIR time is due. Ending PIR Cycle -------\n");
  stopPirCycle();
}

//////////////////////
// CALLBACKS FOR PIR CONTROL Task tPirCntrl (the Task that places the lasers under control of the PIR, looping and waiting for a movement to be detected)
void tcbPirCntrl() {
  setPirValue();
  startOrRestartPirCycleIfPirValueIsHigh();
}

void setPirValue() {
  valPir = digitalRead(INPUT_PIN); // read input value from the pin connected to the IR. If IR has detected motion, digitalRead(INPUT_PIN) will be HIGH.
  // Serial.printf(valPir\n);
}

void startOrRestartPirCycleIfPirValueIsHigh() {
  if (valPir == HIGH) {                                                                              // if the PIR sensor has sensed a motion,
    if (!(tPirCycle.isEnabled())) {
      tPirCycle.setIterations(SI_PIR_ITERATIONS);
      tPirCycle.restart();
    } else {
      tPirCycle.setIterations(SI_PIR_ITERATIONS);
    }
  }
  valPir = LOW;                                                                                      // Reset the ValPir to low (no motion detected)
}

void stopPirCycle() {
  Serial.print("PIR: stopPirCycle(): stopping PIR cycle.\n");
  switchPirRelays(HIGH);                                  // turn all the PIR controlled relays off
  broadcastPirStatus("off");                              // broadcast current pir status
}


// loop over each of the structs representing pins to turn them on or off (if they are controlled by the PIR)
void switchPirRelays(const bool state) {
  Serial.print("PIR: switchPirRelays(const bool state): starting -------\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    if (LaserPins[thisPin].pir_state == HIGH) {
      LaserPins[thisPin].switchOnOffVariables(state);
    }
  }
  Serial.print("PIR: switchPirRelays(const bool state): leaving -------\n");
}


// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void tcbOdAutoSwitchAllRelays();
Task tAutoSwitchAllRelays( 1000, siAutoSwitchInterval, NULL, &userScheduler, false, &LaserPin::tcbOaAutoSwitchAllRelays, &tcbOdAutoSwitchAllRelays );

void tcbOdAutoSwitchAllRelays() {
  LaserPin::switchAllRelays(LaserPins, HIGH);
  LaserPin::inclExclAllRelaysInPir(LaserPins, HIGH);     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
}

void autoSwitchAllRelays(const bool targetState) {
  if (targetState == LOW) {
    tAutoSwitchAllRelays.enable();
    return;
  }
  tAutoSwitchAllRelays.disable();
}

void autoSwitchOneRelay(const short thisPin, const bool targetState) {
  /*  TO DRAFT PROPERLY WHEN NECESSARY
      LaserPins[thisPin].switchOnOffVariables(targetState);
      LaserPins[thisPin].switchPointerBlinkCycleState(targetState);
      isPirCycleOn = false;
      Serial.println("auto Switch request executed");
   */
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SENDING REQUEST TO OTHER CONTROLLER BOXES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Called by the Pir block
// Broadcasts over mesh
void broadcastPirStatus(const char* state) {     // state is "on" or "off". When valPir is HIGH (the IR has detected a move),
                                                 // the Pir block calls this function with the "on" parameter. Alternatively,
                                                 //  when the the pir cycle stops, it calls this function with the "off" parameter.
  Serial.printf("PIR - broadcastPirStatus(): broadcasting status over Mesh via call to broadcastStatusOverMesh(state) with state = %s\n", state);
  myMesh::broadcastStatusOverMesh(state);

  Serial.print("PIR - broadcastPirStatus(): ending.\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP sub functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP: STRUCTS, SERIAL, NETWORK
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void serialInit() {
  Serial.begin(115200);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("\nSETUP: serialInit(): done\n");
}

void initPir() {
  Serial.print("SETUP: initPir(): starting\n");
  pinMode(INPUT_PIN, INPUT);                  // declare sensor as input
  Serial.print("SETUP: initPir(): done\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP: AsyncWebServer
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void listAllCollectedHeaders(AsyncWebServerRequest *request) {
  int headers = request->headers();
  for(int i=0;i<headers;i++){
    AsyncWebHeader* h = request->getHeader(i);
    Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  }
}

void listAllCollectedParams(AsyncWebServerRequest *request) {
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* param = request->getParam(i);
    if(param->isFile()){ //p->isPost() is also true
      Serial.printf("FILE[%s]: %s, size: %u\n", param->name().c_str(), param->value().c_str(), param->size());
    } else if(param->isPost()){
      Serial.printf("POST[%s]: %s\n", param->name().c_str(), param->value().c_str());
    } else {
      Serial.printf("GET[%s]: %s\n", param->name().c_str(), param->value().c_str());
      // storeGetParamsInLineBuf(params, i, param);
    }
  }
}

void startAsyncServer() {
  asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.print("In handler of \"/\" request -------\n");

    // List all collected headers
    listAllCollectedHeaders(request);

    // List all parameters
    listAllCollectedParams(request);

    // Decode request
    myWebServerControler::decodeRequest(LaserPins, request);

    //Send a response
    myWebServerViews myWebServerView(LaserPins, pinBlinkingInterval, PIN_COUNT, iSlaveOnOffReaction, iMasterNodeName, I_MASTER_NODE_PREFIX, I_NODE_NAME, ControlerBoxes, myMesh::BOXES_I_PREFIX, slaveReactionHtml);
    AsyncResponseStream *response = request->beginResponseStream("text/html");  // define a response stream
    response->addHeader("Server","ESP Async Web Server");                       // append stuff to header
    response->printf(myWebServerView.returnTheResponse().c_str());              // converts the arduino String in C string (array of chars)
    request->send(response);                                                    // send the response
  });

  // asyncServer.onNotFound(&onRequest);  // error: no matching function for call to 'AsyncWebServer::onNotFound(void (&)())'
  // asyncServer.onRequestBody(&onBody);  // error: no matching function for call to 'AsyncWebServer::onRequestBody(void (&)())'

  asyncServer.begin();
}

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  //Handle body
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP: enableTasks
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void enableTasks() {
  tPirStartUpDelayBlinkLaser.enable();
  // tPirCntrl.waitFor(&srPirStartUpComplete, TASK_IMMEDIATE, TASK_FOREVER);
}
