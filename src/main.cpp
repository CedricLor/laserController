#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPmDNS.h>            //lib to the network communication
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#define _TASK_PRIORITY
#define _TASK_STD_FUNCTION
#define _TASK_STATUS_REQUEST
#include <TaskScheduler.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)

/*  DONE:
 *  TO DO:
 *  MIDDLE: blinking delay: paired feature --> maybe already done / Check it
 *  MIDDLE: pair - unpair proc: pass the unpairing to the slave or this is going to produce unexpected results
 *  LOW: refactor all part where String is still used to replace them with arrays of char*
 *  LOW: refactoring: get rid of repetitive code where generating html tags
 *  LOW: refactor: use TEMPLATE markers in html code
 *  LOW: refactor: store html code in PROGMEM
 *  LOW: refactor: response to response-> as shown in the ESPAsyncWebServer github page
*/

/*
 * 2752557361, 10.107.105.1 = box 201, master 201, StationIP = 10.177.49.2
 * 2752932713, 10.177.49.1 = box 202, master 201
 * 2752577349, 10.255.69.1 = box 204, master 201
 */
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 // Prototypes //////////////////////////////////////////////////////////////////////////////////////////////
void directPinsSwitch(bool targetState);
void serialInit();
void loadPreferences();
void initStruct(short thisPin);
void initStructs();
void initPir();
void meshSetup();
void startAsyncServer();
void OTAConfig();
void enableTasks();
void autoSwitchTimer();
void laserSafetyLoop();
void switchPirRelays(const bool state);
void broadcastPirStatus(const char* state);
void stopPirCycle();
void setPirValue();
void startOrRestartPirCycleIfPirValueIsHigh();
void switchOnOffVariables(const short thisPin, const bool targetState);
void switchAllRelays(const bool targetState);
void manualSwitchOneRelay(const short thisPin, const bool targetState);
void inclExclAllRelaysInPir(const bool targetState);
void inclExclOneRelayInPir(const short thisPin, const bool targetState);
void changeGlobalBlinkingDelay(const unsigned long blinkingDelay);
void changeIndividualBlinkingDelay(const short pinNumberFromGetRequest, const unsigned long blinkingDelay);
void changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action);
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
void changeTheBlinkingIntervalInTheStruct(const short thisPin, const unsigned long blinkingDelay);
void savePreferences();
void changeTheBlinkingIntervalInTheStruct(const short thisPin, const unsigned long blinkingDelay);
void changeTheMasterBoxId(const short masterBoxNumber);
void changeSlaveReaction(const char* action);
void blinkLaserIfBlinking(const short thisPin);
void ifPairedUpdateOnOff(const short thisPin);
void executeUpdates(const short thisPin);
void blinkLaserIfTimeIsDue(const short thisPin);
void evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate(const short thisPin);
void updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget);
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
void autoSwitchAllRelaysMeshWrapper(const char* masterState, const bool reaction, const short iSenderNodeName);
String createMeshMessage(const char* myStatus);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KEY BOX variables //////////////////////////////////////////////////////////////////////////////////////////////
const short BOXES_COUNT = 10;                                                                                                 // NETWORK BY NETWORK
// short iDefaultMasterNodesNames[10] = {201,202};
const short I_NODE_NAME = 202;                                                                                                // BOX BY BOX
const short I_DEFAULT_MASTER_NODE_NAME = 201;                                                                                 // BOX BY BOX

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

painlessMesh  myMesh;

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

short iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;       // saves the index in the B_SLAVE_ON_OFF_REACTIONS bool char of the choosen reaction to the master states
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AutoSwitch variables /////////////////////////////
unsigned long ulAutoSwitchInterval = 60000UL;
unsigned long ulAutoSwitchStartTime;
bool autoSwitchCycle = false;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PIR variables //////////////////////////////////////////////////////////////////////////////////////////////////
short inputPin = 23;                // choose the input pin (for PIR sensor)
                                  // we start assuming no motion detected
bool valPir = LOW;                 // variable for reading the pin status
const unsigned long UL_PIR_INTERVAL = 60000UL;   // interval of the PIR cycle

// after being started, the Pir values shall not be read for the next 60 seconds, as the PIR is likely to send equivoqual values
const short UI_PIR_START_UP_DELAY = 6;  // This var stores the delay for which the Pir value shall not be read

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scheduler variables ////////////////////////////////////////////////////////////////////////////////////////////
Scheduler     userScheduler;             // to control your personal task

void sendMessage();
// Task taskSendMessage( &userScheduler, TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

/////////////////////////////////
// Tasks related to the IR startup
StatusRequest stPirStartUpComplete;

void cbtPirStartUpDelayBlinkLaser();
bool onEnablePirStartUpDelayBlinkLaser();
void onDisablePirStartUpDelayBlinkLaser();
Task tPirStartUpDelayBlinkLaser( &userScheduler, 10000UL, UI_PIR_START_UP_DELAY, &cbtPirStartUpDelayBlinkLaser, false, &onEnablePirStartUpDelayBlinkLaser, &onDisablePirStartUpDelayBlinkLaser );

void cbtPirStartUpDelayPrintDash();
Task tPirStartUpDelayPrintDash( &userScheduler, 1000UL, 9, &cbtPirStartUpDelayPrintDash );

void cbtLaserOff();
Task tLaserOff( &userScheduler, 0, 1, &cbtLaserOff );

void cbtLaserOn();
Task tLaserOn( &userScheduler, 0, 1, &cbtLaserOn );

