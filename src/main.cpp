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

// v 3.0.8

/*
 * 2752557361, 10.107.105.1 = box 201, master 201, StationIP = 10.177.49.2
 * 2752932713, 10.177.49.1 = box 202, master 201
 * 2752577349, 10.255.69.1 = box 204, master 201
 */
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void serialInit();
void initPir();
void meshSetup();
void startAsyncServer();
void enableTasks();

void laserSafetyLoop();

void switchPirRelays(const bool state);
void broadcastPirStatus(const char* state);
void stopPirCycle();
void setPirValue();
void startOrRestartPirCycleIfPirValueIsHigh();

void blinkLaserIfBlinking(const short thisPin);
void ifMasterPairedThenUpdateOnOffOfSlave(const short thisPin);
void executeUpdates(const short thisPin);
void blinkLaserIfTimeIsDue(const short thisPin);
void evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(const short thisPin);
void updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget);

void broadcastStatusOverMesh(const char* state);

void onRequest();
void onBody();

void meshController(uint32_t senderNodeId, String &msg);
void autoSwitchAllRelaysMeshWrapper(const char* senderStatus, const short iSenderNodeName);
String createMeshMessage(const char* myStatus);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH variables /////////////////////////////////////////////////////////////////////////////////////////////////
#define   MESH_PREFIX     "laser_boxes"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// painlessMesh myMesh;

