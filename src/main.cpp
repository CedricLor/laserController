#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <FS.h>
#include <ESPAsyncWebServer.h>
// #define _TASK_PRIORITY         // TODO: Check if remove
// #define _TASK_STD_FUNCTION     // TODO: Check if remove
// #define _TASK_STATUS_REQUEST
// #include <TaskScheduler.h>
#include <painlessMesh.h>
#include <IPAddress.h>         // TODO: Check if remove
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)

// User stub
void serialInit();
void loadPreferences();
void initStruct(short thisPin);
void initStructs();
void initPir();
void meshSetup();
void startAsyncServer();
void OTAConfig();
void enableTasks();

void laserSafetyLoop();

void switchPirRelays(const bool state);
void directPinsSwitch(bool targetState);
void broadcastPirStatus(const char* state);
void stopPirCycle();
void setPirValue();
void startOrRestartPirCycleIfPirValueIsHigh();

void switchAllRelays(const bool targetState);
void manualSwitchOneRelay(const short thisPin, const bool targetState);
void switchOnOffVariables(const short thisPin, const bool targetState);

void pairAllPins(const bool targetPairingState);
void pairPin(const short thisPin, const bool targetPairingState);
void rePairPin(const short thisPin, const short thePairedPin);

void inclExclAllRelaysInPir(const bool targetState);
void inclExclOneRelayInPir(const short thisPin, const bool targetState);

void changeGlobalBlinkingDelay(const unsigned long blinkingDelay);
void changeIndividualBlinkingDelay(const short pinNumberFromGetRequest, const unsigned long blinkingDelay);
void changeTheBlinkingIntervalInTheStruct(const short thisPin, const unsigned long blinkingDelay);

String printAllLasersCntrl();
String printIndivLaserCntrls();
String printLinksToBoxes();
String printBlinkingDelayWebCntrl(const short thisPin);
String printMasterCntrl();
String printLabel(const String labelText, const String labelFor);
String printMasterSelect();
String printSlaveReactionSelect();
String printCurrentStatus(const short thisPin);
String printOnOffControl(const short thisPin);
String printPirStatusCntrl(const short thisPin);
String printPairingCntrl(const short thisPin);
String printDelaySelect(const short thisPin);
String printHiddenLaserNumb(const short thisPin);

void savePreferences();
void changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action);
void changeTheMasterBoxId(const short masterBoxNumber);
void changeSlaveReaction(const char* action);

void blinkLaserIfBlinking(const short thisPin);
void ifMasterPairedThenUpdateOnOffOfSlave(const short thisPin);
void executeUpdates(const short thisPin);
void blinkLaserIfTimeIsDue(const short thisPin);
void evalIfMasterIsNotInBlinkModeAndIsDueToTurnOffToSetUpdateForSlave(const short thisPin);
void updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget);

void switchPointerBlinkCycleState(const short thisPin, const bool state);

void broadcastStatusOverMesh(const char* state);

void startOTA();
void endOTA();
void progressOTA();
void errorOTA();

void onRequest();
void onBody();

void meshController(uint32_t senderNodeId, String &msg);
void boxTypeSelfUpdate();
IPAddress parseIpString(JsonObject& root, String rootKey);
void autoSwitchAllRelaysMeshWrapper(const char* senderStatus, const short iSenderNodeName);
String createMeshMessage(const char* myStatus);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KEY box variables //////////////////////////////////////////////////////////////////////////////////////////////
const short BOXES_COUNT = 10;                                                                                                 // NETWORK BY NETWORK
// short iDefaultMasterNodesNames[10] = {201,202};
const short I_NODE_NAME = 201;                                                                                                // BOX BY BOX
const short I_DEFAULT_MASTER_NODE_NAME = 211;                                                                                 // BOX BY BOX

short relayPins[] = { 22, 21, 19, 18, 5, 17, 16, 4 };  // an array of pin numbers to which relays are attached                // BOX BY BOX
const short PIN_COUNT = 8;               // the number of pins (i.e. the length of the array)                                 // BOX BY BOX

const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;                                                                     // BOX BY BOX

unsigned long const DEFAULT_PIN_BLINKING_INTERVAL = 10000UL;                                                                // BOX BY BOX


