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