void cbtPirStartUpDelayBlinkLaser() {
  Serial.print("+");

  directPinsSwitch(HIGH);
  highPinsParityDuringStartup = (highPinsParityDuringStartup == 0) ? 1 : 0;
  directPinsSwitch(LOW);
  tPirStartUpDelayPrintDash.restartDelayed();
  if (!(tPirStartUpDelayBlinkLaser.isLastIteration())) {
    tLaserOff.restartDelayed(1000);
    tLaserOn.restartDelayed(2000);
  }
}

bool onEnablePirStartUpDelayBlinkLaser() {
  stPirStartUpComplete.setWaiting();
  return true;
}

void onDisablePirStartUpDelayBlinkLaser() {
  directPinsSwitch(HIGH);
  stPirStartUpComplete.signalComplete();
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN SUBS - SETUP AND LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void setup() {
  delay(2000);
  serialInit();
  loadPreferences();
  initStructs();
  initPir();
  meshSetup();
  startAsyncServer();
  OTAConfig();
  enableTasks();
  Serial.printf("-----------------------------------------------\n-------- SETUP DONE ---------------------------\n-----------------------------------------------\n");
}

void loop() {
  ArduinoOTA.handle();
  userScheduler.execute();   // it will run mesh scheduler as well
  myMesh.update();
  autoSwitchTimer();         // TO ANALYSE: Should probably be before the laser safety loop. Why did I put it after???
  laserSafetyLoop();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Subs LOOP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PIR Controller
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read the state of the PIR and turn the relays on or off depending on the state of the PIR

//////////////////////
// TASK FOR PIR CYCLE ON/OFF
bool tcbOnEnablePirCycle();
void tcbOnDisablePirCycle();
Task tPirCycle ( &userScheduler, 1000, UL_PIR_INTERVAL, NULL, false, &tcbOnEnablePirCycle, &tcbOnDisablePirCycle);

bool tcbOnEnablePirCycle() {
  Serial.printf("PIR: tcbStartPirCycle(): Motion detected!!!\n");
  switchPirRelays(LOW);
  broadcastPirStatus("on");                                                                        // broadcast startup of a new pir Cycle
  Serial.printf("PIR: tcbStartPirCycle(): broadcastPirStatus(\"on\")");
  return true;
}

void tcbOnDisablePirCycle() {
  Serial.printf("PIR: tcbStopPirCycle(): PIR time is due. Ending PIR Cycle -------\n");
  stopPirCycle();
}

//////////////////////
// TASK FOR PIR CONTROL
void tcbPirCntrl();
Task tPirCntrl ( &userScheduler, 0, TASK_FOREVER, &tcbPirCntrl, false);
void tcbPirCntrl() {
  setPirValue();
  startOrRestartPirCycleIfPirValueIsHigh();
}

void setPirValue() {
  valPir = digitalRead(inputPin); // read input value from the pin connected to the IR. If IR has detected motion, digitalRead(inputPin) will be HIGH.  Serial.println(valPir);
}

void startOrRestartPirCycleIfPirValueIsHigh() {
  if (valPir == HIGH) {                                                                              // if the PIR sensor has sensed a motion,
    if (!(tPirCycle.isEnabled())) {
      tPirCycle.setIterations(UL_PIR_INTERVAL);
      tPirCycle.restart();
    } else {
      long _done_it = tPirCycle.getRunCounter();
      long _remaining_it = tPirCycle.getIterations();
      tPirCycle.setIterations(_remaining_it + _done_it * 2);
    }
  }
  valPir = LOW;                                                                                      // Reset the ValPir to low (no motion detected)
}

void stopPirCycle() {
  Serial.printf("PIR: stopPirCycle(): stopping PIR cycle.\n");
  switchPirRelays(HIGH);                                  // turn all the PIR controlled relays off
  broadcastPirStatus("off");                              // broadcast current pir status
}


// loop over each of the structs representing pins to turn them on or off (if they are controlled by the PIR)
void switchPirRelays(const bool state) {
  Serial.printf("PIR: switchPirRelays(const bool state): starting -------\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    if (pin[thisPin].pir_state == HIGH) {
      switchOnOffVariables(thisPin, state);
    }
  }
  Serial.printf("PIR: switchPirRelays(const bool state): leaving -------\n");
}

// TO DO: Combine logic of directPinsSwitch() and switchOnOffVariables()
void directPinsSwitch(const bool targetState) {              // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (short thisPin = highPinsParityDuringStartup; thisPin < PIN_COUNT; thisPin = thisPin + 2) {        // loop around all the structs representing the pins controlling the relays
    digitalWrite(pin[thisPin].number, targetState);           // switch on or off
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WEB CONTROLLER
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void webSwitchRelays(AsyncWebParameter* _p2, bool targetState) {
  if(_p2->value() == "a"){
    switchAllRelays(targetState);
  } else {
    manualSwitchOneRelay(_p2->value().toInt(), targetState);
  }
}

void webInclExclRelaysInPir(AsyncWebParameter* _p2, bool targetState) {
  if(_p2->value() == "a"){
    inclExclAllRelaysInPir(targetState);
  } else {
    inclExclOneRelayInPir(_p2->value().toInt(), targetState);
  }
}


void decodeRequest(AsyncWebServerRequest *request) {
  Serial.println("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): DECODING WEB REQUEST >>>>>>>>>>>>>>>>");

  if(request->hasParam("manualStatus")) {
    AsyncWebParameter* _p1 = request->getParam("manualStatus");
    AsyncWebParameter* _p2 = request->getParam("laser");
    if(_p1->value() == "on"){
      webSwitchRelays(_p2, LOW);
    } else {
      webSwitchRelays(_p2, HIGH);
    }
    return;
  }

  if(request->hasParam("statusIr")) {
    AsyncWebParameter* _p1 = request->getParam("statusIr");
    AsyncWebParameter* _p2 = request->getParam("laser");
    if(_p1->value() == "on"){
      webInclExclRelaysInPir(_p2, HIGH);
    } else {
      webInclExclRelaysInPir(_p2, LOW);
    }
    return;
  }

  if(request->hasParam("blinkingDelay")) {
    AsyncWebParameter* _p1 = request->getParam("blinkingDelay");
    AsyncWebParameter* _p2 = request->getParam("laser");
    Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): laser number for change in blinkingDelay %s\n", _p2->value().c_str());
    if (_p2->value() == "10") {
      Serial.printf("WEB CONTROLLER: decodeRequest(AsyncWebServerRequest *request): %s\n", _p2->value().c_str());
      changeGlobalBlinkingDelay(_p1->value().toInt());
    }
    else {
      changeIndividualBlinkingDelay(_p2->value().toInt(), _p1->value().toInt());
    }
    return;
  }

  if(request->hasParam("masterBox")) {
    AsyncWebParameter* _p1 = request->getParam("masterBox");
    AsyncWebParameter* _p2 = request->getParam("reactionToMaster");
    changeGlobalMasterBoxAndSlaveReaction(_p1->value().toInt(), _p2->value().c_str());
    return;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VIEWS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
String returnTheResponse() {
  String myResponse = "<!DOCTYPE HTML><html>";
  myResponse += "<body>";
  myResponse += "<body>";
  myResponse += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>";
  myResponse += "<h1>";
  myResponse += String(I_NODE_NAME);
  myResponse += "  ";
  myResponse += (box[I_NODE_NAME - BOXES_I_PREFIX].APIP).toString();
  myResponse += "</h1>";
  myResponse += printAllLasersCntrl();
  myResponse += printIndivLaserCntrls();
  myResponse += printLinksToBoxes();
  myResponse += "</body></html>";
  Serial.println(myResponse);
  return myResponse;
}
String printLinksToBoxes() {
  String linksToBoxes = "<div class=\"box_links_wrapper\">";
  IPAddress testIp(0,0,0,0);
  for (short i = 0; i < 10; i++) {
    if (!(box[i].stationIP == testIp)) {
      linksToBoxes += "<div class=\"box_link_wrapper\">Station IP: ";
      linksToBoxes += "<a href=\"http://";
      linksToBoxes += (box[i].stationIP).toString();
      linksToBoxes +=  "/\">Box number: ";
      linksToBoxes += i + 1;
      linksToBoxes += "</a> APIP: ";
      linksToBoxes += "<a href=\"http://";
      linksToBoxes += (box[i].APIP).toString();
      linksToBoxes +=  "/\">Box number: ";
      linksToBoxes += i + 1;
      linksToBoxes += "</a>";
      linksToBoxes += "</div>";
    }
  }
  linksToBoxes += "</div>";
  return linksToBoxes;
}

String printAllLasersCntrl() {
  String laserCntrls = "<div>All Lasers <a href=\"?manualStatus=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=a\"><button>OFF</button></a>";
  laserCntrls += " IR <a href=\"?statusIr=on&laser=a\"><button>ON</button></a>&nbsp;<a href=\"?statusIr=of&laser=a\"><button>OFF</button></a>";
  laserCntrls += printBlinkingDelayWebCntrl(9);
  laserCntrls += "</div>";
  laserCntrls += "<div>";
  laserCntrls += printMasterCntrl();
  laserCntrls += "</div>";
  laserCntrls += "<hr>";
  return laserCntrls;
}

String printMasterCntrl() {

  String masterCntrl = "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  masterCntrl += printLabel("Master box: ", "master-select");
  masterCntrl += printMasterSelect();

  masterCntrl += printLabel(" Reaction: ", "reaction-select");
  masterCntrl += printSlaveReactionSelect();

  masterCntrl += "<button type=\"submit\">Submit</button>";
  masterCntrl += "</form>";

  return masterCntrl;
}

String printOption(const String optionValue, const String optionText, const String selected = "");

String printMasterSelect() {
  String masterSelect = "<select id=\"master-select\" name=\"masterBox\">";
  for (short i = 1; i < 11; i++) {
    String selected = "";
    if (i + I_MASTER_NODE_PREFIX == iMasterNodeName) {
      selected += "selected";
    };
    if (!(i + I_MASTER_NODE_PREFIX == I_NODE_NAME)) {
      masterSelect += printOption(String(i), String(i), selected);
    }
  }
  masterSelect += "</select>";
  return masterSelect;
}

String printSlaveReactionSelect() {
  String slaveReactionSelect = "<select id=\"reaction-select\" name=\"reactionToMaster\">";
  for (short i = 0; i < 4; i++) {
    String selected = "";
    if (i == iSlaveOnOffReaction) {
      selected += "selected";
     };
    slaveReactionSelect += printOption(slaveReactionHtml[i], slaveReaction[i], selected);
  }
  slaveReactionSelect += "</select>";
  return slaveReactionSelect;
}

String printLabel(const String labelText, const String labelFor) {
  String labelCntrl = "<label for=\"";
  labelCntrl += labelFor;
  labelCntrl += "\">";
  labelCntrl += labelText;
  labelCntrl += "</label>";
  return labelCntrl;
}

String printOption(const String optionValue, const String optionText, const String selected) {
  String optionCntrl = "<option value=\"";
  optionCntrl += optionValue;
  optionCntrl += "\" ";
  optionCntrl += selected;
  optionCntrl += ">";
  optionCntrl += optionText;
  optionCntrl += "</option>";
  return optionCntrl;
}

String printIndivLaserCntrls() {
  String laserCntrl;
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    laserCntrl += "<div>Laser # ";
    laserCntrl += thisPin + 1;

    // on/off control
    laserCntrl += printCurrentStatus(thisPin);

    // on/off control
    laserCntrl += printOnOffControl(thisPin);

    // PIR status control
    laserCntrl += printPirStatusCntrl(thisPin);

    // pairing control
    laserCntrl += printPairingCntrl(thisPin);

    // blinking delay control
    laserCntrl += printBlinkingDelayWebCntrl(thisPin);

    laserCntrl += "</div>";
  }
  return laserCntrl;
}

String printCurrentStatus(const short thisPin) {
  String currentStatus;
  if (pin[thisPin].blinking == true) {
    currentStatus += " ON ";
  } else {
    currentStatus += " OFF ";
  }
  if (pin[thisPin].pir_state == HIGH) {
    currentStatus += " in IR mode ";
  } else {
    currentStatus += " in manual mode ";
  }
  return currentStatus;
}

String printOnOffControl(const short thisPin) {
  String onOffCntrl;
  onOffCntrl += "<a href=\"?manualStatus=on&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>OFF</button></a>";
  return onOffCntrl;
}

String printPirStatusCntrl(const short thisPin) {
  String pirStatusCntrl;
  if (pin[thisPin].pir_state == LOW) {
    pirStatusCntrl += "<a href=\"?statusIr=on&laser=";
    pirStatusCntrl += thisPin + 1;
    pirStatusCntrl += "\"><button>IR ON</button></a>&nbsp;";
  }
  else {
    pirStatusCntrl += "<a href=\"?statusIr=of&laser=";
    pirStatusCntrl += thisPin + 1;
    pirStatusCntrl += "\"><button>IR OFF</button></a>";
  }
  return pirStatusCntrl;
}

String printBlinkingDelayWebCntrl(const short thisPin) {
  String blinkingDelayWebCntrl;
  blinkingDelayWebCntrl += "Blinking delay: ";
  blinkingDelayWebCntrl += "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  blinkingDelayWebCntrl += printDelaySelect(thisPin);
  blinkingDelayWebCntrl += printHiddenLaserNumb(thisPin);
  blinkingDelayWebCntrl += "<button type=\"submit\">Submit</button>";
  blinkingDelayWebCntrl += "</form>";
  return blinkingDelayWebCntrl;
}

String printPairingCntrl(const short thisPin) {
  String pairingWebCntrl;
  if (pin[thisPin].paired == 8) {
    pairingWebCntrl += " Unpaired ";
    pairingWebCntrl += "<a href=\"pairingState=pa&laser=";
    pairingWebCntrl += thisPin + 1;
    pairingWebCntrl += "\"><button>PAIR</button></a>&nbsp;";
  } else if (thisPin % 2 == 0) {
    pairingWebCntrl += " Master ";
    pairingWebCntrl += "<a href=\"pairingState=un&laser=";
    pairingWebCntrl += thisPin + 1;
    pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
  } else {
    pairingWebCntrl += " Slave ";
    pairingWebCntrl += "<a href=\"pairingState=un&laser=";
    pairingWebCntrl += thisPin + 1;
    pairingWebCntrl += "\"><button>UNPAIR</button></a>&nbsp;";
  }
  return pairingWebCntrl;
}

String printDelaySelect(const short thisPin) {
  String delaySelect;
  delaySelect += "<select name=\"blinkingDelay\">";
  for (unsigned long delayValue = 5000; delayValue < 35000; delayValue = delayValue + 5000) {
    delaySelect += "<option value=\"";
    delaySelect += delayValue;
    delaySelect += "\"";
    if (thisPin < 9) {
      if (delayValue == pin[thisPin].blinking_interval) {
        delaySelect += "selected";
      }
    } else if (delayValue == pinBlinkingInterval) {
      delaySelect += "selected";
    }
    delaySelect += ">";
    delaySelect += delayValue / 1000;
    delaySelect += " s.</option>";
  }
  delaySelect += "</select>";
  return delaySelect;
}

String printHiddenLaserNumb(const short thisPin) {
  String hiddenLaserCntrl;
  hiddenLaserCntrl += "<input type=\"hidden\" name=\"laser\" value=\"";
  hiddenLaserCntrl += thisPin + 1;
  hiddenLaserCntrl += "\">";
  return hiddenLaserCntrl;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MODELS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void switchPointerBlinkCycleState(const short thisPin, const bool state) {
  // If the state passed on to the function is LOW (i.e.
  // probably the targetState in the calling function),
  // marks that the pin is in a blinking cycle.
  // If not, marks that the blinking cycle for this pin is off.
  if (state == LOW) {
    pin[thisPin].blinking = true;
  } else {
    pin[thisPin].blinking = false;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MANUAL SWITCHES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Manually switches all the lasers
void switchAllRelays(const bool state) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    manualSwitchOneRelay(thisPin, state);
  }
}

// Manually switches a single laser
void manualSwitchOneRelay(const short thisPin, const bool targetState) {
  Serial.printf("MANUAL SWITCHES: manualSwitchOneRelay(const short thisPin, const bool targetState): switching pin[%u] to targetState %s\n", thisPin, (targetState == 0 ? ": on" : ": off"));      // MIGHT CAUSE A BUG!!!
  switchOnOffVariables(thisPin, targetState);
  pin[thisPin].pir_state = LOW;
}

void switchOnOffVariables(const short thisPin, const bool targetState) {
  Serial.printf("MANUAL SWITCHES:switchOnOffVariables(const short thisPin, const bool targetState): switching on/off variables for pin[%u] with targetState =%s \n", thisPin + 1, (targetState == 0 ? "on (LOW)" : "off (HIGH)."));
  switchPointerBlinkCycleState(thisPin, targetState);                     // turn the blinking state of the struct representing the pin on or off
  pin[thisPin].on_off_target = targetState;                               // turn the on_off_target state of the struct on or off
                                                                          // the actual pin will be turned on or off in the LASER SAFETY TIMER
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BLINKING DELAY Control
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void changeGlobalBlinkingDelay(const unsigned long blinkingDelay) {
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    Serial.println("WEB CONTROLLER: Changing pin blinking delay");
    changeTheBlinkingIntervalInTheStruct(thisPin, blinkingDelay);
    pinBlinkingInterval = blinkingDelay;
    savePreferences();
  }
}

void changeIndividualBlinkingDelay(const short pinNumberFromGetRequest, const unsigned long blinkingDelay) {
  changeTheBlinkingIntervalInTheStruct(pinNumberFromGetRequest, blinkingDelay);
}

void changeTheBlinkingIntervalInTheStruct(const short thisPin, const unsigned long blinkingDelay) {
  pin[thisPin].blinking_interval = blinkingDelay;
  Serial.printf("WEB CONTROLLER: changeTheBlinkingIntervalInTheStruct(const short thisPin, const unsigned long blinkingDelay): This pin's blinking delay is now: ");Serial.println(pin[thisPin].blinking_interval);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHANGE MASTER BOX Control
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TO REDRAFT AND TO ADD COMMENTS TO THE CODE OF THE WHOLE BLOCK
void changeGlobalMasterBoxAndSlaveReaction(const short masterBoxNumber, const char* action) {
  changeTheMasterBoxId(masterBoxNumber);
  changeSlaveReaction(action);
  savePreferences();
}

void changeTheMasterBoxId(const short masterBoxNumber) {
  Serial.printf("WEB CONTROLLER: changeTheMasterBoxId(const short masterBoxNumber): Starting with masterBoxNumber = %u\n", masterBoxNumber);
  iMasterNodeName = I_MASTER_NODE_PREFIX + masterBoxNumber;
  Serial.printf("WEB CONTROLLER: changeTheMasterBoxId(const short masterBoxNumber): Done\n");
}

void savePreferences() {
  Serial.printf("PREFERENCES: savePreferences(): starting\n");
  Preferences preferences;
  preferences.begin("savedSettingsNS", false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putUInt("savedSettings", preferences.getUInt("savedSettings", 0) + 1);
  preferences.putUInt("iSlavOnOffReac", iSlaveOnOffReaction);
  preferences.putUInt("iMasterNName", iMasterNodeName);
  preferences.putULong("pinBlinkInt", pinBlinkingInterval);

  preferences.end();
  Serial.printf("WEB CONTROLLER: savePreferences(): done\n");
}

void changeSlaveReaction(const char* action) {
  Serial.printf("WEB CONTROLLER: changeSlaveReaction(char *action): starting with action (char argument) =%s\n", action);
  for (short i=0; i < 4; i++) {
    Serial.printf("WEB CONTROLLER: changeSlaveReaction(): looping over the slaveReactionHtml[] array\n");
    if (strcmp(slaveReactionHtml[i], action) > 0) {
      Serial.printf("WEB CONTROLLER: changeSlaveReaction(): saving iSlaveOnOffReaction\n");
      short t = i;
      iSlaveOnOffReaction = t;
      break; // break for
    }
  }
  Serial.printf("WEB CONTROLLER: changeSlaveReaction(): done\n");
}

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  // see https://stackoverflow.com/questions/35227449/convert-ip-or-mac-address-from-string-to-byte-array-arduino-or-c
  /*  Call it this way:
        const char* ipStr = "50.100.150.200";
        byte ip[4];
        parseBytes(ipStr, '.', ip, 4, 10);
   */
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);  // Convert byte
    str = strchr(str, sep);               // Find next separator
    if (str == NULL || *str == '\0') {
        break;                            // No more separators, exit
    }
    str++;                                // Point to next character after separator
  }
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
    ifPairedUpdateOnOff(thisPin);                           // update the on/off status of any paired laser and its paired companion
    executeUpdates(thisPin);                                // transform the update to the struct to analogical updates in the status of the related pin
  }
}

void blinkLaserIfBlinking(const short thisPin) {
  if (pin[thisPin].blinking == true && (pin[thisPin].paired == 8 || thisPin % 2 == 0)) {          // check if the laser is in blinking mode and is either non-paired or master in a pair
    blinkLaserIfTimeIsDue(thisPin);                                                               // if so, switch its on/off state
  }
}

void blinkLaserIfTimeIsDue(const short thisPin) {
  // function called when a laser is in blinking mode to check if the blinking interval has elapsed
  const unsigned long currentTime = millis();
  if (currentTime - pin[thisPin].previous_time > pin[thisPin].blinking_interval) {    // if blinking interval of the specific laser has elapsed
      pin[thisPin].on_off_target = !pin[thisPin].on_off;                              // switch the pin on/off target variable to the contrary of the pin on/off TO ANALYSE: this may overwrite other changes that have been requested at other stages
  }
}

/*
TO DO: DRAFT WITH A LAMBDA AND USE EVERYWHERE
bool isTimerDue(const long previous_time, const long interval) {
  unsigned long currentTime = millis();
  if (currentTime - previous_time > interval) {
    return true;
  } else {
    return false;
  }
}
*/

void ifPairedUpdateOnOff(const short thisPin) {
  // Serial.println("SAFETY LOOP: ifPairedUpdateOnOff(): starting");
  if (!(pin[thisPin].paired == 8) && (thisPin % 2 == 0)) {                  // if the laser is not unpaired (if paired is set at 8, it means it is not paired)
                                                                            // AND if it is a master
    // Serial.print("xxxxxxxxx paired pin[");Serial.print(thisPin + 1);Serial.println("] is paired xxxxxxxxx");
    // Serial.println("xxxxxxxxx calling evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate for this pin xxxxxxxxx");
    evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate(thisPin);
  }
}

void evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate(const short thisPin) {
  // Serial.print("xxxxxxxxx eval i) blinking status and ii) of pin[");Serial.print(thisPin + 1);Serial.println("]: xxxxxxxxx");
  // evaluate if (i) the pin is NOT blinking and (i) whether its on_off_target state is to turn off
  // if so, it means that the master pin has been turned off. The slave pin should then be turned off.
  if ((pin[thisPin].blinking == false) && (pin[thisPin].on_off_target == HIGH)) {
    // if master is off, update the variables of the slaves to turn it off
    updatePairedSlave(thisPin, pin[thisPin].on_off_target);
  } else {
    // if not, inverse the on_off_target of the slave
    updatePairedSlave(thisPin, !pin[thisPin].on_off_target);
  }
}

void updatePairedSlave(const short thisPin, const bool nextPinOnOffTarget) {
  pin[thisPin + 1].on_off_target = nextPinOnOffTarget;                          // update the on_off target of the paired slave laser
  pin[thisPin + 1].blinking = pin[thisPin].blinking;                            // update the blinking state of the paired slave laser
  pin[thisPin + 1].pir_state = pin[thisPin].pir_state;                          // update the IR state of the paired slave laser
  pin[thisPin + 1].paired = thisPin;                                            // update the paired pin of the slave to this pin
}

void executeUpdates(const short thisPin) {
  if (pin[thisPin].on_off != pin[thisPin].on_off_target) {         // check whether the target on_off state is different than the current on_off state
                                                                   // TO ANALYSE: I have the feeling that the condition to be tested shall be different
                                                                   // in the case a) a laser is in a blinking mode and in the case b) a laser is not in
                                                                   // a blinking mode and cooling off
    digitalWrite(pin[thisPin].number, pin[thisPin].on_off_target); // if so, turn on or off the laser as requested in the target_state
    pin[thisPin].on_off = pin[thisPin].on_off_target;              // align the on_off with the on_off_target
    pin[thisPin].previous_time = millis();                         // reset the safety blinking timer of this pin
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void subAutoSwitchRelaysMsg(const char* masterState, const bool reaction) {
  Serial.printf(" is %s. Turning myself to %s.\n", masterState, (reaction == LOW ? "on." : "off."));
}

// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void autoSwitchAllRelays(const bool targetState) {
  ulAutoSwitchStartTime = millis();                                  // start the timer of the auto switch upon starting the autoSwitch
  switchAllRelays(targetState);                                       // put the pin in manual mode, with the desired state
  autoSwitchCycle = true;                                             // Mark that the autoSwitch cycle has started. Necessary to deactivate other functions.
  Serial.printf("-------- Auto Switch cycle started............ --------\n");
}

void autoSwitchTimer() {
  if (autoSwitchCycle == false) {                                     // Do not execute the autoSwitchTimer if no autoSwitchCycle has started
    return;
  }
  // Serial.println("      SLAVE MODE: autoSwitchTimer(): Auto switch is on (I am the slave of another box). Checking the autoSwitch timer.");
  long currentTime = millis();
  if (currentTime - ulAutoSwitchStartTime > ulAutoSwitchInterval) {  // if the autoSwitch time interval has elapsed
    Serial.printf("      SLAVE MODE: autoSwitchTimer(): Auto Switch Timer has expired. I am released of my slavery.\n");
    autoSwitchCycle = false;                                          // Mark that the autoSwitch cycle has started. Necessary to deactivate other functions.
    inclExclAllRelaysInPir(HIGH);                                     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
    Serial.printf("      SLAVE MODE: autoSwitchTimer(): -------- Auto Switch cycle ended............ --------\n");
  }
}
// Function checked dans une situation où autoSwitchCycle == false.

void autoSwitchOneRelay(const short thisPin, const bool targetState) {
  /*  A REDIGER LORSQUE CE SERA NECESSAIRE
      switchOnOffVariables(thisPin, targetState);
      switchPointerBlinkCycleState(thisPin, targetState);
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

  Serial.println("PIR - broadcastPirStatus(): ending.");
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
  Serial.println();
  Serial.println("SETUP: serialInit(): done");
}

void loadPreferences() {
  Serial.println("SETUP: loadPreferences(): starting");
  Preferences preferences;
  preferences.begin("savedSettingsNS", true);        // Open Preferences with savedSettingsNS namespace. Open storage in Read only mode (second parameter true).
  unsigned int savedSettings = preferences.getUInt("savedSettings", 0);
  if (savedSettings > 0) {
    Serial.print("SETUP: loadPreferences(). NVS has saved settings. Loading values.\n");
    iSlaveOnOffReaction = preferences.getUInt("iSlavOnOffReac", I_DEFAULT_SLAVE_ON_OFF_REACTION);
    Serial.printf("SETUP: loadPreferences(). iSlaveOnOffReaction set to: %u\n", iSlaveOnOffReaction);
    iMasterNodeName = preferences.getUInt("iMasterNName", iMasterNodeName);
    Serial.print("SETUP: loadPreferences(). iMasterNodeName set to: ");Serial.println(iMasterNodeName);
    pinBlinkingInterval = preferences.getULong("pinBlinkInt", pinBlinkingInterval);
    Serial.print("SETUP: loadPreferences(). pinBlinkingInterval set to: ");Serial.println(pinBlinkingInterval);
  }
  preferences.end();
  Serial.println("SETUP: loadPreferences(): done");
}

void initStructs() {
  Serial.printf("SETUP: initStructs(): starting\n");
  for (short thisPin = 0; thisPin < PIN_COUNT; thisPin++) {
    // Initialize structs
    initStruct(thisPin);
    pinMode(pin[thisPin].number, OUTPUT);     // initialization of the pin connected to each of the relay as output
    digitalWrite(pin[thisPin].number, HIGH);  // setting default value of the pins at HIGH (relay closed)
  }
  Serial.printf("SETUP: initStructs(): done\n");
}

void initStruct(short thisPin) {
  short pairedPinNumber = (thisPin % 2 == 0) ? (thisPin + 1) : (thisPin - 1);
  pin[thisPin] = {
    relayPins[thisPin],
    default_pin_on_off_state,        // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    default_pin_on_off_target_state, // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    default_pin_blinking_state,      // by default, pin starts as in a blinking-cycle TO ANALYSE
    millis(),
    pinBlinkingInterval,            // default blinking interval of the pin is 10 s, unless some settings have been saved to EEPROM
    default_pin_pir_state_value,     // by default, the pin is not controlled by the PIR
    pairedPinNumber  // by default, the pins are paired
  };
}

void initPir() {
  Serial.printf("SETUP: initPir(): starting\n");
  pinMode(inputPin, INPUT);                  // declare sensor as input
  Serial.printf("SETUP: initPir(): done\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP: OTA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OTAConfig() {
  Serial.println("SETUP: OTAConfig(): starting");
  ArduinoOTA.onStart( startOTA ); //startOTA is a function created to simplificate the code
  ArduinoOTA.onEnd( endOTA ); //endOTA is a function created to simplificate the code
  //ArduinoOTA.onProgress( progressOTA ); //progressOTA is a function created to simplificate the code
  //ArduinoOTA.onError( errorOTA );//errorOTA is a function created to simplificate the code
  ArduinoOTA.begin();

  //prints the ip address used by ESP
  Serial.printf("SETUP: OTAConfig(): ready\n");
  Serial.print("SETUP: OTAConfig(): IP address: ");Serial.println(WiFi.localIP());
}

void startOTA() {
  Serial.println("Start updating filesystem");
}

void endOTA() {
  Serial.println("\nEnd");
}

void progressOTA(unsigned int progress, unsigned int total) {
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void errorOTA(ota_error_t error) {
  Serial.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR)
    Serial.println("Auth Failed");
  else
  if (error == OTA_BEGIN_ERROR)
    Serial.println("Begin Failed");
  else
  if (error == OTA_CONNECT_ERROR)
    Serial.println("Connect Failed");
  else
  if (error == OTA_RECEIVE_ERROR)
    Serial.println("Receive Failed");
  else
  if (error == OTA_END_ERROR)
    Serial.println("End Failed");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    Serial.println("In handler of \"/\" request -------");

    // List all collected headers
    listAllCollectedHeaders(request);

    // List all parameters
    listAllCollectedParams(request);

    // List all parameters
    decodeRequest(request);

    //Send a response
    AsyncResponseStream *response = request->beginResponseStream("text/html");  // define a response stream
    response->addHeader("Server","ESP Async Web Server");                       // append stuff to header
    response->printf(returnTheResponse().c_str());                              // converts the arduino String in C string (array of chars)
    request->send(response);                                                    // send the response
  });

  // asyncServer.onNotFound(onRequest);  // error: no matching function for call to 'AsyncWebServer::onNotFound(void (&)())'
  // asyncServer.onRequestBody(onBody);  // error: no matching function for call to 'AsyncWebServer::onRequestBody(void (&)())'

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
  tPirCntrl.waitFor(&stPirStartUpComplete);
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
  Serial.printf("   MESH CALLBACK: receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  meshController(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  boxTypeSelfUpdate();
  broadcastStatusOverMesh("na");
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n",myMesh.subConnectionJson().c_str());
  boxTypeSelfUpdate();
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

  boxTypeSelfUpdate();

  myMesh.onReceive(&receivedCallback);
  myMesh.onNewConnection(&newConnectionCallback);
  myMesh.onChangedConnections(&changedConnectionCallback);
  myMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  myMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed

  //userScheduler.addTask( taskSendMessage );
  //taskSendMessage.enable();
}

void boxTypeSelfUpdate() {
  box[I_NODE_NAME - BOXES_I_PREFIX].APIP = myMesh.getAPIP();      // store this boxes APIP in the array of boxes pertaining to the mesh
  box[I_NODE_NAME - BOXES_I_PREFIX].stationIP = myMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  box[I_NODE_NAME - BOXES_I_PREFIX].nodeId = myMesh.getNodeId();  // store this boxes nodeId in the array of boxes pertaining to the mesh
}

void boxTypeUpdate(uint32_t iSenderNodeName, uint32_t senderNodeId, JsonObject& root) {
  Serial.printf("    MESH CONTROLLER: boxTypeUpdate(...) storing the IP adresses of the sender in a box struct in the boxes array \n");
  box[iSenderNodeName - BOXES_I_PREFIX].stationIP = parseIpString(root, "senderStationIP");
  box[iSenderNodeName - BOXES_I_PREFIX].APIP = parseIpString(root, "senderAPIP");
  box[iSenderNodeName - BOXES_I_PREFIX].nodeId = senderNodeId;
  Serial.printf("    MESH CONTROLLER: boxTypeUpdate(...) storing the IP adresses of the sender: done \n");
}

short jsonToInt(JsonObject& root, String rootKey) {
  short iValue;
  const char* sValue = root[rootKey];
  iValue = atoi(sValue);
  return iValue;
}

IPAddress parseIpString(JsonObject& root, String rootKey) {
  const char* ipStr = root[rootKey];
  byte ip[4];
  parseBytes(ipStr, '.', ip, 4, 10);
  return ip;
}

void meshController(uint32_t senderNodeId, String &msg) {
  // React depending on wether the remote is myMaster and if so, on its on or off status

  Serial.printf("    MESH CONTROLLER: meshController(uint32_t senderNodeId, String &msg) starting with senderNodeId == %u and &msg == %s \n", senderNodeId, msg.c_str());
  StaticJsonBuffer<250> jsonBuffer;
  Serial.printf("    MESH CONTROLLER: meshController(uint32_t senderNodeId, String &msg): jsonBuffer created\n");
  JsonObject& root = jsonBuffer.parseObject(msg.c_str());
  Serial.printf("    MESH CONTROLLER: meshController(uint32_t senderNodeId, String &msg): jsonBuffer parsed into JsonObject& root\n");

  const short iSenderNodeName = jsonToInt(root, "senderNodeName");
  Serial.printf("    MESH CONTROLLER: meshController(...) %u alloted from root[\"senderNodeName\"] to iSenderNodeName \n", iSenderNodeName);
  const char* senderStatus = root["senderStatus"];
  Serial.printf("    MESH CONTROLLER: meshController(...) %s alloted from root[\"senderStatus\"] to senderStatus \n", senderStatus);

  // update the box struct corresponding to the sender with the data received from the sender
  boxTypeUpdate(iSenderNodeName, senderNodeId, root);

  // Is the message addressed to me?
  if (!(iSenderNodeName == iMasterNodeName)) {                 // do not react to broadcast message if message not sent by relevant sender
    return;
  }

  // If so, act depending on the sender status
  if (strstr(senderStatus, "on")  > 0) {                              // if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    autoSwitchAllRelaysMeshWrapper("on. ", B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0], iSenderNodeName);  // index numbers in array B_SLAVE_ON_OFF_REACTIONS[]:
                                                                                                  // First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
                                                                                                  // First index number has already been replaced by the iSlaveOnOffReaction variable, which is set either:
                                                                                                  // - at startup in the global variables definition;
                                                                                                  // - via the changeSlaveReaction sub.
                                                                                                  // Second index number is the reaction to the on state of the master box if 1, to the off state if 2
  } else if (strstr(senderStatus, "off")  > 0) {                                                            // if senderStatus does not contain "on", it means that the master box (the mesh sender) is turned off.
    autoSwitchAllRelaysMeshWrapper("off. ", B_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1], iSenderNodeName);
  }
}

void autoSwitchAllRelaysMeshWrapper(const char* masterState, const bool reaction, const short iSenderNodeName) {
  // print to console a sentence such as "192.168.1.202 is on. Turning myself to on"
  // then call the autoSwitchAllRelays
  Serial.printf("    MESH: autoSwitchAllRelaysMeshWrapper(masterState, reaction, iSenderNodeName) starting with iSenderNodeName = %u\n", iSenderNodeName);
  subAutoSwitchRelaysMsg(masterState, reaction);
  autoSwitchAllRelays(reaction);
  Serial.println("    MESH: autoSwitchAllRelaysMeshWrapper(): done");
}

void broadcastStatusOverMesh(const char* state) {
  Serial.printf("MESH: broadcastStatusOverMesh(const char* state): starting with state = %s\n", state);
  String str = createMeshMessage(state);
  Serial.print("MESH: broadcastStatusOverMesh(): about to call mesh.sendBroadcast(str) with str = ");Serial.println(str);
  myMesh.sendBroadcast(str);   // MESH SENDER
}

String createMeshMessage(const char* myStatus) {
  Serial.printf("MESH: createMeshMessage(const char* myStatus) starting with myStatus = %s\n", myStatus);
  boxTypeSelfUpdate();
  IPAddress stationIP = myMesh.getStationIP();
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();
  msg["senderNodeName"] = nodeNameBuilder(I_NODE_NAME, nodeNameBuf);
  msg["senderNodeId"] = box[I_NODE_NAME - BOXES_I_PREFIX].nodeId;
  msg["senderAPIP"] = (box[I_NODE_NAME - BOXES_I_PREFIX].APIP).toString();
  msg["senderStationIP"] = (box[I_NODE_NAME - BOXES_I_PREFIX].stationIP).toString();
  msg["senderStatus"] = myStatus;
  String str;
  msg.printTo(str);
  Serial.print("MESH: CreateMeshJsonMessage() done. Is going to return String str = ");Serial.println(str);
  return str;
}