const bool MESH_ROOT = true;                                                                                                // BOX BY BOX

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH variables /////////////////////////////////////////////////////////////////////////////////////////////////
#define   MESH_PREFIX     "laser_boxes"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

painlessMesh myMesh;

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

struct box_type {
  uint32_t nodeId;
  IPAddress stationIP;
  IPAddress APIP;
};
// const short BOXES_COUNT = 10;                                                                                                // NETWORK BY NETWORK
box_type box[BOXES_COUNT];

const short BOXES_I_PREFIX = 201; // this is the iNodeName of the node in the mesh, that has the lowest iNodeName of the network // NETWORK BY NETWORK

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES FOR REACTION TO NETWORK REQUESTS
///////////////////////////////
// definition of master node //
// const short I_DEFAULT_MASTER_NODE_NAME = 202;            // See BOX KEY VARIABLES                                        // BOX BY BOX
short iMasterNodeName = I_DEFAULT_MASTER_NODE_NAME;
const short I_MASTER_NODE_PREFIX = 200;                                                                                     // NETWORK BY NETWORK
///////////////////////////////
// definition of reactions to master node state
const char* slaveReaction[4] = {"opposed: on - off & off - on", "synchronous: on - on & off - off", "always on: off - on & on - on", "always off: on - off & off - off"};
const char* slaveReactionHtml[4] = {"opp", "syn", "aon", "aof"};

// I_DEFAULT_SLAVE_ON_OFF_REACTION
// I_DEFAULT_SLAVE_ON_OFF_REACTION is: this box is opposed to its master (when the master is on, this box is off)
// const short I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;               // See BOX KEY VARIABLES                                        // BOX BY BOX

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;       // saves the index in the B_SLAVE_ON_OFF_REACTIONS bool array of the choosen reaction to the master states
const bool B_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
// HIGH, LOW = reaction if master is on = HIGH; reaction if master is off = LOW;  // Synchronous: When the master box is on, turn me on AND when the master box is off, turn me off
// LOW, HIGH = reaction if master is on = LOW; reaction if master is off = HIGH;  // Opposed: When the master box is on, turn me off AND when the master box is off, turn me on
// HIGH, HIGH = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always off: When the master box is on, turn me off AND when the master box is off, turn me off
// LOW, LOW = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always on: When the master box is on, turn me off AND when the master box is off, turn me off
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RELAYS variables /////////////////////////////
struct pin_type {
  short number;        // pin number to which the relays are attached
  bool on_off;       // is the pin HIGH or LOW (LOW = the relay is closed, HIGH = the relay is open)
  bool on_off_target;// a variable to store the on / off change requests by the various functions
  bool blinking;     // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
  unsigned long previous_time;
  unsigned long blinking_interval;
  bool pir_state;     // HIGH or LOW: HIGH -> controlled by the PIR
  short paired;        // a variable to store with which other pin this pin is paired (8 means it is not paired)
};

// short relayPins[] = {
//   22, 21, 19, 18, 5, 17, 16, 4
// };                                    // an array of pin numbers to which relays are attached   // See BOX KEY VARIABLES     // BOX BY BOX
//
// short const PIN_COUNT = 8;               // the number of pins (i.e. the length of the array)      // See BOX KEY VARIABLES     // BOX BY BOX

bool const default_pin_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const default_pin_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
bool const default_pin_blinking_state = false;       // by default, pin starts as in a blinking-cycle TO ANALYSE
// unsigned long const DEFAULT_PIN_BLINKING_INTERVAL = 10000UL;   // default blinking interval of the pin is 10 s .   // See BOX KEY VARIABLES
unsigned long pinBlinkingInterval = DEFAULT_PIN_BLINKING_INTERVAL;
bool default_pin_pir_state_value = LOW;       // by default, the pin is not controlled by the PIR
// declare and size an array to contain the structs as a global variable
pin_type pin[PIN_COUNT];
short pinParityWitness = 0;  // pinParityWitness is a variable that can be used when loop around the pins structs array.
                             // it avoids using the modulo.
                             // by switching it to 0 and 1 at each iteration of the loop
                             // in principle, the switch takes the following footprint: pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
                             // this footprint shall be inserted as the last instruction within the loop (so that it is set to the correct state for the following iteration).
                             // once the loop is over, it should be reset to 0: pinParityWitness = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AutoSwitch variables /////////////////////////////
