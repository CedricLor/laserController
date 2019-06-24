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

DONE:
- rename ControlerBox::activeBoxState to ControlerBox::boxActiveState
- rename boxState::_targetActiveBoxState to boxState::_boxTargetState
- rename boxState::_activeBoxStateHasBeenReset to boxState::_boxActiveStateHasBeenReset
- rename boxState::_defaultBoxState to boxState::_boxDefaultState

TO DO:

- ControlerBox::updateOtherBoxProperties
  - to be tested with a microcontroller

- draft ControlerBox::updateOtherBoxProperties (preliminary step to looping on the ControlerBoxes array in the webserver)
  - need to add the sender boxState so that each box knows in which state are the other boxes
  - need to send via myMeshViews and add to ControlerBox the time at which the sender boxState was changed
  - need to send via myMeshViews and add to ControlerBox the time for which the new sender boxState shall apply

- ControlerBoxes[MY_INDEX_IN_CB_ARRAY].valMesh and ControlerBoxes[MY_INDEX_IN_CB_ARRAY].valPir to be changed to static variables

- ControlerBox class: I_NODE_NAME of interface box is currently set at 200; some code shall be added to avoid including the interface box into the ControlerBoxes array

- myMesh::_decodeRequest: last three lines to be seriously updated

- draft myWebServerBase::_SBoxArray() which will return a String, to be incorporated into the webpage of the interface, setting out the states of each box and state setters for each box

- optimizations:
- ControlerBox::_parseIpStringToIPAddress(JsonDocument& root, String rootKey): try to get rid of the String in the parameters;
- short/int to bool or byte
  - each time I use a short int as a bool, change it to a bool
  - each time I use a short for a very short number, change it to a byte

- move _processor and _SBoxArray to myWebServerViews
- find a better way to init ControlerBoxes[MY_INDEX_IN_CB_ARRAY].boxActiveState = 0 than in setup()
- refactor myMeshViews::statusMsg("on"); currently being called from boxState.cpp, upon enabling and disabling _tPlayBoxState