char nodeNameBuf[4];
char* nodeNameBuilder(const short _I_NODE_NAME, char _nodeNameBuf[4]) {
  String _sNodeName = String(_I_NODE_NAME);
  _sNodeName.toCharArray(_nodeNameBuf, 4);
  return _nodeNameBuf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NETWORK variables //////////////////////////////////////////////////////////////////////////////////////////////
//#define   STATION_SSID     "Livebox-CF01"                                                                                   // NETWORK BY NETWORK
//#define   STATION_PASSWORD "BencNiels!"                                                                                     // NETWORK BY NETWORK

const char PREFIX_AP_SSID[5] = "box_";
char myApSsidBuf[8];
char* apSsidBuilder(const short _I_NODE_NAME, char _apSsidBuf[8]) {
  strcat(_apSsidBuf, PREFIX_AP_SSID);
  char _nodeName[4];
  itoa(I_NODE_NAME, _nodeName, 10);
  strcat(_apSsidBuf, _nodeName);
  return _apSsidBuf;
}

//const int SECOND_BYTE_LOCAL_NETWORK = 1;                                                                                    // NETWORK BY NETWORK
//const IPAddress MY_STA_IP(192, 168, SECOND_BYTE_LOCAL_NETWORK, I_NODE_NAME); // the desired IP Address for the station      // NETWORK BY NETWORK

//const IPAddress MY_AP_IP(10, 0, 0, I_NODE_NAME);
//const IPAddress MY_GATEWAY_IP(10, 0, 0, I_NODE_NAME);
//const IPAddress MY_N_MASK(255, 0, 0, 0);

ControlerBox ControlerBoxes[BOXES_COUNT];

const short BOXES_I_PREFIX = 201; // this is the iNodeName of the node in the mesh, that has the lowest iNodeName of the network // NETWORK BY NETWORK

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES FOR REACTION TO NETWORK REQUESTS
///////////////////////////////
// The following variable, at start up, corresponds to the default reaction of this box, when it receives the status of its parent box.
// It is a number from 0 to 3. This number corresponds to the index in the following array (const bool B_SLAVE_ON_OFF_REACTIONS[4][2])
// short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;       // saves the index in the B_SLAVE_ON_OFF_REACTIONS bool array of the choosen reaction to the master states
const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
// HIGH, LOW = reaction if master is on = HIGH; reaction if master is off = LOW;  // Synchronous (index 0): When the master box is on, turn me on AND when the master box is off, turn me off
// LOW, HIGH = reaction if master is on = LOW; reaction if master is off = HIGH;  // Opposed  (index 1): When the master box is on, turn me off AND when the master box is off, turn me on
// HIGH, HIGH = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always off  (index 2): When the master box is on, turn me off AND when the master box is off, turn me off
// LOW, LOW = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always on  (index 3): When the master box is on, turn me off AND when the master box is off, turn me off
// !!!!!! IMPORTANT: If changing the structure of B_SLAVE_ON_OFF_REACTIONS above, update slaveReaction
// in class myWebServerViews and slaveReactionHtml in global.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RELAYS variables /////////////////////////////

// The following variable have been moved to BOX KEY VARIABLES
// short relayPins[] = {
//   22, 21, 19, 18, 5, 17, 16, 4
// };                                    // an array of pin numbers to which relays are attached   // See BOX KEY VARIABLES     // BOX BY BOX
//
// short const PIN_COUNT = 8;               // the number of pins (i.e. the length of the array)      // See BOX KEY VARIABLES     // BOX BY BOX

// declare and size an array to contain the LaserPins class instances as a global variable
LaserPin LaserPins[PIN_COUNT];
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
Scheduler     userScheduler;             // to control your personal task

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
  meshSetup();
  startAsyncServer();
  Myota::OTAConfig();
  enableTasks();
  Serial.print("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
}

void loop() {
  ArduinoOTA.handle();
  userScheduler.execute();   // it will run mesh scheduler as well
  myMesh.update();
  laserSafetyLoop();
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LASER SAFETY TIMER -- EXECUTED AT THE END OF EACH LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void laserSafetyLoop() {
  // Loop around each struct representing a pin connected to a laser before restarting a global loop and
  // make any update that may be required. For instance:
  // - safety time elapsed of lasers in blinking cycle (blinking every 10 to 30 s., to avoid burning the lasers);
  // - update the paired laser or its pair if the lasers are paired;
  // and then, execute the updates.
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    blinkLaserIfBlinking(thisPin);                          // check if laser is in blinking cycle and check whether the blinking interval has elapsed
    ifMasterPairedThenUpdateOnOffOfSlave(thisPin);          // update the on/off status of slave
    executeUpdates(thisPin);                                // transform the update to the struct to analogical updates in the status of the related pin
    LaserPin::pinParityWitness = (LaserPin::pinParityWitness == 0) ? 1 : 0;
  }
  LaserPin::pinParityWitness = 0;
}

void blinkLaserIfBlinking(const short thisPin) {
  /*
     Called for each pin by laserSafetyLoop()
     If a laser is in blinking mode and is either (i) non-paired or (ii) master in a pair,
     if so, switch its on/off state
  */
  if (LaserPins[thisPin].blinking == true && (LaserPins[thisPin].paired == 8 || LaserPin::pinParityWitness == 0)) {
    blinkLaserIfTimeIsDue(thisPin);
  }
}

void blinkLaserIfTimeIsDue(const short thisPin) {
  /*
     Called when a laser is in blinking mode and is either (i) non-paired or (ii) master in a pair
     Checks if the blinking interval of this laser has elapsed
     If so, switches the pin on/off target variable to the contrary of the current pin on/off
     TO ANALYSE: this may overwrite other changes that have been requested at other stages
  */
  const unsigned long currentTime = millis();
  if (currentTime - LaserPins[thisPin].previous_time > LaserPins[thisPin].blinking_interval) {
      LaserPins[thisPin].on_off_target = !LaserPins[thisPin].on_off;
  }
}

void ifMasterPairedThenUpdateOnOffOfSlave(const short thisPin) {
  /*
      Called from within the laser safety loop for each pin
      Test if the laser is paired and if it is a master in a pair
      If so, calls evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave
  */
  if (!(LaserPins[thisPin].paired == 8) && (LaserPin::pinParityWitness == 0)) {
    evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(thisPin);
  }
}

void evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(const short thisPin) {
  /*
      Called by ifMasterPairedThenUpdateOnOffOfSlave() for pins which are master in a pair.
      A. If the pin is NOT in blinking mode AND its on_off_target state is to turn off,
      it means that the master pin has been turned off (off and off blinking mode) => The slave pin should then be turned off.
      Comment: raisonnement bancal:
        Le fait que le on_off_target_state soit sur off quand un laser est "not in blinking mode" ne veut pas nécessairement dire qu'il vient d'être turned off.
        En tout état de cause, ceci permet bien sur de tourner l'interrupteur du slave sur off (mais ceci le fait même si l'interrupteur était déjà éteint).
      B. Else:
        either the master pin is in blinking mode and its on_off_target is to turn off (A = false and B = true),
                                                      or on_off_target is to turn on (A = false and B = false),
                  which means that the master pin, in blinking mode, has received a status change.
        or the master pin is not in blinking mode and its on_off_target is to turn on (A = true and B = false).
                  which means that the master pin has received the instruction to start a blinking cycle.
        In each case, the slave will receive the instruction to put its on_off_target to the opposite of that of its master.
  */
  if ((LaserPins[thisPin].blinking == false) && (LaserPins[thisPin].on_off_target == HIGH)) {
    updatePairedSlave(thisPin, HIGH);
    return;
  }
  updatePairedSlave(thisPin, !LaserPins[thisPin].on_off_target);
}

void updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget) {
  LaserPins[thisPin + 1].on_off_target = nextPinOnOffTarget;                          // update the on_off target of the paired slave laser depending on the received instruction
  LaserPins[thisPin + 1].blinking = LaserPins[thisPin].blinking;                            // align the blinking state of the paired slave laser
  LaserPins[thisPin + 1].pir_state = LaserPins[thisPin].pir_state;                          // align the IR state of the paired slave laser
  // LaserPins[thisPin + 1].paired = thisPin;                                            // align the paired pin of the slave to this pin
}

