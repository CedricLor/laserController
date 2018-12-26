#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <ESPmDNS.h>            //lib to the network communication
#include <WiFiUdp.h>            //lib to the network communication
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <painlessMesh.h>
#include <IPAddress.h>
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)

/*  v.4.8
    DONE:
 *  HIGH: some settings are read from non-volatile storage (loadPreferences())
 *  IN COURSE:
 *  HIGH: some settings are saved to non-volatile storage (global blinking delay, masternode, slavereaction)
 *  But some of them do not save properly (blinking delay, slavereaction). They come from the website...
 *  TO DO:
 *  HIGH: make a switch:
 *    either the slave has access to its master in the mesh network and the box listens (and reacts) to mesh messages;
 *    or the slave has no access to its master via the mesh and the box listens (and reacts) to udp messages;
 *  MIDDLE: blinking delay: paired feature --> maybe already done / Check it
 *  MIDDLE: pair - unpair proc: pass the unpairing to the slave or this is going to produce unexpected results
 *  LOW: refactor all part where String is still used to replace them with arrays of char*
 *  LOW: refactoring: get rid of repetitive code where generating html tags
 *  LOW: refactor: use TEMPLATE markers in html code
 *  LOW: refactor: store html code in PROGMEM
 *  LOW: refactor: response to response-> as shown in the ESPAsyncWebServer github page
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// KEY BOX variables /////////////////////////////////////////////////////////////////////////////////////////
const int I_NODE_NAME = 210;                                                                                                // BOX BY BOX
const int I_DEFAULT_MASTER_NODE_NAME = 205;                                                                                 // BOX BY BOX
int relayPins[] = {
  22, 21, 19, 18, 5, 17, 16, 4
};                                    // an array of pin numbers to which relays are attached                                // BOX BY BOX

int const pinCount = 8;               // the number of pins (i.e. the length of the array)                                   // BOX BY BOX

const unsigned int I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;                                                                      // BOX BY BOX

unsigned long const DEFAULT_PIN_BLINKING_INTERVAL = 10000UL;                                                                 // BOX BY BOX


bool MESH_ROOT = true;                                                                                                       // BOX BY BOX


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH variables ////////////////////////////////////////////////////////////////////////////////////////////
#define   MESH_PREFIX     "laser_boxes"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler     userScheduler;             // to control your personal task     // TO DO: No longer in use; decide what to do with scheduler
painlessMesh  myMesh;

void sendMessage();
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );         // TO DO: No longer in use; decide what to do with scheduler

// size_t destServerId = 0;

char nodeNameBuf[4];
char* nodeNameBuilder(const int _I_NODE_NAME, char _nodeNameBuf[4]) {
  String _sNodeName = String(_I_NODE_NAME);
  _sNodeName.toCharArray(_nodeNameBuf, 4);
  return _nodeNameBuf;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NETWORK variables /////////////////////////////////////////////////////////////////////////////////////////
#define   STATION_SSID     "Livebox-CF01"                                                                                   // NETWORK BY NETWORK
#define   STATION_PASSWORD "BencNiels!"                                                                                     // NETWORK BY NETWORK

const char PREFIX_AP_SSID[5] = "box_";
char myApSsidBuf[8];
char* apSsidBuilder(const int _I_NODE_NAME, char _apSsidBuf[8]) {
  strcat(_apSsidBuf, PREFIX_AP_SSID);
  char _nodeName[4];
  itoa(I_NODE_NAME, _nodeName, 10);
  strcat(_apSsidBuf, _nodeName);
  return _apSsidBuf;
}

const int SECOND_BYTE_LOCAL_NETWORK = 1;                                                                                  // NETWORK BY NETWORK
IPAddress MY_STA_IP(192, 168, SECOND_BYTE_LOCAL_NETWORK, I_NODE_NAME); // the desired IP Address for the station          // NETWORK BY NETWORK
IPAddress gateway(192, 168, SECOND_BYTE_LOCAL_NETWORK, 1); // set gateway to match your network                           // NETWORK BY NETWORK
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network                                              // NETWORK BY NETWORK



IPAddress MY_AP_IP(192, 168, 4, I_NODE_NAME); // the desired IP Address for the access point // BOX BY BOX


///////////////////////////////////////////////////////
// UDP variables (to communicate orders to the other boxes) ///////////////////////////////////////////
const char * udpAddressOfTarget = "192.168.1.255";  // broadcast address or adress of a specific host                       // NETWORK BY NETWORK
const int udpPort = 3333;

const int UDP_TX_PACKET_MAX_SIZE = 50;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "ok";                  // a string to send back

const int UDP_RX_PACKET_MAX_SIZE = 5;       //
char udpMessageBuf[UDP_RX_PACKET_MAX_SIZE]; // buffer to hold outgoing packet

boolean connected = false;                  // Are we currently connected?

WiFiUDP udp;                                // The udp library class

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES FOR REACTION TO NETWORK REQUESTS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables for reaction in meshController ///////////////////////////////////////////
// const int I_DEFAULT_MASTER_NODE_NAME = 202;            // See BOX KEY VARIABLES                                        // BOX BY BOX
int iMasterNodeName = I_DEFAULT_MASTER_NODE_NAME;
const int I_MASTER_NODE_PREFIX = 200;                                                                                     // NETWORK BY NETWORK
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables for reaction in udpController ///////////////////////////////////////////
IPAddress masterIp(192, 168, SECOND_BYTE_LOCAL_NETWORK, iMasterNodeName);       // Ip of the box to serve as master to this one in udp communication
const char* slaveReaction[4] = {"opposed: on - off & off - on", "synchronous: on - on & off - off", "always on: off - on & on - on", "always off: on - off & off - off"};
const char* slaveReactionHtml[4] = {"opp", "syn", "aon", "aof"};

// I_DEFAULT_SLAVE_ON_OFF_REACTION
// I_DEFAULT_SLAVE_ON_OFF_REACTION is: this box is opposed to its master (when the master is on, this box is off)
// const int I_DEFAULT_SLAVE_ON_OFF_REACTION = 0;               // See BOX KEY VARIABLES                                        // BOX BY BOX

unsigned int iSlaveOnOffReaction = I_DEFAULT_SLAVE_ON_OFF_REACTION;       // saves the index in the B_A_SLAVE_ON_OFF_REACTIONS bool char of the choosen reaction to the master states
const bool B_A_SLAVE_ON_OFF_REACTIONS[4][2] = {{HIGH, LOW}, {LOW, HIGH}, {HIGH, HIGH}, {LOW, LOW}};
// HIGH, LOW = reaction if master is on = HIGH; reaction if master is off = LOW;  // Synchronous: When the master box is on, turn me on AND when the master box is off, turn me off
// LOW, HIGH = reaction if master is on = LOW; reaction if master is off = HIGH;  // Opposed: When the master box is on, turn me off AND when the master box is off, turn me on
// HIGH, HIGH = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always off: When the master box is on, turn me off AND when the master box is off, turn me off
// LOW, LOW = reaction if master is on = HIGH; reaction if master is off = HIGH; // Always on: When the master box is on, turn me off AND when the master box is off, turn me off
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// RELAYS variables /////////////////////////////
struct pin_type {
  int number;        // pin number to which the relays are attached
  bool on_off;       // is the pin HIGH or LOW (LOW = the relay is closed, HIGH = the relay is open)
  bool on_off_target;// a variable to store the on / off change requests by the various functions
  bool blinking;     // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
  unsigned long previous_time;
  int blinking_interval;
  int pir_state;     // HIGH or LOW: HIGH -> controlled by the PIR
  int paired;        // a variable to store with which other pin is paired (8 means it is not paired)
};

// int relayPins[] = {
//   22, 21, 19, 18, 5, 17, 16, 4
// };                                    // an array of pin numbers to which relays are attached   // See BOX KEY VARIABLES     // BOX BY BOX
//
// int const pinCount = 8;               // the number of pins (i.e. the length of the array)      // See BOX KEY VARIABLES     // BOX BY BOX

bool const default_pin_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
bool const default_pin_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
bool const default_pin_blinking_state = false;       // by default, pin starts as in a blinking-cycle TO ANALYSE
// unsigned long const default_pin_blinking_interval = 10000UL;   // default blinking interval of the pin is 10 s .   // See BOX KEY VARIABLES
unsigned long pinBlinkingInterval = default_pin_blinking_interval;
int const default_pin_pir_state_value = HIGH;       // by default, the pin is controlled by the PIR
// intialize the structs as a global variable
pin_type pin[pinCount];
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// AutoSwitch variables /////////////////////////////
long auto_switch_interval = 60000;
long auto_switch_start_time;
bool autoSwitchCycle = false;
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// PIR variables /////////////////////////////
int inputPin = 23;                // choose the input pin (for PIR sensor)
                                  // we start assuming no motion detected
int valPir = LOW;                 // variable for reading the pin status
bool isPirCycleOn = false;        // variable to store whether a PIR cycle has started
unsigned long pirPreviousTime = 0UL;         // variable that set the starting time of PIR cycle
const unsigned long pirInterval = 60000UL;   // interval of the PIR cycle
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// after being started, the Pir values shall not be read for the next 60 seconds, as the PIR is likely to send equivoqual values
unsigned long pirStartUpTime;                   // This var will receive a value when the Pir pin will be set as input pin in the setup() function.
const unsigned long pirStartUpDelay = 60000UL;  // This var stores the delay for which the Pir value shall not be read
bool pirStartedUp = false;           // Once the Pir timer will be expired, we will turn this var to true. It will be evaluated at each main loop, in the pirCntrl, to check whether to execute.

const unsigned long ONE_TICK = 1000UL;
const unsigned long OFF_TICKING_INTERVAL = ONE_TICK * 10;
const unsigned long BASE_TICKING_INTERVAL = ONE_TICK;
const unsigned long ON_TICKING_INTERVAL = ONE_TICK;
unsigned long last_pir_start_up_base_tick = 0UL;
unsigned long last_pir_start_up_off_tick = 0UL;
unsigned long last_pir_start_up_on_tick = 0UL;
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Web server variables //////////////////////
AsyncWebServer asyncServer(80);
char linebuf[80];
int charcount=0;
//////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setup() {
  delay(2000);
  //Initialize SERIAL and wait for port to open:
  serialInit();

  //Serial.print("SETUP: buf");Serial.println(buf);
  // read saved settings from non volatil storage and change the variables to match the saved settings
  loadPreferences();

  // initialize the structs representing the relay pins as output:
  initStructs();

  // initialize PIR pin as input:
  initPir();

  // configure the soft ap
  // softApConfig();

  // connect to the network
  // networkConfig();

  // initializing mesh network
  meshSetup();

  // start the AsyncWebServer
  startAsyncServer();

  // OTA configuration
  OTAConfig();

  Serial.println("-----------------------------------------------");
  Serial.println("-------- SETUP DONE ---------------------------");
  Serial.println("-----------------------------------------------");
  Serial.println();
}

void loop() {
  ArduinoOTA.handle();
  pirCntrl();
  //++ udpReceiver();          // TO BE MODIFIED FOR MESH --> meshReceiver(); or equivalent in mesh
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

void pirCntrl() {
  // reading the pir value and reacting to it is subject to a 60 seconds delay after pir startup to avoid equivoqual values returned by the PIR
  // Upon the delay being expired, the pirStartedUp variable will be set to true.
  // Serial.println("In pirCntrl()");
  // Serial.print("pirStartedUp is currently set to ");Serial.println(pirStartedUp);
  if (pirStartedUp == false) {                     // Accordingly, if this variable is false, the PIR startup delay has not expired
    // Serial.println("PIR not yet started");
    pirStartupTimer();                            // check the pirStartupTimer
  } else {
    setPirValue();
    startOrRestartPirCycleIfValPirIsHigh();
    stopPirCycleOncePirTimeIsDue();
    // Serial.println("------ Getting out of pirCntrl -------");
    // Serial.println("---------------------------------------");
  }
}

// after being started, the Pir values shall not be read for the next 60 seconds, as the PIR is likely to send equivoqual values
void pirStartupTimer() {
  //Serial.println("Checking PIR startup timer");
  const unsigned long pirStartUpTimerCurrentTime = millis();                   // get the current time and save it into the variable pirStartUpTimerCurrentTime
  const unsigned long pirCurrentStartUpDelay = pirStartUpTimerCurrentTime - pirStartUpTime;    // evaluate the current delay for which the Pir has been warming up
  if (pirCurrentStartUpDelay < pirStartUpDelay) {                              // check if the Pir Startup Timer has elapsed
    tellHumanUser(pirStartUpTimerCurrentTime);                                    // tell human being by briefly blinking the lasers during Pir warming up
  } else {                                                                     // else, this is the case where the pir startup timer has elapsed
    pirStartedUp = true;                                                       // turn the pirStartedUpVariable to true
    last_pir_start_up_base_tick = 0UL;                                           // reset the last_pir_start_up ticks variable to 0UL (freeing memory)
    last_pir_start_up_off_tick = 0UL;
    last_pir_start_up_on_tick = 0UL;
  }
}

void tellHumanUser(const unsigned long pirStartUpTimerCurrentTime) {
  if ((pirStartUpTimerCurrentTime - last_pir_start_up_base_tick) >= BASE_TICKING_INTERVAL) {     // every second
    Serial.print("-");                                                    // print a "-" to the console
    last_pir_start_up_base_tick += BASE_TICKING_INTERVAL;                 // increment the ticking counter by one tick (one second)
    directEvenPinsSwitch(HIGH);                                            // turn off the laser for one second
    // Serial.print("tellHumanUser: last_pir_start_up_base_tick = ");Serial.println(last_pir_start_up_base_tick);
    if ((pirStartUpTimerCurrentTime - last_pir_start_up_off_tick) >= OFF_TICKING_INTERVAL) {  // every 10 seconds
      last_pir_start_up_off_tick += OFF_TICKING_INTERVAL;                 // increment the ticking counter by the OFF_TICKING_INTERVAL tick (one second)
      Serial.print("+");                                                  // print a "+" to the console
      directEvenPinsSwitch(LOW);                                          // blink the laser for one second
    }
  }
}

void setPirValue() {
  valPir = digitalRead(inputPin); // read input value from the pin connected to the IR. If IR has detected motion, digitalRead(inputPin) will be HIGH.  Serial.println(valPir);
}

void startOrRestartPirCycleIfValPirIsHigh() {
  // Serial.println("------ Motion test: -------");
  if (valPir == HIGH) {                                                                              // if the PIR sensor has sensed a motion,
    startOrRestartPirCycle();
  }
  // Serial.println("------ Reseting valPir to LOW -------");
  valPir = LOW;                                                                                      // Reset the ValPir to low (no motion detected)
}

void stopPirCycleOncePirTimeIsDue() {
  if (isPirCycleOn == true) {
    // Serial.println("------ A PIR cycle is on. Checking whether PIR cycle timer is due -------");
    const unsigned long currentTime = millis();
    if (currentTime - pirPreviousTime > pirInterval) {                                      // if the PIR cycle is on and the PIR delay has expired
      Serial.println("PIR: stopPirCycleOncePirTimeIsDue(): PIR time is due. Ending PIR Cycle -------");
      stopPirCycle();
    } // else {
      // Serial.println("------ PIR cycle timer is not due yet -------");
    // }
  } // else {
    // Serial.println("------ PIR cycle is not on -------");
  // }
}

void stopPirCycle() {
  Serial.println("PIR: stopPirCycle(): stopping PIR cycle.");
  isPirCycleOn = false;                                   // turn the PIR cycle variable OFF
  switchPirRelays(HIGH);                                  // turn all the PIR controlled relays off
  pirPreviousTime = millis();                             // save the current time to let the PINs cool off  // NOT SURE THIS IS REQUIRED
  broadcastPirStatus("off");                              // broadcast current pir status
}

void startOrRestartPirCycle() {
  Serial.println("PIR: startOrRestartPirCycle(): Motion detected!!!");
  pirPreviousTime = millis(); // TO DO: Prepare a web control that allows to block resseting       // reset the starting time of pir cycle at now: this means that it extends the current pirCycle.
  startNewPirCycleIfPirCycleIsCurrrentlyOff();                                                     // evaluate if a Pir cycle is on and start one if necessary
}

void startNewPirCycleIfPirCycleIsCurrrentlyOff() {
  if (isPirCycleOn == false) {                                  // if the PIR cycle is off, turn it on
    startNewPirCycle();                                         // switch the relays which are under PIR control to LOW (i.e. light them up) and mark that a pir cycle has started in isPirCycleOn
  } else {                                                      // else do nothing
    // Serial.println("++++++ PIR cycle is already ON ++++++");
  }
}

// Start a new pir cycle
void startNewPirCycle() {
  // Serial.println("++++++ Starting new PIR cycle ++++++");
  switchPirRelays(LOW);                                       // light up all the PIR controlled relays
  // Serial.println("++++++ PIR controlled pin structs all turned to LOW ++++++");
  // Serial.println("++++++ Turning isPirCycleOn to true ++++++");
  isPirCycleOn = true;                                        // in any case, mark that the Pir Cycle is on.

  Serial.println("------ Will now try to inform other Arduino by sending it a request -------");
  /*
    TO DO: Currently, the broadcast occurs each time the ValPir has received a mvmt. This is convenient as the receiver can decide to to upon receiving this signal.
    Maybe it should broadcast only when a new Pir cycle starts.
    If I prepare a web control that allows to block resseting of the timer, I should probably also only broadcast when a new PirCycle starts and not when it is extended
  */
  broadcastPirStatus("on");                                                                        // broadcast startup of a new pir Cycle
  // Serial.println("------ PirCycle has been turned on -------");
}

