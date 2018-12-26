#include <ArduinoOTA.h>         //lib to the ArduinoOTA functions
#include <painlessMesh.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#define _TASK_PRIORITY         // TODO: Check if remove
#define _TASK_STD_FUNCTION     // TODO: Check if remove
#define _TASK_STATUS_REQUEST
#include <IPAddress.h>         // TODO: Check if remove
#include <Preferences.h>       // Provides friendly access to ESP32's Non-Volatile Storage (same as EEPROM in Arduino)

// FROM PAINLESSMESH BASIC
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
// END FROM PAINLESSMESH BASIC

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

// FROM PAINLESSMESH BASIC
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
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
    Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}
// END FROM PAINLESSMESH BASIC

void setup() {
  Serial.begin(115200);

  // FROM PAINLESSMESH BASIC
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  //mesh.setDebugMsgTypes( CONNECTION | COMMUNICATION ); // all types on
  //mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  // END FROM PAINLESSMESH BASIC
}

void loop() {
  // FROM PAINLESSMESH BASIC
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
  // END FROM PAINLESSMESH BASIC
}