void executeUpdates(const short thisPin) {
  /*
      Called from within the laser safety loop for each pin
      Checks whether the current on_off_target state is different than the current on_off state
      If so:
      1. turn on or off the laser as requested in the on_off_target_state
      2. align the on_off state with the on_off_target state
      3. reset the safety blinking timer of this pin
      // TO ANALYSE: I have the feeling that the condition to be tested shall be different
      // in the case a) a laser is in a blinking cycle and in the case b) a laser is not in
      // a blinking cycle and cooling off
  */
  if (LaserPins[thisPin].on_off != LaserPins[thisPin].on_off_target) {
    digitalWrite(LaserPins[thisPin].number, LaserPins[thisPin].on_off_target);
    LaserPins[thisPin].on_off = LaserPins[thisPin].on_off_target;
    LaserPins[thisPin].previous_time = millis();
  }
}

// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool tcbOaAutoSwitchAllRelays();
void tcbOdAutoSwitchAllRelays();
// Task tAutoSwitchAllRelays( &userScheduler, 1000, siAutoSwitchInterval, NULL, false, &tcbOaAutoSwitchAllRelays, &tcbOdAutoSwitchAllRelays );
Task tAutoSwitchAllRelays( 1000, siAutoSwitchInterval, NULL, &userScheduler, false, &tcbOaAutoSwitchAllRelays, &tcbOdAutoSwitchAllRelays );

bool tcbOaAutoSwitchAllRelays() {
  LaserPin::switchAllRelays(LaserPins, LOW);
  Serial.print("-------- Auto Switch cycle started............ --------\n");
  return true;
}

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
  broadcastStatusOverMesh(state);

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
    myWebServerViews myWebServerView(LaserPins, pinBlinkingInterval, PIN_COUNT, iSlaveOnOffReaction, iMasterNodeName, I_MASTER_NODE_PREFIX, I_NODE_NAME, ControlerBoxes, BOXES_I_PREFIX, slaveReactionHtml);
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
// SETUP: Mesh Network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendMessage() {
  // broadcastStatusOverMesh("on");
  /*
  JsonObject& msg = createMeshJsonMessage("on");
  String str = createMeshStrMessage(msg);
  if (logServerId == 0) // If we don't know the logServer yet
    mesh.sendBroadcast(str);
  else
    mesh.sendSingle(logServerId, str);

  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast(msg);
  Serial.printf("Sending message: %s\n", msg.c_str());
  mesh.sendSingle(destServerId, msg);
  */
  // taskSendMessage.setInterval(TASK_SECOND * 1);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("MESH CALLBACK: receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  meshController(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);
  broadcastStatusOverMesh("na");
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n",myMesh.subConnectionJson().c_str());
  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);
  broadcastStatusOverMesh("na");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", myMesh.getNodeTime(),offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

void meshSetup() {
  myMesh.setDebugMsgTypes( ERROR | STARTUP |/*MESH_STATUS |*/ CONNECTION |/* SYNC |*/ COMMUNICATION /* | GENERAL | MSG_TYPES | REMOTE */);

  myMesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );

  //myMesh.stationManual(STATION_SSID, STATION_PASSWORD, STATION_PORT, station_ip);
  myMesh.setHostname(apSsidBuilder(I_NODE_NAME, myApSsidBuf));
  if (MESH_ROOT == true) {
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    myMesh.setRoot(true);
    // This and all other mesh should ideally now the mesh contains a root
    myMesh.setContainsRoot(true);
  }

  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);

  myMesh.onReceive(&receivedCallback);
  myMesh.onNewConnection(&newConnectionCallback);
  myMesh.onChangedConnections(&changedConnectionCallback);
  myMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  myMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
}