// loop over each of the structs representing pins to turn them on or off (if they are controlled by the PIR)
void switchPirRelays(const int state) {
  Serial.println("------ inside switchPirRelays -------");
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    if (pin[thisPin].pir_state == HIGH) {
    Serial.print("------ pin ");Serial.print(thisPin + 1);Serial.println(" is under pir control -------");
    // only act if the pin is controlled by the PIR
      switchOnOffVariables(thisPin, state);
    }
  }
  Serial.println("------ leaving switchPirRelays -------");
}

void directEvenPinsSwitch(const int targetState) {              // targetState is HIGH or LOW (HIGH to switch off, LOW to switch on)
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {        // loop around all the structs representing the pins controlling the relays
    if (thisPin % 2 == 0) {                                     // blink only the pins represented by an even struct
      digitalWrite(pin[thisPin].number, targetState);           // switch on or off
    }
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
    if (_p2->value() == "9") {
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
  myResponse += MY_STA_IP.toString();
  myResponse += "</h1>";
  myResponse += printAllLasersCntrl();
  myResponse += printIndivLaserCntrls();
  myResponse += "</body></html>";
  Serial.println(myResponse);
  return myResponse;
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
  for (int i = 1; i < 11; i++) {
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
  for (int i = 0; i < 4; i++) {
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
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
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

String printCurrentStatus(const int thisPin) {
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

String printOnOffControl(const int thisPin) {
  String onOffCntrl;
  onOffCntrl += "<a href=\"?manualStatus=on&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>ON</button></a>&nbsp;<a href=\"?manualStatus=of&laser=";
  onOffCntrl += thisPin + 1;
  onOffCntrl += "\"><button>OFF</button></a>";
  return onOffCntrl;
}

String printPirStatusCntrl(const int thisPin) {
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

String printBlinkingDelayWebCntrl(const int thisPin) {
  String blinkingDelayWebCntrl;
  blinkingDelayWebCntrl += "Blinking delay: ";
  blinkingDelayWebCntrl += "<form style=\"display: inline;\" method=\"get\" action=\"\">";
  blinkingDelayWebCntrl += printDelaySelect(thisPin);
  blinkingDelayWebCntrl += printHiddenLaserNumb(thisPin);
  blinkingDelayWebCntrl += "<button type=\"submit\">Submit</button>";
  blinkingDelayWebCntrl += "</form>";
  return blinkingDelayWebCntrl;
}

String printPairingCntrl(const int thisPin) {
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

String printDelaySelect(const int thisPin) {
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

String printHiddenLaserNumb(const int thisPin) {
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
void switchPointerBlinkCycleState(const int thisPin, const int state) {
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
void switchAllRelays(const int state) {
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    manualSwitchOneRelay(thisPin, state);
  }
}

// Manually switches a single laser
void manualSwitchOneRelay(const int thisPin, const int targetState) {
  Serial.print("+*+*+  switching relay ");
  Serial.print(thisPin);
  Serial.println(targetState == 0 ? ": on" : ": off");
  switchOnOffVariables(thisPin, targetState);
  pin[thisPin].pir_state = LOW;
}

void switchOnOffVariables(const int thisPin, const int targetState) {
  Serial.print("+*+*+ switching on/off variables for pin[");Serial.print(thisPin + 1);Serial.print("]");
  Serial.println("+*+*+ targetState is " + targetState == 0 ? "on (LOW)" : "off (HIGH).");
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
void inclExclAllRelaysInPir(const int state) {
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    pin[thisPin].pir_state = state;
  }
}

// When clicking on the "On" or "Off" button on the webpage in the PIR column,
// this function subjects one relay to or releases it from the control of the PIR
void inclExclOneRelayInPir(const int thisPin, const int state) {     // state may be HIGH or LOW. HIGH means that the pin will be under the PIR control. LOW releases it from the PIR control.
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
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    Serial.println("WEB CONTROLLER: Changing pin blinking delay");
    changeTheBlinkingIntervalInTheStruct(thisPin, blinkingDelay);
    pinBlinkingInterval = blinkingDelay;
    savePreferences();
  }
}

void changeIndividualBlinkingDelay(const int pinNumberFromGetRequest, const unsigned long blinkingDelay) {
  changeTheBlinkingIntervalInTheStruct(pinNumberFromGetRequest, blinkingDelay);
}

void changeTheBlinkingIntervalInTheStruct(const int thisPin, const unsigned long blinkingDelay) {
  pin[thisPin].blinking_interval = blinkingDelay;
  Serial.print("WEB CONTROLLER: changeTheBlinkingIntervalInTheStruct(const int thisPin): This pin's blinking delay is now: ");Serial.println(pin[thisPin].blinking_interval);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHANGE MASTER BOX Control
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TO REDRAFT AND TO ADD COMMENTS TO THE CODE OF THE WHOLE BLOCK
void changeGlobalMasterBoxAndSlaveReaction(const int masterBoxNumber, const char* action) {
  changeTheMasterBoxId(masterBoxNumber);
  changeSlaveReaction(action);
  savePreferences();
}

void changeTheMasterBoxId(const int masterBoxNumber) {
  Serial.print("WEB CONTROLLER: changeTheMasterBoxId(const int masterBoxNumber): Starting with masterBoxNumber = ");Serial.println(masterBoxNumber);
  // UDP/IP processing
  Serial.println("WEB CONTROLLER: changeTheMasterBoxId(const int masterBoxNumber): Starting UDP-IP processing");
  byte _masterBoxIp[4] = {192, 168, SECOND_BYTE_LOCAL_NETWORK, I_MASTER_NODE_PREFIX + masterBoxNumber};
  masterIp = _masterBoxIp;                             // allocate the value of local variable masterBoxIp to the public variable masterIp
  Serial.print("WEB CONTROLLER: changeTheMasterBoxId(const int masterBoxNumber): UDP-IP processing done. masterIp changed to ");Serial.println(masterIp);

  // Mesh processing
  Serial.println("WEB CONTROLLER: changeTheMasterBoxId(const int masterBoxNumber): Starting Mesh processing");
  iMasterNodeName = I_MASTER_NODE_PREFIX + masterBoxNumber;
  Serial.println("WEB CONTROLLER: changeTheMasterBoxId(const int masterBoxNumber): Mesh processing done. masterNodeName changed to ");
  Serial.println("WEB CONTROLLER: changeTheMasterBoxId(const int masterBoxNumber): Done");
}

void savePreferences() {
  Serial.println("PREFERENCES: savePreferences(): starting");
  Preferences preferences;
  preferences.begin("savedSettingsNS", false);        // Open Preferences with savedSettingsNS namespace. Open storage in RW-mode (second parameter has to be false).

  preferences.putUInt("savedSettings", preferences.getUInt("savedSettings", 0) + 1);

  Serial.println("PREFERENCES: savePreferences(). Saving iSlaveOnOffReaction to NVS");
  preferences.putUInt("iSlavOnOffReac", iSlaveOnOffReaction);
  Serial.print("PREFERENCES: savePreferences(). iSlaveOnOffReaction saved as: ");Serial.println(iSlaveOnOffReaction);

  Serial.println("PREFERENCES: savePreferences(). Saving iMasterNodeName to NVS");
  preferences.putUInt("iMasterNName", iMasterNodeName);
  Serial.print("PREFERENCES: savePreferences(). iMasterNodeName saved as: ");Serial.println(iMasterNodeName);

  Serial.println("PREFERENCES: savePreferences(). Saving pinBlinkingInterval to NVS");
  preferences.putULong("pinBlinkInt", pinBlinkingInterval);
  Serial.print("PREFERENCES: savePreferences(). pinBlinkingInterval saved as: ");Serial.println(pinBlinkingInterval);

  preferences.end();
  Serial.println("WEB CONTROLLER: savePreferences(): done");
}

void changeSlaveReaction(const char* action) {
  Serial.print("WEB CONTROLLER: changeSlaveReaction(char *action): starting with action (char argument) =");Serial.println(action);
  int i;
  for (i=0; i < 4; i++) {
    Serial.println("WEB CONTROLLER: changeSlaveReaction(): looping over the slaveReactionHtml[] array");
    if (strcmp(slaveReactionHtml[i], action) > 0) {
      Serial.println("WEB CONTROLLER: changeSlaveReaction(): saving iSlaveOnOffReaction");
      unsigned int t = i;
      iSlaveOnOffReaction = t;
      break; // break for
    }
  }
  Serial.println("WEB CONTROLLER: changeSlaveReaction(): done");
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
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    blinkLaserIfBlinking(thisPin);                          // check if laser is in blinking cycle and check whether the blinking interval has elapsed
    ifPairedUpdateOnOff(thisPin);                           // update the on/off status of any paired laser and its paired companion
    executeUpdates(thisPin);                                // transform the update to the struct to analogical updates in the status of the related pin
  }
}

void blinkLaserIfBlinking(const int thisPin) {
  if (pin[thisPin].blinking == true && (pin[thisPin].paired == 8 || thisPin % 2 == 0)) {          // check if the laser is in blinking mode and is either non-paired or master in a pair
    blinkLaserIfTimeIsDue(thisPin);                                                               // if so, switch its on/off state
  }
}

void blinkLaserIfTimeIsDue(const int thisPin) {
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

void ifPairedUpdateOnOff(const int thisPin) {
  // Serial.println("SAFETY LOOP: ifPairedUpdateOnOff(): starting");
  if (!(pin[thisPin].paired == 8) && (thisPin % 2 == 0)) {                  // if the laser is not unpaired (if paired is set at 8, it means it is not paired)
                                                                            // AND if it is a master
    // Serial.print("xxxxxxxxx paired pin[");Serial.print(thisPin + 1);Serial.println("] is paired xxxxxxxxx");
    // Serial.println("xxxxxxxxx calling evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate for this pin xxxxxxxxx");
    evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate(thisPin);
  }
}

void evalIfIsNotBlinkingAndIsDueToTurnOffToSetUpdate(const int thisPin) {
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

void updatePairedSlave(const int thisPin, const bool nextPinOnOffTarget) {
  pin[thisPin + 1].on_off_target = nextPinOnOffTarget;                          // update the on_off target of the paired slave laser
  pin[thisPin + 1].blinking = pin[thisPin].blinking;                            // update the blinking state of the paired slave laser
  pin[thisPin + 1].pir_state = pin[thisPin].pir_state;                          // update the IR state of the paired slave laser
  pin[thisPin + 1].paired = thisPin;                                            // update the paired pin of the slave to this pin
}

void executeUpdates(const int thisPin) {
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
// RECEIVING UDP PACKETS
void udpReceiver() {
  Serial.println("UDP RECEIVER: UDPReceiver() starting -_- -_-");
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("UDP RECEIVER: UDPReceiver(): received UDP packet of size ");Serial.print(packetSize);Serial.print(" from ");Serial.print(udp.remoteIP());Serial.print(", port ");Serial.println(udp.remotePort());
    // read the packet into packetBufffer
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    Serial.print("UDP RECEIVER: UDPReceiver(): Content of udp packet: ");Serial.println(packetBuffer);

    udpController(udp.remoteIP(), packetBuffer);

    // send a reply to the IP address and port that sent us the packet we received
    // udp.beginPacket(udp.remoteIP(), udp.remotePort());
    // udp.write(ReplyBuffer);
    // udp.endPacket();
  }
}

void udpController(IPAddress udpRemoteIP, char * packetBuffer) {
  // 1. Serial print something like "192.168.1.202 is on"
  // 2. React depending on the on or off status of the remote
  if (!(udpRemoteIP == masterIp)) {                 // do not react to broadcast message if message not sent by relevant sender
    return;
  }
  if (strstr(packetBuffer, "on")  > 0) {            // if packetBuffer contains "on", it means that the master box (the UDP sender) is turned on
    autoSwitchAllRelaysUdpWrapper("on. ", B_A_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0], udpRemoteIP);  //
                                                                                                  // First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
                                                                                                  // First index number has already be replaced by the iSlaveOnOffReaction variable, which is set either:
                                                                                                  // - at startup in the global variables definition;
                                                                                                  // - via the changeSlaveReaction sub.
                                                                                                  // Second index number is the reaction to the on state of the master box if 1, to the off state if 2
  } else {                                          // if packetBuffer contains "off", it means that the master box (the UDP sender) is turned off
    // Serial.print("off. ");
    // autoSwitchAllRelaysWrapper("off. ", off_reaction, udpRemoteIP);
    autoSwitchAllRelaysUdpWrapper("off. ", B_A_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1], udpRemoteIP);  //
                                                                                                  // First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
                                                                                                  // First index number has already be replaced by the iSlaveOnOffReaction variable, which is set either:
                                                                                                  // - at startup in the global variables definition;
                                                                                                  // - via the changeSlaveReaction sub.
                                                                                                  // Second index number is the reaction to the on state of the master box if 1, to the off state if 2
  }
}

void autoSwitchAllRelaysUdpWrapper(const char* masterState, const bool reaction, IPAddress udpRemoteIP) {
  // print to console a sentence such as "192.168.1.202 is on. Turning myself to on."
  // then call the autoSwitchAllRelays
  Serial.println(udpRemoteIP);
  subAutoSwitchRelaysMsg(masterState, reaction);
  autoSwitchAllRelays(reaction);
}

void subAutoSwitchRelaysMsg(const char* masterState, const bool reaction) {
  Serial.print(" is ");                                  // Serial print the word " is "
  Serial.print(masterState);
  Serial.print("Turning myself to ");
  Serial.println(reaction == LOW ? "on." : "off.");
}

// AUTO-SWITCHES UPON REQUEST FROM OTHER BOX
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void autoSwitchAllRelays(const int targetState) {
  auto_switch_start_time = millis();                                  // start the timer of the auto switch upon starting the autoSwitch
  switchAllRelays(targetState);                                       // put the pin in manual mode, with the desired state
  autoSwitchCycle = true;                                             // Mark that the autoSwitch cycle has started. Necessary to deactivate other functions.
  Serial.println("-------- Auto Switch cycle started............ --------");
}

void autoSwitchTimer() {
  if (autoSwitchCycle == false) {                                     // Do not execute the autoSwitchTimer if no autoSwitchCycle has started
    return;
  }
  // Serial.println("      SLAVE MODE: autoSwitchTimer(): Auto switch is on (I am the slave of another box). Checking the autoSwitch timer.");
  long currentTime = millis();
  if (currentTime - auto_switch_start_time > auto_switch_interval) {  // if the autoSwitch time interval has elapsed
    Serial.println("      SLAVE MODE: autoSwitchTimer(): Auto Switch Timer has expired. I am released of my slavery.");
    autoSwitchCycle = false;                                          // Mark that the autoSwitch cycle has started. Necessary to deactivate other functions.
    inclExclAllRelaysInPir(HIGH);                                     // IN PRINCIPLE, RESTORE ITS PREVIOUS STATE. CURRENTLY: Will include all the relays in PIR mode
    Serial.println("      SLAVE MODE: autoSwitchTimer(): -------- Auto Switch cycle ended............ --------");
  }
}
// Function checked dans une situation où autoSwitchCycle == false.

void autoSwitchOneRelay(const int thisPin, const int targetState) {
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
// Broadcasts over UDP and mesh
void broadcastPirStatus(const char* state) {     // state is "on" or "off". When valPir is HIGH (the IR has detected a move),
                                                  // the Pir block calls this function with the "on" parameter. Alternatively,
                                                  //  when the the pir cycle stops, it calls this function with the "off" parameter.
  Serial.print("PIR - broadcastPirStatus(const char* state) starting with state = ");Serial.println(state);

  // UDP broadcast
  Serial.println("PIR - broadcastPirStatus(): Broadcasting status over UDP");
  if (connected) {
    Serial.println("PIR - broadcastPirStatus(): We are connected. Sending UDP packet.");
    udp.beginPacket(udpAddressOfTarget,udpPort);
    udp.printf(state);
    udp.endPacket();
    Serial.println("PIR - broadcastPirStatus(): UDP packet sent.");
  } else {
    Serial.println("PIR - broadcastPirStatus(): Not connected. UDP packet not sent.");
  }

  Serial.print("PIR - broadcastPirStatus(): broadcasting status over Mesh via call to broadcastStatusOverMesh(state) with state = ");Serial.println(state);
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
  Serial.println("SETUP: initStructs(): starting");
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    // Initialize structs
    initStruct(thisPin);
    pinMode(pin[thisPin].number, OUTPUT);     // initialization of the pin connected to each of the relay as output
    digitalWrite(pin[thisPin].number, HIGH);  // setting default value of the pins at HIGH (relay closed)
  }
  Serial.println("SETUP: initStructs(): done");
}

void initStruct(int thisPin) {
  /*
  Intializing each of the structs representing the pins.
  Recall: struct's structure
  struct pin_type {
    int number;        // pin number to which the relays are attached
    bool on_off;       // is the pin HIGH or LOW (LOW = the relay is closed, HIGH = the relay is open)
    bool on_off_target;// a variable to store the on / off change requests by the various functions
    bool blinking;     // is the pin in a blinking cycle (true = the pin is in a blinking cycle, false = the pin is not in a blinking cycle)
    unsigned long previous_time;
    int blinking_interval;
    int pir_state;     // HIGH or LOW: HIGH -> controlled by the PIR
    int paired;        // a variable to store with which other pin is paired (8 means it is not paired)
  };
  Default variable for the structs:
  bool const default_pin_on_off_state = HIGH;         // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
  bool const default_pin_on_off_target_state = HIGH; // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
  bool const default_pin_blinking_state = false;       // by default, pin starts as in a blinking-cycle TO ANALYSE
  long const default_pin_blinking_interval = 10000;   // default blinking interval of the pin is 10 s
  int const default_pin_pir_state_value = HIGH;       // by default, the pin is controlled by the PIR
  */


  pin[thisPin] = {
    relayPins[thisPin],
    default_pin_on_off_state,        // by default, the pin starts as HIGH (the relays is off and laser also) TO ANALYSE: THIS IS WHAT MAKES THE CLICK-CLICK AT STARTUP
    default_pin_on_off_target_state, // by default, the pin starts as not having received any request to change its state from a function TO ANALYSE: THIS IS WHAT MAKES THIS CLICK-CLICK AT START UP
    default_pin_blinking_state,      // by default, pin starts as in a blinking-cycle TO ANALYSE
    millis(),
    pin_blinking_interval,            // default blinking interval of the pin is 10 s, unless some settings have been saved to EEPROM
    default_pin_pir_state_value,     // by default, the pin is controlled by the PIR
    (thisPin % 2 == 0) ? (thisPin + 1) : (thisPin - 1)  // by default, the pins are paired
  };
}

void initPir() {
  Serial.println("SETUP: initPir(): starting");
  pinMode(inputPin, INPUT);                  // declare sensor as input
  pirStartUpTime = millis();
  Serial.println("SETUP: initPir(): done");
}

void softApConfig() {
  Serial.println("SETUP: softApConfig(): starting");
  // esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B| WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR);
  // Set the AP name
  WiFi.softAP(apSsidBuilder(I_NODE_NAME, myApSsidBuf));
  WiFi.softAPConfig(MY_AP_IP, gateway, subnet);
  Serial.println("SETUP: softApConfig(): done");
}

void networkConfig() {
  Serial.println("SETUP: networkConfig(): starting");
  WiFi.config(MY_STA_IP, gateway, subnet);
  // WiFi.onEvent(WiFiEvent);                          // Inserted for UDP
  Serial.println("SETUP: networkConfig(): done");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
  switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        //When connected set
        Serial.print("SETUP: WifiEvent(WiFiEvent_t event): WiFi connected! IP address: ");
        Serial.println(WiFi.localIP());
        //initializes the UDP state
        //This initializes the transfer buffer
        udp.begin(WiFi.localIP(),udpPort);
        connected = true;
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        connected = false;
        break;
  }
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
  ArduinoOTA.onProgress( progressOTA ); //progressOTA is a function created to simplificate the code
  ArduinoOTA.onError( errorOTA );//errorOTA is a function created to simplificate the code
  ArduinoOTA.begin();

  //prints the ip address used by ESP
  Serial.println("SETUP: OTA Ready");
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
  int i;
  for(i=0;i<headers;i++){
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

  asyncServer.onNotFound(onRequest);
  asyncServer.onRequestBody(onBody);

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
  Serial.printf("   MESH CALLBACK: receivedCallback(): Received from %u msg=%s\n", from, msg.c_str());
  meshController(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n",myMesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", myMesh.getNodeTime(),offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}

void meshSetup() {
  // myMesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  // myMesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  myMesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION | COMMUNICATION );  // set before init() so that you can see startup messages

  myMesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6 );

  //myMesh.stationManual(STATION_SSID, STATION_PASSWORD);
  myMesh.setHostname(apSsidBuilder(I_NODE_NAME, myApSsidBuf));
  if (MESH_ROOT == true) {
    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
    myMesh.setRoot(true);
    // This and all other mesh should ideally now the mesh contains a root
    myMesh.setContainsRoot(true);
  }

  myMesh.onReceive(&receivedCallback);
  myMesh.onNewConnection(&newConnectionCallback);
  myMesh.onChangedConnections(&changedConnectionCallback);
  myMesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  myMesh.onNodeDelayReceived(&delayReceivedCallback);                                   // Might not be needed

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void meshController(uint32_t senderNodeId, String &msg) {
  // 1. Serial print something like "192.168.1.202 is on"
  // 2. React depending on the on or off status of the remote

  Serial.printf("    MESH CONTROLLER: meshController(uint32_t senderNodeId, String &msg) starting with senderNodeId == %u and &msg == %s \n", senderNodeId, msg.c_str());
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg.c_str());
  const int iSenderNodeName = atoi(root["senderNodeName"]);
  Serial.printf("    MESH CONTROLLER: meshController(...) %u alloted to root[\"senderNodeName\"] \n", iSenderNodeName);
  const char* senderStatus = root["senderStatus"];
  Serial.printf("    MESH CONTROLLER: meshController(...) %s alloted to root[\"senderStatus\"] \n", senderStatus);
  Serial.printf("    MESH CONTROLLER: meshController(...). Now testing if iSenderNodeName == iMasterNodeName:\n");
  if (!(iSenderNodeName == iMasterNodeName)) {                 // do not react to broadcast message if message not sent by relevant sender
    Serial.printf("    MESH CONTROLLER: meshController(...). %u is not equal to %u", iSenderNodeName, iMasterNodeName);
    return;
  }
  Serial.printf("    MESH CONTROLLER: meshController(...). %u == %u\n", iSenderNodeName, iMasterNodeName);
  Serial.printf("    MESH CONTROLLER: meshController(...). Now testing if senderStatus == on or of:\n");
  if (strstr(senderStatus, "on")  > 0) {                              // if senderStatus contains "on", it means that the master box (the mesh sender) is turned on.
    Serial.printf("    MESH CONTROLLER: meshController(...). %s == \"on\"\n", senderStatus, iMasterNodeName);
    Serial.printf("    MESH CONTROLLER: meshController(...). now calling autoSwitchAllRelaysMeshWrapper with params: \"on .\", %b and %u\n", B_A_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0], iSenderNodeName);
    autoSwitchAllRelaysMeshWrapper("on. ", B_A_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][0], iSenderNodeName);  // index numbers in array B_A_SLAVE_ON_OFF_REACTIONS[]:
                                                                                                  // First index number is one of the pair of HIGH/LOW reactions listed in the first dimension of the array.
                                                                                                  // First index number has already been replaced by the iSlaveOnOffReaction variable, which is set either:
                                                                                                  // - at startup in the global variables definition;
                                                                                                  // - via the changeSlaveReaction sub.
                                                                                                  // Second index number is the reaction to the on state of the master box if 1, to the off state if 2
  } else {                                                            // if senderStatus does not contain "on", it means that the master box (the mesh sender) is turned off.
    Serial.printf("    MESH CONTROLLER: meshController(...). %s == \"of\"\n", senderStatus, iMasterNodeName);
    Serial.printf("    MESH CONTROLLER: meshController(...). now calling autoSwitchAllRelaysMeshWrapper with params: \"of .\", %b and %u\n", B_A_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1], iSenderNodeName);
    autoSwitchAllRelaysMeshWrapper("off. ", B_A_SLAVE_ON_OFF_REACTIONS[iSlaveOnOffReaction][1], iSenderNodeName);
  }
}

void autoSwitchAllRelaysMeshWrapper(const char* masterState, const bool reaction, const int iSenderNodeName) {
  // print to console a sentence such as "192.168.1.202 is on. Turning myself to on"
  // then call the autoSwitchAllRelays
  Serial.println("    MESH: autoSwitchAllRelaysMeshWrapper(masterState, reaction, iSenderNodeName) starting");
  Serial.print("    MESH: autoSwitchAllRelaysMeshWrapper(). iSenderNodeName = ");Serial.println(iSenderNodeName);    // Serial print the remote adress
  Serial.printf("    MESH: autoSwitchAllRelaysMeshWrapper().Box %u", iSenderNodeName);
  subAutoSwitchRelaysMsg(masterState, reaction);
  autoSwitchAllRelays(reaction);
  Serial.println("    MESH: autoSwitchAllRelaysMeshWrapper(): done");
}

void broadcastStatusOverMesh(const char* state) {
  Serial.print("MESH: broadcastStatusOverMesh(const char* state): starting with state = ");Serial.println(state);
  String str = createMeshMessage(state);
  Serial.print("MESH: broadcastStatusOverMesh(): about to call mesh.sendBroadcast(str) with str = ");Serial.println(str);
  myMesh.sendBroadcast(str);   // MESH SENDER
}

String createMeshMessage(const char* myStatus) {
  Serial.print("MESH: CreateMeshJsonMessage(const char* myStatus) starting with myStatus = ");Serial.println(myStatus);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& msg = jsonBuffer.createObject();
  msg["senderNodeName"] = nodeNameBuilder(I_NODE_NAME, nodeNameBuf);
  msg["senderNodeId"] = myMesh.getNodeId();
  msg["senderStatus"] = myStatus;
  String str;
  msg.printTo(str);
  Serial.print("MESH: CreateMeshJsonMessage() done. Is going to return String str = ");Serial.println(str);
  return str;
}
