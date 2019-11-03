/*
  controllerBoxThis.cpp - controllerBoxThis integrates laserSignal 
  (controllerBoxesCollection + boxState) with pirController.
  Created by Cedric Lor, Octobre 23, 2019.
*/

#include "Arduino.h"
#include "controllerBoxThis.h"

controllerBoxThis::controllerBoxThis():
  globBaseVars(globalBaseVariables),
  thisCtrlerBox(globBaseVars.gui16NodeName),
  thisMeshViews(thisCtrlerBox),
  thisLaserSignalHandler(thisCtrlerBox, thisMeshViews),
  thisPirController(thisCtrlerBox, thisLaserSignalHandler, tSetPirTimeStampAndBrdcstMsg, tPirSpeedBumper, thisMeshViews, 12)
{
  tReboot.set(3000, TASK_ONCE, NULL, NULL, [&](){_reboot();});
}




/** void controllerBoxThis::updateThisBoxNetWorkProperties()
 * 
 *  actualizes the box network properties (i.e. its nodeId, its station IP and 
 *  its APIP).
 * 
 *  thisCtrlerBox is the ControlerBox instance representing this box. */
void controllerBoxThis::_updateThisBoxNetWorkProperties() {
  thisCtrlerBox.networkData.nodeId = globBaseVars.laserControllerMesh.getNodeId();       // store this boxes nodeId in the array of boxes pertaining to the mesh
  thisCtrlerBox.networkData.stationIP = globBaseVars.laserControllerMesh.getStationIP(); // store this boxes StationIP in the array of boxes pertaining to the mesh
  thisCtrlerBox.networkData.APIP = globBaseVars.laserControllerMesh.getAPIP();           // store this boxes APIP in the array of boxes pertaining to the mesh
}





/** void controllerBoxThis::updateThisBoxNodeName()
 * 
 *  actualizes this box nodeName (i.e. the nodeName is set box by box in globBaseVars).
 * 
 *  thisCtrlerBox is the ControlerBox instance representing this box. */
void controllerBoxThis::_updateThisBoxNodeName() {
  thisCtrlerBox.ui16NodeName = globBaseVars.gui16NodeName;
}





void controllerBoxThis::updateThisBoxProperties() {
  _updateThisBoxNetWorkProperties();
  _updateThisBoxNodeName();
  // For this box, i16BoxActiveState and ui32BoxActiveStateStartTime are updated
  // by calls to setBoxActiveState from boxState
  if (globBaseVars.MY_DEBUG == true) {
    Serial.println("controllerBoxThis::updateThisBoxProperties(): Updated myself. Calling printProperties().\n");
    thisCtrlerBox._printProperties(0);
  };
}





void controllerBoxThis::addLaserTasks() {

  // conditional?
  globBaseVars.scheduler.addTask(thisLaserSignalHandler.tSetBoxState);
  if (globBaseVars.hasLasers == true) {
    globBaseVars.scheduler.addTask(tSetPirTimeStampAndBrdcstMsg);
    globBaseVars.scheduler.addTask(tPirSpeedBumper);
    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl._stepColl.tPreloadNextStep);
    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl.tPlayBoxState);

    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl._laserSequences.tPreloadNextLaserSequence);
    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl._laserSequences.getBars().tPreloadNextBar);

    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl._laserSequences.tPlayLaserSequence);
    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl._laserSequences.getBars().tPlayBar);
    globBaseVars.scheduler.addTask(thisLaserSignalHandler.thisBxStateColl._laserSequences.getBars().getNotes().tPlayNote);
  }

}





void controllerBoxThis::_reboot() {
  if (globBaseVars.MY_DG_WS) { Serial.printf("ControlerBox::tReboot(): About to reboot.\n"); }
  ESP.restart();
}