short jsonToInt(JsonObject& root, String rootKey) {
  short iValue;
  const char* sValue = root[rootKey];
  iValue = atoi(sValue);
  return iValue;
}

void meshController(uint32_t senderNodeId, String &msg) {
  // React depending on wether the remote is myMaster and if so, on its on or off status

  Serial.printf("MESH CONTROLLER: meshController(uint32_t senderNodeId, String &msg) starting with senderNodeId == %u and &msg == %s \n", senderNodeId, msg.c_str());
  StaticJsonBuffer<250> jsonBuffer;
  Serial.print("MESH CONTROLLER: meshController(...): jsonBuffer created\n");
  JsonObject& root = jsonBuffer.parseObject(msg.c_str());
  Serial.print("MESH CONTROLLER: meshController(...): jsonBuffer parsed into JsonObject& root\n");

  const short iSenderNodeName = jsonToInt(root, "senderNodeName");
  Serial.printf("MESH CONTROLLER: meshController(...) %u alloted from root[\"senderNodeName\"] to iSenderNodeName \n", iSenderNodeName);

  const char* cSenderStatus = root["senderStatus"];
  Serial.printf("MESH CONTROLLER: meshController(...) %s alloted from root[\"senderStatus\"] to senderStatus \n", cSenderStatus);

  // Is the message addressed to me?
  if (!(iSenderNodeName == iMasterNodeName)) {                 // do not react to broadcast message if message not sent by relevant sender
    return;
  }

  // If so, act depending on the sender status
  autoSwitchAllRelaysMeshWrapper(cSenderStatus, iSenderNodeName);
}

void autoSwitchAllRelaysMeshWrapper(const char* senderStatus, const short iSenderNodeName) {
  /*
      Explanation of index numbers in the array of boolean arrays B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0 or 1]:
      const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
      - First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
        It is set via the iSlaveOnOffReaction variable. It is itself set either:
        - at startup and equal to the global constant I_DEFAULT_SLAVE_ON_OFF_REACTION (in the global variables definition);
        - via the changeSlaveReaction sub (in case the user has decided to change it via a web control).
      - Second index number is the reaction to the on state of the master box if 0, to its off state if 1.
  */
  Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(senderStatus, iSenderNodeName) starting.\niSenderNodeName = %u\n. senderStatus = %s.\n", iSenderNodeName, senderStatus);
  const char* myFutureState = B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0] == LOW ? "on" : "off";
  if (strstr(senderStatus, "on")  > 0) {                              // if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(...): Turning myself to %s.\n", myFutureState);
    autoSwitchAllRelays(B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0]);
  } else if (strstr(senderStatus, "off")  > 0) {
    Serial.printf("MESH: autoSwitchAllRelaysMeshWrapper(...): Turning myself to %s.\n", myFutureState);
    autoSwitchAllRelays(B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1]);
  }
  Serial.print("MESH: autoSwitchAllRelaysMeshWrapper(...): done\n");
}

void broadcastStatusOverMesh(const char* state) {
  Serial.printf("MESH: broadcastStatusOverMesh(const char* state): starting with state = %s\n", state);
  ControlerBox::boxTypeSelfUpdate(ControlerBoxes, I_NODE_NAME, BOXES_I_PREFIX);
  String str = createMeshMessage(state);
  Serial.print("MESH: broadcastStatusOverMesh(): about to call mesh.sendBroadcast(str) with str = ");Serial.println(str);
  myMesh.sendBroadcast(str);   // MESH SENDER
}

String createMeshMessage(const char* myStatus) {
  Serial.printf("MESH: createMeshMessage(const char* myStatus) starting with myStatus = %s\n", myStatus);

  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();

  msg["senderNodeName"] = nodeNameBuilder(I_NODE_NAME, nodeNameBuf);
  msg["senderAPIP"] = (ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].APIP).toString();
  msg["senderStationIP"] = (ControlerBoxes[I_NODE_NAME - BOXES_I_PREFIX].stationIP).toString();
  msg["senderStatus"] = myStatus;

  String str;
  msg.printTo(str);
  Serial.print("MESH: CreateMeshJsonMessage(...) done. Returning String: ");Serial.println(str);
  return str;
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
