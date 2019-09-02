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
- rename ControlerBox::activeBoxState to ControlerBox::i16BoxActiveState
- rename boxState::_targetActiveBoxState to boxState::_boxTargetState
- rename boxState::_activeBoxStateHasBeenReset to boxState::_boxTargetStateHasChanged
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


TODO:
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
  - ControlerBox class: UI8_NODE_NAME of interface box is currently set at 200; some code shall be added to avoid including the interface box into the ControlerBoxes array
  - move _processor and _SBoxArray to myWebServerViews
  - find a better way to init ControlerBoxes[MY_INDEX_IN_CB_ARRAY].i16BoxActiveState = 0 than in setup()
  - refactor myMeshViews::statusMsg("on"); currently being called from boxState.cpp, upon enabling and disabling tPlayBoxState
  - rewrite the Serial.print and Serial.println to Serial.printf; don't forget https://stackoverflow.com/questions/8986625/warning-format-c-expects-type-int-but-argument-2-has-type-char


- find a way to decrease ControlerBox::connectedBoxesCount when a box gets disconnected
- add a little static array to ControlerBox to store the index numbers of ControlerBoxes of the box that are connected





///////////////////////////////////////////////
// Notes on Task Scheduler
///////////////////////////////////////////////
When calling setInterval() within the onEnable callback of the Task,
the main callback is called within the ancient delay.
The following iteration on the main callback will take place after the new delay




///////////////////////////////////////////////
// Notes on painlessMesh
///////////////////////////////////////////////
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


  API:
  - void painlessMesh::stop()
  Stop the node. This will cause the node to disconnect from all other nodes and stop/sending messages.


  - void painlessMesh::onNewConnection( &newConnectionCallback )
  This fires every time the local node makes a new connection.   The callback has the following structure.
  void newConnectionCallback( uint32_t nodeId )
  nodeId is new connected node ID in the mesh.


  - void painlessMesh::onChangedConnections( &changedConnectionsCallback )
  This fires every time there is a change in mesh topology. Callback has the following structure.
  void onChangedConnections()
  There are no parameters passed. This is a signal only.


  - bool painlessMesh::isConnected( nodeId )
  Returns if a given node is currently connected to the mesh.
  nodeId is node ID that the request refers to.


  - String painlessMesh::subConnectionJson()
  Returns mesh topology in JSON format.


  - std::list<uint32_t> painlessMesh::getNodeList()
  Get a list of all known nodes. This includes nodes that are both directly and indirectly connected to the current node.



///////////////////////////////////////////////
// Notes on std:: lib
///////////////////////////////////////////////
  ArduinoJson
    std::string
    std::ostream
    std::istream
    std::streamsize

  ESPAsyncTCP
    std::vector
    std::function

  painlessMesh
    std::function
    std::map
    std::list
    std::cout
    std::endl
    std::min
    std::shared_ptr
    std::move
    std::make_shared
    std::swap
    std::map
    std::enable_shared_from_this
    std::stringstream
    std::size_t
    std::vector
    std::pair
