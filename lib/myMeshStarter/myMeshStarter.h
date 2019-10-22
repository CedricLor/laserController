/*
  myMesh.h - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.
*/

#include "Arduino.h"


#ifndef myMeshStarter_h
#define myMeshStarter_h

#include <painlessMesh.h>
#include <ESPmDNS.h>
#include <mySavedPrefs.h>
#include <ControlerBox.h>


class myMeshStarter
{
  public:
    myMeshStarter();    // Variables for Interface on AP mode

    static Task tRestart;
    static bool hasBeenStarted;
    static bool oetcbRestart();

    void myMeshSetup();

  private:

    // setup subs
    void _initAndConfigureMesh();
    void _initMesh();
    void _initStationManual();
    void _rootTheMesh();
    void _interfaceOnAPInit();
    void _setupMdns();
};

#endif