short siAutoSwitchInterval = 60;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PIR variables //////////////////////////////////////////////////////////////////////////////////////////////////
short inputPin = 23;                  // choose the input pin (for PIR sensor)
                                      // we start assuming no motion detected
bool valPir = LOW;                    // variable for reading the pin status
const int I_PIR_INTERVAL = 1000;      // interval in the PIR cycle task (runs every second)
const short SI_PIR_ITERATIONS = 60;   // iteration of the PIR cycle

// after being started, the Pir values shall not be read for the next 60 seconds, as the PIR is likely to send equivoqual values
const short SI_PIR_START_UP_DELAY_ITERATIONS = 7;  // This const stores the number of times the tPirStartUpDelay Task shall repeat and inform the user that the total delay for the PIR to startup has not expired
const long L_PIR_START_UP_DELAY = 10000UL;         // This const stores the duration of the cycles (10 seconds) of the tPirStartUpDelay Task
short highPinsParityDuringStartup = 0;             /*  variable to store which of the odd or even pins controlling the lasers are high during the pirStartUp delay.
                                                              0 = even pins are [high] and odds are [low];
                                                              1 = odd pins are [low] and evens are [high];
                                                   */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Web server variables ///////////////////////////////////////////////////////////////////////////////////////////
AsyncWebServer asyncServer(80);
char linebuf[80];
short charcount=0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FROM PAINLESSMESH BASIC
Scheduler userScheduler; // to control your personal task
// END FROM PAINLESSMESH BASIC

// FROM PAINLESSMESH BASIC
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage, &userScheduler );
// END FROM PAINLESSMESH BASIC

/////////////////////////////////
// Tasks related to the IR startup
// StatusRequest srPirStartUpComplete;

void cbtPirStartUpDelayBlinkLaser();
bool onEnablePirStartUpDelayBlinkLaser();
void onDisablePirStartUpDelayBlinkLaser();
Task tPirStartUpDelayBlinkLaser( L_PIR_START_UP_DELAY, SI_PIR_START_UP_DELAY_ITERATIONS, &cbtPirStartUpDelayBlinkLaser, &userScheduler, false, &onEnablePirStartUpDelayBlinkLaser, &onDisablePirStartUpDelayBlinkLaser );
// Task tPirStartUpDelayBlinkLaser( L_PIR_START_UP_DELAY, SI_PIR_START_UP_DELAY_ITERATIONS, &cbtPirStartUpDelayBlinkLaser, &userScheduler, false, &onEnablePirStartUpDelayBlinkLaser, &onDisablePirStartUpDelayBlinkLaser );

void cbtPirStartUpDelayPrintDash();
Task tPirStartUpDelayPrintDash( 1000UL, 9, &cbtPirStartUpDelayPrintDash, &userScheduler );
// Task tPirStartUpDelayPrintDash( 1000UL, 9, &cbtPirStartUpDelayPrintDash, &userScheduler );

void cbtLaserOff();
Task tLaserOff( 0, 1, &cbtLaserOff, &userScheduler );
// Task tLaserOff( 0, 1, &cbtLaserOff, &userScheduler );

void cbtLaserOn();
Task tLaserOn( 0, 1, &cbtLaserOn, &userScheduler );
// Task tLaserOn( 0, 1, &cbtLaserOn, &userScheduler );

void cbtPirStartUpDelayBlinkLaser() {
  Serial.print("+");

  if (!(tPirStartUpDelayBlinkLaser.isFirstIteration())) {
    directPinsSwitch(HIGH);
    highPinsParityDuringStartup = (highPinsParityDuringStartup == 0) ? 1 : 0;
  }
  directPinsSwitch(LOW);
  tPirStartUpDelayPrintDash.restartDelayed();
  if (!(tPirStartUpDelayBlinkLaser.isLastIteration())) {
    tLaserOff.restartDelayed(1000);
    tLaserOn.restartDelayed(2000);
  }
}

bool onEnablePirStartUpDelayBlinkLaser() {
  pairAllPins(false);
//  srPirStartUpComplete.setWaiting();
  return true;
}

