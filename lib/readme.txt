Dependency structure

|--main.cpp
|  |
|  |--myWebServerBase.cpp
|  |  |--myWebServerBase.h
|  |  |  |--AsyncTCP.h
|  |  |
|  |  |--myWebServerControler.cpp ("private" class: called only from myWebServerBase.cpp)
|  |  |  |--myWebServerControler.h
|  |  |  |--MasterSlaveBox.cpp
|  |  |  |  |--MasterSlaveBox.h
|  |  |
|  |  |--myWebServerViews.cpp ("private" class: called only from myWebServerBase.cpp)
|  |  |  |--myWebServerViews.h
|  |  |  |--myWebServerViews.h
|  |  |  |--ControlerBox.cpp
|  |  |  |  |--ControlerBox.h
|  |  |  |--global.cpp
|  |  |  |  |--global.h
|  |
|  |--boxState.cpp
|  |  |--boxState.h
|  |  |--ControlerBox.cpp (called to read and set some values, in particular on this box)
|  |  |  |--ControlerBox.h
|  |  |
|  |  |--sequence.cpp
|  |  |  |--sequence.h
|  |  |  |--global.cpp (called to start some tasks and play some functions)
|  |  |  |  |--global.h
|  |  |  |
|  |  |  |--bar.cpp (an array of bars (micro-sequences of notes, each with a tempo in ms)
|  |  |  |  |--bar.h
|  |  |  |  |--note.cpp (a static class playing a note for a maximum 30 seconds)
|  |  |  |  |  |--note.h
|  |  |  |  |  |--tone.cpp (an array of tones, containing all the possible lasers on/off configurations)
|  |  |  |  |  |  |--tone.h
|  |  |  |
|  |  |  |--tone.cpp
|  |  |  |  |--tone.h
|  |  |  |  |--global.cpp (called to retrieve some values)
|  |  |  |  |  |--global.h
|  |  |  |
|  |  |--myMeshViews.cpp
|  |  |  |--myMeshViews.h
|  |
|  |--myMesh.cpp
|  |  |--myMesh.h
|  |  |
|  |  |--myMeshControler.cpp (private - called only from my Mesh)
|  |  |  |--myMeshControler.h
|  |  |  |--MasterSlaveBox.cpp
|  |  |  |  |--MasterSlaveBox.h
|  |  |  |--//LaserGroupedUnitsArray.cpp
|  |  |  |  |--//LaserGroupedUnitsArray.h
|  |  |
|  |  |--myMeshViews.cpp
|  |  |  |--myMeshViews.h
|  |  |  |--ControlerBox.cpp
|  |  |  |  |--ControlerBox.h
|  |  |  |--//LaserGroupedUnitsArray.cpp
|  |  |  |  |--//LaserGroupedUnitsArray.h


This directory is intended for project specific (private) libraries.
PlatformIO will compile them to static libraries and link them to executable files.

The source code of each library should be placed in separate directories, like
"lib/private_lib/[here are source files]".

For example, see the structure of the following two libraries `Foo` and `Bar`:
|--lib
|  |
|  |--Bar
|  |  |--docs
|  |  |--examples
|  |  |--src
|  |     |- Bar.c
|  |     |- Bar.h
|  |  |- library.json (optional, custom build options, etc) https://docs.platformio.org/page/librarymanager/config.html
|  |
|  |--Foo
|  |  |- Foo.c
|  |  |- Foo.h
|  |
|  |- readme.txt --> THIS FILE
|
|- platformio.ini
|--src
   |- main.c

Then in `src/main.c` you should use:

#include <Foo.h>
#include <Bar.h>

// rest H/C/CPP code

PlatformIO will find your libraries automatically, configure preprocessor's
include paths and build them.

More information about PlatformIO Library Dependency Finder
- https://docs.platformio.org/page/librarymanager/ldf.html

// DONE AND TODO

DONE:
- rename ControlerBox::activeBoxState to ControlerBox::boxActiveState
- rename boxState::_targetActiveBoxState to boxState::_boxTargetState
- rename boxState::_activeBoxStateHasBeenReset to boxState::_boxActiveStateHasBeenReset
- rename boxState::_defaultBoxState to boxState::_boxDefaultState
- ControlerBox::updateOtherBoxProperties
  - to be tested with a microcontroller
- ControlerBoxes[MY_INDEX_IN_CB_ARRAY].valMesh and ControlerBoxes[MY_INDEX_IN_CB_ARRAY].valPir to be changed to static variables

- ControlerBox::updateOtherBoxProperties and ControlerBox::updateThisBoxProperties
  - added the sender boxState to the JSON message sent via the mesh, so that each box knows in which state are the other boxes
  - send via myMeshViews::statusMsg() the time at which the sender boxState was changed
  - add to ControlerBox the time at which the sender boxState was changed

- move _SBoxArray to myWebServerViews
- draft myWebServerBase::_SBoxArray() which will return a String, to be incorporated into the webpage of the interface, displaying out the states of each box and state setters for each box

- myMesh::_decodeRequest: last three lines to be seriously updated
- review webServerControler to start decoding requests properly
- add additional states as drafted on the paper

- refactor the notes and sequences to get closer to a music score player

- on new connection callback (Mesh), the laser boxes send a message. As currently drafted,
when I will have 10 laser boxes, this will overflow the mesh: add speed bumper
(wait for x seconds before sending with x calculated from the index number or boxname)
-> modified: on newConnection, only the newly connected box sends broadcast all its status message. The other boxes do not do anything and just wait.

C. Implement websockets


TO DO:
- manual state communication sequence
  - what shall the manual state enable me to do?
  - webServerViews: draft the manual state page
  - webServerControler: draft the manual state receiver
  - meshViews: draft the manual state sender
  - meshController: draft the manual state receiver

- save and read data in NVS or SPIFFS

- check how to get rid of LaserPin and LaserPinsArray classes

1. High priority
  A. Decode request coming from users
    - review webServerControler to start decoding requests properly

  B. optimizations
  - ControlerBox::_parseIpStringToIPAddress(JsonDocument& root, String rootKey): try to get rid of the String in the parameters;
  - short/int to bool or byte
    - each time I use a short int as a bool, change it to a bool
    - each time I use a short for a very short number, change it to a byte

  D. Implement network sequences
  E. Implement some kind of non-volatile storage of sequences, boxStotes or network sequences

  F. Iterate and start sending more data and develop more setters
  - ControlerBox::updateOtherBoxProperties and ControlerBox::updateThisBoxProperties
    - send via myMeshViews and add to ControlerBox the time for which the new sender boxState shall apply

2. Medium priority
- Somewhere (where???):
  - before sending messages, check that the mesh exists; if it does not exist, start a Task ready to send the data when the mesh exists
  - upon connecting to the mesh, the connecting box shall send its current state (including the time at which it started)
  - upon connecting to the mesh, the other boxes shall send to the new box their current states

3. Low priority: small optimizations
  - ControlerBox class: B_NODE_NAME of interface box is currently set at 200; some code shall be added to avoid including the interface box into the ControlerBoxes array
  - move _processor and _SBoxArray to myWebServerViews
  - find a better way to init ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState = 0 than in setup()
  - refactor myMeshViews::statusMsg("on"); currently being called from boxState.cpp, upon enabling and disabling _tPlayBoxState
  - rewrite the Serial.print and Serial.println to Serial.printf; don't forget https://stackoverflow.com/questions/8986625/warning-format-c-expects-type-int-but-argument-2-has-type-char


- find a way to decrease ControlerBox::connectedBoxesCount when a box gets disconnected
- add a little static array to ControlerBox to store the index numbers of ControlerBoxes of the box that are connected

Notes on Task Scheduler
When calling setInterval() within the onEnable callback of the Task, the main callback is called within the ancient delay.
The following iteration on the main callback will take place after the new delay

Notes on painlessMesh
I. Connection and Disconnection: on the node that stays connected
  A. when disconnecting a member from the Mesh, on the node that stays connected,
  changedConnectionCallback() is triggered: laserControllerMesh.subConnectionJson() = {"nodeId":2760139053,"root":true}

  B. when connecting a new member to the Mesh, on the node that stays connected,
  1. changedConnectionCallback() is triggered; laserControllerMesh.subConnectionJson() = {"nodeId":2760139053,"root":true,"subs":[{"nodeId":764691274}]}
  2. newConnectionCallback() is then triggered; laserControllerMesh.subConnectionJson() = {"nodeId":2760139053,"root":true,"subs":[{"nodeId":764691274}]}

II: Connection and Disconnection: on the node that is newly connected
  1. Sometimes, changed connection callback is called a first time; laserControllerMesh.subConnectionJson() = {"nodeId":764691274,"subs":[{
"nodeId":2760139053}]}
  2. Changed connection callback is called a second time; laserControllerMesh.subConnectionJson() = {"nodeId":764691274}
  3. New connection callback is called; laserControllerMesh.subConnectionJson() = {"nodeId":764691274,"subs":[{
"nodeId":2760139053}]}

C. Mesh Protocol: message types
  Time Synchronization (upon connecting (and maybe, if a time desync has appeared))
  type: 4: Time Sync messages
  type: 0: Other box shall request MeshTime
  type: 1: Please send me MeshTime
  type: 2: Please receive MeshTime

  Routing Information (every 3 seconds)
  type: 5: NODE_SYNC_REQUEST
  type: 6: NODE_SYNC_REPLY

  User Messages
  type: 8: Mesh broadcast
  type: 9: Single addressed message

- DISCONNECTING TRACE:
CONNECTION: onDisconnect():
GENERAL: getNodeTime(): time=609264646
CONNECTION: onDisconnect(): dropping 764691274 now= 609264646
CONNECTION: MeshConnection::close().
CONNECTION: mesh->_scheduler.addTask():
CONNECTION: mesh->droppedConnectionTask.enable():
CONNECTION: before if (client->connected()):
CONNECTION: after if (client->connected()):
CONNECTION: after if station && WiFi.status() == WL_CONNECTED):
CONNECTION: eraseClosedConnections():
CONNECTION: ~MeshConnection():
CONNECTION: MeshConnection::close() Done.
CONNECTION: closingTask():
GENERAL: getNodeTime(): time=609300964

