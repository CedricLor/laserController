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
|  |  |  |--note.cpp (called to play some member functions)
|  |  |  |  |--note.h
|  |  |  |  |--global.cpp (called to retrieve some values)
|  |  |  |  |  |--global.h
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
- draft myWebServerBase::_SBoxArray() which will return a String, to be incorporated into the webpage of the interface, setting out the states of each box and state setters for each box



TO DO:
1. High priority
  A. Decode request coming from users
    - myMesh::_decodeRequest: last three lines to be seriously updated
    - review webServerControler to start decoding requests properly

  B. optimizations
  - ControlerBox::_parseIpStringToIPAddress(JsonDocument& root, String rootKey): try to get rid of the String in the parameters;
  - short/int to bool or byte
    - each time I use a short int as a bool, change it to a bool
    - each time I use a short for a very short number, change it to a byte

  C. Implement websockets
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
  - ControlerBox class: I_NODE_NAME of interface box is currently set at 200; some code shall be added to avoid including the interface box into the ControlerBoxes array
  - move _processor and _SBoxArray to myWebServerViews
  - find a better way to init ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState = 0 than in setup()
  - refactor myMeshViews::statusMsg("on"); currently being called from boxState.cpp, upon enabling and disabling _tPlayBoxState
  - rewrite the Serial.print and Serial.println to Serial.printf; don't forget https://stackoverflow.com/questions/8986625/warning-format-c-expects-type-int-but-argument-2-has-type-char
