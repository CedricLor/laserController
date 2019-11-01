/*
  myMesh.cpp - Library to handle painlessMesh core related functions.
  Created by Cedric Lor, January 17, 2019.
*/

#include "Arduino.h"
#include "myMeshStarter.h"
#include <esp_wifi.h>


bool myMeshStarter::hasBeenStarted = false;

myMeshStarter::myMeshStarter() {};


void myMeshStarter::myMeshSetup() {
  if ( globalBaseVariables.MY_DEBUG == true ) {
    // globalBaseVariables.laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP |/*MESH_STATUS |*/ CONNECTION |/* SYNC |*/ COMMUNICATION /* | GENERAL | MSG_TYPES | REMOTE */);
    globalBaseVariables.laserControllerMesh.setDebugMsgTypes( ERROR | STARTUP | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION /* | GENERAL */ | MSG_TYPES | REMOTE );
  }

  _initAndConfigureMesh();

  // Serial.println("myMesh::meshSetup(): About to call updateThisBoxProperties:");
  thisControllerBox.updateThisBoxProperties();

  _setupMdns();
}


Task myMeshStarter::tRestart(0, 1, NULL, NULL, false, &oetcbRestart, NULL);


bool myMeshStarter::oetcbRestart() {
  Serial.println("myMeshStarter::oetcbRestart()");
  return hasBeenStarted;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup Helpers

void myMeshStarter::_initAndConfigureMesh() {
  // 1. Init mesh
  _initMesh();

  // 2. Init station manual
  _initStationManual();

  // 3. Root the mesh
  _rootTheMesh();
}

/* _initMesh()
   Either init the mesh with interface/bridge on AP or interface on STATION */
void myMeshStarter::_initMesh() {
  /** If this ESP is defined as IF and is NOT Root node:
   *  - it shall connect to the mesh using its "station" interface;
   *  - it shall serve the web IF on its softAP. */
  if (globalBaseVariables.hasInterface && !(globalBaseVariables.isRoot)) {
    // Special init for case of physically mobile interface (web interface served on AP)
    _interfaceOnAPInit();
  } 
  /** Else: 2 cases: 
   * (i) this ESP is not defined as IF; OR
   * (ii) this ESP is defined as IF and as Root node.
   * In such cases, init the mesh as recommended by the librarie's devs:
   * - if it is root and IF, it will connect to the mesh using its "softAP" interface;
   * - if it is not root, both SoftAP and Station will connect to the mesh. */  
  else {
    globalBaseVariables.laserControllerMesh.init(meshPrefix, meshPass, meshPort, WIFI_AP_STA, ui8WifiChannel, meshHidden, meshMaxConnection);
  }
}

/** _initStationManual()
 *   If globalBaseVariables.hasInterface and globalBaseVariables.isRoot, this box's STATION 
 *   will try to connect to an external network and the web users will have access 
 *   to the STATION through their browser.
 *   The other mesh nodes will connect on the AP of this one. (This is the recommended use case
 *   by the devs of painlessMesh.)
*/
void myMeshStarter::_initStationManual() {
  if (globalBaseVariables.hasInterface && globalBaseVariables.isRoot) {
    globalBaseVariables.laserControllerMesh.stationManual(ssid, pass, ui16GatewayPort, gatewayIP, fixedIP, fixedNetmaskIP);
    // globalBaseVariables.laserControllerMesh.stationManual(ssid, pass);
  }
}

/* _rootTheMesh()
  One root node per mesh is recommanded. Once a node has been set as root,
  it and all other mesh member should know that the mesh contains a root.
*/
void myMeshStarter::_rootTheMesh() {
  if (globalBaseVariables.isRoot) {
    globalBaseVariables.laserControllerMesh.setRoot(true);
    globalBaseVariables.ui32RootNodeId = globalBaseVariables.laserControllerMesh.getNodeId();
  }
  globalBaseVariables.laserControllerMesh.setContainsRoot(true);
}

/* _interfaceOnAPInit()
   If the interface is on the AP, web users have access to it on the AP. They have to
   connect just like on any ESP32 AP.  Accordingly, the AP shall be configured as any
   AP on an ESP32.
   This node will use its STATION to scanning for other mesh nodes and access to
   the mesh (accordingly, scanStationManual shall not be called; scanning shall 
   remain under the full control of painlessMesh).
*/
void myMeshStarter::_interfaceOnAPInit() {
  // 1. init the mesh in station only
  globalBaseVariables.laserControllerMesh.init(meshPrefix, meshPass, meshPort, WIFI_STA, ui8WifiChannel, meshHidden, meshMaxConnection);
  // 2. configure the soft AP
  if(!WiFi.softAPConfig(softApMyIp, softApMeAsGatewayIp, softApNetmask)){
    Serial.println("\nAP Config Failed\n");
  }
  if (WiFi.softAP(softApSsid, softApPassword, 
                  ui8WifiChannel, softApHidden, 
                  softApMaxConnection)){
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("Network " + String(softApSsid) + " running");
    Serial.println("AP IP address:" + String(myIP));
  } else {
    Serial.println("\nStarting AP failed.\n");
  }
}

void myMeshStarter::_setupMdns() {
  snprintf(globalBaseVariables.gcHostnamePrefix, 10, "%s%u", globalBaseVariables.gcHostnamePrefix, (uint32_t)(globalBaseVariables.gui16NodeName));
  // globalBaseVariables.laserControllerMesh.setHostname(globalBaseVariables.gcHostnamePrefix);
  // begin mDNS responder: argument is the name to broadcast. In this example
  // "ESP32_200". It will be broadcasted as ESP32_200.local.
  if (!MDNS.begin(globalBaseVariables.gcHostnamePrefix)) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");

  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("arduino", "tcp", 3232);
}