CONNECTION: closingTask(): dropping 764691274 now= 609300964
myMesh::changedConnectionCallback(): Changed connections {"nodeId":2760139053,"root":t
rue}
--------------------- CHANGED CONNECTION --------------------------

- CONNECTING TRACE:
- CHANGED CONNECTION STACK:
CONNECTION: New AP connection incoming
CONNECTION: meshConnectedCb(): we are AP
GENERAL: MeshConnection(): leaving
COMMUNICATION: meshRecvCb(): fromId=0
GENERAL: getNodeTime(): time=93680589
COMMUNICATION: meshRecvCb(): Recvd from 0-->{"nodeId":764691274,"type":5,"dest":0,"fro
m":764691274}<--
SYNC: handleNodeSync(): with 0
myMesh::changedConnectionCallback(): Changed connections {"nodeId":2760139053,"root":t
rue,"subs":[{"nodeId":764691274}]}
--------------------- CHANGED CONNECTION --------------------------
SYNC: syncSubConnections(): changedId = 764691274
SYNC: syncSubConnections(): in for loop
SYNC: syncSubConnections(): out of for loop
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239884

GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":0,"from
":764691274}

- CONNECTING TRACE:
- NEW CONNECTION STACK:
CONNECTION: newConnectionTask():
GENERAL: getNodeTime(): time=93781626
CONNECTION: newConnectionTask(): adding 764691274 now= 93781626
myMesh::newConnectionCallback(): New Connection, nodeId = 764691274
++++++++++++++++++++++++ NEW CONNECTION +++++++++++++++++++++++++++
GENERAL: sentBufferTask()
GENERAL: sentBufferTask()
COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=94026507
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"type":4,"dest":2760139053,"from"
:764691274,"msg":{"type":1,"t0":8852804}}<--
GENERAL: getNodeTime(): time=94033827
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239656
GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"type":4,"dest":2760139053,"from":76469127
4,"msg":{"type":1,"t0":8852804}}
GENERAL: sentBufferTask()
GENERAL: sentBufferTask()
COMMUNICATION: meshRecvCb(): fromId=764691274