void onDisablePirStartUpDelayBlinkLaser() {
  pairAllPins(true);
  directPinsSwitch(HIGH);
  inclExclAllRelaysInPir(HIGH);                                     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: includes all the relays in PIR mode
//  srPirStartUpComplete.signalComplete();
}

void cbtPirStartUpDelayPrintDash() {
  Serial.print("-");
}

void cbtLaserOff() {
  directPinsSwitch(HIGH);
}

void cbtLaserOn() {
  directPinsSwitch(LOW);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Switches relay pins on and off during PIRStartUp
void directPinsSwitch(const bool targetState) {              // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisPin = highPinsParityDuringStartup; thisPin < PIN_COUNT; thisPin = thisPin + 2) {        // loop around all the structs representing the pins controlling the relays
    switchOnOffVariables(thisPin, targetState);
  }
}

void switchOnOffVariables(const short thisPin, const bool targetState) {
  // Serial.printf("MANUAL SWITCHES: switchOnOffVariables(const short thisPin, const bool targetState): switching on/off variables for pin[%u] with targetState = %s \n", thisPin, (targetState == 0 ? "on (LOW)" : "off (HIGH)"));
  switchPointerBlinkCycleState(thisPin, targetState);                     // turn the blinking state of the struct representing the pin on or off
  pin[thisPin].on_off_target = targetState;                               // turn the on_off_target state of the struct on or off
                                                                          // the actual pin will be turned on or off in the LASER SAFETY TIMER
}

void switchPointerBlinkCycleState(const short thisPin, const bool state) {
  // If the state passed on to the function is LOW (i.e.
  // probably the targetState in the calling function),
  // marks that the pin is in a blinking cycle.
  // If not, marks that the blinking cycle for this pin is off.
  (state == LOW) ? pin[thisPin].blinking = true : pin[thisPin].blinking = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PAIRING SWITCHES: Pairing and unpairing of pins
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pairAllPins(const bool targetPairingState) {
  // for (short thisPin = 0; thisPin < PIN_COUNT; thisPin = thisPin + 2) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    pairPin(thisPin, targetPairingState);
    pinParityWitness = (pinParityWitness == 0) ? 1 : 0;
  }
  pinParityWitness = 0;
}

void pairPin(const short thisPin, const bool targetPairingState) {
  const short thePairedPin = (pinParityWitness == 0) ? thisPin + 1 : thisPin - 1;
  if  (targetPairingState == false) {
    pin[thisPin].paired = 8;
    (pinParityWitness == 0) ? pin[thePairedPin].paired = 8 : pin[thePairedPin].paired = 8;
  } else {
    rePairPin(thisPin, thePairedPin);
  }
}

void rePairPin(const short thisPin, const short thePairedPin) {
  pin[thisPin].paired = thePairedPin;
  pin[thePairedPin].paired = thisPin;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PIR SUBJECTION SWITCHES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects or frees all the relays to or of the control of the PIR
void inclExclAllRelaysInPir(const bool state) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    pin[thisPin].pir_state = state;
  }
}

// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects one relay to or releases it from the control of the PIR
void inclExclOneRelayInPir(const short thisPin, const bool state) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
  pin[thisPin].pir_state = state;                 // set the pin_state variable in HIGH or LOW mode. In HIGH, the pin will be under the control of the PIR and reciprocally.
  switchOnOffVariables(thisPin, HIGH);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// FROM PAINLESSMESH BASIC
void sendMessage() {
  String msg = "Hello from node ";
  msg += myMesh.getNodeId();
  myMesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("--------- startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("+++++++++--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections %s\n",myMesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", myMesh.getNodeTime(),offset);
}
// END FROM PAINLESSMESH BASIC

void setup() {
  Serial.begin(115200);

  // FROM PAINLESSMESH BASIC
  //myMesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  myMesh.setDebugMsgTypes( CONNECTION | COMMUNICATION ); // all types on
  //myMesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  myMesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  myMesh.onReceive(&receivedCallback);
  myMesh.onNewConnection(&newConnectionCallback);
  myMesh.onChangedConnections(&changedConnectionCallback);
  myMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  // END FROM PAINLESSMESH BASIC
}

void loop() {
  // FROM PAINLESSMESH BASIC
  userScheduler.execute(); // it will run mesh scheduler as well
  myMesh.update();
  // END FROM PAINLESSMESH BASIC
}