GENERAL: getNodeTime(): time=94281507
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"type":4,"dest":2760139053,"from"
:764691274,"msg":{"type":1,"t0":94271188}}<--
GENERAL: getNodeTime(): time=94288970
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239660
GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"type":4,"dest":2760139053,"from":76469127
4,"msg":{"type":1,"t0":94271188}}

COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=101171102
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"nodeId":764691274,"type":5,"dest
":2760139053,"from":764691274}<--
SYNC: handleNodeSync(): with 764691274
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239828
GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":2760139
053,"from":764691274}

COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=108715089
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"nodeId":764691274,"type":5,"dest
":2760139053,"from":764691274}<--
SYNC: handleNodeSync(): with 764691274
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239828
GENERAL: sentBufferTask()

COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":2760139
053,"from":764691274}

COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=116253675
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"nodeId":764691274,"type":5,"dest
":2760139053,"from":764691274}<--
SYNC: handleNodeSync(): with 764691274
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239828
GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":2760139053,"from":764691274}

COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=123790550
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"nodeId":764691274,"type":5,"dest
":2760139053,"from":764691274}<--
SYNC: handleNodeSync(): with 764691274
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239828
GENERAL: sentBufferTask()

COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":2760139
053,"from":764691274}

COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=131333920
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"nodeId":764691274,"type":5,"dest
":2760139053,"from":764691274}<--
SYNC: handleNodeSync(): with 764691274
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239828

GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":2760139
053,"from":764691274}

COMMUNICATION: meshRecvCb(): fromId=764691274
GENERAL: getNodeTime(): time=138873884
COMMUNICATION: meshRecvCb(): Recvd from 764691274-->{"nodeId":764691274,"type":5,"dest
":2760139053,"from":764691274}<--
SYNC: handleNodeSync(): with 764691274
COMMUNICATION: addMessage(): Package sent to queue beginning -> 1 , FreeMem: 239828
GENERAL: sentBufferTask()
COMMUNICATION: writeNext(): Package sent = {"nodeId":764691274,"type":5,"dest":2760139
053,"from":764691274}
